#ifndef SOURCEMETA_ONE_INDEX_GENERATORS_H_
#define SOURCEMETA_ONE_INDEX_GENERATORS_H_

#include "error.h"

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/build.h>
#include <sourcemeta/one/metapack.h>
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
#include <cstring>       // std::memcpy
#include <filesystem>    // std::filesystem
#include <fstream>       // std::ofstream
#include <limits>        // std::numeric_limits
#include <memory>        // std::unique_ptr
#include <mutex>         // std::mutex, std::lock_guard
#include <queue>         // std::queue
#include <set>           // std::set
#include <sstream>       // std::ostringstream
#include <unordered_map> // std::unordered_map
#include <utility>       // std::move, std::pair

namespace sourcemeta::one {

#pragma pack(push, 1)
struct MetapackDialectExtension {
  std::uint16_t dialect_length;
};
#pragma pack(pop)

static auto make_dialect_extension(const std::string_view dialect)
    -> std::vector<std::uint8_t> {
  assert(dialect.size() <= std::numeric_limits<std::uint16_t>::max());
  std::vector<std::uint8_t> result;
  result.resize(sizeof(MetapackDialectExtension) + dialect.size());
  MetapackDialectExtension header{};
  header.dialect_length = static_cast<std::uint16_t>(dialect.size());
  std::memcpy(result.data(), &header, sizeof(header));
  std::memcpy(result.data() + sizeof(header), dialect.data(), dialect.size());
  return result;
}

struct GENERATE_VERSION {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    std::filesystem::create_directories(action.destination.parent_path());
    std::ofstream stream{action.destination};
    assert(!stream.fail());
    sourcemeta::core::stringify(
        sourcemeta::core::JSON{std::string{action.data}}, stream);
  }
};

struct GENERATE_COMMENT {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    std::filesystem::create_directories(action.destination.parent_path());
    std::ofstream stream{action.destination};
    assert(!stream.fail());
    sourcemeta::core::stringify(
        sourcemeta::core::JSON{std::string{action.data}}, stream);
  }
};

struct GENERATE_CONFIGURATION {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &raw_configuration) -> void {
    std::filesystem::create_directories(action.destination.parent_path());
    std::ofstream stream{action.destination};
    assert(!stream.fail());
    sourcemeta::core::stringify(raw_configuration, stream);
  }
};

struct GENERATE_MATERIALISED_SCHEMA {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    auto schema{resolver(action.data)};
    assert(schema.has_value());
    const auto dialect_identifier{sourcemeta::core::dialect(schema.value())};
    assert(!dialect_identifier.empty());
    const auto metaschema{resolver(dialect_identifier, callback)};
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

    // Most schemas are not metaschemas, so this check is a nice
    // heuristic to avoid the cost of resolving the base dialect
    // on most of them
    if (schema->is_object() && schema->defines("$vocabulary")) {
      const auto declared_vocabularies{sourcemeta::core::parse_vocabularies(
          schema.value(),
          [&callback, &resolver](const auto identifier) {
            return resolver(identifier, callback);
          },
          dialect_identifier)};
      if (declared_vocabularies.has_value()) {
        declared_vocabularies.value().throw_if_any_unknown_required(
            "The metaschema requires an unrecognised vocabulary");
      }
    }

    sourcemeta::core::format(
        schema.value(), sourcemeta::core::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        dialect_identifier);
    const auto timestamp_end{std::chrono::steady_clock::now()};

