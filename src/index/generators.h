#ifndef SOURCEMETA_ONE_INDEX_GENERATORS_H_
#define SOURCEMETA_ONE_INDEX_GENERATORS_H_

#include "error.h"

#include <sourcemeta/one/build.h>
#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/editorschema.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/configuration.h>
#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/linter.h>

#if defined(SOURCEMETA_ONE_ENTERPRISE)
#include <sourcemeta/one/enterprise_index.h>
#endif

#include <cassert>       // assert
#include <filesystem>    // std::filesystem
#include <fstream>       // std::ofstream
#include <memory>        // std::unique_ptr
#include <mutex>         // std::mutex, std::lock_guard
#include <queue>         // std::queue
#include <set>           // std::set
#include <sstream>       // std::ostringstream
#include <unordered_map> // std::unordered_map
#include <utility>       // std::move, std::pair
#include <variant>       // std::visit

namespace sourcemeta::one {

struct GENERATE_VERSION {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    std::filesystem::create_directories(action.destination.parent_path());
    std::ofstream stream{action.destination};
    assert(!stream.fail());
    sourcemeta::core::stringify(
        sourcemeta::core::JSON{std::string{action.data}}, stream);
    return true;
  }
};

struct GENERATE_COMMENT {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    std::filesystem::create_directories(action.destination.parent_path());
    std::ofstream stream{action.destination};
    assert(!stream.fail());
    sourcemeta::core::stringify(
        sourcemeta::core::JSON{std::string{action.data}}, stream);
    return true;
  }
};

struct GENERATE_CONFIGURATION {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &raw_configuration) -> bool {
    std::filesystem::create_directories(action.destination.parent_path());
    std::ofstream stream{action.destination};
    assert(!stream.fail());
    sourcemeta::core::stringify(raw_configuration, stream);
    return true;
  }
};

struct GENERATE_MATERIALISED_SCHEMA {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    auto schema{resolver(action.data)};
    assert(schema.has_value());
    const auto dialect_identifier{sourcemeta::core::dialect(schema.value())};
    assert(!dialect_identifier.empty());
    const auto metaschema{resolver(dialect_identifier)};
    assert(metaschema.has_value());

    // Validate the schemas against their meta-schemas
    sourcemeta::blaze::SimpleOutput output{schema.value()};
    sourcemeta::blaze::Evaluator evaluator;
    const auto result{evaluator.validate(
        GENERATE_MATERIALISED_SCHEMA::compile(std::string{dialect_identifier},
                                              metaschema.value(), resolver),
        schema.value(), std::ref(output))};
    if (!result) {
      throw MetaschemaError(output);
    }

    sourcemeta::core::format(
        schema.value(), sourcemeta::core::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        dialect_identifier);
    const auto timestamp_end{std::chrono::steady_clock::now()};

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::write_pretty_json(
        action.destination, schema.value(), "application/schema+json",
        sourcemeta::one::Encoding::GZIP,
        sourcemeta::core::JSON{std::string{dialect_identifier}},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    resolver.cache_path(action.data, action.destination);
    return true;
  }

private:
  static auto compile(const std::string &cache_key,
                      const sourcemeta::core::JSON &schema,
                      const sourcemeta::one::Resolver &resolver)
      -> const sourcemeta::blaze::Template & {
    static std::mutex mutex;
    static std::unordered_map<std::string, sourcemeta::blaze::Template> cache;
    std::lock_guard<std::mutex> lock{mutex};
    const auto match{cache.find(cache_key)};
    if (match == cache.cend()) {
      auto schema_template{sourcemeta::blaze::compile(
          schema, sourcemeta::core::schema_walker,
          [&resolver](const auto identifier) { return resolver(identifier); },
          sourcemeta::blaze::default_schema_compiler,
          // The point of this class is to show nice errors to the user
          sourcemeta::blaze::Mode::Exhaustive)};
      return cache.emplace(cache_key, std::move(schema_template)).first->second;
    } else {
      return match->second;
    }
  }
};

struct GENERATE_POINTER_POSITIONS {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    sourcemeta::core::PointerPositionTracker tracker;
    sourcemeta::one::read_json(action.dependencies.front(), std::ref(tracker));
    const auto result{sourcemeta::core::to_json(tracker)};
    const auto timestamp_end{std::chrono::steady_clock::now()};
    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::Encoding::GZIP, sourcemeta::core::JSON{nullptr},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }
};

