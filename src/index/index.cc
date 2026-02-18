#include <sourcemeta/blaze/linter.h>

#include <sourcemeta/core/build.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/options.h>
#include <sourcemeta/core/parallel.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/shared.h>
#include <sourcemeta/one/web.h>

#include "explorer.h"
#include "generators.h"
#include "output.h"

#include <algorithm>   // std::sort
#include <cassert>     // assert
#include <chrono>      // std::chrono
#include <cstdlib>     // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>   // std::exception
#include <filesystem>  // std::filesystem
#include <iomanip>     // std::setw, std::setfill
#include <iostream>    // std::cerr, std::cout
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

// We rely on this special prefix to avoid file system collisions. The reason it
// works is that URIs cannot have "%" without percent-encoding it as "%25", and
// the resolver will not unescape it back when computing the relative path to an
// entry
constexpr auto SENTINEL{"%"};

static auto attribute_not_disabled(
    const sourcemeta::one::Configuration::Collection &collection,
    const sourcemeta::core::JSON::String &property) -> bool {
  return !collection.extra.defines(property) ||
         !collection.extra.at(property).is_boolean() ||
         collection.extra.at(property).to_boolean();
}

static auto print_progress(std::mutex &mutex, const std::size_t threads,
                           const std::string_view title,
                           const std::string_view prefix,
                           const std::size_t current, const std::size_t total)
    -> void {
  const auto percentage{current * 100 / total};
  std::lock_guard<std::mutex> lock{mutex};
  std::cerr << "(" << std::setfill(' ') << std::setw(3)
            << static_cast<int>(percentage) << "%) " << title << ": " << prefix
            << " [" << std::this_thread::get_id() << "/" << threads << "]\n";
}

template <typename Handler, typename Adapter>
static auto
DISPATCH(const std::filesystem::path &destination,
         const sourcemeta::core::BuildDependencies<typename Adapter::node_type>
             &dependencies,
         const typename Handler::Context &context, std::mutex &mutex,
         const std::string_view title, const std::string_view prefix,
         const std::string_view suffix, Adapter &adapter,
         sourcemeta::one::Output &output) -> void {
  if (!sourcemeta::core::build<typename Handler::Context>(
          adapter, Handler::handler, destination, dependencies, context)) {
    std::lock_guard<std::mutex> lock{mutex};
    std::cerr << "(skip) " << title << ": " << prefix << " [" << suffix
              << "]\n";
  }

  // We need to mark files regardless of whether they were generated or not
  output.track(destination);
  output.track(destination.string() + ".deps");
}