    const auto extension_bytes{make_dialect_extension(dialect_identifier)};
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, schema.value(), "application/schema+json",
        sourcemeta::one::MetapackEncoding::GZIP,
        std::span<const std::uint8_t>{extension_bytes},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    resolver.cache_path(action.data, action.destination);
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
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto schema_option{
        sourcemeta::one::metapack_read_json(action.dependencies.front())};
    assert(schema_option.has_value());
    const auto &schema{schema_option.value()};
    std::ostringstream schema_stream;
    sourcemeta::core::prettify(schema, schema_stream);
    sourcemeta::core::PointerPositionTracker tracker;
    sourcemeta::core::JSON parsed{nullptr};
    sourcemeta::core::parse_json(schema_stream.str(), parsed,
                                 std::ref(tracker));
    const auto result{sourcemeta::core::to_json(tracker)};
    const auto timestamp_end{std::chrono::steady_clock::now()};
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

struct GENERATE_FRAME_LOCATIONS {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto contents_option{
        sourcemeta::one::metapack_read_json(action.dependencies.front())};
    assert(contents_option.has_value());
    const auto &contents{contents_option.value()};
    std::ostringstream contents_stream;
    sourcemeta::core::prettify(contents, contents_stream);
    sourcemeta::core::PointerPositionTracker tracker;
    sourcemeta::core::JSON parsed{nullptr};
    sourcemeta::core::parse_json(contents_stream.str(), parsed,
                                 std::ref(tracker));
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Locations};
    frame.analyse(contents, sourcemeta::core::schema_walker,
                  [&callback, &resolver](const auto identifier) {
                    return resolver(identifier, callback);
                  });
    const auto result{frame.to_json(tracker).at("locations")};
    const auto timestamp_end{std::chrono::steady_clock::now()};
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

struct GENERATE_DEPENDENCIES {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto contents_option{
        sourcemeta::one::metapack_read_json(action.dependencies.front())};
    assert(contents_option.has_value());
    const auto &contents{contents_option.value()};
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
          trace.assign("at", sourcemeta::core::JSON{
                                 sourcemeta::core::to_string(pointer)});
          result.push_back(std::move(trace));
        });
    // Otherwise we are returning non-sense
    assert(result.unique());
    const auto timestamp_end{std::chrono::steady_clock::now()};

    sourcemeta::one::metapack_write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
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

