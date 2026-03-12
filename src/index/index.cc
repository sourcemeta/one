#include <sourcemeta/blaze/linter.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/options.h>
#include <sourcemeta/core/parallel.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/build.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/shared.h>
#include <sourcemeta/one/web.h>

#include "explorer.h"
#include "generators.h"

#include <algorithm>     // std::sort
#include <atomic>        // std::atomic
#include <cassert>       // assert
#include <chrono>        // std::chrono
#include <cstdlib>       // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>     // std::exception
#include <filesystem>    // std::filesystem
#include <fstream>       // std::ofstream
#include <functional>    // std::reference_wrapper, std::cref
#include <iomanip>       // std::setw, std::setfill
#include <iostream>      // std::cerr, std::cout
#include <mutex>         // std::mutex, std::lock_guard
#include <optional>      // std::optional
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PROFILE_INIT(state)                                                    \
  std::pair<                                                                   \
      std::vector<std::pair<std::string_view, std::chrono::milliseconds>>,     \
      std::chrono::steady_clock::time_point>                                   \
      state {                                                                  \
    {}, std::chrono::steady_clock::now()                                       \
  }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PROFILE_END(state, label)                                              \
  (state).first.emplace_back(                                                  \
      (label), std::chrono::duration_cast<std::chrono::milliseconds>(          \
                   std::chrono::steady_clock::now() - (state).second));        \
  (state).second = std::chrono::steady_clock::now()

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

// Walk up from a destination path until the "%" sentinel is found,
// then look up the URI via path_to_uri keyed by
// <base>/<relative>/%/schema.metapack
static auto uri_for_destination(
    const std::filesystem::path &destination,
    const std::unordered_map<std::string, std::string> &path_to_uri)
    -> const std::string & {
  auto current{destination};
  while (current.filename() != SENTINEL) {
    current = current.parent_path();
  }

  const auto schema_metapack{current / "schema.metapack"};
  const auto match{path_to_uri.find(schema_metapack.string())};
  assert(match != path_to_uri.end());
  return match->second;
}

static auto
commit_entry(std::mutex &entries_mutex, sourcemeta::one::BuildEntries &entries,
             const std::filesystem::path &destination,
             const std::vector<std::filesystem::path> &static_dependencies,
             std::vector<std::filesystem::path> &&dynamic_dependencies)
    -> void {
  const auto now{std::filesystem::file_time_type::clock::now()};
  std::lock_guard<std::mutex> lock{entries_mutex};
  auto &entry{entries[destination.native()]};
  entry.file_mark = now;
  entry.static_dependencies = static_dependencies;
  entry.dynamic_dependencies = std::move(dynamic_dependencies);
}