static auto index_main(const std::string_view &program,
                       const sourcemeta::core::Options &app) -> int {
  std::cout << "Sourcemeta One " << sourcemeta::one::edition() << " v"
            << sourcemeta::one::version() << "\n";

  if (app.positional().size() != 2) {
    std::cout << "Usage: " << std::filesystem::path{program}.filename().string()
              << " <one.json> <path/to/output/directory>\n";
    return EXIT_FAILURE;
  }

  /////////////////////////////////////////////////////////////////////////////
  // (1) Prepare the output directory
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::one::Output output{app.positional().at(1)};
  std::cerr << "Writing output to: " << output.path().string() << "\n";

  /////////////////////////////////////////////////////////////////////////////
  // (2) Process the configuration file
  /////////////////////////////////////////////////////////////////////////////

  const auto configuration_path{
      std::filesystem::canonical(app.positional().at(0))};
  std::cerr << "Using configuration: " << configuration_path.string() << "\n";
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, SOURCEMETA_ONE_COLLECTIONS)};

  if (app.contains("configuration")) {
    sourcemeta::core::prettify(raw_configuration, std::cout);
    std::cout << "\n";
    return EXIT_SUCCESS;
  }

  auto configuration{sourcemeta::one::Configuration::parse(raw_configuration)};

  /////////////////////////////////////////////////////////////////////////////
  // (3) Support overriding the target URL from the CLI
  /////////////////////////////////////////////////////////////////////////////

  if (app.contains("url")) {
    std::cerr << "Overriding the URL in the configuration file with: "
              << app.at("url").at(0) << "\n";
    sourcemeta::core::URI url{std::string{app.at("url").at(0)}};
    if (url.is_absolute() && url.scheme().has_value() &&
        (url.scheme().value() == "https" || url.scheme().value() == "http")) {
      // TODO: Write a test that covers URL overriding
      configuration.url =
          sourcemeta::core::URI::canonicalize(std::string{app.at("url").at(0)});
    } else {
      std::cerr << "error: The URL option must be an absolute HTTP(s) URL\n";
      return EXIT_FAILURE;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // (4) Store a mark of the One version for target dependencies
  /////////////////////////////////////////////////////////////////////////////

  // We do this so that targets can be re-built if the One version changes
  const auto mark_version_path{output.path() / "version.json"};
  // Note we only write back if the content changed in order to not accidentally
  // bump up the file modified time
  output.write_json_if_different(
      mark_version_path, sourcemeta::core::JSON{sourcemeta::one::version()});

  /////////////////////////////////////////////////////////////////////////////
  // (5) Store the full configuration file for target dependencies
  /////////////////////////////////////////////////////////////////////////////

  // For targets that depend on the contents of the configuration or on anything
  // potentially derived from the configuration, such as the resolver
  const auto mark_configuration_path{output.path() / "configuration.json"};
  // Note we only write back if the content changed in order to not accidentally
  // bump up the file modified time
  output.write_json_if_different(mark_configuration_path, raw_configuration);

  /////////////////////////////////////////////////////////////////////////////
  // (6) First pass to locate all of the schemas we will be indexing
  // NOTE: No files are generated. We only want to know what's out there
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::one::Resolver resolver;
  // This step is very fast, so going parallel about it seems overkill, even
  // though in theory we could
  for (const auto &pair : configuration.entries) {
    const auto *collection{
        std::get_if<sourcemeta::one::Configuration::Collection>(&pair.second)};
    if (!collection) {
      continue;
    }

    for (const auto &entry : std::filesystem::recursive_directory_iterator{
             collection->absolute_path}) {
      if (!entry.is_regular_file()) {
        continue;
      }

      const auto extension{entry.path().extension()};
      // TODO: Allow the configuration file to override this
      const auto looks_like_schema_file{
          extension == ".yaml" || extension == ".yml" || extension == ".json"};
      if (!looks_like_schema_file) {
        continue;
      }

      std::cerr << "Detecting: " << entry.path().string() << " (#"
                << resolver.size() + 1 << ")\n";

      const auto mapping{resolver.add(configuration.url, pair.first,
                                      *collection, entry.path())};
      // Useful for debugging
      if (app.contains("verbose")) {
        std::cerr << mapping.first.get() << " => " << mapping.second.get()
                  << "\n";
      }
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  // (7) Do a first analysis pass on the schemas and materialise them for
  // further analysis. We do this so that we don't end up rebasing the same
  // schemas over and over again depending on the order of analysis later on
  /////////////////////////////////////////////////////////////////////////////

  const auto schemas_path{output.path() / "schemas"};
  sourcemeta::core::BuildAdapterFilesystem adapter;
  // Mainly to not screw up the logs
  std::mutex mutex;
  const auto concurrency{app.contains("concurrency")
                             ? std::stoull(app.at("concurrency").front().data())
                             : std::thread::hardware_concurrency()};
  sourcemeta::core::parallel_for_each(
      resolver.begin(), resolver.end(),
      [&output, &schemas_path, &resolver, &mutex, &adapter,
       &mark_configuration_path, &mark_version_path](
          const auto &schema, const auto threads, const auto cursor) {
        print_progress(mutex, threads, "Ingesting", schema.first, cursor,
                       resolver.size());
        const auto destination{schemas_path / schema.second.relative_path /
                               SENTINEL / "schema.metapack"};
        DISPATCH<sourcemeta::one::GENERATE_MATERIALISED_SCHEMA>(
            destination,
            {schema.second.path,
             // This target depends on the configuration file given things like
             // resolve maps and base URIs
             mark_configuration_path, mark_version_path},
            {schema.first, resolver}, mutex, "Ingesting", schema.first,
            "materialise", adapter, output);

        // Mark the materialised schema in the resolver
        resolver.cache_path(schema.first, destination);
      },
      concurrency);

  /////////////////////////////////////////////////////////////////////////////
  // (8) Generate all the artifacts that purely depend on the schemas
  /////////////////////////////////////////////////////////////////////////////

  // Give it a generous thread stack size, otherwise we might overflow
  // the small-by-default thread stack with Blaze
  constexpr auto THREAD_STACK_SIZE{8 * 1024 * 1024};

  const auto explorer_path{output.path() / "explorer"};
  sourcemeta::core::parallel_for_each(
      resolver.begin(), resolver.end(),
      [&output, &schemas_path, &explorer_path, &resolver, &mutex, &adapter,
       &mark_configuration_path, &mark_version_path](
          const auto &schema, const auto threads, const auto cursor) {
        print_progress(mutex, threads, "Analysing", schema.first, cursor,
                       resolver.size());
        const auto base_path{schemas_path / schema.second.relative_path /
                             SENTINEL};

        DISPATCH<sourcemeta::one::GENERATE_POINTER_POSITIONS>(
            base_path / "positions.metapack",
            {base_path / "schema.metapack", mark_version_path}, resolver, mutex,
            "Analysing", schema.first, "positions", adapter, output);

        DISPATCH<sourcemeta::one::GENERATE_FRAME_LOCATIONS>(
            base_path / "locations.metapack",
            {base_path / "schema.metapack", mark_version_path}, resolver, mutex,
            "Analysing", schema.first, "locations", adapter, output);

        DISPATCH<sourcemeta::one::GENERATE_DEPENDENCIES>(
            base_path / "dependencies.metapack",
            {base_path / "schema.metapack", mark_version_path}, resolver, mutex,
            "Analysing", schema.first, "dependencies", adapter, output);

        DISPATCH<sourcemeta::one::GENERATE_STATS>(
            base_path / "stats.metapack",
            {base_path / "schema.metapack", mark_version_path}, resolver, mutex,
            "Analysing", schema.first, "stats", adapter, output);

        DISPATCH<sourcemeta::one::GENERATE_HEALTH>(
            base_path / "health.metapack",
            {base_path / "schema.metapack", base_path / "dependencies.metapack",
             mark_version_path},
            {std::ref(resolver), std::cref(schema.second.collection.get())},
            mutex, "Analysing", schema.first, "health", adapter, output);

        DISPATCH<sourcemeta::one::GENERATE_BUNDLE>(
            base_path / "bundle.metapack",
            {base_path / "schema.metapack", base_path / "dependencies.metapack",
             mark_version_path},
            resolver, mutex, "Analysing", schema.first, "bundle", adapter,
            output);

        DISPATCH<sourcemeta::one::GENERATE_EDITOR>(
            base_path / "editor.metapack",
            {base_path / "bundle.metapack", mark_version_path}, resolver, mutex,
            "Analysing", schema.first, "editor", adapter, output);

        if (attribute_not_disabled(schema.second.collection.get(),
                                   "x-sourcemeta-one:evaluate")) {
          DISPATCH<sourcemeta::one::GENERATE_BLAZE_TEMPLATE>(
              base_path / "blaze-exhaustive.metapack",
              {base_path / "bundle.metapack", mark_version_path},
              sourcemeta::blaze::Mode::Exhaustive, mutex, "Analysing",
              schema.first, "blaze-exhaustive", adapter, output);
          DISPATCH<sourcemeta::one::GENERATE_BLAZE_TEMPLATE>(
              base_path / "blaze-fast.metapack",
              {base_path / "bundle.metapack", mark_version_path},
              sourcemeta::blaze::Mode::FastValidation, mutex, "Analysing",
              schema.first, "blaze-fast", adapter, output);
        }

        DISPATCH<sourcemeta::one::GENERATE_EXPLORER_SCHEMA_METADATA>(
            explorer_path / schema.second.relative_path / SENTINEL /
                "schema.metapack",
            {base_path / "schema.metapack", base_path / "health.metapack",
             base_path / "dependencies.metapack",
             // As this target reads the alert from the configuration file
             mark_configuration_path, mark_version_path},
            {resolver, schema.second.collection.get(),
             schema.second.relative_path},
            mutex, "Analysing", schema.first, "metadata", adapter, output);
      },
      concurrency, THREAD_STACK_SIZE);

  /////////////////////////////////////////////////////////////////////////////
  // (9) Scan the generated files so far to prepare for more complex targets
  /////////////////////////////////////////////////////////////////////////////

  // This is a pretty fast step that will be useful for us to properly declare
  // dependencies for HTML and navigational targets

  print_progress(mutex, concurrency, "Reviewing", schemas_path.string(), 1, 2);
  std::vector<std::filesystem::path> directories;
  // The top-level is itself a directory
  directories.emplace_back(schemas_path);
  std::vector<std::filesystem::path> summaries;
  std::vector<std::filesystem::path> dependencies;
  if (std::filesystem::exists(schemas_path)) {
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator{schemas_path}) {
      if (output.is_untracked_file(entry.path())) {
        continue;
      }

      if (entry.is_directory() && entry.path().filename() != SENTINEL) {
        const auto children{
            std::distance(std::filesystem::directory_iterator(entry.path()),
                          std::filesystem::directory_iterator{})};
        if (children == 0 ||
            (std::filesystem::exists(entry.path() / SENTINEL) &&
             children == 1)) {
          continue;
        }

        assert(entry.path().is_absolute());
        directories.emplace_back(entry.path());
      } else if (entry.is_regular_file() &&
                 entry.path().filename() == "schema.metapack" &&
                 entry.path().parent_path().filename() == SENTINEL) {
        summaries.emplace_back(explorer_path / std::filesystem::relative(
                                                   entry.path(), schemas_path));
        dependencies.emplace_back(entry.path().parent_path() /
                                  "dependencies.metapack");
      }
    }

    // Re-order the directories so that the most nested ones come first, as we
    // often need to process directories in that order
    std::ranges::sort(directories, [](const std::filesystem::path &left,
                                      const std::filesystem::path &right) {
      const auto left_depth{std::distance(left.begin(), left.end())};
      const auto right_depth{std::distance(right.begin(), right.end())};
      if (left_depth == right_depth) {
        return left < right;
      } else {
        return left_depth > right_depth;
      }
    });
  }

  print_progress(mutex, concurrency, "Reviewing", schemas_path.string(), 2, 2);
  DISPATCH<sourcemeta::one::GENERATE_DEPENDENCY_TREE>(
      output.path() / "dependency-tree.metapack", dependencies, resolver, mutex,
      "Reviewing", schemas_path.string(), "dependencies", adapter, output);

  /////////////////////////////////////////////////////////////////////////////
  // (10) A further pass on the schemas after review
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::core::parallel_for_each(
      resolver.begin(), resolver.end(),
      [&output, &schemas_path, &resolver, &mutex,
       &adapter](const auto &schema, const auto threads, const auto cursor) {
        print_progress(mutex, threads, "Reworking", schema.first, cursor,
                       resolver.size());
        const auto base_path{schemas_path / schema.second.relative_path /
                             SENTINEL};
        DISPATCH<sourcemeta::one::GENERATE_DEPENDENTS>(
            base_path / "dependents.metapack",
            {output.path() / "dependency-tree.metapack"}, schema.first, mutex,
            "Reworking", schema.first, "dependents", adapter, output);
      },
      concurrency, THREAD_STACK_SIZE);

  /////////////////////////////////////////////////////////////////////////////
  // (11) Generate the JSON-based explorer
  /////////////////////////////////////////////////////////////////////////////

  print_progress(mutex, concurrency, "Producing", explorer_path.string(), 0,
                 100);
  DISPATCH<sourcemeta::one::GENERATE_EXPLORER_SEARCH_INDEX>(
      explorer_path / SENTINEL / "search.metapack", summaries, nullptr, mutex,
      "Producing", explorer_path.string(), "search", adapter, output);

  // Directory generation depends on the configuration for metadata
  summaries.emplace_back(mark_configuration_path);
  summaries.emplace_back(mark_version_path);
  // Note that we can't parallelise this loop, as we need to do it bottom-up
  for (std::size_t cursor = 0; cursor < directories.size(); cursor++) {
    const auto &entry{directories[cursor]};
    const auto relative_path{std::filesystem::relative(entry, schemas_path)};
    print_progress(mutex, 1, "Producing", relative_path.string(), cursor + 1,
                   directories.size());
    const auto destination{std::filesystem::weakly_canonical(
        explorer_path / relative_path / SENTINEL / "directory.metapack")};
    DISPATCH<sourcemeta::one::GENERATE_EXPLORER_DIRECTORY_LIST>(
        destination,
        // If any of the entry summary files changes, by definition we need to
        // re-compute. This is a good enough dependency we can use without
        // having to manually scan the entire directory structure once more
        summaries,
        {.directory = entry,
         .configuration = configuration,
         .output = output,
         .explorer_path = explorer_path,
         .schemas_path = schemas_path},
        mutex, "Producing", relative_path.string(), "directory", adapter,
        output);
  }

  // Restore the summaries list as it was before
  summaries.pop_back();
  summaries.pop_back();

  /////////////////////////////////////////////////////////////////////////////
  // (12) Generate the HTML web interface
  /////////////////////////////////////////////////////////////////////////////

  if (configuration.html.has_value()) {
    sourcemeta::core::parallel_for_each(
        directories.begin(), directories.end(),
        [&configuration, &output, &schemas_path, &explorer_path, &directories,
         &summaries, &mutex, &adapter, &mark_configuration_path,
         &mark_version_path](const auto &entry, const auto threads,
                             const auto cursor) {
          const auto relative_path{
              std::filesystem::relative(entry, schemas_path)};
          print_progress(mutex, threads, "Rendering", relative_path.string(),
                         cursor, directories.size() + summaries.size());

          if (relative_path == ".") {
            DISPATCH<sourcemeta::one::GENERATE_WEB_INDEX>(
                explorer_path / SENTINEL / "directory-html.metapack",
                {explorer_path / SENTINEL / "directory.metapack",
                 // We rely on the configuration for site metadata
                 mark_configuration_path, mark_version_path},
                configuration, mutex, "Rendering", relative_path.string(),
                "index", adapter, output);
            DISPATCH<sourcemeta::one::GENERATE_WEB_NOT_FOUND>(
                explorer_path / SENTINEL / "404.metapack",
                {// We rely on the configuration for site metadata
                 mark_configuration_path, mark_version_path},
                configuration, mutex, "Rendering", relative_path.string(),
                "not-found", adapter, output);
          } else {
            DISPATCH<sourcemeta::one::GENERATE_WEB_DIRECTORY>(
                explorer_path / relative_path / SENTINEL /
                    "directory-html.metapack",
                {explorer_path / relative_path / SENTINEL /
                     "directory.metapack",
                 // We rely on the configuration for site metadata
                 mark_configuration_path, mark_version_path},
                configuration, mutex, "Rendering", relative_path.string(),
                "directory", adapter, output);
          }
        },
        concurrency);

    sourcemeta::core::parallel_for_each(
        summaries.begin(), summaries.end(),
        [&configuration, &output, &schemas_path, &explorer_path, &directories,
         &summaries, &mutex, &adapter, &mark_configuration_path,
         &mark_version_path](const auto &entry, const auto threads,
                             const auto cursor) {
          const auto relative_path{
              std::filesystem::relative(entry, explorer_path)
                  .parent_path()
                  .parent_path()};
          print_progress(mutex, threads, "Rendering", relative_path.string(),
                         cursor + directories.size(),
                         summaries.size() + directories.size());
          const auto schema_path{schemas_path / relative_path / SENTINEL};
          DISPATCH<sourcemeta::one::GENERATE_WEB_SCHEMA>(
              entry.parent_path() / "schema-html.metapack",
              {entry, schema_path / "dependencies.metapack",
               schema_path / "health.metapack",
               schema_path / "dependents.metapack",
               // We rely on the configuration for site metadata
               mark_configuration_path, mark_version_path},
              configuration, mutex, "Rendering", relative_path.string(),
              "schema", adapter, output);
        },
        concurrency);
  }

  /////////////////////////////////////////////////////////////////////////////
  // (13) Generate the pre computed routes
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::core::URITemplateRouter router;
  router.add("/self/v1/api/list", sourcemeta::one::HANDLER_SELF_V1_API_LIST);
  router.add("/self/v1/api/list/{+path}",
             sourcemeta::one::HANDLER_SELF_V1_API_LIST_PATH);
  router.add("/self/v1/api/schemas/dependencies/{+schema}",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_DEPENDENCIES);
  router.add("/self/v1/api/schemas/dependents/{+schema}",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_DEPENDENTS);
  router.add("/self/v1/api/schemas/health/{+schema}",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_HEALTH);
  router.add("/self/v1/api/schemas/locations/{+schema}",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_LOCATIONS);
  router.add("/self/v1/api/schemas/positions/{+schema}",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_POSITIONS);
  router.add("/self/v1/api/schemas/stats/{+schema}",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_STATS);
  router.add("/self/v1/api/schemas/metadata/{+schema}",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_METADATA);
  router.add("/self/v1/api/schemas/evaluate/{+schema}",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_EVALUATE);
  router.add("/self/v1/api/schemas/trace/{+schema}",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_TRACE);
  router.add("/self/v1/api/schemas/search",
             sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_SEARCH);
  router.add("/self/v1/api/{+any}",
             sourcemeta::one::HANDLER_SELF_V1_API_DEFAULT);

  if (configuration.html.has_value()) {
    router.add("/self/static/{+path}", sourcemeta::one::HANDLER_SELF_STATIC);
  }

  const auto routes_path{output.path() / "routes.bin"};
  DISPATCH<sourcemeta::one::GENERATE_URITEMPLATE_ROUTES>(
      routes_path, {mark_configuration_path, mark_version_path}, router, mutex,
      "Producing", routes_path.string(), "routes", adapter, output);

  /////////////////////////////////////////////////////////////////////////////
  // Finish generation
  /////////////////////////////////////////////////////////////////////////////

  // TODO: Print the size of the output directory here

  output.remove_unknown_files();

  // TODO: Add a test for this
  if (app.contains("profile")) {
    std::cerr << "Profiling...\n";
    std::vector<std::pair<std::filesystem::path, std::chrono::milliseconds>>
        durations;
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator{output.path()}) {
      if (entry.is_regular_file() && entry.path().extension() == ".metapack") {
        try {
          const auto file{sourcemeta::one::read_stream_raw(entry.path())};
          assert(file.has_value());
          durations.emplace_back(entry.path(), file.value().duration);
        } catch (...) {
          std::cerr << "Could not profile file: " << entry.path() << "\n";
          throw;
        }
      }
    }

    std::sort(durations.begin(), durations.end(),
              [](const auto &left, const auto &right) {
                return left.second > right.second;
              });

    constexpr std::size_t PROFILE_ENTRIES_MAXIMUM{25};
    for (std::size_t index = 0;
         index < std::min(durations.size(),
                          static_cast<std::size_t>(PROFILE_ENTRIES_MAXIMUM));
         index++) {
      std::cout << durations[index].second.count() << "ms "
                << std::filesystem::relative(durations[index].first,
                                             output.path())
                       .string()
                << "\n";
    }
  }

  return EXIT_SUCCESS;
}

auto main(int argc, char *argv[]) noexcept -> int {
  try {
    sourcemeta::core::Options app;
    // TODO: Support a --help flag
    app.option("url", {"u"});
    app.option("concurrency", {"c"});
    app.flag("verbose", {"v"});
    app.flag("profile", {"p"});
    app.flag("configuration", {"g"});
    app.parse(argc, argv);
    const std::string_view program{argv[0]};

    return index_main(program, app);
  } catch (const sourcemeta::one::ConfigurationReadError &error) {
    std::cerr << "error: " << error.what() << "\n";
    std::cerr << "  from " << error.from().string() << "\n";
    std::cerr << "  at \"" << sourcemeta::core::to_string(error.location())
              << "\"\n";
    std::cerr << "  to " << error.target().string() << "\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::ConfigurationUnknownBuiltInCollectionError
               &error) {
    std::cerr << "error: " << error.what() << "\n";
    std::cerr << "  from " << error.from().string() << "\n";
    std::cerr << "  at \"" << sourcemeta::core::to_string(error.location())
              << "\"\n";
    std::cerr << "  to " << error.identifier() << "\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::OptionsUnexpectedValueFlagError &error) {
    std::cerr << "error: " << error.what() << " '" << error.option() << "'\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::OptionsMissingOptionValueError &error) {
    std::cerr << "error: " << error.what() << " '" << error.option() << "'\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::OptionsUnknownOptionError &error) {
    std::cerr << "error: " << error.what() << " '" << error.option() << "'\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    std::cerr << "error: " << error.what() << "\n" << error.stacktrace();
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::MetaschemaError &error) {
    std::cerr << "error: " << error.what() << "\n" << error.stacktrace();
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::CustomRuleError &error) {
    std::cerr << "error: " << error.what() << "\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::blaze::LinterInvalidNameError &error) {
    std::cerr << "error: " << error.what() << "\n  at name "
              << error.identifier() << "\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::blaze::LinterMissingNameError &error) {
    std::cerr << "error: " << error.what() << "\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::URIParseError &error) {
    std::cerr << "error: " << error.what() << "\n  at column " << error.column()
              << "\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    std::cerr << "error: " << error.what() << "\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::ResolverOutsideBaseError &error) {
    std::cerr << "error: " << error.what() << "\n  at " << error.uri()
              << "\n  with base " << error.base() << "\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::SchemaReferenceObjectResourceError &error) {
    std::cerr << "error: " << error.what() << "\n  " << error.identifier()
              << "\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    std::cerr << "error: " << error.what() << "\n  " << error.identifier()
              << "\n\nDid you forget to register a schema with such URI in the "
                 "one?\n";
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::SchemaReferenceError &error) {
    std::cerr << "error: " << error.what() << "\n  " << error.identifier()
              << "\n    at schema location \"";
    sourcemeta::core::stringify(error.location(), std::cerr);
    std::cerr << "\"\n";
    return EXIT_FAILURE;
  } catch (const std::filesystem::filesystem_error &error) {
    if (error.code() == std::make_error_condition(std::errc::file_exists) ||
        error.code() == std::make_error_condition(std::errc::not_a_directory)) {
      std::cerr << "error: file already exists\n  at " << error.path1().string()
                << "\n";
    } else if (error.code() == std::errc::no_such_file_or_directory) {
      std::cerr << "error: could not locate the requested file\n  at "
                << error.path1().string() << "\n";
    } else {
      std::cerr << error.what() << "\n";
    }

    return EXIT_FAILURE;
  } catch (const std::exception &error) {
    std::cerr << "unexpected error: " << error.what() << "\n";
    return EXIT_FAILURE;
  }
}