// The relevant input dependencies files are determined by delta. The handler
// reads only those few files to build the reverse dependency graph
struct GENERATE_DEPENDENTS {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};

    using DirectMap =
        std::unordered_map<sourcemeta::core::JSON::String,
                           std::unordered_set<sourcemeta::core::JSON::String>>;
    DirectMap direct;
    for (const auto &dependency : action.dependencies) {
      const auto contents_option{
          sourcemeta::one::metapack_read_json(dependency)};
      assert(contents_option.has_value());
      const auto &contents{contents_option.value()};
      assert(contents.is_array());
      for (const auto &entry : contents.as_array()) {
        direct[entry.at("to").to_string()].emplace(
            entry.at("from").to_string());
      }
    }

    using TransitiveMap =
        std::unordered_map<sourcemeta::core::JSON::String,
                           std::set<std::pair<sourcemeta::core::JSON::String,
                                              sourcemeta::core::JSON::String>>>;
    TransitiveMap transitive;
    for (const auto &[target, _] : direct) {
      auto &edges{transitive[target]};
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

    auto result{sourcemeta::core::JSON::make_array()};
    const auto match{transitive.find(std::string{action.data})};
    if (match != transitive.cend()) {
      for (const auto &[from, to] : match->second) {
        auto object{sourcemeta::core::JSON::make_object()};
        object.assign("from", sourcemeta::core::JSON{from});
        object.assign("to", sourcemeta::core::JSON{to});
        result.push_back(std::move(object));
      }
    }

    const auto timestamp_end{std::chrono::steady_clock::now()};

    sourcemeta::one::metapack_write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

struct GENERATE_HEALTH {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto contents_option{
        sourcemeta::one::metapack_read_json(action.dependencies.front())};
    assert(contents_option.has_value());
    const auto &contents{contents_option.value()};
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
            pointers.push_back(
                sourcemeta::core::JSON{sourcemeta::core::to_string(pointer)});
          } else {
            for (const auto &location : outcome.locations) {
              pointers.push_back(sourcemeta::core::JSON{
                  sourcemeta::core::to_string(pointer.concat(location))});
            }
          }

          entry.assign("pointers", std::move(pointers));
          errors.push_back(std::move(entry));
        })};

    auto report{sourcemeta::core::JSON::make_object()};
    report.assign("score", sourcemeta::core::to_json(result.second));
    report.assign("errors", std::move(errors));
    const auto timestamp_end{std::chrono::steady_clock::now()};

    sourcemeta::one::metapack_write_pretty_json(
        action.destination, report, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
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
      const auto *config_path{
          configuration.extra.try_at("x-sourcemeta-one:path")};
      assert(config_path);
      throw CustomRuleError(std::filesystem::path{config_path->to_string()});
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
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    auto schema_option{
        sourcemeta::one::metapack_read_json(action.dependencies.front())};
    assert(schema_option.has_value());
    auto schema{std::move(schema_option.value())};
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

    const auto extension_bytes{make_dialect_extension(dialect_identifier)};
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, schema, "application/schema+json",
        sourcemeta::one::MetapackEncoding::GZIP,
        std::span<const std::uint8_t>{extension_bytes},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

struct GENERATE_EDITOR {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    auto schema_option{
        sourcemeta::one::metapack_read_json(action.dependencies.front())};
    assert(schema_option.has_value());
    auto schema{std::move(schema_option.value())};
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

    const auto extension_bytes{make_dialect_extension(dialect_identifier)};
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, schema, "application/schema+json",
        sourcemeta::one::MetapackEncoding::GZIP,
        std::span<const std::uint8_t>{extension_bytes},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

static auto generate_blaze_template(
    const std::filesystem::path &destination,
    const sourcemeta::one::BuildPlan::Action::Dependencies &dependencies,
    const sourcemeta::one::BuildDynamicCallback &callback,
    sourcemeta::one::Resolver &resolver, const sourcemeta::blaze::Mode mode)
    -> void {
  const auto timestamp_start{std::chrono::steady_clock::now()};
  const auto contents_option{
      sourcemeta::one::metapack_read_json(dependencies.front())};
  assert(contents_option.has_value());
  const auto &contents{contents_option.value()};
  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(contents, sourcemeta::core::schema_walker,
                [&callback, &resolver](const auto identifier) {
                  return resolver(identifier, callback);
                });
  const auto schema_template{sourcemeta::blaze::compile(
      contents, sourcemeta::core::schema_walker,
      [&callback, &resolver](const auto identifier) {
        return resolver(identifier, callback);
      },
      sourcemeta::blaze::default_schema_compiler, frame, frame.root(), mode)};
  const auto result{sourcemeta::blaze::to_json(schema_template)};
  const auto timestamp_end{std::chrono::steady_clock::now()};
  sourcemeta::one::metapack_write_json(
      destination, result, "application/json",
      sourcemeta::one::MetapackEncoding::GZIP, {},
      std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                            timestamp_start));
}

struct GENERATE_BLAZE_TEMPLATE_EXHAUSTIVE {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    generate_blaze_template(action.destination, action.dependencies, callback,
                            resolver, sourcemeta::blaze::Mode::Exhaustive);
  }
};

struct GENERATE_BLAZE_TEMPLATE_FAST {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    generate_blaze_template(action.destination, action.dependencies, callback,
                            resolver, sourcemeta::blaze::Mode::FastValidation);
  }
};

