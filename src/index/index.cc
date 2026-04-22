#include <sourcemeta/blaze/alterschema.h>

#include <sourcemeta/core/error.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/options.h>
#include <sourcemeta/core/parallel.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>
#include <sourcemeta/core/yaml.h>

#include <sourcemeta/one/build.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/shared.h>
#include <sourcemeta/one/web.h>

#include "explorer.h"
#include "generators.h"

#include <algorithm>     // std::ranges::sort
#include <array>         // std::array
#include <atomic>        // std::atomic
#include <cassert>       // assert
#include <chrono>        // std::chrono
#include <cstdint>       // std::uint8_t
#include <cstdlib>       // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>     // std::exception
#include <filesystem>    // std::filesystem
#include <functional>    // std::reference_wrapper, std::cref
#include <mutex>         // std::mutex, std::lock_guard
#include <print>         // std::print, std::println
#include <sstream>       // std::ostringstream
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

using BuildHandlerFunction = auto (*)(
    const sourcemeta::one::BuildState &,
    const sourcemeta::one::BuildPlan::Action &,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &, const sourcemeta::core::JSON &)
    -> void;

static constexpr std::array<BuildHandlerFunction, 23> HANDLERS{{
    &sourcemeta::one::GENERATE_MATERIALISED_SCHEMA::handler,
    &sourcemeta::one::GENERATE_POINTER_POSITIONS::handler,
    &sourcemeta::one::GENERATE_FRAME_LOCATIONS::handler,
    &sourcemeta::one::GENERATE_DEPENDENCIES::handler,
    &sourcemeta::one::GENERATE_STATS::handler,
    &sourcemeta::one::GENERATE_HEALTH::handler,
    &sourcemeta::one::GENERATE_BUNDLE::handler,
    &sourcemeta::one::GENERATE_EDITOR::handler,
    &sourcemeta::one::GENERATE_BLAZE_TEMPLATE_EXHAUSTIVE::handler,
    &sourcemeta::one::GENERATE_BLAZE_TEMPLATE_FAST::handler,
    &sourcemeta::one::GENERATE_EXPLORER_SCHEMA_METADATA::handler,
    &sourcemeta::one::GENERATE_DEPENDENTS::handler,
    &sourcemeta::one::GENERATE_EXPLORER_SEARCH_INDEX::handler,
    &sourcemeta::one::GENERATE_EXPLORER_DIRECTORY_LIST::handler,
    &sourcemeta::one::GENERATE_WEB_INDEX::handler,
    &sourcemeta::one::GENERATE_WEB_NOT_FOUND::handler,
    &sourcemeta::one::GENERATE_WEB_DIRECTORY::handler,
    &sourcemeta::one::GENERATE_WEB_SCHEMA::handler,
    &sourcemeta::one::GENERATE_COMMENT::handler,
    &sourcemeta::one::GENERATE_CONFIGURATION::handler,
    &sourcemeta::one::GENERATE_VERSION::handler,
    &sourcemeta::one::GENERATE_URITEMPLATE_ROUTES::handler,
    nullptr,
}};

static auto parse_numeric_option(const sourcemeta::core::Options &app,
                                 const std::string_view option)
    -> unsigned long long {
  const auto &raw{app.at(option).front()};
  try {
    return std::stoull(raw.data());
  } catch (const std::invalid_argument &) {
    throw sourcemeta::one::OptionInvalidNumericValueError(std::string{option},
                                                          std::string{raw});
  } catch (const std::out_of_range &) {
    throw sourcemeta::one::OptionInvalidNumericValueError(std::string{option},
                                                          std::string{raw});
  }
}

static auto print_progress(std::mutex &mutex, const std::size_t threads,
                           const std::string_view title,
                           const std::string_view prefix,
                           const std::size_t current, const std::size_t total)
    -> void {
  const auto percentage{current * 100 / total};
  std::lock_guard<std::mutex> lock{mutex};
  std::print(stderr, "({:3}%) {}: {} [{}/{}]\n", static_cast<int>(percentage),
             title, prefix, std::this_thread::get_id(), threads);
}

