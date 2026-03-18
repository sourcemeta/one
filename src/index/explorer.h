#ifndef SOURCEMETA_ONE_INDEX_EXPLORER_H_
#define SOURCEMETA_ONE_INDEX_EXPLORER_H_

#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <sourcemeta/one/build.h>

#include <algorithm>   // std::sort
#include <cassert>     // assert
#include <chrono>      // std::chrono
#include <cmath>       // std::lround
#include <cstring>     // std::memcpy
#include <filesystem>  // std::filesystem
#include <limits>      // std::numeric_limits
#include <numeric>     // std::accumulate
#include <optional>    // std::optional
#include <regex>       // std::regex, std::regex_search, std::smatch
#include <string>      // std::string, std::stoul
#include <string_view> // std::string_view
#include <tuple>       // std::tuple, std::make_tuple
#include <utility>     // std::move
#include <vector>      // std::vector

// TODO: We need a SemVer module in Core

static auto try_parse_version(const sourcemeta::core::JSON::String &name)
    -> std::optional<std::tuple<unsigned, unsigned, unsigned>> {
  static const std::regex version_regex(R"(v?(\d+)\.(\d+)\.(\d+))");
  std::smatch match;
  if (std::regex_search(name, match, version_regex)) {
    return std::make_tuple(std::stoul(match[1]), std::stoul(match[2]),
                           std::stoul(match[3]));
  } else {
    return std::nullopt;
  }
}

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

namespace sourcemeta::one {

#pragma pack(push, 1)
struct MetapackExplorerSchemaExtension {
  std::int64_t health;
  std::int64_t bytes;
  std::int64_t dependencies;
  std::uint16_t path_length;
  std::uint16_t identifier_length;
  std::uint16_t base_dialect_length;
  std::uint16_t dialect_length;
  std::uint16_t title_length;
  std::uint16_t description_length;
  std::uint16_t alert_length;
  std::uint16_t provenance_length;
};
#pragma pack(pop)

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

inline auto
explorer_extension_provenance(const MetapackExplorerSchemaExtension *extension,
                              const std::uint8_t *base) -> std::string_view {
  const std::size_t offset{
      static_cast<std::size_t>(extension->path_length) +
      extension->identifier_length + extension->base_dialect_length +
      extension->dialect_length + extension->title_length +
      extension->description_length + extension->alert_length};
  return explorer_extension_string(extension, base, offset,
                                   extension->provenance_length);
}

static auto make_explorer_schema_extension(
    const std::int64_t health, const std::int64_t bytes,
    const std::int64_t dependencies, const std::string_view path,
    const std::string_view identifier, const std::string_view base_dialect,
    const std::string_view dialect, const std::string_view title,
    const std::string_view description, const std::string_view alert,
    const std::string_view provenance) -> std::vector<std::uint8_t> {
  constexpr auto max_length{std::numeric_limits<std::uint16_t>::max()};
  assert(path.size() <= max_length);
  assert(identifier.size() <= max_length);
  assert(base_dialect.size() <= max_length);
  assert(dialect.size() <= max_length);
  assert(title.size() <= max_length);
  assert(description.size() <= max_length);
  assert(alert.size() <= max_length);
  assert(provenance.size() <= max_length);

  const auto strings_size{
      path.size() + identifier.size() + base_dialect.size() + dialect.size() +
      title.size() + description.size() + alert.size() + provenance.size()};
  std::vector<std::uint8_t> result;
  result.resize(sizeof(MetapackExplorerSchemaExtension) + strings_size);

  MetapackExplorerSchemaExtension header{};
  header.health = health;
  header.bytes = bytes;
  header.dependencies = dependencies;
  header.path_length = static_cast<std::uint16_t>(path.size());
  header.identifier_length = static_cast<std::uint16_t>(identifier.size());
  header.base_dialect_length = static_cast<std::uint16_t>(base_dialect.size());
  header.dialect_length = static_cast<std::uint16_t>(dialect.size());
  header.title_length = static_cast<std::uint16_t>(title.size());
  header.description_length = static_cast<std::uint16_t>(description.size());
  header.alert_length = static_cast<std::uint16_t>(alert.size());
  header.provenance_length = static_cast<std::uint16_t>(provenance.size());

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
  append(provenance);

  return result;
}

struct GENERATE_EXPLORER_SCHEMA_METADATA {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &callback,
                      sourcemeta::one::Resolver &resolver,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    const auto &resolver_entry{resolver.entry(action.data)};
    // Read the schema to get data and bytes
    sourcemeta::core::FileView schema_view{action.dependencies.front()};
    const auto schema_info{sourcemeta::one::metapack_info(schema_view).value()};
    const auto schema_data{
        sourcemeta::one::metapack_read_json(action.dependencies.front())
            .value()};
    const auto id{sourcemeta::core::identify(
        schema_data, [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        })};
    assert(!id.empty());
    auto result{sourcemeta::core::JSON::make_object()};

