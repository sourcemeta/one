#include <sourcemeta/core/io.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/time.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/shared.h>

#include <chrono>      // std::chrono::seconds
#include <cstdint>     // std::uint8_t, std::uint16_t, std::uint32_t
#include <exception>   // std::exception
#include <filesystem>  // std::filesystem
#include <optional>    // std::optional
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

#pragma pack(push, 1)
struct DialectExtension {
  std::uint16_t dialect_length;
};
#pragma pack(pop)

auto uri_to_relative_path(const std::string_view uri,
                          const std::string_view server_uri)
    -> std::optional<std::filesystem::path> {
  std::optional<sourcemeta::core::URI> parsed;
  try {
    parsed.emplace(uri);
    parsed->canonicalize();
  } catch (const std::exception &) {
    return std::nullopt;
  }

  parsed->relative_to(sourcemeta::core::URI{server_uri});
  if (parsed->is_absolute()) {
    return std::nullopt;
  }

  const auto path_option{parsed->path()};
  if (!path_option.has_value() || path_option.value().empty() ||
      path_option.value().starts_with("..") ||
      path_option.value().starts_with("/")) {
    return std::nullopt;
  }

  const auto stripped{sourcemeta::core::remove_suffix_ignore_case(
      path_option.value(), ".json")};
  if (stripped.empty()) {
    return std::nullopt;
  }
  std::filesystem::path result{stripped};
  sourcemeta::core::to_lowercase(result);
  return result;
}

} // namespace

namespace sourcemeta::one {

auto RouterAction::artifact_resolve_path(const std::string_view input,
                                         const InputKind kind, const Tree tree,
                                         const std::string_view artifact_name,
                                         const bool check_existence) const
    -> std::optional<std::filesystem::path> {
  const std::string_view tree_segment{tree == Tree::Schemas ? "schemas"
                                                            : "explorer"};
  const auto tree_root{this->index_directory_ / tree_segment};
  auto directory{tree_root};
  if (!input.empty()) {
    // TODO: Investigate whether the URI and Match modes can be unified. The
    // only divergence today is `.json` suffix handling: URI inputs are JSON
    // Schema identifiers where `foo/bar.json` and `foo/bar` refer to the same
    // schema, so the canonical form drops the suffix. Match inputs are raw
    // URITemplate captures where `v2.0.json` is a different path than `v2.0`,
    // so the suffix must be preserved
    if (kind == InputKind::URI) {
      auto resolved{uri_to_relative_path(input, this->server_uri_)};
      if (!resolved.has_value()) {
        auto stripped{sourcemeta::core::URI::strip_path_prefix(
            input, this->server_uri_base_path_)};
        if (!stripped.has_value() || stripped.value().empty()) {
          return std::nullopt;
        }
        resolved = std::filesystem::path{std::move(stripped).value()};
      }
      directory /= std::move(resolved).value();
    } else {
      auto relative{std::filesystem::path{input}.relative_path()};
      if (!relative.empty()) {
        sourcemeta::core::to_lowercase(relative);
        directory /= relative;
      }
    }
  }
  directory /= "%";
  directory /= std::string{artifact_name} + ".metapack";
  auto canonical{sourcemeta::core::weakly_canonical(directory)};
  if (!sourcemeta::core::is_under_path(canonical, tree_root)) {
    return std::nullopt;
  }
  // TODO: Normalise this opt-out away. The only caller that disables this
  // (list_directory MCP) needs to distinguish "path violates traversal"
  // (lenient fallback to root) from "valid path but file missing" (return
  // an error). Both collapse to nullopt when `check_existence` is true. A
  // future iteration should either drop the lenient fallback (cleaner UX,
  // requires hurl test updates) or expose a separate predicate so the caller
  // can detect traversal violations without combining concerns here
  if (check_existence && !std::filesystem::exists(canonical)) {
    return std::nullopt;
  }
  return canonical;
}

auto RouterAction::artifact_read_json(
    const std::filesystem::path &absolute_path) const
    -> std::optional<sourcemeta::core::JSON> {
  return sourcemeta::one::metapack_read_json(absolute_path);
}

auto RouterAction::artifact_serve(const std::filesystem::path &absolute_path,
                                  const char *code, const bool enable_cors,
                                  const std::string_view mime,
                                  const std::string_view link,
                                  HTTPRequest &request, HTTPResponse &response,
                                  const std::string_view error_schema) const
    -> void {
  if (request.method() != "get" && request.method() != "head") {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
        "method-not-allowed", "This HTTP method is invalid for this URL",
        error_schema);
    return;
  }

  if (!std::filesystem::exists(absolute_path)) {
    sourcemeta::one::json_error(request, response,
                                sourcemeta::one::STATUS_NOT_FOUND, "not-found",
                                "There is nothing at this URL", error_schema);
    return;
  }