struct GENERATE_FRAME_LOCATIONS {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    sourcemeta::core::PointerPositionTracker tracker;
    const auto contents{sourcemeta::one::read_json(action.dependencies.front(),
                                                   std::ref(tracker))};
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Locations};
    frame.analyse(contents, sourcemeta::core::schema_walker,
                  [&callback, &resolver](const auto identifier) {
                    return resolver(identifier, callback);
                  });
    const auto result{frame.to_json(tracker).at("locations")};
    const auto timestamp_end{std::chrono::steady_clock::now()};
    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::Encoding::GZIP, sourcemeta::core::JSON{nullptr},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }
};

struct GENERATE_DEPENDENCIES {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto contents{
        sourcemeta::one::read_json(action.dependencies.front())};
    auto result{sourcemeta::core::JSON::make_array()};
    sourcemeta::core::dependencies(
        contents, sourcemeta::core::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        [&result](const auto &origin, const auto &pointer, const auto &target,
                  const auto &) {
          auto trace{sourcemeta::core::JSON::make_object()};
          trace.assign("from", without_json_extension(origin));
          trace.assign("to", without_json_extension(target));
          trace.assign("at", sourcemeta::core::to_json(pointer));
          result.push_back(std::move(trace));
        });
    // Otherwise we are returning non-sense
    assert(result.unique());
    const auto timestamp_end{std::chrono::steady_clock::now()};

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::Encoding::GZIP, sourcemeta::core::JSON{nullptr},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }

private:
  static auto without_json_extension(const std::string_view uri)
      -> sourcemeta::core::JSON {
    if (uri.ends_with(".json")) {
      return sourcemeta::core::JSON{std::string{uri.substr(0, uri.size() - 5)}};
    }

    return sourcemeta::core::JSON{std::string{uri}};
  }
};

struct GENERATE_DEPENDENTS {
  using DirectMap =
      std::unordered_map<sourcemeta::core::JSON::String,
                         std::unordered_set<sourcemeta::core::JSON::String>>;
  using TransitiveMap =
      std::unordered_map<sourcemeta::core::JSON::String,
                         std::set<std::pair<sourcemeta::core::JSON::String,
                                            sourcemeta::core::JSON::String>>>;

  struct SharedComputation {
    TransitiveMap transitive;
    std::unordered_map<std::string, std::string> uri_to_deps_path;
  };

  static inline std::once_flag compute_flag_;
  static inline SharedComputation shared_;

  static auto handler(const sourcemeta::one::BuildState &entries,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};

    std::call_once(compute_flag_, [&entries]() {
      // Collect dependencies.metapack file paths from BuildState
      std::vector<std::filesystem::path> deps_files;
      {
        const auto lock{entries.take_lock()};
        for (const auto key : entries.keys()) {
          if (key.ends_with("/%/dependencies.metapack")) {
            deps_files.emplace_back(std::string{key});
          }
        }
      }

      // Read JSON files and build direct dependency map
      DirectMap direct;
      for (const auto &dep_file : deps_files) {
        const auto contents{sourcemeta::one::read_json(dep_file)};
        assert(contents.is_array());
        const auto &dep_file_string{dep_file.native()};
        for (const auto &entry : contents.as_array()) {
          const auto &from_uri{entry.at("from").to_string()};
          shared_.uri_to_deps_path.try_emplace(from_uri, dep_file_string);
          direct[entry.at("to").to_string()].emplace(from_uri);
        }
      }

      // BFS for transitive closure
      for (const auto &[target, _] : direct) {
        auto &edges{shared_.transitive[target]};
        std::unordered_set<sourcemeta::core::JSON::StringView> visited;
        visited.emplace(target);
        std::queue<sourcemeta::core::JSON::String> queue;
        queue.emplace(target);
        while (!queue.empty()) {
          const auto current{std::move(queue.front())};
          queue.pop();
          const auto match{direct.find(current)};
          if (match == direct.cend()) {
            continue;
          }

          for (const auto &dependent : match->second) {
            edges.emplace(dependent, current);
            if (visited.emplace(dependent).second) {
              queue.emplace(dependent);
            }
          }
        }
      }
    });

    // Extract this schema's portion
    auto result{sourcemeta::core::JSON::make_array()};
    const auto match{shared_.transitive.find(std::string{action.data})};
    if (match != shared_.transitive.cend()) {
      for (const auto &[from, to] : match->second) {
        auto object{sourcemeta::core::JSON::make_object()};
        object.assign("from", sourcemeta::core::JSON{from});
        object.assign("to", sourcemeta::core::JSON{to});
        result.push_back(std::move(object));
      }
    }