    result.assign("bytes", sourcemeta::core::JSON{static_cast<std::size_t>(
                               schema_info.content_bytes)});
    result.assign("identifier", sourcemeta::core::JSON{std::string{id}});
    result.assign("path", sourcemeta::core::JSON{
                              "/" + resolver_entry.relative_path.string()});
    const auto base_dialect{sourcemeta::core::base_dialect(
        schema_data, [&callback, &resolver](const auto identifier) {
          return resolver(identifier, callback);
        })};
    assert(base_dialect.has_value());
    result.assign("baseDialect",
                  sourcemeta::core::JSON{std::string{
                      sourcemeta::core::to_string(base_dialect.value())}});
    const auto dialect{sourcemeta::core::dialect(schema_data)};
    assert(!dialect.empty());
    result.assign("dialect", sourcemeta::core::JSON{std::string{dialect}});

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
        const auto vocabularies{sourcemeta::core::vocabularies(
            [&callback, &resolver](const auto identifier) {
              return resolver(identifier, callback);
            },
            base_dialect.value(), dialect)};
        const auto &walker_result{
            sourcemeta::core::schema_walker("examples", vocabularies)};
        if (walker_result.type ==
                sourcemeta::core::SchemaKeywordType::Annotation ||
            walker_result.type ==
                sourcemeta::core::SchemaKeywordType::Comment) {
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

    const auto health{
        sourcemeta::one::metapack_read_json(action.dependencies.at(1)).value()};
    result.assign("health", health.at("score"));

    const auto schema_dependencies{
        sourcemeta::one::metapack_read_json(action.dependencies.at(2)).value()};
    result.assign("dependencies",
                  sourcemeta::core::to_json(schema_dependencies.size()));

    const auto &collection{*resolver_entry.collection};

    if (collection.extra.defines("x-sourcemeta-one:alert")) {
      assert(collection.extra.at("x-sourcemeta-one:alert").is_string());
      result.assign("alert", collection.extra.at("x-sourcemeta-one:alert"));
    } else {
      result.assign("alert", sourcemeta::core::JSON{nullptr});
    }

    if (collection.extra.defines("x-sourcemeta-one:provenance")) {
      assert(collection.extra.at("x-sourcemeta-one:provenance").is_string());
      result.assign("provenance",
                    collection.extra.at("x-sourcemeta-one:provenance"));
    } else {
      result.assign("provenance", sourcemeta::core::JSON{nullptr});
    }

    result.assign("breadcrumb",
                  make_breadcrumb(resolver_entry.relative_path, false));

    const auto timestamp_end{std::chrono::steady_clock::now()};

    const auto extension_bytes{make_explorer_schema_extension(
        result.at("health").to_integer(),
        static_cast<std::int64_t>(schema_info.content_bytes),
        result.at("dependencies").to_integer(), result.at("path").to_string(),
        result.at("identifier").to_string(),
        result.at("baseDialect").to_string(), result.at("dialect").to_string(),
        result.defines("title") ? result.at("title").to_string() : "",
        result.defines("description") ? result.at("description").to_string()
                                      : "",
        result.at("alert").is_string() ? result.at("alert").to_string() : "",
        result.at("provenance").is_string()
            ? result.at("provenance").to_string()
            : "")};

    sourcemeta::one::metapack_write_pretty_json(
        action.destination, result, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP,
        std::span<const std::uint8_t>{extension_bytes},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }
};

struct GENERATE_EXPLORER_SEARCH_INDEX {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    std::vector<sourcemeta::core::JSON> result;
    result.reserve(action.dependencies.size());