  sourcemeta::core::FileView view{absolute_path};
  if (view.size() <
      sizeof(sourcemeta::one::MetapackHeader) + sizeof(std::uint32_t)) {
    sourcemeta::one::json_error(request, response,
                                sourcemeta::one::STATUS_NOT_FOUND, "not-found",
                                "There is nothing at this URL", error_schema);
    return;
  }

  const auto info{sourcemeta::one::metapack_info(view)};
  if (!info.has_value()) {
    sourcemeta::one::json_error(request, response,
                                sourcemeta::one::STATUS_NOT_FOUND, "not-found",
                                "There is nothing at this URL", error_schema);
    return;
  }

  // Note that `If-Modified-Since` can only be used with a `GET` or `HEAD`.
  // See
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/If-Modified-Since
  const auto if_modified_since{request.header_gmt("if-modified-since")};
  // Time comparison can be flaky, but adding a bit of tolerance leads
  // to more consistent behavior.
  if (if_modified_since.has_value() &&
      (if_modified_since.value() + std::chrono::seconds(1)) >=
          info->last_modified) {
    response.write_status(sourcemeta::one::STATUS_NOT_MODIFIED);
    if (enable_cors) {
      response.write_header("Access-Control-Allow-Origin", "*");
    }

    sourcemeta::one::send_response(sourcemeta::one::STATUS_NOT_MODIFIED,
                                   request, response);
    return;
  }

  const auto &checksum{info->checksum_hex};
  std::ostringstream etag_value_strong;
  std::ostringstream etag_value_weak;
  etag_value_strong << '"' << checksum << '"';
  etag_value_weak << 'W' << '/' << '"' << checksum << '"';
  for (const auto &match : request.header_list("if-none-match")) {
    // Cache hit
    if (match.first == "*" || match.first == etag_value_weak.str() ||
        match.first == etag_value_strong.str()) {
      response.write_status(sourcemeta::one::STATUS_NOT_MODIFIED);
      if (enable_cors) {
        response.write_header("Access-Control-Allow-Origin", "*");
      }

      sourcemeta::one::send_response(sourcemeta::one::STATUS_NOT_MODIFIED,
                                     request, response);
      return;
    }
  }

  response.write_status(code);

  // To support requests from web browsers
  if (enable_cors) {
    response.write_header("Access-Control-Allow-Origin", "*");
  }

  if (!mime.empty()) {
    response.write_header("Content-Type", mime);
  } else {
    response.write_header("Content-Type", info->mime);
  }

  response.write_header("Last-Modified",
                        sourcemeta::core::to_gmt(info->last_modified));

  std::ostringstream etag;
  etag << '"' << checksum << '"';
  response.write_header("ETag", std::move(etag).str());

  // See
  // https://json-schema.org/draft/2020-12/json-schema-core.html#section-9.5.1.1
  if (!link.empty()) {
    sourcemeta::one::write_link_header(response, link);
  } else {
    const auto *dialect_ext{
        sourcemeta::one::metapack_extension<DialectExtension>(view)};
    const auto extension_total{sourcemeta::one::metapack_extension_size(view)};
    const std::string_view dialect =
        (dialect_ext != nullptr && dialect_ext->dialect_length > 0 &&
         sizeof(DialectExtension) + dialect_ext->dialect_length <=
             extension_total)
            ? std::string_view{
                  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
                  reinterpret_cast<const char *>(view.as<std::uint8_t>(
                      sourcemeta::one::metapack_extension_offset(view) +
                      sizeof(DialectExtension))),
                  dialect_ext->dialect_length}
            : std::string_view{};
    if (!dialect.empty()) {
      sourcemeta::one::write_link_header(response, dialect);
    }
  }

  const auto payload_start{sourcemeta::one::metapack_payload_offset(view)};
  if (!payload_start.has_value()) {
    sourcemeta::one::json_error(request, response,
                                sourcemeta::one::STATUS_NOT_FOUND, "not-found",
                                "There is nothing at this URL", error_schema);
    return;
  }

  const auto payload_size{view.size() - payload_start.value()};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string contents{reinterpret_cast<const char *>(
                                 view.as<std::uint8_t>(payload_start.value())),
                             payload_size};

  if (info->encoding == sourcemeta::one::MetapackEncoding::GZIP) {
    sourcemeta::one::send_response(code, request, response, contents,
                                   sourcemeta::one::Encoding::GZIP);
  } else {
    sourcemeta::one::send_response(code, request, response, contents,
                                   sourcemeta::one::Encoding::Identity);
  }
}

} // namespace sourcemeta::one