    // Register contributing dependencies.metapack paths as dynamic deps
    // for precise dirty tracking on future rebuilds
    if (match != shared_.transitive.cend()) {
      std::unordered_set<std::string_view> registered;
      for (const auto &[from_uri, _] : match->second) {
        const auto path_match{shared_.uri_to_deps_path.find(from_uri)};
        if (path_match != shared_.uri_to_deps_path.cend() &&
            registered.insert(path_match->second).second) {
          callback(std::filesystem::path{path_match->second});
        }
      }
    }

    const auto timestamp_end{std::chrono::steady_clock::now()};

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::Encoding::GZIP, sourcemeta::core::JSON{nullptr},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }
};

struct GENERATE_HEALTH {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto contents{
        sourcemeta::one::read_json(action.dependencies.front())};
    const auto &collection{*resolver.entry(action.data).collection};
    auto &cache_entry{bundle_for(collection, resolver, callback)};
    auto errors{sourcemeta::core::JSON::make_array()};
    const auto result{cache_entry.bundle.check(
        contents, sourcemeta::core::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        [&errors, &cache_entry](const auto &pointer, const auto &name,
                                const auto &message, const auto &outcome,
                                const bool) {
          auto entry{sourcemeta::core::JSON::make_object()};
          entry.assign("name", sourcemeta::core::JSON{name});
          entry.assign("message", sourcemeta::core::JSON{message});
          entry.assign("description",
                       sourcemeta::core::to_json(outcome.description));
          entry.assign("custom", sourcemeta::core::JSON{
                                     cache_entry.custom_names.contains(name)});

          auto pointers{sourcemeta::core::JSON::make_array()};
          if (outcome.locations.empty()) {
            pointers.push_back(sourcemeta::core::to_json(pointer));
          } else {
            for (const auto &location : outcome.locations) {
              pointers.push_back(
                  sourcemeta::core::to_json(pointer.concat(location)));
            }
          }

          entry.assign("pointers", std::move(pointers));
          errors.push_back(std::move(entry));
        })};

    auto report{sourcemeta::core::JSON::make_object()};
    report.assign("score", sourcemeta::core::to_json(result.second));
    report.assign("errors", std::move(errors));
    const auto timestamp_end{std::chrono::steady_clock::now()};

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::write_pretty_json(
        action.destination, report, "application/json",
        sourcemeta::one::Encoding::GZIP, sourcemeta::core::JSON{nullptr},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }

private:
  struct CacheEntry {
    sourcemeta::core::SchemaTransformer bundle;
    std::unordered_set<std::string_view> custom_names;
  };

  static auto bundle_for(
      const sourcemeta::blaze::Configuration &configuration,
      [[maybe_unused]] const sourcemeta::one::Resolver &resolver,
      [[maybe_unused]] const sourcemeta::one::BuildDynamicCallback &callback)
      -> CacheEntry & {
    static std::mutex cache_mutex;
    static std::unordered_map<const void *, std::unique_ptr<CacheEntry>> cache;
    const auto *key{static_cast<const void *>(&configuration)};
    std::lock_guard<std::mutex> lock{cache_mutex};
    const auto match{cache.find(key)};
    if (match != cache.cend()) {
      return *match->second;
    }

    auto entry{std::make_unique<CacheEntry>()};
    sourcemeta::core::add(entry->bundle,
                          sourcemeta::core::AlterSchemaMode::Linter);
    entry->bundle.add<sourcemeta::blaze::ValidExamples>(
        sourcemeta::blaze::default_schema_compiler);
    entry->bundle.add<sourcemeta::blaze::ValidDefault>(
        sourcemeta::blaze::default_schema_compiler);

#if defined(SOURCEMETA_ONE_ENTERPRISE)
    sourcemeta::one::load_custom_lint_rules(entry->bundle, entry->custom_names,
                                            configuration, resolver, callback);
#else
    if (!configuration.lint.rules.empty()) {
      // TODO: Show enough information to know where the error is coming from
      throw CustomRuleError();
    }
#endif

    return *cache.emplace(key, std::move(entry)).first->second;
  }
};