struct GENERATE_STATS {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto schema_option{
        sourcemeta::one::metapack_read_json(action.dependencies.front())};
    assert(schema_option.has_value());
    const auto &schema{schema_option.value()};
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
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, sourcemeta::core::to_json(result),
        "application/json", sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

struct GENERATE_URITEMPLATE_ROUTES {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration,
                      const sourcemeta::core::JSON &) -> void {
    sourcemeta::core::URITemplateRouter router{configuration.base_path};

    const sourcemeta::core::URITemplateRouter::Argument list_arguments[] = {
        {"artifact", std::string_view{"directory"}},
        {"responseSchema",
         std::string_view{"/self/v1/schemas/api/list/response"}}};
    router.add("/self/v1/api/list", sourcemeta::one::HANDLER_SELF_V1_API_LIST,
               list_arguments);
    router.add("/self/v1/api/list/{+path}",
               sourcemeta::one::HANDLER_SELF_V1_API_LIST_PATH, list_arguments);

    const sourcemeta::core::URITemplateRouter::Argument
        dependencies_arguments[] = {
            {"artifact", std::string_view{"dependencies"}},
            {"responseSchema",
             std::string_view{
                 "/self/v1/schemas/api/schemas/dependencies/response"}}};
    router.add("/self/v1/api/schemas/dependencies/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_DEPENDENCIES,
               dependencies_arguments);

    const sourcemeta::core::URITemplateRouter::Argument dependents_arguments[] =
        {{"artifact", std::string_view{"dependents"}},
         {"responseSchema",
          std::string_view{
              "/self/v1/schemas/api/schemas/dependents/response"}}};
    router.add("/self/v1/api/schemas/dependents/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_DEPENDENTS,
               dependents_arguments);

    const sourcemeta::core::URITemplateRouter::Argument health_arguments[] = {
        {"artifact", std::string_view{"health"}},
        {"responseSchema",
         std::string_view{"/self/v1/schemas/api/schemas/health/response"}}};
    router.add("/self/v1/api/schemas/health/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_HEALTH,
               health_arguments);

    const sourcemeta::core::URITemplateRouter::Argument locations_arguments[] =
        {{"artifact", std::string_view{"locations"}},
         {"responseSchema",
          std::string_view{"/self/v1/schemas/api/schemas/locations/response"}}};
    router.add("/self/v1/api/schemas/locations/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_LOCATIONS,
               locations_arguments);

    const sourcemeta::core::URITemplateRouter::Argument positions_arguments[] =
        {{"artifact", std::string_view{"positions"}},
         {"responseSchema",
          std::string_view{"/self/v1/schemas/api/schemas/positions/response"}}};
    router.add("/self/v1/api/schemas/positions/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_POSITIONS,
               positions_arguments);

    const sourcemeta::core::URITemplateRouter::Argument stats_arguments[] = {
        {"artifact", std::string_view{"stats"}},
        {"responseSchema",
         std::string_view{"/self/v1/schemas/api/schemas/stats/response"}}};
    router.add("/self/v1/api/schemas/stats/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_STATS,
               stats_arguments);

    const sourcemeta::core::URITemplateRouter::Argument metadata_arguments[] = {
        {"artifact", std::string_view{"schema"}},
        {"responseSchema",
         std::string_view{"/self/v1/schemas/api/schemas/metadata/response"}}};
    router.add("/self/v1/api/schemas/metadata/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_METADATA,
               metadata_arguments);

    const sourcemeta::core::URITemplateRouter::Argument evaluate_arguments[] = {
        {"mode", static_cast<std::int64_t>(0)}};
    router.add("/self/v1/api/schemas/evaluate/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_EVALUATE,
               evaluate_arguments);

    const sourcemeta::core::URITemplateRouter::Argument trace_arguments[] = {
        {"mode", static_cast<std::int64_t>(1)}};
    router.add("/self/v1/api/schemas/trace/{+schema}",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_TRACE,
               trace_arguments);

    router.add("/self/v1/api/schemas/search",
               sourcemeta::one::HANDLER_SELF_V1_API_SCHEMAS_SEARCH);
    router.add("/self/v1/health", sourcemeta::one::HANDLER_SELF_V1_HEALTH);
    router.add("/self/v1/api/{+any}",
               sourcemeta::one::HANDLER_SELF_V1_API_DEFAULT);

    if (action.data == "Full") {
      const sourcemeta::core::URITemplateRouter::Argument static_arguments[] = {
          {"path", std::string_view{SOURCEMETA_ONE_STATIC}}};
      router.add("/self/static/{+path}", sourcemeta::one::HANDLER_SELF_STATIC,
                 static_arguments);
    }

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::core::URITemplateRouterView::save(router, action.destination);
  }
};

} // namespace sourcemeta::one

#endif
