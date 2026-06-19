#ifndef SOURCEMETA_ONE_INDEX_GENERATORS_H_
#define SOURCEMETA_ONE_INDEX_GENERATORS_H_

#include "error.h"

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/build.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/blaze/alterschema.h>
#include <sourcemeta/blaze/bundle.h>
#include <sourcemeta/blaze/editor.h>
#include <sourcemeta/blaze/format.h>
#include <sourcemeta/blaze/foundation.h>
#include <sourcemeta/blaze/frame.h>
#include <sourcemeta/blaze/frame_error.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/configuration.h>
#include <sourcemeta/blaze/evaluator.h>

#if defined(SOURCEMETA_ONE_ENTERPRISE)
#include <sourcemeta/one/enterprise_index.h>
#endif

#include <algorithm>     // std::ranges::find
#include <cassert>       // assert
#include <cstring>       // std::memcpy
#include <filesystem>    // std::filesystem
#include <limits>        // std::numeric_limits
#include <memory>        // std::unique_ptr
#include <mutex>         // std::mutex, std::lock_guard
#include <optional>      // std::optional
#include <ostream>       // std::ostream
#include <queue>         // std::queue
#include <set>           // std::set
#include <sstream>       // std::ostringstream
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <tuple>         // std::tuple
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
    sourcemeta::core::atomic_write_file(
        action.destination, [&](std::ostream &stream) {
          sourcemeta::core::stringify(sourcemeta::core::JSON{action.data},
                                      stream);
        });
  }
};

struct GENERATE_COMMENT {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    sourcemeta::core::atomic_write_file(
        action.destination, [&](std::ostream &stream) {
          sourcemeta::core::stringify(sourcemeta::core::JSON{action.data},
                                      stream);
        });
  }
};