static auto execute_plan(std::mutex &mutex,
                         sourcemeta::one::BuildState &entries,
                         const std::filesystem::path &canonical_output,
                         sourcemeta::one::Resolver &resolver,
                         const sourcemeta::one::Configuration &configuration,
                         const sourcemeta::core::JSON &raw_configuration,
                         const std::size_t concurrency,
                         sourcemeta::one::BuildPlan &plan,
                         const std::string_view label) -> void {
  // Give it a generous thread stack size, otherwise we might overflow
  // the small-by-default thread stack with Blaze
  constexpr auto THREAD_STACK_SIZE{8 * 1024 * 1024};
  std::atomic<std::size_t> progress_counter{0};
  for (auto &wave : plan.waves) {
    sourcemeta::core::parallel_for_each(
        wave.begin(), wave.end(),
        [&](auto &action, const auto threads, const auto) {
          const auto current{
              progress_counter.fetch_add(1, std::memory_order_relaxed) + 1};
          const std::string_view destination_view{action.destination.native()};
          const auto relative_path{
              destination_view.substr(canonical_output.native().size() + 1)};

          if (action.type == sourcemeta::one::BuildPlan::Action::Type::Remove) {
            print_progress(mutex, threads, "Disposing", relative_path, current,
                           plan.size);
            std::filesystem::remove_all(action.destination);
            const auto lock{entries.take_lock()};
            entries.forget(action.destination.native());
            return;
          }

          print_progress(mutex, threads, label, relative_path, current,
                         plan.size);
          const auto handler{HANDLERS[static_cast<std::uint8_t>(action.type)]};
          assert(handler);
          try {
            handler(
                entries, action,
                [&action](const auto &path) {
                  action.dependencies.emplace_back(path);
                },
                resolver, configuration, raw_configuration);
          } catch (const sourcemeta::core::SchemaResolutionError &error) {
            const auto *entry{
                action.data.empty() ? nullptr : &resolver.entry(action.data)};
            if (entry) {
              throw sourcemeta::core::FileError<
                  sourcemeta::core::SchemaResolutionError>(
                  entry->path, error.identifier(), error.what());
            }

            throw;
          } catch (const sourcemeta::core::SchemaReferenceError &error) {
            const auto *entry{
                action.data.empty() ? nullptr : &resolver.entry(action.data)};
            if (entry) {
              throw sourcemeta::core::FileError<
                  sourcemeta::core::SchemaReferenceError>(
                  entry->path, error.identifier(), error.location(),
                  error.what());
            }

            throw;
          } catch (const sourcemeta::core::SchemaReferenceObjectResourceError
                       &error) {
            const auto *entry{
                action.data.empty() ? nullptr : &resolver.entry(action.data)};
            if (entry) {
              throw sourcemeta::core::FileError<
                  sourcemeta::core::SchemaReferenceObjectResourceError>(
                  entry->path, error.identifier());
            }

            throw;
          } catch (const sourcemeta::core::SchemaVocabularyError &error) {
            const auto *entry{
                action.data.empty() ? nullptr : &resolver.entry(action.data)};
            if (entry) {
              throw sourcemeta::core::FileError<
                  sourcemeta::core::SchemaVocabularyError>(
                  entry->path, error.uri(), error.what());
            }

            throw;
          }

          const auto lock{entries.take_lock()};
          entries.commit(action.destination, std::move(action.dependencies));
        },
        concurrency, THREAD_STACK_SIZE);
  }
}

constexpr std::string_view USAGE_DETAILS{R"EOF(
Global Options:

   --help, -h                     Print this help information and quit
   --configuration, -g            Print the resolved configuration and quit
   --resolve-schema, -r <uri>     Resolve a URI to its schema path and quit
   --skip-banner, -s              Skip the startup banner

Index Options:

   --verbose, -v                  Enable verbose output
   --concurrency, -c <number>     Set the number of concurrent threads
   --profile, -p                  Output information about slowest steps
   --time, -t                     Output high-level timing information
   --comment, -m <text>           Attach a comment to the build

Advanced Options:

   --maximum-direct-directory-entries <number>

     Set the maximum number of direct entries in a directory listing

For more documentation, visit https://one.sourcemeta.com
)EOF"};

