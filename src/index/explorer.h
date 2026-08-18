#ifndef SOURCEMETA_ONE_INDEX_EXPLORER_H_
#define SOURCEMETA_ONE_INDEX_EXPLORER_H_

#include "endpoints.h"

#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/search.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/blaze/foundation.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/semver.h>
#include <sourcemeta/core/text.h>

#include <sourcemeta/one/build.h>

#if defined(SOURCEMETA_ONE_ENTERPRISE)
#include <sourcemeta/one/enterprise_index.h>
#endif

#include <algorithm>     // std::ranges::sort
#include <cassert>       // assert
#include <chrono>        // std::chrono
#include <cmath>         // std::lround
#include <cstring>       // std::memcpy
#include <filesystem>    // std::filesystem
#include <limits>        // std::numeric_limits
#include <numeric>       // std::accumulate
#include <optional>      // std::optional
#include <queue>         // std::queue
#include <set>           // std::set
#include <sstream>       // std::ostringstream
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <tuple>         // std::tuple
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move, std::pair, std::unreachable
#include <vector>        // std::vector

static auto make_breadcrumb(const std::filesystem::path &relative_path,
                            const bool is_directory) -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_array()};
  std::filesystem::path current_path{"/"};
  const auto parts_count{
      std::distance(relative_path.begin(), relative_path.end())};
  std::size_t index{0};
  for (const auto &part : relative_path) {
    current_path = current_path / part;
    auto entry{sourcemeta::core::JSON::make_object()};
    entry.assign("name", sourcemeta::core::JSON{part});
    const auto is_last{index == static_cast<std::size_t>(parts_count - 1)};
    // Add trailing slash to directory paths to distinguish between
    // schema and directory entries which might have the same name
    if (!is_last || is_directory) {
      entry.assign("path", sourcemeta::core::JSON{current_path.string() + "/"});
    } else {
      entry.assign("path", sourcemeta::core::JSON{current_path});
    }

    result.push_back(std::move(entry));
    index++;
  }

  return result;
}

static auto
inflate_metadata(const sourcemeta::one::Configuration &configuration,
                 const std::filesystem::path &path,
                 sourcemeta::core::JSON &target) -> void {
  const auto match{configuration.entries.find(path)};
  if (match == configuration.entries.cend()) {
    return;
  }

  std::visit(
      [&target](const auto &entry) {
        if (entry.title.has_value()) {
          target.assign_if_missing(
              "title", sourcemeta::core::to_json(entry.title.value()));
        }

        if (entry.description.has_value()) {
          target.assign_if_missing(
              "description",
              sourcemeta::core::to_json(entry.description.value()));
        }

        if (entry.email.has_value()) {
          target.assign_if_missing(
              "email", sourcemeta::core::to_json(entry.email.value()));
        }

        if (entry.github.has_value()) {
          target.assign_if_missing(
              "github", sourcemeta::core::to_json(entry.github.value()));
        }

        if (entry.website.has_value()) {
          target.assign_if_missing(
              "website", sourcemeta::core::to_json(entry.website.value()));
        }
      },
      match->second);
}

static auto child_registry_path(const std::string &directory_registry_path,
                                const std::string &name) -> std::string {
  return (directory_registry_path == "/" ? std::string{}
                                         : directory_registry_path) +
         "/" + name;
}

static auto
make_private(const sourcemeta::one::Authentication::Table &authentication,
             const std::string &registry_path) -> sourcemeta::core::JSON {
  // Whether a policy governs the path, declared on it or inherited from above.
  // Every surface that says so reads this, so none of them can disagree about
  // what private means. The indexer composes the path from the content tree,
  // so it is already relative to the instance root
  // A table that could not be read knows nothing about who governs what, so
  // every location is described as private rather than as open to everybody
  const auto governing{authentication.governing(
      sourcemeta::one::Authentication::Path::relative(registry_path))};
  return sourcemeta::core::JSON{!governing.has_value() ||
                                !governing.value().empty()};
}

