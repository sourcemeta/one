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

#include <algorithm>   // std::sort
#include <array>       // std::array
#include <atomic>      // std::atomic
#include <cassert>     // assert
#include <chrono>      // std::chrono
#include <cstdint>     // std::uint8_t
#include <cstdlib>     // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>   // std::exception
#include <filesystem>  // std::filesystem
#include <functional>  // std::reference_wrapper, std::cref
#include <iomanip>     // std::setw, std::setfill
#include <iostream>    // std::cerr, std::cout
#include <mutex>       // std::mutex, std::lock_guard
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

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

using BuildHandlerFunction = void (*)(
    const sourcemeta::one::BuildPlan::Action &,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &, const sourcemeta::core::JSON &);

static constexpr std::array<BuildHandlerFunction, 24> HANDLERS{{
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
    &sourcemeta::one::GENERATE_DEPENDENCY_TREE::handler,
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

  sourcemeta::one::BuildState entries;
  const auto state_path{canonical_output / "state.bin"};
  entries.load(state_path);

  // Only trust on-disk files when the state was loaded successfully,
  // otherwise the entries map and the on-disk artefacts are out of sync

  std::string current_version;
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

  // Determine comment
  const std::string comment{app.contains("comment")
                                ? std::string{app.at("comment").at(0)}
                                : std::string{}};

  // Determine build type
  const auto build_type{configuration.html.has_value()
                            ? sourcemeta::one::BuildPlan::Type::Full
                            : sourcemeta::one::BuildPlan::Type::Headless};

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

      const auto extension{entry.path().extension()};
      // TODO: Allow the configuration file to override this
      if (extension != ".yaml" && extension != ".yml" && extension != ".json") {
        continue;
      }

      std::cerr << "Detecting: " << entry.path().string() << " (#"
                << detected_schemas.size() + 1 << ")\n";
      detected_schemas.push_back({pair.first, std::cref(*collection),
                                  entry.path(), entry.last_write_time()});
    }
  };

  PROFILE_END(profiling, "Detect");

  /////////////////////////////////////////////////////////////////////////////
  // (7) Resolve all detected schemas in parallel
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::one::Resolver resolver;
  resolver.reserve(detected_schemas.size());

  // Phase 1: populate resolver from cache for unchanged source files.

  // Skip the cache entirely if the configuration changed, as cached
  // identifiers and paths may no longer be valid
  const auto resolver_cache_valid{raw_configuration == current_configuration &&
                                  current_version ==
                                      sourcemeta::one::version()};
  std::vector<std::reference_wrapper<const DetectedSchema>> uncached_schemas;
  for (const auto &detected : detected_schemas) {
    const auto *cached{
        resolver_cache_valid
            ? entries.resolve(detected.path.native(), detected.mtime)
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
              .cache_path = std::nullopt,
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
      [&configuration, &resolver, &mutex, &entries, &uncached_schemas,
       &app](const auto &detected_ref, const auto threads, const auto cursor) {
        const auto &detected{detected_ref.get()};
        print_progress(mutex, threads, "Resolving",
                       detected.path.filename().string(), cursor,
                       uncached_schemas.size());
        const auto result{resolver.add(
            configuration.url, detected.collection_relative_path,
            detected.collection.get(), detected.path, detected.mtime)};

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
          std::cerr << result.second.get().original_identifier << " => "
                    << result.first.get() << "\n";
        }
      },
      concurrency);

  PROFILE_END(profiling, "Resolve");

  /////////////////////////////////////////////////////////////////////////////
  // (8) Build schema info map and compute the delta plan
  /////////////////////////////////////////////////////////////////////////////

  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  auto plan{sourcemeta::one::delta(build_type, entries, canonical_output,
                                   resolver.data(), sourcemeta::one::version(),
                                   current_version, comment, raw_configuration,
                                   current_configuration, changed, removed)};

  PROFILE_END(profiling, "Delta");

  /////////////////////////////////////////////////////////////////////////////
  // (9) Execute the plan wave by wave
  /////////////////////////////////////////////////////////////////////////////

  // Give it a generous thread stack size, otherwise we might overflow
  // the small-by-default thread stack with Blaze
  constexpr auto THREAD_STACK_SIZE{8 * 1024 * 1024};

  std::atomic<std::size_t> progress_counter{0};
  std::mutex entries_mutex;

  for (auto &wave : plan.waves) {
    // TODO: Maybe we can optimise this for waves of a single action
    // by avoiding the creation of a whole thread plus the locks?
    sourcemeta::core::parallel_for_each(
        wave.begin(), wave.end(),
        [&](auto &action, const auto threads, const auto) {
          const auto current{
              progress_counter.fetch_add(1, std::memory_order_relaxed) + 1};
          const std::string_view destination_view{action.destination.native()};
          print_progress(
              mutex, threads,
              action.type == sourcemeta::one::BuildPlan::Action::Type::Remove
                  ? "Disposing"
                  : "Producing",
              destination_view.substr(canonical_output.native().size() + 1),
              current, plan.size);

          if (action.type == sourcemeta::one::BuildPlan::Action::Type::Remove) {
            std::filesystem::remove_all(action.destination);
            std::lock_guard<std::mutex> lock{entries_mutex};
            entries.forget(action.destination.native());
            return;
          }

          const auto handler{HANDLERS[static_cast<std::uint8_t>(action.type)]};
          assert(handler);
          handler(
              action,
              [&action](const auto &path) {
                action.dependencies.emplace_back(path);
              },
              resolver, configuration, raw_configuration);

          {
            std::lock_guard<std::mutex> lock{entries_mutex};
            entries.commit(action.destination, std::move(action.dependencies));
          }
        },
        concurrency, THREAD_STACK_SIZE);
  }

  PROFILE_END(profiling, "Build");

  /////////////////////////////////////////////////////////////////////////////
  // (9) Save state
  /////////////////////////////////////////////////////////////////////////////

  entries.save(state_path);

  PROFILE_END(profiling, "Cleanup");

  /////////////////////////////////////////////////////////////////////////////
  // (10) Compute metrics
  /////////////////////////////////////////////////////////////////////////////

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