struct GENERATE_CONFIGURATION {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &raw_configuration) -> void {
    sourcemeta::core::atomic_write_file(
        action.destination, [&](std::ostream &stream) {
          sourcemeta::core::stringify(raw_configuration, stream);
        });
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
    const auto dialect_identifier{sourcemeta::blaze::dialect(schema.value())};
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
      const auto declared_vocabularies{sourcemeta::blaze::parse_vocabularies(
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

    sourcemeta::blaze::format(
        schema.value(), sourcemeta::blaze::schema_walker,
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
          schema, sourcemeta::blaze::schema_walker,
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
    sourcemeta::blaze::SchemaFrame frame{
        sourcemeta::blaze::SchemaFrame::Mode::Locations};
    frame.analyse(contents, sourcemeta::blaze::schema_walker,
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
                      const sourcemeta::one::Configuration &configuration,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto contents_option{
        sourcemeta::one::metapack_read_json(action.dependencies.front())};
    assert(contents_option.has_value());
    const auto &contents{contents_option.value()};
    auto result{sourcemeta::core::JSON::make_array()};
    sourcemeta::blaze::dependencies(
        contents, sourcemeta::blaze::schema_walker,
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

    if (result.size() > 0) {
      const sourcemeta::one::Authentication authentication{
          action.dependencies.at(1)};
      for (const auto &edge : result.as_array()) {
        const auto &referrer_uri{edge.at("from").to_string()};
        const auto &referent_uri{edge.at("to").to_string()};
        const auto referrer{registry_path(referrer_uri, configuration.url)};
        const auto referent{registry_path(referent_uri, configuration.url)};
        if (referrer.has_value() && referent.has_value() &&
            !authentication.reference_permitted(referrer.value(),
                                                referent.value())) {
          throw CrossPolicyReferenceError(referrer_uri, referent_uri);
        }
      }
    }

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
      return sourcemeta::core::JSON{uri.substr(0, uri.size() - 5)};
    }

    return sourcemeta::core::JSON{uri};
  }

  static auto registry_path(const std::string_view uri,
                            const std::string_view base)
      -> std::optional<std::string_view> {
    if (!uri.starts_with(base)) {
      return std::nullopt;
    }

    const auto remainder{uri.substr(base.size())};
    if (!base.ends_with('/') && !remainder.empty() &&
        !remainder.starts_with('/')) {
      return std::nullopt;
    }

    return remainder;
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
                           std::set<std::pair<sourcemeta::core::JSON::String,
                                              sourcemeta::core::JSON::String>>>;
    DirectMap direct;
    for (const auto &dependency : action.dependencies) {
      const auto contents_option{
          sourcemeta::one::metapack_read_json(dependency)};
      assert(contents_option.has_value());
      const auto &contents{contents_option.value()};
      assert(contents.is_array());
      for (const auto &entry : contents.as_array()) {
        direct[entry.at("to").to_string()].emplace(entry.at("from").to_string(),
                                                   entry.at("at").to_string());
      }
    }

    using TransitiveMap = std::unordered_map<
        sourcemeta::core::JSON::String,
        std::set<std::tuple<sourcemeta::core::JSON::String,
                            sourcemeta::core::JSON::String,
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

        for (const auto &[dependent, at] : match->second) {
          edges.emplace(dependent, current, at);
          if (visited.emplace(dependent).second) {
            queue.emplace(dependent);
          }
        }
      }
    }

    auto result{sourcemeta::core::JSON::make_array()};
    const auto match{transitive.find(std::string{action.data})};
    if (match != transitive.cend()) {
      for (const auto &[from, to, at] : match->second) {
        auto object{sourcemeta::core::JSON::make_object()};
        object.assign("from", sourcemeta::core::JSON{from});
        object.assign("to", sourcemeta::core::JSON{to});
        object.assign("at", sourcemeta::core::JSON{at});
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
        contents, sourcemeta::blaze::schema_walker,
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
    sourcemeta::blaze::SchemaTransformer bundle;
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
    sourcemeta::blaze::add(entry->bundle,
                           sourcemeta::blaze::AlterSchemaMode::Linter);

#if defined(SOURCEMETA_ONE_ENTERPRISE)
    sourcemeta::one::load_custom_lint_rules(entry->bundle, entry->custom_names,
                                            configuration, resolver, callback);
#else
    if (!configuration.lint.rules.empty()) {
      const auto *config_path{
          configuration.extra.try_at("x-sourcemeta-one:path")};
      assert(config_path);
      throw EnterpriseOnlyFeatureError(
          std::filesystem::path{config_path->to_string()},
          "Custom linter rules are only available on the enterprise edition");
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
    // The registry serves every meta-schema a schema may declare, so
    // bundles only need to embed references and can skip meta-schemas
    sourcemeta::blaze::bundle(
        schema, sourcemeta::blaze::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        },
        sourcemeta::blaze::BundleMode::References);
    const auto dialect_identifier{sourcemeta::blaze::dialect(schema)};
    assert(!dialect_identifier.empty());
    sourcemeta::blaze::format(
        schema, sourcemeta::blaze::schema_walker,
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
    sourcemeta::blaze::for_editor(
        schema, sourcemeta::blaze::schema_walker,
        [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        });
    const auto dialect_identifier{sourcemeta::blaze::dialect(schema)};
    assert(!dialect_identifier.empty());
    sourcemeta::blaze::format(
        schema, sourcemeta::blaze::schema_walker,
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
  sourcemeta::blaze::SchemaFrame frame{
      sourcemeta::blaze::SchemaFrame::Mode::References};
  frame.analyse(contents, sourcemeta::blaze::schema_walker,
                [&callback, &resolver](const auto identifier) {
                  return resolver(identifier, callback);
                });
  const auto schema_template{sourcemeta::blaze::compile(
      contents, sourcemeta::blaze::schema_walker,
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
    for (const auto &entry : sourcemeta::blaze::SchemaIterator{
             schema, sourcemeta::blaze::schema_walker,
             [&callback, &resolver](const auto identifier) {
               return resolver(identifier, callback);
             }}) {
      if (!entry.subschema.get().is_object()) {
        continue;
      }

      for (const auto &property : entry.subschema.get().as_object()) {
        const auto &walker_result{sourcemeta::blaze::schema_walker(
            property.first, entry.vocabularies)};
        if (walker_result.vocabulary.has_value()) {
          result[std::string{sourcemeta::blaze::to_string(
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
    sourcemeta::core::URITemplateRouter router{configuration.base_path,
                                               configuration.url};

    const auto list_schema{configuration.base_path +
                           "/self/v1/schemas/api/list/response"};
    const auto dependencies_schema{
        configuration.base_path +
        "/self/v1/schemas/api/schemas/dependencies/response"};
    const auto dependents_schema{
        configuration.base_path +
        "/self/v1/schemas/api/schemas/dependents/response"};
    const auto health_schema{configuration.base_path +
                             "/self/v1/schemas/api/schemas/health/response"};
    const auto locations_schema{
        configuration.base_path +
        "/self/v1/schemas/api/schemas/locations/response"};
    const auto positions_schema{
        configuration.base_path +
        "/self/v1/schemas/api/schemas/positions/response"};
    const auto stats_schema{configuration.base_path +
                            "/self/v1/schemas/api/schemas/stats/response"};
    const auto metadata_schema{
        configuration.base_path +
        "/self/v1/schemas/api/schemas/metadata/response"};
    const auto evaluate_request_schema{
        configuration.base_path +
        "/self/v1/schemas/api/schemas/evaluate/request"};
    const auto evaluate_response_schema{
        configuration.base_path +
        "/self/v1/schemas/api/schemas/evaluate/response"};
    const auto trace_request_schema{
        configuration.base_path + "/self/v1/schemas/api/schemas/trace/request"};
    const auto trace_response_schema{
        configuration.base_path +
        "/self/v1/schemas/api/schemas/trace/response"};
    const auto search_response_schema{
        configuration.base_path +
        "/self/v1/schemas/api/schemas/search/response"};
    const auto list_directory_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/list-directory/request"};
    const auto list_directory_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/list-directory/response"};
    const auto get_schema_dependencies_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request"};
    const auto get_schema_dependencies_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response"};
    const auto get_schema_dependents_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-dependents/request"};
    const auto get_schema_dependents_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-dependents/response"};
    const auto get_schema_health_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-health/request"};
    const auto get_schema_health_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-health/response"};
    const auto get_schema_locations_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-locations/request"};
    const auto get_schema_locations_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-locations/response"};
    const auto get_schema_positions_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-positions/request"};
    const auto get_schema_positions_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-positions/response"};
    const auto get_schema_stats_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-stats/request"};
    const auto get_schema_stats_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-stats/response"};
    const auto get_schema_metadata_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-metadata/request"};
    const auto get_schema_metadata_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/get-schema-metadata/response"};
    const auto evaluate_schema_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/evaluate-schema/request"};
    const auto evaluate_schema_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/evaluate-schema/response"};
    const auto trace_schema_evaluation_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request"};
    const auto trace_schema_evaluation_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response"};
    const auto search_schemas_request_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/search-schemas/request"};
    const auto search_schemas_response_schema{
        configuration.base_path +
        "/self/v1/schemas/mcp/tools/call/search-schemas/response"};
    const auto error_schema{configuration.base_path +
                            "/self/v1/schemas/api/error"};
    const auto mcp_request_schema{configuration.base_path +
                                  "/self/v1/schemas/mcp/request"};
    const auto mcp_response_schema{configuration.base_path +
                                   "/self/v1/schemas/mcp/response"};

    sourcemeta::core::URITemplateRouter::Identifier next_id{1};

    if (configuration.api) {
      const sourcemeta::core::URITemplateRouter::Argument
          otherwise_arguments[] = {
              {"errorSchema", std::string_view{error_schema}}};
      router.otherwise(sourcemeta::one::ACTION_TYPE_DEFAULT_V1,
                       otherwise_arguments);

      const sourcemeta::core::URITemplateRouter::Argument list_arguments[] = {
          {"responseSchema", std::string_view{list_schema}},
          {"mcpRequestSchema", std::string_view{list_directory_request_schema}},
          {"mcpResponseSchema",
           std::string_view{list_directory_response_schema}},
          {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/list{/path*}", "list_directory", next_id++,
                 sourcemeta::one::ACTION_TYPE_LIST_DIRECTORY_V1,
                 list_arguments);

      const sourcemeta::core::URITemplateRouter::Argument
          dependencies_arguments[] = {
              {"responseSchema", std::string_view{dependencies_schema}},
              {"mcpRequestSchema",
               std::string_view{get_schema_dependencies_request_schema}},
              {"mcpResponseSchema",
               std::string_view{get_schema_dependencies_response_schema}},
              {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/dependencies/{+schema}",
                 "get_schema_dependencies", next_id++,
                 sourcemeta::one::ACTION_TYPE_GET_SCHEMA_DEPENDENCIES_V1,
                 dependencies_arguments);

      const sourcemeta::core::URITemplateRouter::Argument
          dependents_arguments[] = {
              {"responseSchema", std::string_view{dependents_schema}},
              {"mcpRequestSchema",
               std::string_view{get_schema_dependents_request_schema}},
              {"mcpResponseSchema",
               std::string_view{get_schema_dependents_response_schema}},
              {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/dependents/{+schema}",
                 "get_schema_dependents", next_id++,
                 sourcemeta::one::ACTION_TYPE_GET_SCHEMA_DEPENDENTS_V1,
                 dependents_arguments);

      const sourcemeta::core::URITemplateRouter::Argument health_arguments[] = {
          {"responseSchema", std::string_view{health_schema}},
          {"mcpRequestSchema",
           std::string_view{get_schema_health_request_schema}},
          {"mcpResponseSchema",
           std::string_view{get_schema_health_response_schema}},
          {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/health/{+schema}", "get_schema_health",
                 next_id++, sourcemeta::one::ACTION_TYPE_GET_SCHEMA_HEALTH_V1,
                 health_arguments);

      const sourcemeta::core::URITemplateRouter::Argument
          locations_arguments[] = {
              {"responseSchema", std::string_view{locations_schema}},
              {"mcpRequestSchema",
               std::string_view{get_schema_locations_request_schema}},
              {"mcpResponseSchema",
               std::string_view{get_schema_locations_response_schema}},
              {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/locations/{+schema}",
                 "get_schema_locations", next_id++,
                 sourcemeta::one::ACTION_TYPE_GET_SCHEMA_LOCATIONS_V1,
                 locations_arguments);

      const sourcemeta::core::URITemplateRouter::Argument
          positions_arguments[] = {
              {"responseSchema", std::string_view{positions_schema}},
              {"mcpRequestSchema",
               std::string_view{get_schema_positions_request_schema}},
              {"mcpResponseSchema",
               std::string_view{get_schema_positions_response_schema}},
              {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/positions/{+schema}",
                 "get_schema_positions", next_id++,
                 sourcemeta::one::ACTION_TYPE_GET_SCHEMA_POSITIONS_V1,
                 positions_arguments);

      const sourcemeta::core::URITemplateRouter::Argument stats_arguments[] = {
          {"responseSchema", std::string_view{stats_schema}},
          {"mcpRequestSchema",
           std::string_view{get_schema_stats_request_schema}},
          {"mcpResponseSchema",
           std::string_view{get_schema_stats_response_schema}},
          {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/stats/{+schema}", "get_schema_stats",
                 next_id++, sourcemeta::one::ACTION_TYPE_GET_SCHEMA_STATS_V1,
                 stats_arguments);

      const sourcemeta::core::URITemplateRouter::Argument metadata_arguments[] =
          {{"responseSchema", std::string_view{metadata_schema}},
           {"mcpRequestSchema",
            std::string_view{get_schema_metadata_request_schema}},
           {"mcpResponseSchema",
            std::string_view{get_schema_metadata_response_schema}},
           {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/metadata/{+schema}",
                 "get_schema_metadata", next_id++,
                 sourcemeta::one::ACTION_TYPE_GET_SCHEMA_METADATA_V1,
                 metadata_arguments);

      const sourcemeta::core::URITemplateRouter::Argument evaluate_arguments[] =
          {{"requestSchema", std::string_view{evaluate_request_schema}},
           {"responseSchema", std::string_view{evaluate_response_schema}},
           {"mcpRequestSchema",
            std::string_view{evaluate_schema_request_schema}},
           {"mcpResponseSchema",
            std::string_view{evaluate_schema_response_schema}},
           {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/evaluate/{+schema}", "evaluate_schema",
                 next_id++, sourcemeta::one::ACTION_TYPE_JSONSCHEMA_EVALUATE_V1,
                 evaluate_arguments);

      const sourcemeta::core::URITemplateRouter::Argument trace_arguments[] = {
          {"requestSchema", std::string_view{trace_request_schema}},
          {"responseSchema", std::string_view{trace_response_schema}},
          {"mcpRequestSchema",
           std::string_view{trace_schema_evaluation_request_schema}},
          {"mcpResponseSchema",
           std::string_view{trace_schema_evaluation_response_schema}},
          {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/trace/{+schema}",
                 "trace_schema_evaluation", next_id++,
                 sourcemeta::one::ACTION_TYPE_JSONSCHEMA_TRACE_V1,
                 trace_arguments);

      const sourcemeta::core::URITemplateRouter::Argument search_arguments[] = {
          {"responseSchema", std::string_view{search_response_schema}},
          {"mcpRequestSchema", std::string_view{search_schemas_request_schema}},
          {"mcpResponseSchema",
           std::string_view{search_schemas_response_schema}},
          {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/schemas/search", "search_schemas", next_id++,
                 sourcemeta::one::ACTION_TYPE_SCHEMA_SEARCH_V1,
                 search_arguments);

      const sourcemeta::core::URITemplateRouter::Argument
          health_check_arguments[] = {
              {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/health", "check_server_health", next_id++,
                 sourcemeta::one::ACTION_TYPE_HEALTH_CHECK_V1,
                 health_check_arguments);

      const sourcemeta::core::URITemplateRouter::Argument mcp_arguments[] = {
          {"requestSchema", std::string_view{mcp_request_schema}},
          {"responseSchema", std::string_view{mcp_response_schema}}};
      router.add("/self/v1/mcp", "handle_mcp_request", next_id++,
                 sourcemeta::one::ACTION_TYPE_MCP_V1, mcp_arguments);
      // Trailing-slash variant for clients that normalise URLs by
      // appending `/`. Both routes dispatch to the same MCP handler
      router.add("/self/v1/mcp/", "handle_mcp_request_trailing_slash",
                 next_id++, sourcemeta::one::ACTION_TYPE_MCP_V1, mcp_arguments);

      const sourcemeta::core::URITemplateRouter::Argument
          not_found_arguments[] = {
              {"errorSchema", std::string_view{error_schema}}};
      router.add("/self/v1/api/{+any}", "handle_not_found", next_id++,
                 sourcemeta::one::ACTION_TYPE_NOT_FOUND_V1,
                 not_found_arguments);

      if (action.data == "Full") {
        const sourcemeta::core::URITemplateRouter::Argument static_arguments[] =
            {{"path", std::string_view{SOURCEMETA_ONE_STATIC}},
             {"errorSchema", std::string_view{error_schema}}};
        router.add("/self/v1/static/{+path}", "serve_static_asset", next_id++,
                   sourcemeta::one::ACTION_TYPE_SERVE_STATIC_V1,
                   static_arguments);
      } else {
        const sourcemeta::core::URITemplateRouter::Argument static_arguments[] =
            {{"errorSchema", std::string_view{error_schema}}};
        router.add("/self/v1/static/{+path}", "serve_static_asset", next_id++,
                   sourcemeta::one::ACTION_TYPE_SERVE_STATIC_V1,
                   static_arguments);
      }
    } else {
      router.otherwise(sourcemeta::one::ACTION_TYPE_DEFAULT_V1);
    }

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::core::URITemplateRouterView::save(router, action.destination);
  }
};

struct GENERATE_AUTHENTICATION {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration,
                      const sourcemeta::core::JSON &) -> void {
    // The policy paths and keys are borrowed by the policies, so keep them
    // alive alongside the policies that reference them
    std::vector<std::vector<std::string_view>> policy_paths;
    policy_paths.reserve(configuration.authentication.size());
    std::vector<std::vector<std::string_view>> policy_keys;
    policy_keys.reserve(configuration.authentication.size());
    std::vector<sourcemeta::one::Authentication::Policy> policies;
    policies.reserve(configuration.authentication.size());
    for (const auto &entry : configuration.authentication) {
      std::vector<std::string_view> paths;
      paths.reserve(entry.paths.size());
      for (const auto &path : entry.paths) {
        paths.push_back(path);
      }

      policy_paths.push_back(std::move(paths));

#if defined(SOURCEMETA_ONE_ENTERPRISE)
      // Enterprise supports every policy type and any path scope
      if (entry.type ==
          sourcemeta::one::Configuration::AuthenticationEntry::Type::ApiKey) {
        std::vector<std::string_view> keys;
        keys.reserve(entry.keys.size());
        for (const auto &key : entry.keys) {
          keys.push_back(key);
        }

        policy_keys.push_back(std::move(keys));
        policies.push_back({sourcemeta::one::Authentication::Type::ApiKey,
                            policy_paths.back(), policy_keys.back()});
      } else {
        policies.push_back({sourcemeta::one::Authentication::Type::Public,
                            policy_paths.back()});
      }
#else
      // The community edition only serves public access covering the whole
      // registry, so any other type or a non-root path is an enterprise
      // feature
      if (entry.type !=
          sourcemeta::one::Configuration::AuthenticationEntry::Type::Public) {
        throw EnterpriseOnlyFeatureError(
            configuration.path,
            "Authentication is only available on the enterprise edition");
      }

      if (std::ranges::any_of(entry.paths,
                              [](const auto &path) { return path != "/"; })) {
        throw EnterpriseOnlyFeatureError(
            configuration.path,
            "Authentication and non-root public paths are only available on "
            "the enterprise edition");
      }

      policies.push_back(
          {sourcemeta::one::Authentication::Type::Public, policy_paths.back()});
#endif
    }

    for (const auto &entry : configuration.authentication) {
      for (const auto &policy_path : entry.paths) {
        std::string_view scope{policy_path};
        if (scope.starts_with('/')) {
          scope.remove_prefix(1);
        }

        bool matched{scope.empty()};
        for (const auto &content : configuration.entries) {
          const auto target{content.first.generic_string()};
          if (is_segment_prefix(scope, target) ||
              is_segment_prefix(target, scope)) {
            matched = true;
            break;
          }
        }

        if (!matched) {
          throw AuthenticationUnknownPathError(std::string{policy_path});
        }
      }
    }

    std::filesystem::create_directories(action.destination.parent_path());
    sourcemeta::one::Authentication::save(policies, action.destination);
  }

private:
  static auto is_segment_prefix(const std::string_view prefix,
                                const std::string_view path) -> bool {
    if (prefix.empty() || prefix == path) {
      return true;
    }

    return path.size() > prefix.size() && path.starts_with(prefix) &&
           path[prefix.size()] == '/';
  }
};

} // namespace sourcemeta::one

#endif