static auto index_main(const std::string_view &program,
                       const sourcemeta::core::Options &app) -> int {
  if (app.contains("help")) {
    if (!app.contains("skip-banner")) {
      std::println("Sourcemeta One {} v{}\n", sourcemeta::one::edition(),
                   sourcemeta::one::version());
    }

    std::println("Usage: {} <one.json> <path/to/output/directory>",
                 std::filesystem::path{program}.filename().string());
    std::print("{}", USAGE_DETAILS);
    return EXIT_SUCCESS;
  }

  if (app.positional().size() != 2) {
    if (!app.contains("skip-banner")) {
      std::println("Sourcemeta One {} v{}\n", sourcemeta::one::edition(),
                   sourcemeta::one::version());
    }

    std::println("Usage: {} <one.json> <path/to/output/directory>",
                 std::filesystem::path{program}.filename().string());
    std::print("{}", USAGE_DETAILS);
    return EXIT_FAILURE;
  }

  if (!app.contains("skip-banner")) {
    std::println(stderr, "Sourcemeta One {} v{}", sourcemeta::one::edition(),
                 sourcemeta::one::version());
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

  std::println(stderr, "Writing output to: {}", output_path.string());

  /////////////////////////////////////////////////////////////////////////////
  // (2) Process the configuration file
  /////////////////////////////////////////////////////////////////////////////

  const auto configuration_path{
      std::filesystem::canonical(app.positional().at(0))};
  std::println(stderr, "Using configuration: {}", configuration_path.string());
  std::unordered_set<std::string> configuration_files;
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, SOURCEMETA_ONE_COLLECTIONS, configuration_files)};

  if (app.contains("configuration")) {
    std::ostringstream configuration_output;
    sourcemeta::core::prettify(raw_configuration, configuration_output);
    std::println("{}", configuration_output.str());
    return EXIT_SUCCESS;
  }

  auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  /////////////////////////////////////////////////////////////////////////////
  // (3) Resolve a URI to a schema filesystem path
  /////////////////////////////////////////////////////////////////////////////

  if (app.contains("resolve-schema")) {
    const auto &resolve_schema_value{app.at("resolve-schema").front()};
    sourcemeta::core::URI input_uri{""};
    try {
      input_uri = sourcemeta::core::URI{std::string{resolve_schema_value}};
    } catch (const sourcemeta::core::URIParseError &) {
      throw sourcemeta::one::OptionInvalidURIValueError(
          "resolve-schema", std::string{resolve_schema_value});
    }

    std::println(stderr, "Resolving schema for URI: {}", input_uri.recompose());
    const auto result{configuration.resolve_schema(input_uri)};
    if (result.has_value()) {
      std::println("{}", result.value().string());
      return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
  }

  /////////////////////////////////////////////////////////////////////////////
  // (4) Prepare the output directory and load previous state
  /////////////////////////////////////////////////////////////////////////////

  std::filesystem::create_directories(output_path);
  const auto canonical_output{std::filesystem::canonical(output_path)};

  sourcemeta::one::BuildState entries;
  const auto state_path{canonical_output / "state.bin"};
  entries.load(state_path);

  // Only trust on-disk files when the state was loaded successfully,
  // otherwise the entries map and the on-disk artefacts are out of sync

  std::string current_version;
  const auto this_version{sourcemeta::one::version()};
  const auto version_path{canonical_output / "version.json"};
  if (!entries.empty() && std::filesystem::exists(version_path)) {
    const auto version_json{sourcemeta::core::read_json(version_path)};
    current_version = version_json.to_string();
  }

  auto current_configuration{sourcemeta::core::JSON{nullptr}};
  const auto configuration_json_path{canonical_output / "configuration.json"};
  if (!entries.empty() && std::filesystem::exists(configuration_json_path)) {
    current_configuration =
        sourcemeta::core::read_json(configuration_json_path);
  }

  const std::string comment{app.contains("comment")
                                ? std::string{app.at("comment").at(0)}
                                : std::string{}};
  const auto build_type{configuration.html.has_value()
                            ? sourcemeta::one::BuildPlan::Type::Full
                            : sourcemeta::one::BuildPlan::Type::Headless};

  // Mainly to not screw up the logs
  std::mutex mutex;
  const auto concurrency{app.contains("concurrency")
                             ? parse_numeric_option(app, "concurrency")
                             : std::thread::hardware_concurrency()};

  PROFILE_END(profiling, "Startup");

  /////////////////////////////////////////////////////////////////////////////
  // (5) First pass to locate all of the schemas we will be indexing
  // NOTE: No files are generated. We only want to know what's out there
  /////////////////////////////////////////////////////////////////////////////

  struct DetectedSchema {
    std::filesystem::path collection_relative_path;
    std::reference_wrapper<const sourcemeta::one::Configuration::Collection>
        collection;
    std::filesystem::path path;
    std::filesystem::file_time_type mtime;
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

      if (configuration_files.contains(
              std::filesystem::weakly_canonical(entry.path()).native())) {
        continue;
      }

      const auto extension{entry.path().extension()};
      // TODO: Allow the configuration file to override this
      if (extension != ".yaml" && extension != ".yml" && extension != ".json") {
        continue;
      }

      std::println(stderr, "Detecting: {} (#{})", entry.path().string(),
                   detected_schemas.size() + 1);
      detected_schemas.push_back({pair.first, std::cref(*collection),
                                  entry.path(), entry.last_write_time()});
    }
  };

  PROFILE_END(profiling, "Detect");

  /////////////////////////////////////////////////////////////////////////////
  // (6) Resolve all detected schemas in parallel
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::one::Resolver resolver{configuration.url};
  resolver.reserve(detected_schemas.size());

  // Phase 1: populate resolver from cache for unchanged source files.

  // Skip the cache entirely if the configuration changed, as cached
  // identifiers and paths may no longer be valid
  const auto incremental{raw_configuration == current_configuration &&
                         current_version == this_version};
  std::vector<std::reference_wrapper<const DetectedSchema>> uncached_schemas;
  for (const auto &detected : detected_schemas) {
    const auto *cached{
        incremental ? entries.resolve(detected.path.native(), detected.mtime)
                    : nullptr};
    if (cached != nullptr) {
      const auto &collection{detected.collection.get()};
      resolver.emplace(
          cached->new_identifier,
          sourcemeta::one::Resolver::Entry{
              .path = detected.path,
              .relative_path = cached->relative_path,
              .mtime = detected.mtime,
              .evaluate =
                  sourcemeta::one::Configuration::should_evaluate(collection),
              .cache_path = canonical_output / "schemas" /
                            cached->relative_path / "%" / "schema.metapack",
              .dialect = cached->dialect,
              .original_identifier = cached->original_identifier,
              .collection = &collection});
    } else {
      uncached_schemas.emplace_back(detected);
    }
  }

  // Phase 2: resolve uncached schemas and commit to cache
  sourcemeta::core::parallel_for_each(
      uncached_schemas.begin(), uncached_schemas.end(),
      [&resolver, &mutex, &entries, &uncached_schemas,
       &app](const auto &detected_ref, const auto threads, const auto cursor) {
        const auto &detected{detected_ref.get()};
        print_progress(mutex, threads, "Resolving",
                       detected.path.filename().string(), cursor,
                       uncached_schemas.size());
        const auto result{resolver.add(detected.collection_relative_path,
                                       detected.collection.get(), detected.path,
                                       detected.mtime)};

        {
          const auto &resolved{result.second.get()};
          std::lock_guard<std::mutex> lock{mutex};
          entries.commit(detected.path.native(),
                         sourcemeta::one::BuildState::ResolverEntry{
                             .file_mark = detected.mtime,
                             .new_identifier = std::string{result.first.get()},
                             .original_identifier =
                                 std::string{resolved.original_identifier},
                             .dialect = std::string{resolved.dialect},
                             .relative_path = resolved.relative_path.string()});
        }

        if (app.contains("verbose")) {
          std::lock_guard<std::mutex> lock{mutex};
          std::println(stderr, "{} => {}",
                       result.second.get().original_identifier,
                       result.first.get());
        }
      },
      concurrency);

  PROFILE_END(profiling, "Resolve");

  /////////////////////////////////////////////////////////////////////////////
  // (7) Run the delta plans
  /////////////////////////////////////////////////////////////////////////////

  const sourcemeta::one::BuildLimits limits{
      .maximum_direct_directory_entries =
          app.contains("maximum-direct-directory-entries")
              ? parse_numeric_option(app, "maximum-direct-directory-entries")
              : 1000};

  auto produce_plan{
      sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce, build_type,
                             entries, canonical_output, resolver.data(),
                             this_version, incremental, comment, limits)};
  PROFILE_END(profiling, "Producing (Delta)");
  execute_plan(mutex, entries, canonical_output, resolver, configuration,
               raw_configuration, concurrency, produce_plan, "Producing");
  PROFILE_END(profiling, "Producing (Build)");

  auto combine_plan{
      sourcemeta::one::delta(sourcemeta::one::BuildPhase::Combine, build_type,
                             entries, canonical_output, resolver.data(),
                             this_version, incremental, comment, limits)};
  PROFILE_END(profiling, "Combining (Delta)");
  execute_plan(mutex, entries, canonical_output, resolver, configuration,
               raw_configuration, concurrency, combine_plan, "Combining");
  PROFILE_END(profiling, "Combining (Build)");

  /////////////////////////////////////////////////////////////////////////////
  // (8) Save state and profile
  /////////////////////////////////////////////////////////////////////////////

  entries.save(state_path);

  PROFILE_END(profiling, "Cleanup");

  /////////////////////////////////////////////////////////////////////////////
  // (9) Output metrics
  /////////////////////////////////////////////////////////////////////////////

  // TODO: Add a test for this
  if (app.contains("profile")) {
    std::println(stderr, "Profiling...");
    std::vector<std::pair<std::filesystem::path, std::chrono::milliseconds>>
        durations;
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator{canonical_output}) {
      if (entry.is_regular_file() && entry.path().extension() == ".metapack") {
        try {
          sourcemeta::core::FileView file_view{entry.path()};
          const auto file_info_option{
              sourcemeta::one::metapack_info(file_view)};
          assert(file_info_option.has_value());
          const auto &file_info{file_info_option.value()};
          durations.emplace_back(entry.path(), file_info.duration);
        } catch (...) {
          std::println(stderr, "Could not profile file: {}",
                       entry.path().string());
          throw;
        }
      }
    }

    std::ranges::sort(durations, [](const auto &left, const auto &right) {
      return left.second > right.second;
    });

    constexpr std::size_t PROFILE_ENTRIES_MAXIMUM{25};
    for (std::size_t index = 0;
         index < std::min(durations.size(),
                          static_cast<std::size_t>(PROFILE_ENTRIES_MAXIMUM));
         index++) {
      std::println(
          "{}ms {}", durations[index].second.count(),
          std::filesystem::relative(durations[index].first, canonical_output)
              .string());
    }
  }

  PROFILE_END(profiling, "Profile");

  if (app.contains("time")) {
    for (const auto &entry : profiling.first) {
      std::println("{}ms {}", entry.second.count(), entry.first);
    }
  }

  return EXIT_SUCCESS;
}