    for (const auto &dependency : action.dependencies) {
      sourcemeta::core::FileView dependency_view{dependency};
      const auto extension_offset{
          sourcemeta::one::metapack_extension_offset(dependency_view)};
      if (extension_offset == 0) {
        continue;
      }

      const auto *extension{
          sourcemeta::one::metapack_extension<MetapackExplorerSchemaExtension>(
              dependency_view)};
      if (extension == nullptr) {
        continue;
      }

      const auto *extension_base{
          dependency_view.as<std::uint8_t>(extension_offset)};
      const auto path{explorer_extension_path(extension, extension_base)};
      const auto title{explorer_extension_title(extension, extension_base)};
      const auto description{
          explorer_extension_description(extension, extension_base)};

      auto entry{sourcemeta::core::JSON::make_array()};
      entry.push_back(sourcemeta::core::JSON{std::string{path}});
      entry.push_back(sourcemeta::core::JSON{std::string{title}});
      entry.push_back(sourcemeta::core::JSON{std::string{description}});
      result.push_back(std::move(entry));
    }

    std::sort(result.begin(), result.end(),
              [](const sourcemeta::core::JSON &left,
                 const sourcemeta::core::JSON &right) {
                assert(left.is_array() && left.size() == 3);
                assert(right.is_array() && right.size() == 3);

                // Prioritise entries that have more meta-data filled in
                const auto left_score = (!left.at(1).empty() ? 1 : 0) +
                                        (!left.at(2).empty() ? 1 : 0);
                const auto right_score = (!right.at(1).empty() ? 1 : 0) +
                                         (!right.at(2).empty() ? 1 : 0);
                if (left_score != right_score) {
                  return left_score > right_score;
                }

                // Otherwise revert to lexicographic comparisons
                // TODO: Ideally we sort based on schema health too, given
                // lint results
                if (left_score > 0) {
                  return left.at(0).to_string() < right.at(0).to_string();
                }

                return false;
              });

    const auto timestamp_end{std::chrono::steady_clock::now()};

