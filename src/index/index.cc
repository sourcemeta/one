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
#include "output.h"

#include <algorithm>     // std::sort
#include <cassert>       // assert
#include <chrono>        // std::chrono
#include <cstdlib>       // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>     // std::exception
#include <filesystem>    // std::filesystem
#include <functional>    // std::reference_wrapper, std::cref
#include <iomanip>       // std::setw, std::setfill
#include <iostream>      // std::cerr, std::cout
#include <optional>      // std::optional
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set
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

template <typename Handler, typename Adapter>
static auto
DISPATCH(const std::filesystem::path &destination,
         const sourcemeta::one::BuildDependencies<typename Adapter::node_type>
             &dependencies,
         const typename Handler::Context &context, std::mutex &mutex,
         const std::string_view title, const std::string_view prefix,
         const std::string_view suffix, Adapter &adapter,
         sourcemeta::one::Output &output) -> bool {
  const auto was_built{sourcemeta::one::build<typename Handler::Context>(
      adapter, Handler::handler, destination, dependencies, context)};
  if (!was_built) {
    std::lock_guard<std::mutex> lock{mutex};
    std::cerr << "(skip) " << title << ": " << prefix << " [" << suffix
              << "]\n";
  }

  // We need to mark files regardless of whether they were generated or not
  output.track(destination);
  output.track(destination.string() + ".deps");
  return was_built;
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
  // (5) Prepare the output directory
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::one::Output output{output_path};

  /////////////////////////////////////////////////////////////////////////////
  // (6) Store a mark of the One version for target dependencies
  /////////////////////////////////////////////////////////////////////////////

  // We do this so that targets can be re-built if the One version changes
  const auto mark_version_path{output.path() / "version.json"};
  // Note we only write back if the content changed in order to not accidentally
  // bump up the file modified time
  output.write_json_if_different(
      mark_version_path, sourcemeta::core::JSON{sourcemeta::one::version()});

  /////////////////////////////////////////////////////////////////////////////
  // (7) Store the full configuration file for target dependencies
  /////////////////////////////////////////////////////////////////////////////

  // For targets that depend on the contents of the configuration or on anything
  // potentially derived from the configuration, such as the resolver
  const auto mark_configuration_path{output.path() / "configuration.json"};
  // Note we only write back if the content changed in order to not accidentally
  // bump up the file modified time
  output.write_json_if_different(mark_configuration_path, raw_configuration);

  /////////////////////////////////////////////////////////////////////////////
  // (8) Store the optional comment for informational purposes
  /////////////////////////////////////////////////////////////////////////////

  if (app.contains("comment")) {
    const auto comment_path{output.path() / "comment.json"};
    output.write_json_if_different(
        comment_path,
        sourcemeta::core::JSON{std::string{app.at("comment").at(0)}});
  }

  PROFILE_END(profiling, "Startup");

  // Mainly to not screw up the logs
  std::mutex mutex;
  const auto concurrency{app.contains("concurrency")
                             ? std::stoull(app.at("concurrency").front().data())
                             : std::thread::hardware_concurrency()};

  /////////////////////////////////////////////////////////////////////////////
  // (9) First pass to locate all of the schemas we will be indexing
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
  // (10) Resolve all detected schemas in parallel
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
  // (11) Do a first analysis pass on the schemas and materialise them for
  // further analysis. We do this so that we don't end up rebasing the same
  // schemas over and over again depending on the order of analysis later on
  /////////////////////////////////////////////////////////////////////////////

  const auto schemas_path{output.path() / "schemas"};
  const auto display_schemas_path{
      std::filesystem::relative(schemas_path, output.path())};
  sourcemeta::one::BuildAdapterFilesystem adapter{output.path()};
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
            {sourcemeta::one::make_dependency(schema.second.path),
             // This target depends on the configuration file given things like
             // resolve maps and base URIs
             sourcemeta::one::make_dependency(mark_configuration_path),
             sourcemeta::one::make_dependency(mark_version_path)},
            {schema.first, resolver}, mutex, "Ingesting", schema.first,
            "materialise", adapter, output);

        // Mark the materialised schema in the resolver
        resolver.cache_path(schema.first, destination);
      },
      concurrency);

  PROFILE_END(profiling, "Ingest");

  /////////////////////////////////////////////////////////////////////////////
  // (12) Generate all the artifacts that purely depend on the schemas
  /////////////////////////////////////////////////////////////////////////////

  // Give it a generous thread stack size, otherwise we might overflow
  // the small-by-default thread stack with Blaze
  constexpr auto THREAD_STACK_SIZE{8 * 1024 * 1024};

  const auto explorer_path{output.path() / "explorer"};
  const auto display_explorer_path{
      std::filesystem::relative(explorer_path, output.path())};
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
            {sourcemeta::one::make_dependency(base_path / "schema.metapack"),
             sourcemeta::one::make_dependency(mark_version_path)},
            resolver, mutex, "Analysing", schema.first, "positions", adapter,
            output);

        DISPATCH<sourcemeta::one::GENERATE_FRAME_LOCATIONS>(
            base_path / "locations.metapack",
            {sourcemeta::one::make_dependency(base_path / "schema.metapack"),
             sourcemeta::one::make_dependency(mark_version_path)},
            resolver, mutex, "Analysing", schema.first, "locations", adapter,
            output);

        DISPATCH<sourcemeta::one::GENERATE_DEPENDENCIES>(
            base_path / "dependencies.metapack",
            {sourcemeta::one::make_dependency(base_path / "schema.metapack"),
             sourcemeta::one::make_dependency(mark_version_path)},
            resolver, mutex, "Analysing", schema.first, "dependencies", adapter,
            output);

        DISPATCH<sourcemeta::one::GENERATE_STATS>(
            base_path / "stats.metapack",
            {sourcemeta::one::make_dependency(base_path / "schema.metapack"),
             sourcemeta::one::make_dependency(mark_version_path)},
            resolver, mutex, "Analysing", schema.first, "stats", adapter,
            output);

        DISPATCH<sourcemeta::one::GENERATE_HEALTH>(
            base_path / "health.metapack",
            {sourcemeta::one::make_dependency(base_path / "schema.metapack"),
             sourcemeta::one::make_dependency(base_path /
                                              "dependencies.metapack"),
             sourcemeta::one::make_dependency(mark_version_path)},
            {std::ref(resolver), std::cref(schema.second.collection.get())},
            mutex, "Analysing", schema.first, "health", adapter, output);

        DISPATCH<sourcemeta::one::GENERATE_BUNDLE>(
            base_path / "bundle.metapack",
            {sourcemeta::one::make_dependency(base_path / "schema.metapack"),
             sourcemeta::one::make_dependency(base_path /
                                              "dependencies.metapack"),
             sourcemeta::one::make_dependency(mark_version_path)},
            resolver, mutex, "Analysing", schema.first, "bundle", adapter,
            output);

        DISPATCH<sourcemeta::one::GENERATE_EDITOR>(
            base_path / "editor.metapack",
            {sourcemeta::one::make_dependency(base_path / "bundle.metapack"),
             sourcemeta::one::make_dependency(mark_version_path)},
            resolver, mutex, "Analysing", schema.first, "editor", adapter,
            output);

        if (attribute_not_disabled(schema.second.collection.get(),
                                   "x-sourcemeta-one:evaluate")) {
          DISPATCH<sourcemeta::one::GENERATE_BLAZE_TEMPLATE>(
              base_path / "blaze-exhaustive.metapack",
              {sourcemeta::one::make_dependency(base_path / "bundle.metapack"),
               sourcemeta::one::make_dependency(mark_version_path)},
              sourcemeta::blaze::Mode::Exhaustive, mutex, "Analysing",
              schema.first, "blaze-exhaustive", adapter, output);
          DISPATCH<sourcemeta::one::GENERATE_BLAZE_TEMPLATE>(
              base_path / "blaze-fast.metapack",
              {sourcemeta::one::make_dependency(base_path / "bundle.metapack"),
               sourcemeta::one::make_dependency(mark_version_path)},
              sourcemeta::blaze::Mode::FastValidation, mutex, "Analysing",
              schema.first, "blaze-fast", adapter, output);
        }

        DISPATCH<sourcemeta::one::GENERATE_EXPLORER_SCHEMA_METADATA>(
            explorer_path / schema.second.relative_path / SENTINEL /
                "schema.metapack",
            {sourcemeta::one::make_dependency(base_path / "schema.metapack"),
             sourcemeta::one::make_dependency(base_path / "health.metapack"),
             sourcemeta::one::make_dependency(base_path /
                                              "dependencies.metapack"),
             // As this target reads the alert from the configuration file
             sourcemeta::one::make_dependency(mark_configuration_path),
             sourcemeta::one::make_dependency(mark_version_path)},
            {resolver, schema.second.collection.get(),
             schema.second.relative_path},
            mutex, "Analysing", schema.first, "metadata", adapter, output);
      },
      concurrency, THREAD_STACK_SIZE);

  PROFILE_END(profiling, "Analyse");

  /////////////////////////////////////////////////////////////////////////////
  // (13) Scan the generated files so far to prepare for more complex targets
  /////////////////////////////////////////////////////////////////////////////

  // This is a pretty fast step that will be useful for us to properly declare
  // dependencies for HTML and navigational targets

  print_progress(mutex, concurrency, "Reviewing", display_schemas_path.string(),
                 1, 3);
  std::vector<std::filesystem::path> directories;
  // The top-level is itself a directory
  directories.emplace_back(schemas_path);
  sourcemeta::one::BuildDependencies<std::filesystem::path> summaries;
  sourcemeta::one::BuildDependencies<std::filesystem::path> dependencies;
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
        summaries.emplace_back(sourcemeta::one::BuildDependencyKind::Static,
                               explorer_path / std::filesystem::relative(
                                                   entry.path(), schemas_path));
        dependencies.emplace_back(sourcemeta::one::BuildDependencyKind::Static,
                                  entry.path().parent_path() /
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

  print_progress(mutex, concurrency, "Reviewing", display_schemas_path.string(),
                 2, 3);

  const auto dependency_tree_path{output.path() / "dependency-tree.metapack"};

  // Read the old dependency tree before DISPATCH overwrites it, so we can
  // diff against the new tree to find which schemas' dependents changed
  std::optional<sourcemeta::core::JSON> old_dependency_tree;
  if (std::filesystem::exists(dependency_tree_path)) {
    old_dependency_tree = sourcemeta::one::read_json(dependency_tree_path);
  }

  const auto dependency_tree_rebuilt{
      DISPATCH<sourcemeta::one::GENERATE_DEPENDENCY_TREE>(
          dependency_tree_path, dependencies, resolver, mutex, "Reviewing",
          display_schemas_path.string(), "dependencies", adapter, output)};

  // Determine which schemas' dependents actually changed by diffing
  // the old and new dependency trees per key. We keep new_dependency_tree
  // alive so affected_dependents can hold string_views into its keys.
  std::optional<sourcemeta::core::JSON> new_dependency_tree;
  std::unordered_set<std::string_view> affected_dependents;
  if (dependency_tree_rebuilt) {
    new_dependency_tree = sourcemeta::one::read_json(dependency_tree_path);
    if (old_dependency_tree.has_value()) {
      for (const auto &entry : new_dependency_tree->as_object()) {
        const auto *old_value{old_dependency_tree->try_at(entry.first)};
        if (!old_value || *old_value != entry.second) {
          affected_dependents.insert(entry.first);
        }
      }

      for (const auto &entry : old_dependency_tree->as_object()) {
        if (!new_dependency_tree->defines(entry.first)) {
          affected_dependents.insert(entry.first);
        }
      }
    } else {
      for (const auto &entry : new_dependency_tree->as_object()) {
        affected_dependents.insert(entry.first);
      }
    }
  }

  struct ReworkEntry {
    std::reference_wrapper<const sourcemeta::core::JSON::String> uri;
    std::filesystem::path dependents_path;
  };

  std::vector<ReworkEntry> rework_entries;
  for (const auto &schema : resolver) {
    auto dependents_path{schemas_path / schema.second.relative_path / SENTINEL /
                         "dependents.metapack"};
    const auto dependents_deps_path{dependents_path.string() + ".deps"};
    if (affected_dependents.contains(schema.first) ||
        !std::filesystem::exists(dependents_path) ||
        !std::filesystem::exists(dependents_deps_path)) {
      rework_entries.push_back(
          {std::cref(schema.first), std::move(dependents_path)});
    } else {
      output.track(dependents_path);
      output.track(dependents_path.string() + ".deps");
    }
  }

  print_progress(mutex, concurrency, "Reviewing", display_schemas_path.string(),
                 3, 3);

  PROFILE_END(profiling, "Review");

  /////////////////////////////////////////////////////////////////////////////
  // (14) A further pass on the schemas after review
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::core::parallel_for_each(
      rework_entries.begin(), rework_entries.end(),
      [&output, &rework_entries, &mutex, &adapter, &dependency_tree_path](
          const auto &entry, const auto threads, const auto cursor) {
        print_progress(mutex, threads, "Reworking", entry.uri.get(), cursor,
                       rework_entries.size());
        DISPATCH<sourcemeta::one::GENERATE_DEPENDENTS>(
            entry.dependents_path,
            {sourcemeta::one::make_dependency(dependency_tree_path)},
            entry.uri.get(), mutex, "Reworking", entry.uri.get(), "dependents",
            adapter, output);
      },
      concurrency, THREAD_STACK_SIZE);

  PROFILE_END(profiling, "Rework");

  /////////////////////////////////////////////////////////////////////////////
  // (15) Generate the JSON-based explorer
  /////////////////////////////////////////////////////////////////////////////

  print_progress(mutex, concurrency, "Producing",
                 display_explorer_path.string(), 0, 100);
  DISPATCH<sourcemeta::one::GENERATE_EXPLORER_SEARCH_INDEX>(
      explorer_path / SENTINEL / "search.metapack", summaries, nullptr, mutex,
      "Producing", display_explorer_path.string(), "search", adapter, output);

  // Directory generation depends on the configuration for metadata
  summaries.emplace_back(sourcemeta::one::BuildDependencyKind::Static,
                         mark_configuration_path);
  summaries.emplace_back(sourcemeta::one::BuildDependencyKind::Static,
                         mark_version_path);
  // Note that we can't parallelise this loop, as we need to do it bottom-up
  for (std::size_t cursor = 0; cursor < directories.size(); cursor++) {
    const auto &entry{directories[cursor]};
    const auto relative_path{std::filesystem::relative(entry, schemas_path)};
    print_progress(mutex, 1, "Producing", relative_path.string(), cursor + 1,
                   directories.size());
    const auto destination{
        (explorer_path / relative_path / SENTINEL / "directory.metapack")
            .lexically_normal()};
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

  PROFILE_END(profiling, "Produce");

  /////////////////////////////////////////////////////////////////////////////
  // (16) Generate the HTML web interface
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
                {sourcemeta::one::make_dependency(explorer_path / SENTINEL /
                                                  "directory.metapack"),
                 // We rely on the configuration for site metadata
                 sourcemeta::one::make_dependency(mark_configuration_path),
                 sourcemeta::one::make_dependency(mark_version_path)},
                configuration, mutex, "Rendering", relative_path.string(),
                "index", adapter, output);
            DISPATCH<sourcemeta::one::GENERATE_WEB_NOT_FOUND>(
                explorer_path / SENTINEL / "404.metapack",
                {// We rely on the configuration for site metadata
                 sourcemeta::one::make_dependency(mark_configuration_path),
                 sourcemeta::one::make_dependency(mark_version_path)},
                configuration, mutex, "Rendering", relative_path.string(),
                "not-found", adapter, output);
          } else {
            DISPATCH<sourcemeta::one::GENERATE_WEB_DIRECTORY>(
                explorer_path / relative_path / SENTINEL /
                    "directory-html.metapack",
                {sourcemeta::one::make_dependency(explorer_path /
                                                  relative_path / SENTINEL /
                                                  "directory.metapack"),
                 // We rely on the configuration for site metadata
                 sourcemeta::one::make_dependency(mark_configuration_path),
                 sourcemeta::one::make_dependency(mark_version_path)},
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
              std::filesystem::relative(entry.second, explorer_path)
                  .parent_path()
                  .parent_path()};
          print_progress(mutex, threads, "Rendering", relative_path.string(),
                         cursor + directories.size(),
                         summaries.size() + directories.size());
          const auto schema_path{schemas_path / relative_path / SENTINEL};
          DISPATCH<sourcemeta::one::GENERATE_WEB_SCHEMA>(
              entry.second.parent_path() / "schema-html.metapack",
              {sourcemeta::one::make_dependency(entry.second),
               sourcemeta::one::make_dependency(schema_path /
                                                "dependencies.metapack"),
               sourcemeta::one::make_dependency(schema_path /
                                                "health.metapack"),
               sourcemeta::one::make_dependency(schema_path /
                                                "dependents.metapack"),
               // We rely on the configuration for site metadata
               sourcemeta::one::make_dependency(mark_configuration_path),
               sourcemeta::one::make_dependency(mark_version_path)},
              configuration, mutex, "Rendering", relative_path.string(),
              "schema", adapter, output);
        },
        concurrency);
  }

  PROFILE_END(profiling, "Render");

  /////////////////////////////////////////////////////////////////////////////
  // (17) Generate the pre computed routes
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
  router.add("/self/v1/health", sourcemeta::one::HANDLER_SELF_V1_HEALTH);
  router.add("/self/v1/api/{+any}",
             sourcemeta::one::HANDLER_SELF_V1_API_DEFAULT);

  if (configuration.html.has_value()) {
    router.add("/self/static/{+path}", sourcemeta::one::HANDLER_SELF_STATIC);
  }

  const auto routes_path{output.path() / "routes.bin"};
  const auto display_routes_path{
      std::filesystem::relative(routes_path, output.path())};
  DISPATCH<sourcemeta::one::GENERATE_URITEMPLATE_ROUTES>(
      routes_path,
      {sourcemeta::one::make_dependency(mark_configuration_path),
       sourcemeta::one::make_dependency(mark_version_path)},
      router, mutex, "Producing", display_routes_path.string(), "routes",
      adapter, output);

  PROFILE_END(profiling, "Routes");

  /////////////////////////////////////////////////////////////////////////////
  // Finish generation
  /////////////////////////////////////////////////////////////////////////////

  // TODO: Print the size of the output directory here

  output.remove_unknown_files();

  PROFILE_END(profiling, "Cleanup");

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