static auto index_main(const std::string_view &program,
                       const sourcemeta::core::Options &app) -> int {
  if (!app.contains("skip-banner")) {
    std::cerr << "Sourcemeta One " << sourcemeta::one::edition() << " v"
              << sourcemeta::one::version() << "\n";
  }

  if (app.positional().size() != 2) {
    std::cerr << "Usage: " << std::filesystem::path{program}.filename().string()
              << " <one.json> <path/to/output/directory>\n";
    return EXIT_FAILURE;
  }

  PROFILE_INIT(profiling);

  /////////////////////////////////////////////////////////////////////////////
  // (1) Parse the output directory
  /////////////////////////////////////////////////////////////////////////////

  const auto output_path{
      std::filesystem::weakly_canonical(app.positional().at(1))};

  if (std::filesystem::exists(output_path) &&
      !std::filesystem::is_directory(output_path)) {
    throw std::filesystem::filesystem_error{
        "file already exists", output_path,
        std::make_error_code(std::errc::file_exists)};
  }

  std::cerr << "Writing output to: " << output_path.string() << "\n";

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

  auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path.parent_path())};

  /////////////////////////////////////////////////////////////////////////////
  // (3) Support overriding the target URL from the CLI
  /////////////////////////////////////////////////////////////////////////////

  if (app.contains("url")) {
    std::cerr << "Overriding the URL in the configuration file with: "
              << app.at("url").at(0) << "\n";
    sourcemeta::core::URI url{std::string{app.at("url").at(0)}};
    if (url.is_absolute() && url.scheme().has_value() &&
        (url.scheme().value() == "https" || url.scheme().value() == "http")) {
      configuration.url =
          sourcemeta::core::URI::canonicalize(std::string{app.at("url").at(0)});
    } else {
      std::cerr << "error: The URL option must be an absolute HTTP(s) URL\n";
      return EXIT_FAILURE;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // (4) Resolve a URI to a schema filesystem path
  /////////////////////////////////////////////////////////////////////////////

  if (app.contains("resolve-schema")) {
    const sourcemeta::core::URI input_uri{
        std::string{app.at("resolve-schema").front()}};
    std::cerr << "Resolving schema for URI: " << input_uri.recompose() << "\n";
    const auto result{configuration.resolve_schema(input_uri)};
    if (result.has_value()) {
      std::cout << result.value().string() << "\n";
      return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
  }

  /////////////////////////////////////////////////////////////////////////////
  // (5) Prepare the output directory and load previous state
  /////////////////////////////////////////////////////////////////////////////

  std::filesystem::create_directories(output_path);
  const auto canonical_output{std::filesystem::canonical(output_path)};

  sourcemeta::one::BuildEntries entries;
  const auto state_path{canonical_output / "state.bin"};
  if (std::filesystem::exists(state_path)) {
    try {
      sourcemeta::one::load_state(state_path, entries);
    } catch (...) {
      entries.clear();
    }
  }

  // Read current version from existing version.json (if present)
  // Only trust on-disk files when state.bin was loaded successfully,
  // otherwise the entries map and the on-disk artefacts are out of sync
  std::string current_version;
  const auto version_path{canonical_output / "version.json"};
  if (!entries.empty() && std::filesystem::exists(version_path)) {
    const auto version_json{sourcemeta::core::read_json(version_path)};
    current_version = version_json.to_string();
  }

  // Read current configuration from existing configuration.json (if present)
  auto current_configuration{sourcemeta::core::JSON{nullptr}};
  const auto configuration_json_path{canonical_output / "configuration.json"};
  if (!entries.empty() && std::filesystem::exists(configuration_json_path)) {
    current_configuration =
        sourcemeta::core::read_json(configuration_json_path);
  }

  // Determine comment
  const std::string comment{app.contains("comment")
                                ? std::string{app.at("comment").at(0)}
                                : std::string{}};

  // Determine build type
  const auto build_type{configuration.html.has_value()
                            ? sourcemeta::one::BuildType::Full
                            : sourcemeta::one::BuildType::Headless};

  PROFILE_END(profiling, "Startup");

  // Mainly to not screw up the logs
  std::mutex mutex;
  const auto concurrency{app.contains("concurrency")
                             ? std::stoull(app.at("concurrency").front().data())
                             : std::thread::hardware_concurrency()};

  /////////////////////////////////////////////////////////////////////////////
  // (6) First pass to locate all of the schemas we will be indexing
  // NOTE: No files are generated. We only want to know what's out there
  /////////////////////////////////////////////////////////////////////////////

  struct DetectedSchema {
    std::filesystem::path collection_relative_path;
    std::reference_wrapper<const sourcemeta::one::Configuration::Collection>
        collection;
    std::filesystem::path path;
  };

  std::vector<DetectedSchema> detected_schemas;
  for (const auto &pair : configuration.entries) {
    const auto *collection{
        std::get_if<sourcemeta::one::Configuration::Collection>(&pair.second)};
    if (!collection) {
      continue;
    }

    if (!std::filesystem::is_directory(collection->absolute_path)) {
      continue;
    }

    for (const auto &entry : std::filesystem::recursive_directory_iterator{
             collection->absolute_path}) {
      if (!entry.is_regular_file()) {
        continue;
      }

      const auto extension{entry.path().extension()};
      // TODO: Allow the configuration file to override this
      if (extension != ".yaml" && extension != ".yml" && extension != ".json") {
        continue;
      }

      std::cerr << "Detecting: " << entry.path().string() << " (#"
                << detected_schemas.size() + 1 << ")\n";
      detected_schemas.push_back(
          {pair.first, std::cref(*collection), entry.path()});
    }
  };

  PROFILE_END(profiling, "Detect");

  /////////////////////////////////////////////////////////////////////////////
  // (7) Resolve all detected schemas in parallel
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::one::Resolver resolver;
  sourcemeta::core::parallel_for_each(
      detected_schemas.begin(), detected_schemas.end(),
      [&configuration, &resolver, &mutex, &detected_schemas,
       &app](const auto &detected, const auto threads, const auto cursor) {
        print_progress(mutex, threads, "Resolving",
                       detected.path.filename().string(), cursor,
                       detected_schemas.size());
        const auto mapping{
            resolver.add(configuration.url, detected.collection_relative_path,
                         detected.collection.get(), detected.path)};
        if (app.contains("verbose")) {
          std::lock_guard<std::mutex> lock{mutex};
          std::cerr << mapping.first.get() << " => " << mapping.second.get()
                    << "\n";
        }
      },
      concurrency);

  PROFILE_END(profiling, "Resolve");

  /////////////////////////////////////////////////////////////////////////////
  // (8) Build schema info map and compute the delta plan
  /////////////////////////////////////////////////////////////////////////////

  const auto schemas_path{canonical_output / "schemas"};
  const auto explorer_path{canonical_output / "explorer"};

  // Build BuildSchemaInformation map from resolver
  std::unordered_map<std::string, sourcemeta::one::BuildSchemaInformation>
      schemas;
  for (const auto &[uri, resolver_entry] : resolver) {
    schemas[uri] = {
        .source = resolver_entry.path,
        .relative_output = resolver_entry.relative_path,
        .mtime = std::filesystem::last_write_time(resolver_entry.path),
        .evaluate = attribute_not_disabled(resolver_entry.collection.get(),
                                           "x-sourcemeta-one:evaluate")};
  }

  // Build reverse map: schema.metapack path -> URI
  // We add entries for both schemas/ and explorer/ bases so that
  // uri_for_destination works for any destination under either tree
  std::unordered_map<std::string, std::string> path_to_uri;
  std::unordered_map<std::string, const sourcemeta::one::Resolver::Entry *>
      uri_to_entry;
  for (const auto &[uri, resolver_entry] : resolver) {
    const auto schemas_base{schemas_path / resolver_entry.relative_path /
                            SENTINEL};
    path_to_uri[(schemas_base / "schema.metapack").string()] = uri;
    const auto explorer_base{explorer_path / resolver_entry.relative_path /
                             SENTINEL};
    path_to_uri[(explorer_base / "schema.metapack").string()] = uri;
    uri_to_entry[uri] = &resolver_entry;
  }

  // Compute the delta plan (empty changed/removed for now)
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  auto plan{sourcemeta::one::delta(build_type, entries, canonical_output,
                                   schemas, sourcemeta::one::version(),
                                   current_version, comment, raw_configuration,
                                   current_configuration, changed, removed)};

  PROFILE_END(profiling, "Prepare");

  /////////////////////////////////////////////////////////////////////////////
  // (9) Execute the plan wave by wave
  /////////////////////////////////////////////////////////////////////////////

  // Give it a generous thread stack size, otherwise we might overflow
  // the small-by-default thread stack with Blaze
  constexpr auto THREAD_STACK_SIZE{8 * 1024 * 1024};

  std::atomic<std::size_t> progress_counter{0};
  std::mutex entries_mutex;

  for (auto &wave : plan.waves) {
    sourcemeta::core::parallel_for_each(
        wave.begin(), wave.end(),
        [&](auto &action, const auto threads, const auto) {
          using sourcemeta::one::BuildAction;

          const auto current{
              progress_counter.fetch_add(1, std::memory_order_relaxed) + 1};
          const std::string_view destination_view{action.destination.native()};
          print_progress(
              mutex, threads,
              action.type == BuildAction::Remove ? "Disposing" : "Producing",
              destination_view.substr(canonical_output.native().size() + 1),
              current, plan.size);

          // Collect dynamic dependencies from handler callbacks
          std::vector<std::filesystem::path> dynamic_dependencies;
          const sourcemeta::one::BuildDynamicCallback dynamic_callback{
              [&dynamic_dependencies](const auto &path) {
                dynamic_dependencies.emplace_back(path);
              }};

          switch (action.type) {
            case BuildAction::Materialise: {
              const auto &uri{
                  uri_for_destination(action.destination, path_to_uri)};
              sourcemeta::one::GENERATE_MATERIALISED_SCHEMA::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, uri);
              resolver.cache_path(uri, action.destination);
              break;
            }

            case BuildAction::Positions: {
              sourcemeta::one::GENERATE_POINTER_POSITIONS::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver);
              break;
            }

            case BuildAction::Locations: {
              sourcemeta::one::GENERATE_FRAME_LOCATIONS::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver);
              break;
            }

            case BuildAction::Dependencies: {
              sourcemeta::one::GENERATE_DEPENDENCIES::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver);
              break;
            }

            case BuildAction::Stats: {
              sourcemeta::one::GENERATE_STATS::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver);
              break;
            }

            case BuildAction::Health: {
              const auto &uri{
                  uri_for_destination(action.destination, path_to_uri)};
              const auto *resolver_entry{uri_to_entry.at(uri)};
              sourcemeta::one::GENERATE_HEALTH::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, resolver_entry->collection.get());
              break;
            }

            case BuildAction::Bundle: {
              sourcemeta::one::GENERATE_BUNDLE::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver);
              break;
            }

            case BuildAction::Editor: {
              sourcemeta::one::GENERATE_EDITOR::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver);
              break;
            }

            case BuildAction::BlazeExhaustive: {
              sourcemeta::one::GENERATE_BLAZE_TEMPLATE::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, sourcemeta::blaze::Mode::Exhaustive);
              break;
            }

            case BuildAction::BlazeFast: {
              sourcemeta::one::GENERATE_BLAZE_TEMPLATE::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, sourcemeta::blaze::Mode::FastValidation);
              break;
            }

            case BuildAction::SchemaMetadata: {
              const auto &uri{
                  uri_for_destination(action.destination, path_to_uri)};
              const auto *resolver_entry{uri_to_entry.at(uri)};
              const sourcemeta::one::GENERATE_EXPLORER_SCHEMA_METADATA::Context
                  context{resolver_entry->collection.get(),
                          resolver_entry->relative_path};
              sourcemeta::one::GENERATE_EXPLORER_SCHEMA_METADATA::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, context);
              break;
            }

            case BuildAction::DependencyTree: {
              sourcemeta::one::GENERATE_DEPENDENCY_TREE::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver);
              break;
            }

            case BuildAction::Dependents: {
              const auto &uri{
                  uri_for_destination(action.destination, path_to_uri)};
              sourcemeta::one::GENERATE_DEPENDENTS::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, uri);
              break;
            }

            case BuildAction::SearchIndex: {
              sourcemeta::one::GENERATE_EXPLORER_SEARCH_INDEX::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver);
              break;
            }

            case BuildAction::DirectoryList: {
              const sourcemeta::one::GENERATE_EXPLORER_DIRECTORY_LIST::Context
                  context{.configuration = configuration,
                          .explorer_path = explorer_path};
              sourcemeta::one::GENERATE_EXPLORER_DIRECTORY_LIST::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, context);
              break;
            }

            case BuildAction::WebIndex: {
              sourcemeta::one::GENERATE_WEB_INDEX::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, configuration);
              break;
            }

            case BuildAction::WebNotFound: {
              sourcemeta::one::GENERATE_WEB_NOT_FOUND::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, configuration);
              break;
            }

            case BuildAction::WebDirectory: {
              sourcemeta::one::GENERATE_WEB_DIRECTORY::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, configuration);
              break;
            }

            case BuildAction::WebSchema: {
              sourcemeta::one::GENERATE_WEB_SCHEMA::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, configuration);
              break;
            }

            case BuildAction::Routes: {
              sourcemeta::core::URITemplateRouter router;
              router.add("/self/v1/api/list",
                         sourcemeta::one::HANDLER_SELF_V1_API_LIST);
              router.add("/self/v1/api/list/{+path}",
                         sourcemeta::one::HANDLER_SELF_V1_API_LIST_PATH);
              router.add(
                  "/self/v1/api/schemas/dependencies/{+schema}",
                  sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_DEPENDENCIES);
              router.add(
                  "/self/v1/api/schemas/dependents/{+schema}",
                  sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_DEPENDENTS);
              router.add("/self/v1/api/schemas/health/{+schema}",
                         sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_HEALTH);
              router.add(
                  "/self/v1/api/schemas/locations/{+schema}",
                  sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_LOCATIONS);
              router.add(
                  "/self/v1/api/schemas/positions/{+schema}",
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
              router.add("/self/v1/health",
                         sourcemeta::one::HANDLER_SELF_V1_HEALTH);
              router.add("/self/v1/api/{+any}",
                         sourcemeta::one::HANDLER_SELF_V1_API_DEFAULT);

              if (build_type == sourcemeta::one::BuildType::Full) {
                router.add("/self/static/{+path}",
                           sourcemeta::one::HANDLER_SELF_STATIC);
              }

              sourcemeta::one::GENERATE_URITEMPLATE_ROUTES::handler(
                  action.destination, action.dependencies, dynamic_callback,
                  resolver, router);
              break;
            }

            case BuildAction::Version: {
              std::filesystem::create_directories(
                  action.destination.parent_path());
              std::ofstream stream{action.destination};
              assert(!stream.fail());
              sourcemeta::core::stringify(
                  sourcemeta::core::JSON{sourcemeta::one::version()}, stream);
              break;
            }

            case BuildAction::Configuration: {
              std::filesystem::create_directories(
                  action.destination.parent_path());
              std::ofstream stream{action.destination};
              assert(!stream.fail());
              sourcemeta::core::stringify(raw_configuration, stream);
              break;
            }

            case BuildAction::Comment: {
              std::filesystem::create_directories(
                  action.destination.parent_path());
              std::ofstream stream{action.destination};
              assert(!stream.fail());
              sourcemeta::core::stringify(sourcemeta::core::JSON{comment},
                                          stream);
              break;
            }

            case BuildAction::Remove: {
              std::filesystem::remove_all(action.destination);
              std::lock_guard<std::mutex> lock{entries_mutex};
              entries.erase(action.destination.string());
              return;
            }
          }

          commit_entry(entries_mutex, entries, action.destination,
                       action.dependencies, std::move(dynamic_dependencies));
        },
        concurrency, THREAD_STACK_SIZE);
  }

  PROFILE_END(profiling, "Build");

  /////////////////////////////////////////////////////////////////////////////
  // Save state
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::one::save_state(state_path, entries);

  PROFILE_END(profiling, "Cleanup");

  // TODO: Add a test for this
  if (app.contains("profile")) {
    std::cerr << "Profiling...\n";
    std::vector<std::pair<std::filesystem::path, std::chrono::milliseconds>>
        durations;
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator{canonical_output}) {
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
                                             canonical_output)
                       .string()
                << "\n";
    }
  }

  PROFILE_END(profiling, "Profile");

  if (app.contains("time")) {
    for (const auto &entry : profiling.first) {
      std::cout << entry.second.count() << "ms " << entry.first << "\n";
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
    app.flag("time", {"t"});
    app.flag("configuration", {"g"});
    app.option("resolve-schema", {"r"});
    app.flag("skip-banner", {"s"});
    app.option("comment", {"m"});
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