auto main(int argc, char *argv[]) noexcept -> int {
  try {
    sourcemeta::core::Options app;
    app.flag("help", {"h"});
    app.option("concurrency", {"c"});
    app.flag("verbose", {"v"});
    app.flag("profile", {"p"});
    app.flag("time", {"t"});
    app.flag("configuration", {"g"});
    app.option("resolve-schema", {"r"});
    app.flag("skip-banner", {"s"});
    app.option("comment", {"m"});
    app.option("maximum-direct-directory-entries", {});
    app.parse(argc, argv);
    const std::string_view program{argv[0]};

    return index_main(program, app);
  } catch (const sourcemeta::one::ConfigurationCyclicReferenceError &error) {
    std::print(stderr,
               "error: {}\n  from path {}\n  at location \"{}\"\n"
               "  to path {}\n",
               error.what(), error.from().string(),
               sourcemeta::core::to_string(error.location()),
               error.target().string());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::ConfigurationReadError &error) {
    std::print(stderr,
               "error: {}\n  from path {}\n  at location \"{}\"\n"
               "  to path {}\n",
               error.what(), error.from().string(),
               sourcemeta::core::to_string(error.location()),
               error.target().string());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::ConfigurationUnknownBuiltInCollectionError
               &error) {
    std::print(stderr,
               "error: {}\n  from path {}\n  at location \"{}\"\n"
               "  to identifier {}\n",
               error.what(), error.from().string(),
               sourcemeta::core::to_string(error.location()),
               error.identifier());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::OptionInvalidNumericValueError &error) {
    std::print(stderr, "error: {}\n  at option {}\n  with value {}\n",
               error.what(), error.option(), error.value());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::OptionInvalidURIValueError &error) {
    std::print(stderr, "error: {}\n  at option {}\n  with value {}\n",
               error.what(), error.option(), error.value());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::OptionsUnexpectedValueFlagError &error) {
    std::print(stderr, "error: {}\n  at option {}\n", error.what(),
               error.option());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::OptionsMissingOptionValueError &error) {
    std::print(stderr, "error: {}\n  at option {}\n", error.what(),
               error.option());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::OptionsUnknownOptionError &error) {
    std::print(stderr, "error: {}\n  at option {}\n", error.what(),
               error.option());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    std::print(stderr, "error: {}\n  at path {}\n{}", error.what(),
               error.path().string(), error.stacktrace());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::MetaschemaError &error) {
    std::print(stderr, "error: {}\n{}", error.what(), error.stacktrace());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::CustomRuleError &error) {
    std::print(stderr, "error: {}\n  at path {}\n", error.what(),
               error.path().string());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::FileError<
           sourcemeta::blaze::SchemaRuleInvalidNamePatternError> &error) {
    std::print(stderr,
               "error: The schema rule name must match {}\n"
               "  at path {}\n  at name {}\n",
               error.regex(), error.path().string(), error.identifier());
    return EXIT_FAILURE;
  } catch (const sourcemeta::blaze::SchemaRuleInvalidNamePatternError &error) {
    std::print(stderr,
               "error: The schema rule name must match {}\n  at name {}\n",
               error.regex(), error.identifier());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::FileError<
           sourcemeta::blaze::SchemaRuleInvalidNameError> &error) {
    std::print(stderr, "error: {}\n  at path {}\n  at name {}\n", error.what(),
               error.path().string(), error.identifier());
    return EXIT_FAILURE;
  } catch (const sourcemeta::blaze::SchemaRuleInvalidNameError &error) {
    std::print(stderr, "error: {}\n  at name {}\n", error.what(),
               error.identifier());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::FileError<
           sourcemeta::blaze::SchemaRuleMissingNameError> &error) {
    std::print(stderr, "error: {}\n  at path {}\n", error.what(),
               error.path().string());
    return EXIT_FAILURE;
  } catch (const sourcemeta::blaze::SchemaRuleMissingNameError &error) {
    std::println(stderr, "error: {}", error.what());
    return EXIT_FAILURE;
  } catch (
      const sourcemeta::core::FileError<sourcemeta::core::SchemaVocabularyError>
          &error) {
    std::print(stderr, "error: {}\n  at vocabulary {}\n  at path {}\n",
               error.what(), error.uri(), error.path().string());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::FileError<
           sourcemeta::core::SchemaUnknownBaseDialectError> &error) {
    std::print(stderr, "error: {}\n  at path {}\n", error.what(),
               error.path().string());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    std::println(stderr, "error: {}", error.what());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::FileError<
           sourcemeta::core::SchemaUnknownDialectError> &error) {
    std::print(stderr, "error: {}\n  at path {}\n", error.what(),
               error.path().string());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::BuildTooManyDirectoryEntriesError &error) {
    std::print(stderr, "error: {}\n  at path {}\n  with count {}\n",
               error.what(), error.path().string(), error.count());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::ResolverNotASchemaError &error) {
    std::print(stderr, "error: {}\n  at path {}\n", error.what(),
               error.path().string());
    return EXIT_FAILURE;
  } catch (const sourcemeta::one::ResolverOutsideBaseError &error) {
    std::print(stderr,
               "error: {}\n  at path {}\n  at identifier {}\n  with base {}\n",
               error.what(), error.path().string(), error.uri(), error.base());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::FileError<
           sourcemeta::core::SchemaReferenceObjectResourceError> &error) {
    std::print(stderr, "error: {}\n  at path {}\n  at identifier {}\n",
               error.what(), error.path().string(), error.identifier());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::SchemaReferenceObjectResourceError &error) {
    std::print(stderr, "error: {}\n  at identifier {}\n", error.what(),
               error.identifier());
    return EXIT_FAILURE;
  } catch (
      const sourcemeta::core::FileError<sourcemeta::core::SchemaResolutionError>
          &error) {
    std::print(stderr,
               "error: {}\n  at identifier {}\n  at path {}\n\n"
               "Did you forget to register a schema with such URI?\n",
               error.what(), error.identifier(), error.path().string());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    std::print(stderr,
               "error: {}\n  at identifier {}\n\n"
               "Did you forget to register a schema with such URI?\n",
               error.what(), error.identifier());
    return EXIT_FAILURE;
  } catch (
      const sourcemeta::core::FileError<sourcemeta::core::SchemaReferenceError>
          &error) {
    std::print(stderr,
               "error: {}\n  to identifier {}\n  at path {}\n"
               "  at location \"{}\"\n",
               error.what(), error.identifier(), error.path().string(),
               sourcemeta::core::to_string(error.location()));
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::SchemaReferenceError &error) {
    std::print(stderr, "error: {}\n  to identifier {}\n  at location \"{}\"\n",
               error.what(), error.identifier(),
               sourcemeta::core::to_string(error.location()));
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::JSONFileParseError &error) {
    std::print(
        stderr, "error: {}\n  at path {}\n  at line {}\n  at column {}\n",
        error.what(), error.path().string(), error.line(), error.column());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::FileError<sourcemeta::core::YAMLParseError>
               &error) {
    std::print(
        stderr, "error: {}\n  at path {}\n  at line {}\n  at column {}\n",
        error.what(), error.path().string(), error.line(), error.column());
    return EXIT_FAILURE;
  } catch (
      const sourcemeta::core::FileError<sourcemeta::core::SchemaKeywordError>
          &error) {
    std::print(stderr,
               "error: {}\n  at path {}\n  at keyword {}\n"
               "  at value {}\n",
               error.what(), error.path().string(), error.keyword(),
               error.value());
    return EXIT_FAILURE;
  } catch (const sourcemeta::core::FileError<sourcemeta::core::SchemaFrameError>
               &error) {
    std::print(stderr, "error: {}\n  at path {}\n  at identifier {}\n",
               error.what(), error.path().string(), error.identifier());
    return EXIT_FAILURE;
  } catch (const std::filesystem::filesystem_error &error) {
    if (error.code() == std::make_error_condition(std::errc::file_exists) ||
        error.code() == std::make_error_condition(std::errc::not_a_directory)) {
      std::print(stderr,
                 "error: File already exists and is not a directory\n"
                 "  at path {}\n",
                 error.path1().string());
    } else if (error.code() == std::errc::no_such_file_or_directory) {
      std::print(stderr,
                 "error: Could not locate the requested file\n  at path {}\n",
                 error.path1().string());
    } else {
      std::println(stderr, "{}", error.what());
    }

    return EXIT_FAILURE;
  } catch (const std::exception &error) {
    std::println(stderr, "unexpected error: {}", error.what());
    return EXIT_FAILURE;
  }
}