struct GENERATE_BUNDLE {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    auto schema{sourcemeta::one::read_json(action.dependencies.front())};
    sourcemeta::core::bundle(schema, sourcemeta::core::schema_walker,
                             [&callback, &resolver](const auto identifier) {
                               return resolver(identifier, callback);
                             });
    const auto dialect_identifier{sourcemeta::core::dialect(schema)};
    assert(!dialect_identifier.empty());
    sourcemeta::core::format(
        schema, sourcemeta::core::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        dialect_identifier);
    const auto timestamp_end{std::chrono::steady_clock::now()};

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::write_pretty_json(
        action.destination, schema, "application/schema+json",
        sourcemeta::one::Encoding::GZIP,
        sourcemeta::core::JSON{std::string{dialect_identifier}},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }
};

struct GENERATE_EDITOR {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    auto schema{sourcemeta::one::read_json(action.dependencies.front())};
    sourcemeta::core::for_editor(schema, sourcemeta::core::schema_walker,
                                 [&callback, &resolver](const auto identifier) {
                                   return resolver(identifier, callback);
                                 });
    const auto dialect_identifier{sourcemeta::core::dialect(schema)};
    assert(!dialect_identifier.empty());
    sourcemeta::core::format(
        schema, sourcemeta::core::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        dialect_identifier);
    const auto timestamp_end{std::chrono::steady_clock::now()};

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::write_pretty_json(
        action.destination, schema, "application/schema+json",
        sourcemeta::one::Encoding::GZIP,
        sourcemeta::core::JSON{std::string{dialect_identifier}},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }
};

static auto generate_blaze_template(
    const std::filesystem::path &destination,
    const sourcemeta::one::BuildPlan::Action::Dependencies &dependencies,
    const sourcemeta::blaze::Mode mode) -> void {
  const auto timestamp_start{std::chrono::steady_clock::now()};
  const auto contents{sourcemeta::one::read_json(dependencies.front())};
  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(contents, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);
  const auto schema_template{sourcemeta::blaze::compile(
      contents, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver,
      sourcemeta::blaze::default_schema_compiler, frame, frame.root(), mode)};
  const auto result{sourcemeta::blaze::to_json(schema_template)};
  const auto timestamp_end{std::chrono::steady_clock::now()};
  std::filesystem::create_directories(destination.parent_path());
  sourcemeta::one::write_json(
      destination, result, "application/json", sourcemeta::one::Encoding::GZIP,
      sourcemeta::core::JSON{nullptr},
      std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                            timestamp_start));
}

struct GENERATE_BLAZE_TEMPLATE_EXHAUSTIVE {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    generate_blaze_template(action.destination, action.dependencies,
                            sourcemeta::blaze::Mode::Exhaustive);
    return true;
  }
};

struct GENERATE_BLAZE_TEMPLATE_FAST {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    generate_blaze_template(action.destination, action.dependencies,
                            sourcemeta::blaze::Mode::FastValidation);
    return true;
  }
};

struct GENERATE_STATS {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto schema{sourcemeta::one::read_json(action.dependencies.front())};
    std::map<sourcemeta::core::JSON::String,
             std::map<sourcemeta::core::JSON::String, std::uint64_t>>
        result;
    for (const auto &entry : sourcemeta::core::SchemaIterator{
             schema, sourcemeta::core::schema_walker,
             [&callback, &resolver](const auto identifier) {
               return resolver(identifier, callback);
             }}) {
      if (!entry.subschema.get().is_object()) {
        continue;
      }

      for (const auto &property : entry.subschema.get().as_object()) {
        const auto &walker_result{sourcemeta::core::schema_walker(
            property.first, entry.vocabularies)};
        if (walker_result.vocabulary.has_value()) {
          result[std::string{sourcemeta::core::to_string(
              walker_result.vocabulary.value())}][property.first] += 1;
        } else {
          result["unknown"][property.first] += 1;
        }
      }
    }

    const auto timestamp_end{std::chrono::steady_clock::now()};
    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::write_pretty_json(
        action.destination, sourcemeta::core::to_json(result),
        "application/json", sourcemeta::one::Encoding::GZIP,
        sourcemeta::core::JSON{nullptr},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }
};

struct GENERATE_URITEMPLATE_ROUTES {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
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

    if (action.data == "Full") {
      router.add("/self/static/{+path}", sourcemeta::one::HANDLER_SELF_STATIC);
    }

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::core::URITemplateRouterView::save(router, action.destination);
    return true;
  }
};

} // namespace sourcemeta::one

#endif