namespace sourcemeta::one {

#pragma pack(push, 1)
struct MetapackVersionInfo {
  std::uint8_t is_version;
  std::uint32_t major;
  std::uint32_t minor;
  std::uint32_t patch;
};

struct MetapackExplorerSchemaExtension {
  std::int64_t health;
  std::int64_t bytes;
  std::int64_t bytes_bundled;
  std::int64_t dependencies;
  MetapackVersionInfo version;
  std::uint8_t priority;
  std::uint16_t path_length;
  std::uint16_t identifier_length;
  std::uint16_t base_dialect_length;
  std::uint16_t dialect_length;
  std::uint16_t title_length;
  std::uint16_t description_length;
  std::uint16_t alert_length;
};

struct MetapackDirectoryExtension {
  MetapackVersionInfo version;
  std::int64_t health;
  std::int64_t schemas;
  std::uint16_t path_length;
  std::uint16_t title_length;
  std::uint16_t description_length;
  std::uint16_t email_length;
  std::uint16_t github_length;
  std::uint16_t website_length;
};
#pragma pack(pop)

static auto parse_version_info(const std::string_view name)
    -> MetapackVersionInfo {
  const auto version{sourcemeta::core::SemVer::from(
      name, sourcemeta::core::SemVer::Mode::Loose)};
  if (version.has_value()) {
    return {1, static_cast<std::uint32_t>(version->major()),
            static_cast<std::uint32_t>(version->minor()),
            static_cast<std::uint32_t>(version->patch())};
  }

  return {0, 0, 0, 0};
}

inline auto directory_extension_string(const MetapackDirectoryExtension *,
                                       const std::uint8_t *base,
                                       const std::size_t field_offset,
                                       const std::size_t field_length)
    -> std::string_view {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return {reinterpret_cast<const char *>(
              base + sizeof(MetapackDirectoryExtension) + field_offset),
          field_length};
}

static auto make_directory_extension(
    const MetapackVersionInfo &version, const std::int64_t health,
    const std::int64_t schemas, const std::string_view path,
    const std::string_view title, const std::string_view description,
    const std::string_view email, const std::string_view github,
    const std::string_view website) -> std::vector<std::uint8_t> {
  assert(path.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(title.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(description.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(email.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(github.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(website.size() <= std::numeric_limits<std::uint16_t>::max());

  const auto strings_size{path.size() + title.size() + description.size() +
                          email.size() + github.size() + website.size()};
  std::vector<std::uint8_t> result;
  result.resize(sizeof(MetapackDirectoryExtension) + strings_size);

  MetapackDirectoryExtension header{};
  header.version = version;
  header.health = health;
  header.schemas = schemas;
  header.path_length = static_cast<std::uint16_t>(path.size());
  header.title_length = static_cast<std::uint16_t>(title.size());
  header.description_length = static_cast<std::uint16_t>(description.size());
  header.email_length = static_cast<std::uint16_t>(email.size());
  header.github_length = static_cast<std::uint16_t>(github.size());
  header.website_length = static_cast<std::uint16_t>(website.size());

  auto *cursor{result.data()};
  std::memcpy(cursor, &header, sizeof(header));
  cursor += sizeof(header);

  const auto append = [&cursor](const std::string_view string) {
    std::memcpy(cursor, string.data(), string.size());
    cursor += string.size();
  };

  append(path);
  append(title);
  append(description);
  append(email);
  append(github);
  append(website);

  return result;
}

inline auto explorer_extension_string(const MetapackExplorerSchemaExtension *,
                                      const std::uint8_t *base,
                                      const std::size_t field_offset,
                                      const std::size_t field_length)
    -> std::string_view {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return {reinterpret_cast<const char *>(
              base + sizeof(MetapackExplorerSchemaExtension) + field_offset),
          field_length};
}

inline auto
explorer_extension_path(const MetapackExplorerSchemaExtension *extension,
                        const std::uint8_t *base) -> std::string_view {
  return explorer_extension_string(extension, base, 0, extension->path_length);
}

inline auto
explorer_extension_identifier(const MetapackExplorerSchemaExtension *extension,
                              const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{extension->path_length};
  return explorer_extension_string(extension, base, offset,
                                   extension->identifier_length);
}

inline auto explorer_extension_base_dialect(
    const MetapackExplorerSchemaExtension *extension, const std::uint8_t *base)
    -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length};
  return explorer_extension_string(extension, base, offset,
                                   extension->base_dialect_length);
}

inline auto
explorer_extension_dialect(const MetapackExplorerSchemaExtension *extension,
                           const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length +
                           extension->base_dialect_length};
  return explorer_extension_string(extension, base, offset,
                                   extension->dialect_length);
}

inline auto
explorer_extension_title(const MetapackExplorerSchemaExtension *extension,
                         const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length +
                           extension->base_dialect_length +
                           extension->dialect_length};
  return explorer_extension_string(extension, base, offset,
                                   extension->title_length);
}

inline auto
explorer_extension_description(const MetapackExplorerSchemaExtension *extension,
                               const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length +
                           extension->base_dialect_length +
                           extension->dialect_length + extension->title_length};
  return explorer_extension_string(extension, base, offset,
                                   extension->description_length);
}

inline auto
explorer_extension_alert(const MetapackExplorerSchemaExtension *extension,
                         const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{static_cast<std::size_t>(extension->path_length) +
                           extension->identifier_length +
                           extension->base_dialect_length +
                           extension->dialect_length + extension->title_length +
                           extension->description_length};
  return explorer_extension_string(extension, base, offset,
                                   extension->alert_length);
}

static auto make_explorer_schema_extension(
    const std::int64_t health, const std::int64_t bytes,
    const std::int64_t bytes_bundled, const std::int64_t dependencies,
    const MetapackVersionInfo &version, const std::uint8_t priority,
    const std::string_view path, const std::string_view identifier,
    const std::string_view base_dialect, const std::string_view dialect,
    const std::string_view title, const std::string_view description,
    const std::string_view alert) -> std::vector<std::uint8_t> {
  assert(path.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(identifier.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(base_dialect.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(dialect.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(title.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(description.size() <= std::numeric_limits<std::uint16_t>::max());
  assert(alert.size() <= std::numeric_limits<std::uint16_t>::max());

  const auto strings_size{path.size() + identifier.size() +
                          base_dialect.size() + dialect.size() + title.size() +
                          description.size() + alert.size()};
  std::vector<std::uint8_t> result;
  result.resize(sizeof(MetapackExplorerSchemaExtension) + strings_size);

  MetapackExplorerSchemaExtension header{};
  header.health = health;
  header.bytes = bytes;
  header.bytes_bundled = bytes_bundled;
  header.dependencies = dependencies;
  header.version = version;
  header.priority = priority;
  header.path_length = static_cast<std::uint16_t>(path.size());
  header.identifier_length = static_cast<std::uint16_t>(identifier.size());
  header.base_dialect_length = static_cast<std::uint16_t>(base_dialect.size());
  header.dialect_length = static_cast<std::uint16_t>(dialect.size());
  header.title_length = static_cast<std::uint16_t>(title.size());
  header.description_length = static_cast<std::uint16_t>(description.size());
  header.alert_length = static_cast<std::uint16_t>(alert.size());

  auto *cursor{result.data()};
  std::memcpy(cursor, &header, sizeof(header));
  cursor += sizeof(header);

  const auto append = [&cursor](const std::string_view string) {
    std::memcpy(cursor, string.data(), string.size());
    cursor += string.size();
  };

  append(path);
  append(identifier);
  append(base_dialect);
  append(dialect);
  append(title);
  append(description);
  append(alert);

  return result;
}

struct GENERATE_EXPLORER_SCHEMA_METADATA {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto &resolver_entry{resolver.entry(action.data)};
    // Read the schema to get data and bytes
    sourcemeta::core::FileView schema_view{action.dependencies.front()};
    const auto schema_info_option{sourcemeta::one::metapack_info(schema_view)};
    assert(schema_info_option.has_value());
    const auto &schema_info{schema_info_option.value()};
    sourcemeta::core::FileView bundle_view{action.dependencies.at(3)};
    const auto bundle_info_option{sourcemeta::one::metapack_info(bundle_view)};
    assert(bundle_info_option.has_value());
    const auto &bundle_info{bundle_info_option.value()};
    const auto schema_data_option{
        sourcemeta::one::metapack_read_json(action.dependencies.front())};
    assert(schema_data_option.has_value());
    const auto &schema_data{schema_data_option.value()};
    const auto id{sourcemeta::blaze::identify(
        schema_data, [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        })};
    assert(!id.empty());
    auto result{sourcemeta::core::JSON::make_object()};

    result.assign("bytes", sourcemeta::core::JSON{static_cast<std::size_t>(
                               schema_info.content_bytes)});
    result.assign("bytesBundled",
                  sourcemeta::core::JSON{
                      static_cast<std::size_t>(bundle_info.content_bytes)});
    result.assign("identifier", sourcemeta::core::JSON{id});
    result.assign("path", sourcemeta::core::JSON{
                              "/" + resolver_entry.relative_path.string()});
    const auto base_dialect{sourcemeta::blaze::base_dialect(
        schema_data, [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        })};
    assert(base_dialect.has_value());
    result.assign("baseDialect",
                  sourcemeta::core::JSON{
                      sourcemeta::blaze::to_string(base_dialect.value())});
    const auto dialect{sourcemeta::blaze::dialect(schema_data)};
    assert(!dialect.empty());
    result.assign("dialect", sourcemeta::core::JSON{dialect});

    if (schema_data.is_object()) {
      const auto title{schema_data.try_at("title")};
      if (title && title->is_string()) {
        result.assign("title", sourcemeta::core::JSON{title->trim()});
      }
      const auto description{schema_data.try_at("description")};
      if (description && description->is_string()) {
        result.assign("description",
                      sourcemeta::core::JSON{description->trim()});
      }

      auto examples_array{sourcemeta::core::JSON::make_array()};
      const auto *examples{schema_data.try_at("examples")};
      if (examples && examples->is_array() && !examples->empty()) {
        const auto vocabularies{sourcemeta::blaze::vocabularies(
            [&callback, &resolver](const auto identifier) {
              return resolver(identifier, callback);
            },
            base_dialect.value(), dialect)};
        const auto &walker_result{
            sourcemeta::blaze::schema_walker("examples", vocabularies)};
        if (walker_result.type ==
                sourcemeta::blaze::SchemaKeywordType::Annotation ||
            walker_result.type ==
                sourcemeta::blaze::SchemaKeywordType::Comment) {
          constexpr std::size_t EXAMPLES_MAXIMUM{10};
          for (std::size_t cursor = 0;
               cursor < std::min(EXAMPLES_MAXIMUM, examples->size());
               cursor++) {
            examples_array.push_back(examples->at(cursor));
          }
        }
      }

      result.assign("examples", std::move(examples_array));
    }

    const auto health_option{
        sourcemeta::one::metapack_read_json(action.dependencies.at(1))};
    assert(health_option.has_value());
    const auto &health{health_option.value()};
    result.assign("health", health.at("score"));

    const auto schema_dependencies_option{
        sourcemeta::one::metapack_read_json(action.dependencies.at(2))};
    assert(schema_dependencies_option.has_value());
    const auto &schema_dependencies{schema_dependencies_option.value()};
    result.assign("dependencies",
                  sourcemeta::core::to_json(schema_dependencies.size()));

    const auto &collection{*resolver_entry.collection};

    if (collection.extra.defines("x-sourcemeta-one:alert")) {
      assert(collection.extra.at("x-sourcemeta-one:alert").is_string());
      result.assign("alert", collection.extra.at("x-sourcemeta-one:alert"));
    } else {
      result.assign("alert", sourcemeta::core::JSON{nullptr});
    }

    result.assign(
        "priority",
        sourcemeta::core::JSON{static_cast<sourcemeta::core::JSON::Integer>(
            sourcemeta::one::Configuration::priority(collection))});

    result.assign("breadcrumb",
                  make_breadcrumb(resolver_entry.relative_path, false));

    const sourcemeta::one::Authentication::Table authentication{
        action.dependencies.back()};
    result.assign("private",
                  make_private(authentication, result.at("path").to_string()));

    const auto timestamp_end{std::chrono::steady_clock::now()};

    const auto schema_name{
        action.destination.parent_path().parent_path().filename().string()};
    // The binary extension format stores per-field lengths as `uint16_t`,
    // so title and description that overflow that cap must be truncated
    // before they are packed. The full strings remain in the JSON above
    constexpr auto MAX_EXTENSION_FIELD_LENGTH{
        static_cast<std::size_t>(std::numeric_limits<std::uint16_t>::max())};
    constexpr std::string_view EXTENSION_TRUNCATION_MARKER{"..."};
    std::string extension_title{result.defines("title")
                                    ? result.at("title").to_string()
                                    : std::string{}};
    sourcemeta::core::truncate(extension_title,
                               MAX_EXTENSION_FIELD_LENGTH -
                                   EXTENSION_TRUNCATION_MARKER.size(),
                               EXTENSION_TRUNCATION_MARKER);
    std::string extension_description{result.defines("description")
                                          ? result.at("description").to_string()
                                          : std::string{}};
    sourcemeta::core::truncate(extension_description,
                               MAX_EXTENSION_FIELD_LENGTH -
                                   EXTENSION_TRUNCATION_MARKER.size(),
                               EXTENSION_TRUNCATION_MARKER);
    const auto extension_bytes{make_explorer_schema_extension(
        result.at("health").to_integer(),
        static_cast<std::int64_t>(schema_info.content_bytes),
        static_cast<std::int64_t>(bundle_info.content_bytes),
        result.at("dependencies").to_integer(), parse_version_info(schema_name),
        static_cast<std::uint8_t>(result.at("priority").to_integer()),
        result.at("path").to_string(), result.at("identifier").to_string(),
        result.at("baseDialect").to_string(), result.at("dialect").to_string(),
        extension_title, extension_description,
        result.at("alert").is_string() ? result.at("alert").to_string() : "")};

    sourcemeta::one::metapack_write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP,
        std::span<const std::uint8_t>{extension_bytes},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
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
    // What this reads is chosen for it, so a referrer a view is without never
    // reaches here and there is nothing to leave out
    DirectMap direct;
    for (const auto &dependency : action.dependencies) {
      if (dependency.filename() == "authentication.bin") {
        continue;
      }

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

    // Only this leaf's transitive dependents are needed, so traverse the
    // reverse graph from it alone rather than computing the closure for every
    // node and discarding all but one
    std::set<std::tuple<sourcemeta::core::JSON::String,
                        sourcemeta::core::JSON::String,
                        sourcemeta::core::JSON::String>>
        edges;
    const sourcemeta::core::JSON::String origin{action.data};
    std::unordered_set<sourcemeta::core::JSON::StringView> visited;
    visited.emplace(origin);
    std::queue<sourcemeta::core::JSON::String> queue;
    queue.emplace(origin);
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

    auto result{sourcemeta::core::JSON::make_array()};
    for (const auto &[from, to, at] : edges) {
      auto object{sourcemeta::core::JSON::make_object()};
      object.assign("from", sourcemeta::core::JSON{from});
      object.assign("to", sourcemeta::core::JSON{to});
      object.assign("at", sourcemeta::core::JSON{at});
      result.push_back(std::move(object));
    }

    const auto timestamp_end{std::chrono::steady_clock::now()};

    sourcemeta::one::metapack_write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

struct GENERATE_EXPLORER_SEARCH_INDEX {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    std::vector<sourcemeta::one::SearchEntry> entries;

    for (const auto &dependency : action.dependencies) {
      const auto directory_option{
          sourcemeta::one::metapack_read_json(dependency)};
      assert(directory_option.has_value());
      const auto &directory{directory_option.value()};
      assert(directory.is_object());
      assert(directory.defines("entries"));

      for (const auto &directory_entry : directory.at("entries").as_array()) {
        if (!directory_entry.defines("type") ||
            directory_entry.at("type").to_string() != "schema") {
          continue;
        }

        entries.push_back(
            {directory_entry.at("path").to_string(),
             directory_entry.at("identifier").to_string(),
             directory_entry.defines("title")
                 ? directory_entry.at("title").to_string()
                 : "",
             directory_entry.defines("description")
                 ? directory_entry.at("description").to_string()
                 : "",
             directory_entry.defines("health")
                 ? static_cast<std::uint8_t>(
                       directory_entry.at("health").to_integer())
                 : static_cast<std::uint8_t>(0),
             directory_entry.defines("priority")
                 ? static_cast<std::uint8_t>(
                       directory_entry.at("priority").to_integer())
                 : static_cast<std::uint8_t>(100),
             directory_entry.defines("bytes")
                 ? static_cast<std::uint64_t>(
                       directory_entry.at("bytes").to_integer())
                 : static_cast<std::uint64_t>(0),
             directory_entry.defines("bytesBundled")
                 ? static_cast<std::uint64_t>(
                       directory_entry.at("bytesBundled").to_integer())
                 : static_cast<std::uint64_t>(0)});
      }
    }

    const auto payload{sourcemeta::one::make_search(std::move(entries))};
    const auto timestamp_end{std::chrono::steady_clock::now()};

    const std::string_view payload_view{
        payload.empty()
            ? std::string_view{}
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            : std::string_view{reinterpret_cast<const char *>(payload.data()),
                               payload.size()}};
    sourcemeta::one::metapack_write_text(
        action.destination, payload_view, "application/octet-stream",
        // We don't want to compress this one so we can
        // quickly skim through it while streaming it
        sourcemeta::one::MetapackEncoding::Identity, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

// The ways of signing in, written once for the whole instance. The page that
// offers them renders from this and nothing else, so what a person is shown and
// what a custom interface reads cannot describe different instances
struct GENERATE_LOGIN {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};

    auto document{sourcemeta::core::JSON::make_object()};
    if (configuration.html.has_value()) {
      document.assign("title",
                      sourcemeta::core::JSON{configuration.html->name});
    }

    // A policy that admits a program has nowhere to send a person, so naming it
    // here would offer a way in that does not exist
    auto providers{sourcemeta::core::JSON::make_array()};
    for (const auto &policy : configuration.authentication) {
      if (!sourcemeta::one::is_interactive(policy)) {
        continue;
      }

      auto provider{sourcemeta::core::JSON::make_object()};
      provider.assign("name", sourcemeta::core::JSON{policy.name});
      provider.assign("title", sourcemeta::core::JSON{policy.title});
      std::string path{sourcemeta::one::ENDPOINT_AUTH_LOGIN_PAGE};
      path.push_back('/');
      path.append(policy.name);
      provider.assign("path", sourcemeta::core::JSON{std::move(path)});
      providers.push_back(std::move(provider));
    }

    document.assign("providers", std::move(providers));

    const auto timestamp_end{std::chrono::steady_clock::now()};
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, document, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

// What a listing calls a schema, and how many it names before it hands out a
// cursor. Both are settled here, since the pages are written rather than
// assembled
inline constexpr std::string_view MCP_RESOURCE_MIME_TYPE{
    "application/schema+json"};
inline constexpr std::size_t MCP_RESOURCES_PAGE_SIZE{50};

struct GENERATE_MCP {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};

#if defined(SOURCEMETA_ONE_ENTERPRISE)
    constexpr std::string_view SERVER_NAME{"sourcemeta-one-enterprise"};
    constexpr std::string_view SERVER_TITLE{"Sourcemeta One Enterprise"};
#else
    constexpr std::string_view SERVER_NAME{"sourcemeta-one"};
    constexpr std::string_view SERVER_TITLE{"Sourcemeta One"};
#endif

    constexpr std::string_view INSTRUCTIONS_BODY{
        "Sourcemeta One is a JSON Schema registry. It serves a catalog of "
        "JSON Schemas organized as a tree of directories. Every schema is "
        "exposed as an MCP resource via the `JSON Schema` resource template "
        "(see `resources/templates/list`) and is addressable by its "
        "canonical absolute URI. This instance is sovereign over its own "
        "URL namespace. Schemas whose URIs lie outside this namespace will "
        "not be served here. Use `tools/list` to discover the operations "
        "this catalog exposes. Because this server is a JSON Schema "
        "registry, the schemas defining each tool's input parameters are "
        "themselves resources in this registry. To inspect a tool's full "
        "parameter shape, call `resources/read` on the `$ref` URL in that "
        "tool's `inputSchema`. Learn more at https://one.sourcemeta.com"};

    std::ostringstream instructions;
    if (configuration.html.has_value()) {
      instructions << "This is an instance of Sourcemeta One named \""
                   << configuration.html->name << "\" ("
                   << configuration.html->description << "). ";
    } else {
      instructions << "This is an instance of Sourcemeta One. ";
    }
    instructions << INSTRUCTIONS_BODY;

    std::string template_uri{configuration.url};
    if (!template_uri.empty() && template_uri.back() == '/') {
      template_uri.pop_back();
    }
    template_uri.append("/{+path}{?bundle}");

    auto resource_templates{sourcemeta::core::JSON::make_array()};
    resource_templates.push_back(sourcemeta::core::mcp_make_resource_template(
        template_uri, "JSON Schema",
        "A JSON Schema in this catalog, addressable by its canonical "
        "absolute URI under this server's origin. This instance is "
        "sovereign over its own URL namespace. Schemas whose URIs lie "
        "outside this namespace will not be served here. Substitute "
        "`{+path}` with the schema's catalog path. Include `{?bundle}` "
        "(its presence alone triggers bundling, any value is ignored) "
        "to receive the schema with every external `$ref` inlined into "
        "a single self-contained document. The URI must not contain a "
        "fragment. Use `resources/list` to discover the available "
        "schemas rather than guessing paths",
        "application/schema+json"));

    auto tools{sourcemeta::core::JSON::make_array()};
    auto tool_routes{sourcemeta::core::JSON::make_object()};
    auto protected_resource_metadata{sourcemeta::core::JSON{nullptr}};
    std::string resource_identifier;

#if defined(SOURCEMETA_ONE_ENTERPRISE)
    {
      const sourcemeta::core::URITemplateRouterView router_view{
          action.dependencies.at(2)};
      const sourcemeta::one::Authentication::Table authentication{
          action.dependencies.back()};

      // The artifact is written once per view and names the view it is for, so
      // what it offers is settled here rather than worked out again per request
      const auto view{authentication.view(action.view)};

      sourcemeta::one::generate_mcp_tools(
          router_view,
          [&authentication, view](const std::string_view uri_template) {
            return authentication.visible(
                sourcemeta::one::Authentication::Path::relative(
                    sourcemeta::one::route_scope(uri_template)),
                view);
          },
          tools, tool_routes);
      sourcemeta::one::generate_protected_resource_metadata(
          authentication, configuration, sourcemeta::one::ENDPOINT_MCP,
          protected_resource_metadata);
      resource_identifier = sourcemeta::one::mcp_resource_identifier(
          configuration, sourcemeta::one::ENDPOINT_MCP);
    }
#endif

    auto initialize_ingredients{sourcemeta::core::JSON::make_array()};
    initialize_ingredients.push_back(sourcemeta::core::JSON{false});
    initialize_ingredients.push_back(sourcemeta::core::JSON{true});
    initialize_ingredients.push_back(sourcemeta::core::JSON{!tools.empty()});
    initialize_ingredients.push_back(sourcemeta::core::JSON{false});
    initialize_ingredients.push_back(sourcemeta::core::JSON{false});
    initialize_ingredients.push_back(sourcemeta::core::JSON{SERVER_NAME});
    initialize_ingredients.push_back(
        sourcemeta::core::JSON{sourcemeta::one::version()});
    initialize_ingredients.push_back(sourcemeta::core::JSON{SERVER_TITLE});
    initialize_ingredients.push_back(sourcemeta::core::JSON{
        configuration.html.has_value()
            ? std::string_view{configuration.html->description}
            : std::string_view{}});
    initialize_ingredients.push_back(sourcemeta::core::JSON{
        configuration.html.has_value() ? std::string_view{configuration.url}
                                       : std::string_view{}});
    initialize_ingredients.push_back(
        sourcemeta::core::JSON{instructions.str()});

    auto resource_templates_response{sourcemeta::core::JSON::make_object()};
    resource_templates_response.assign("resourceTemplates",
                                       std::move(resource_templates));

    // Every page a caller might ask for, written once for the view rather than
    // put together on each request. What a page holds follows from the index it
    // is built from, and that index already holds what this view may reach, so
    // the answer depends on neither who asks nor when
    //
    // TODO: Keep the pages out of the document that is parsed at startup. What
    // is wanted is that answering reads an answer rather than builds one, and
    // that does not require the answer to be a document. As written, every page
    // of every view is materialised here and the whole of it is held parsed for
    // as long as the server runs, which grows with the catalog and again with
    // the number of views, where the search index beside it costs a mapping and
    // no more. Worth exploring holding them pre-serialised, or in the mapped
    // shape the search index already uses, so a page is written to the socket
    // rather than reassembled from a tree
    auto resource_pages{sourcemeta::core::JSON::make_array()};
    {
      sourcemeta::one::SearchView search{action.dependencies.front()};
      const auto total{search.count()};
      std::size_t offset{0};
      do {
        auto resources{sourcemeta::core::JSON::make_array()};
        search.for_each(
            offset, MCP_RESOURCES_PAGE_SIZE,
            [&configuration, &resources](
                const sourcemeta::one::SearchListEntry &entry) -> void {
              std::string uri{configuration.origin};
              uri.append(entry.path);
              resources.push_back(sourcemeta::core::mcp_make_resource(
                  uri, entry.title.empty() ? entry.path : entry.title,
                  MCP_RESOURCE_MIME_TYPE, entry.description,
                  static_cast<std::size_t>(entry.bytes_raw),
                  static_cast<double>(entry.priority) / 100.0));
            });

        auto page{sourcemeta::core::JSON::make_object()};
        page.assign("resources", std::move(resources));
        if (offset + MCP_RESOURCES_PAGE_SIZE < total) {
          page.assign("nextCursor", sourcemeta::core::JSON{std::to_string(
                                        offset + MCP_RESOURCES_PAGE_SIZE)});
        }

        resource_pages.push_back(std::move(page));
        offset += MCP_RESOURCES_PAGE_SIZE;
        // A catalog holding nothing still has a first page, so that asking for
        // the beginning is answered rather than refused
      } while (offset < total);
    }

    auto document{sourcemeta::core::JSON::make_object()};
    document.assign("origin", sourcemeta::core::JSON{configuration.origin});
    document.assign(std::string{sourcemeta::core::MCP_METHOD_RESOURCES_LIST},
                    std::move(resource_pages));
    // How many the pages above were cut at, so that whoever reads them maps a
    // cursor the way they were actually written rather than the way a second
    // copy of this number happens to say. An artifact outlives the build that
    // wrote it, and the two are then free to disagree
    document.assign(
        "resourcePageSize",
        sourcemeta::core::JSON{static_cast<sourcemeta::core::JSON::Integer>(
            MCP_RESOURCES_PAGE_SIZE)});
    document.assign(std::string{sourcemeta::core::MCP_METHOD_INITIALIZE},
                    std::move(initialize_ingredients));
    document.assign(
        std::string{sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST},
        std::move(resource_templates_response));
    document.assign(std::string{sourcemeta::core::MCP_METHOD_TOOLS_LIST},
                    std::move(tools));
    document.assign("toolRoutes", std::move(tool_routes));
    if (!resource_identifier.empty()) {
      document.assign("resourceIdentifier",
                      sourcemeta::core::JSON{std::move(resource_identifier)});
    }
    if (!protected_resource_metadata.is_null()) {
      document.assign("protectedResourceMetadata",
                      std::move(protected_resource_metadata));
    }

    const auto timestamp_end{std::chrono::steady_clock::now()};
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, document, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

// The mutable build state is only safe to read while holding its lock, as
// these reads run concurrently with commits from sibling actions
static auto in_overlay_synchronised(const sourcemeta::one::BuildState &state,
                                    const std::string &key) -> bool {
  const auto lock{state.take_lock()};
  return state.in_overlay(key);
}

struct GENERATE_EXPLORER_DIRECTORY_LIST {
  static auto handler(const sourcemeta::one::BuildState &state,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration,
                      const sourcemeta::core::JSON &) -> void {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    auto entries{sourcemeta::core::JSON::make_array()};
    std::vector<sourcemeta::core::JSON::Integer> scores;
    std::int64_t child_schemas_total{0};

    const auto directory_path{action.destination.parent_path().parent_path()};
    std::filesystem::path relative_path;
    auto current{directory_path};
    // The view tree names the view before the path it describes, so what this
    // listing is a listing of starts one segment below the tree itself
    while (current.has_filename()) {
      if (current.parent_path().filename() == "explorer") {
        relative_path = std::filesystem::relative(directory_path, current);
        break;
      }

      current = current.parent_path();
    }

    const sourcemeta::one::Authentication::Table authentication{
        action.dependencies.back()};
    const std::string directory_registry_path{
        relative_path == "." ? std::string{"/"}
                             : "/" + relative_path.generic_string()};

    struct SortableEntry {
      sourcemeta::core::JSON json;
      MetapackVersionInfo version;
      std::string name;
    };

    std::vector<SortableEntry> directory_entries;
    std::vector<SortableEntry> schema_entries;

    const auto old_listing{
        std::filesystem::exists(action.destination)
            ? sourcemeta::one::metapack_read_json(action.destination)
            : std::nullopt};
    std::unordered_map<std::string_view, const sourcemeta::core::JSON *>
        old_schema_entries;
    std::unordered_map<std::string_view, const sourcemeta::core::JSON *>
        old_directory_entries;
    if (old_listing.has_value()) {
      assert(old_listing->defines("entries"));
      for (const auto &old_entry : old_listing->at("entries").as_array()) {
        assert(old_entry.defines("name"));
        const auto &old_name{old_entry.at("name").to_string()};
        if (old_entry.defines("type") &&
            old_entry.at("type").to_string() == "directory") {
          old_directory_entries.emplace(old_name, &old_entry);
        } else {
          old_schema_entries.emplace(old_name, &old_entry);
        }
      }
    }

    for (const auto &dependency : action.dependencies) {
      const auto filename{dependency.filename().string()};
      const auto child_name{
          dependency.parent_path().parent_path().filename().string()};

      if (filename == "directory.metapack") {
        if (old_listing.has_value() &&
            !in_overlay_synchronised(state, dependency.native())) {
          auto it = old_directory_entries.find(child_name);
          if (it != old_directory_entries.end()) {
            const auto &old_entry{*it->second};
            if (old_entry.defines("health")) {
              scores.emplace_back(old_entry.at("health").to_integer());
            }
            if (old_entry.defines("schemas")) {
              assert(old_entry.at("schemas").is_positive());
              child_schemas_total += old_entry.at("schemas").to_integer();
            }
            directory_entries.push_back(
                {old_entry, parse_version_info(child_name), child_name});
            continue;
          }
        }

        sourcemeta::core::FileView directory_view{dependency};
        const auto directory_extension_offset{
            sourcemeta::one::metapack_extension_offset(directory_view)};
        const auto *directory_extension{
            sourcemeta::one::metapack_extension<MetapackDirectoryExtension>(
                directory_view)};

        auto entry_json{sourcemeta::core::JSON::make_object()};
        entry_json.assign("name", sourcemeta::core::JSON{child_name});
        entry_json.assign("type", sourcemeta::core::JSON{"directory"});
        MetapackVersionInfo directory_version{};

        if (directory_extension != nullptr && directory_extension_offset != 0) {
          const auto *directory_base{
              directory_view.as<std::uint8_t>(directory_extension_offset)};
          directory_version = directory_extension->version;

          entry_json.assign(
              "health", sourcemeta::core::JSON{directory_extension->health});
          scores.emplace_back(directory_extension->health);
          assert(directory_extension->schemas >= 0);
          entry_json.assign(
              "schemas", sourcemeta::core::JSON{directory_extension->schemas});
          child_schemas_total += directory_extension->schemas;

          const auto directory_path_string{
              directory_extension_string(directory_extension, directory_base, 0,
                                         directory_extension->path_length)};
          entry_json.assign(
              "path",
              sourcemeta::core::JSON{std::string{directory_path_string} + "/"});

          const std::size_t title_offset{directory_extension->path_length};
          const auto directory_title{directory_extension_string(
              directory_extension, directory_base, title_offset,
              directory_extension->title_length)};
          if (!directory_title.empty()) {
            entry_json.assign("title", sourcemeta::core::JSON{directory_title});
          }

          const std::size_t description_offset{
              title_offset + directory_extension->title_length};
          const auto directory_description{directory_extension_string(
              directory_extension, directory_base, description_offset,
              directory_extension->description_length)};
          if (!directory_description.empty()) {
            entry_json.assign("description",
                              sourcemeta::core::JSON{directory_description});
          }

          const std::size_t email_offset{
              description_offset + directory_extension->description_length};
          const auto directory_email{directory_extension_string(
              directory_extension, directory_base, email_offset,
              directory_extension->email_length)};
          if (!directory_email.empty()) {
            entry_json.assign("email", sourcemeta::core::JSON{directory_email});
          }

          const std::size_t github_offset{email_offset +
                                          directory_extension->email_length};
          const auto directory_github{directory_extension_string(
              directory_extension, directory_base, github_offset,
              directory_extension->github_length)};
          if (!directory_github.empty()) {
            entry_json.assign("github",
                              sourcemeta::core::JSON{directory_github});
          }

          const std::size_t website_offset{github_offset +
                                           directory_extension->github_length};
          const auto directory_website{directory_extension_string(
              directory_extension, directory_base, website_offset,
              directory_extension->website_length)};
          if (!directory_website.empty()) {
            entry_json.assign("website",
                              sourcemeta::core::JSON{directory_website});
          }
        } else {
          directory_version = parse_version_info(child_name);

          auto directory_json_option{
              sourcemeta::one::metapack_read_json(dependency)};
          assert(directory_json_option.has_value());
          auto directory_json{std::move(directory_json_option.value())};
          assert(directory_json.is_object());
          assert(directory_json.defines("health"));
          assert(directory_json.at("health").is_integer());
          scores.emplace_back(directory_json.at("health").to_integer());

          entry_json.assign("health", directory_json.at("health"));
          assert(directory_json.defines("schemas"));
          assert(directory_json.at("schemas").is_integer());
          assert(directory_json.at("schemas").is_positive());
          entry_json.assign("schemas", directory_json.at("schemas"));
          child_schemas_total += directory_json.at("schemas").to_integer();
          assert(directory_json.defines("path"));
          entry_json.assign("path",
                            sourcemeta::core::JSON{
                                directory_json.at("path").to_string() + "/"});
          if (directory_json.defines("title")) {
            entry_json.assign("title", directory_json.at("title"));
          }
          if (directory_json.defines("description")) {
            entry_json.assign("description", directory_json.at("description"));
          }
          if (directory_json.defines("email")) {
            entry_json.assign("email", directory_json.at("email"));
          }
          if (directory_json.defines("github")) {
            entry_json.assign("github", directory_json.at("github"));
          }
          if (directory_json.defines("website")) {
            entry_json.assign("website", directory_json.at("website"));
          }
        }

        directory_entries.push_back(
            {std::move(entry_json), directory_version, {}});
        directory_entries.back().name =
            directory_entries.back().json.at("name").to_string();
      } else if (filename == "schema.metapack") {
        if (old_listing.has_value() &&
            !in_overlay_synchronised(state, dependency.native())) {
          auto it = old_schema_entries.find(child_name);
          if (it != old_schema_entries.end()) {
            const auto &old_entry{*it->second};
            if (old_entry.defines("health")) {
              scores.emplace_back(old_entry.at("health").to_integer());
            }
            schema_entries.push_back(
                {old_entry, parse_version_info(child_name), child_name});
            continue;
          }
        }

        sourcemeta::core::FileView dependency_view{dependency};
        const auto extension_offset{
            sourcemeta::one::metapack_extension_offset(dependency_view)};
        const auto *extension{sourcemeta::one::metapack_extension<
            MetapackExplorerSchemaExtension>(dependency_view)};

        if (extension == nullptr || extension_offset == 0) {
          continue;
        }

        const auto *extension_base{
            dependency_view.as<std::uint8_t>(extension_offset)};
        auto entry_json{sourcemeta::core::JSON::make_object()};
        entry_json.assign("name", sourcemeta::core::JSON{child_name});
        entry_json.assign("type", sourcemeta::core::JSON{"schema"});

        const auto schema_path{
            explorer_extension_path(extension, extension_base)};
        entry_json.assign(
            "path", sourcemeta::core::JSON{std::filesystem::path{schema_path}});
        entry_json.assign("identifier",
                          sourcemeta::core::JSON{explorer_extension_identifier(
                              extension, extension_base)});
        entry_json.assign("bytes", sourcemeta::core::JSON{extension->bytes});
        entry_json.assign("bytesBundled",
                          sourcemeta::core::JSON{extension->bytes_bundled});
        entry_json.assign("baseDialect", sourcemeta::core::JSON{
                                             explorer_extension_base_dialect(
                                                 extension, extension_base)});
        entry_json.assign("dialect",
                          sourcemeta::core::JSON{explorer_extension_dialect(
                              extension, extension_base)});
        entry_json.assign("health", sourcemeta::core::JSON{extension->health});
        entry_json.assign("dependencies",
                          sourcemeta::core::JSON{extension->dependencies});
        entry_json.assign(
            "priority",
            sourcemeta::core::JSON{static_cast<sourcemeta::core::JSON::Integer>(
                extension->priority)});

        const auto title{explorer_extension_title(extension, extension_base)};
        if (!title.empty()) {
          entry_json.assign("title", sourcemeta::core::JSON{title});
        }

        const auto description{
            explorer_extension_description(extension, extension_base)};
        if (!description.empty()) {
          entry_json.assign("description", sourcemeta::core::JSON{description});
        }

        const auto alert{explorer_extension_alert(extension, extension_base)};
        if (!alert.empty()) {
          entry_json.assign("alert", sourcemeta::core::JSON{alert});
        } else {
          entry_json.assign("alert", sourcemeta::core::JSON{nullptr});
        }

        scores.emplace_back(extension->health);
        schema_entries.push_back(
            {std::move(entry_json), extension->version, {}});
        schema_entries.back().name =
            schema_entries.back().json.at("name").to_string();
      }
    }

    const auto version_comparator = [](const SortableEntry &left,
                                       const SortableEntry &right) -> bool {
      if (left.version.is_version != right.version.is_version) {
        return left.version.is_version > right.version.is_version;
      }

      if (left.version.is_version && right.version.is_version) {
        if (left.version.major != right.version.major)
          return left.version.major > right.version.major;
        if (left.version.minor != right.version.minor)
          return left.version.minor > right.version.minor;
        return left.version.patch > right.version.patch;
      }

      return left.name < right.name;
    };

    std::ranges::sort(directory_entries, version_comparator);
    std::ranges::sort(schema_entries, version_comparator);

    for (auto &entry : directory_entries) {
      entry.json.assign(
          "private", make_private(authentication,
                                  child_registry_path(directory_registry_path,
                                                      entry.name)));
      entries.push_back(std::move(entry.json));
    }
    for (auto &entry : schema_entries) {
      entry.json.assign(
          "private", make_private(authentication,
                                  child_registry_path(directory_registry_path,
                                                      entry.name)));
      entries.push_back(std::move(entry.json));
    }

    auto meta{sourcemeta::core::JSON::make_object()};

    inflate_metadata(configuration, relative_path, meta);

    if (!scores.empty()) {
      const auto accumulated_health = static_cast<int>(
          std::lround(static_cast<double>(std::accumulate(scores.cbegin(),
                                                          scores.cend(), 0LL)) /
                      static_cast<double>(scores.size())));
      meta.assign("health", sourcemeta::core::JSON{accumulated_health});
    } else {
      meta.assign("health", sourcemeta::core::JSON{0});
    }

    const auto total_schemas{static_cast<std::int64_t>(schema_entries.size()) +
                             child_schemas_total};
    assert(total_schemas >= 0);
    meta.assign("schemas", sourcemeta::core::JSON{total_schemas});

    meta.assign("entries", std::move(entries));
    meta.assign("private",
                make_private(authentication, directory_registry_path));

    if (relative_path == ".") {
      meta.assign("path", sourcemeta::core::JSON{"/"});
      meta.assign("url", sourcemeta::core::JSON{configuration.url});
      meta.assign("breadcrumb", make_breadcrumb(std::filesystem::path{}, true));
    } else {
      meta.assign("path", sourcemeta::core::JSON{"/" + relative_path.string()});
      meta.assign("url", sourcemeta::core::JSON{configuration.url + "/" +
                                                relative_path.string()});
      meta.assign("breadcrumb", make_breadcrumb(relative_path, true));
    }

    const auto directory_name{
        action.destination.parent_path().parent_path().filename().string()};
    const auto directory_extension_bytes{make_directory_extension(
        parse_version_info(directory_name), meta.at("health").to_integer(),
        meta.at("schemas").to_integer(), meta.at("path").to_string(),
        meta.defines("title") ? meta.at("title").to_string() : "",
        meta.defines("description") ? meta.at("description").to_string() : "",
        meta.defines("email") ? meta.at("email").to_string() : "",
        meta.defines("github") ? meta.at("github").to_string() : "",
        meta.defines("website") ? meta.at("website").to_string() : "")};

    const auto timestamp_end{std::chrono::steady_clock::now()};
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, meta, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP,
        std::span<const std::uint8_t>{directory_extension_bytes},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
  }
};

} // namespace sourcemeta::one

#endif