    sourcemeta::one::metapack_write_jsonl(
        action.destination, result, "application/jsonl",
        // We don't want to compress this one so we can
        // quickly skim through it while streaming it
        sourcemeta::one::MetapackEncoding::Identity, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }
};

struct GENERATE_EXPLORER_DIRECTORY_LIST {
  static auto handler(const sourcemeta::one::BuildState &,
                      const sourcemeta::one::BuildPlan::Action &action,
                      const sourcemeta::one::BuildDynamicCallback &,
                      sourcemeta::one::Resolver &,
                      const sourcemeta::one::Configuration &configuration,
                      const sourcemeta::core::JSON &) -> bool {
    const auto timestamp_start{std::chrono::steady_clock::now()};
    auto entries{sourcemeta::core::JSON::make_array()};
    std::vector<sourcemeta::core::JSON::Integer> scores;

    const auto directory_path{action.destination.parent_path().parent_path()};
    std::filesystem::path relative_path;
    auto current{directory_path};
    while (current.has_filename()) {
      if (current.filename() == "explorer") {
        relative_path = std::filesystem::relative(directory_path, current);
        break;
      }

      current = current.parent_path();
    }

    for (const auto &dependency : action.dependencies) {
      const auto filename{dependency.filename().string()};
      const auto child_name{
          dependency.parent_path().parent_path().filename().string()};

      if (filename == "directory.metapack") {
        auto directory_json{
            sourcemeta::one::metapack_read_json(dependency).value()};
        assert(directory_json.is_object());
        assert(directory_json.defines("health"));
        assert(directory_json.at("health").is_integer());
        scores.emplace_back(directory_json.at("health").to_integer());

        auto entry_json{sourcemeta::core::JSON::make_object()};
        entry_json.assign("health", directory_json.at("health"));
        entry_json.assign("name", sourcemeta::core::JSON{child_name});
        entry_json.assign("type", sourcemeta::core::JSON{"directory"});
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
        entries.push_back(std::move(entry_json));
      } else if (filename == "schema.metapack") {
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
        entry_json.assign("path", sourcemeta::core::JSON{std::filesystem::path{
                                      std::string{schema_path}}});
        entry_json.assign(
            "identifier",
            sourcemeta::core::JSON{std::string{
                explorer_extension_identifier(extension, extension_base)}});
        entry_json.assign("bytes", sourcemeta::core::JSON{extension->bytes});
        entry_json.assign(
            "baseDialect",
            sourcemeta::core::JSON{std::string{
                explorer_extension_base_dialect(extension, extension_base)}});
        entry_json.assign("dialect", sourcemeta::core::JSON{
                                         std::string{explorer_extension_dialect(
                                             extension, extension_base)}});
        entry_json.assign("health", sourcemeta::core::JSON{extension->health});
        entry_json.assign("dependencies",
                          sourcemeta::core::JSON{extension->dependencies});

        const auto title{explorer_extension_title(extension, extension_base)};
        if (!title.empty()) {
          entry_json.assign("title",
                            sourcemeta::core::JSON{std::string{title}});
        }

        const auto description{
            explorer_extension_description(extension, extension_base)};
        if (!description.empty()) {
          entry_json.assign("description",
                            sourcemeta::core::JSON{std::string{description}});
        }

        const auto alert{explorer_extension_alert(extension, extension_base)};
        if (!alert.empty()) {
          entry_json.assign("alert",
                            sourcemeta::core::JSON{std::string{alert}});
        } else {
          entry_json.assign("alert", sourcemeta::core::JSON{nullptr});
        }

        const auto provenance{
            explorer_extension_provenance(extension, extension_base)};
        if (!provenance.empty()) {
          entry_json.assign("provenance",
                            sourcemeta::core::JSON{std::string{provenance}});
        } else {
          entry_json.assign("provenance", sourcemeta::core::JSON{nullptr});
        }

        scores.emplace_back(extension->health);
        entries.push_back(std::move(entry_json));
      }
    }

    struct SortKey {
      std::string_view type;
      std::optional<std::tuple<unsigned, unsigned, unsigned>> version;
      std::string_view name;
    };

    std::vector<SortKey> sort_keys;
    sort_keys.reserve(entries.size());
    for (const auto &entry : entries.as_array()) {
      const auto &type{entry.at("type").to_string()};
      const auto &name{entry.at("name").to_string()};
      sort_keys.push_back({type, try_parse_version(name), name});
    }

    std::vector<std::size_t> indices(entries.size());
    for (std::size_t index = 0; index < indices.size(); index++) {
      indices[index] = index;
    }

    std::sort(indices.begin(), indices.end(),
              [&sort_keys](const auto left, const auto right) {
                const auto &left_key{sort_keys[left]};
                const auto &right_key{sort_keys[right]};
                if (left_key.type == right_key.type) {
                  if (left_key.version.has_value() &&
                      right_key.version.has_value()) {
                    return left_key.version.value() > right_key.version.value();
                  }

                  return left_key.name > right_key.name;
                }

                return left_key.type < right_key.type;
              });

    auto sorted_entries{sourcemeta::core::JSON::make_array()};
    for (const auto index : indices) {
      sorted_entries.push_back(entries.at(index));
    }

    entries = std::move(sorted_entries);

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

    meta.assign("entries", std::move(entries));

    if (relative_path == ".") {
      meta.assign("path", sourcemeta::core::JSON{"/"});
      meta.assign("url", sourcemeta::core::JSON{configuration.url});
      meta.assign("breadcrumb", make_breadcrumb(std::filesystem::path{}, true));
    } else {
      meta.assign("path", sourcemeta::core::JSON{std::string{"/"} +
                                                 relative_path.string()});
      meta.assign("url", sourcemeta::core::JSON{configuration.url + "/" +
                                                relative_path.string()});
      meta.assign("breadcrumb", make_breadcrumb(relative_path, true));
    }

    const auto timestamp_end{std::chrono::steady_clock::now()};
    sourcemeta::one::metapack_write_pretty_json(
        action.destination, meta, "application/json",
        sourcemeta::one::MetapackEncoding::GZIP, {},
        std::chrono::duration_cast<std::chrono::milliseconds>(timestamp_end -
                                                              timestamp_start));
    return true;
  }
};

} // namespace sourcemeta::one

#endif
