#include <sourcemeta/core/http.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/time.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/shared.h>

#include <cassert>     // assert
#include <chrono>      // std::chrono::seconds
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint16_t, std::uint32_t
#include <exception>   // std::exception
#include <filesystem>  // std::filesystem
#include <format>      // std::format
#include <limits>      // std::numeric_limits
#include <optional>    // std::optional
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
  if (path_option.has_value() && path_option.value().starts_with("/")) {
    return std::nullopt;
  }

  std::string_view normalized{path_option.has_value()
                                  ? std::string_view{path_option.value()}
                                  : std::string_view{}};
  while (normalized.starts_with("../")) {
    normalized.remove_prefix(3);
  }
  if (normalized == "..") {
    normalized = std::string_view{};
  }

  const auto stripped{
      sourcemeta::core::remove_suffix_ignore_case(normalized, ".json")};
  std::filesystem::path result{stripped};
  sourcemeta::core::to_lowercase(result);
  return result;
}

} // namespace

namespace sourcemeta::one {

auto RouterAction::artifact_locate(const std::string_view input,
                                   const Tree tree,
                                   const std::string_view artifact_name) const
    -> std::optional<std::filesystem::path> {
  const std::string_view tree_segment{tree == Tree::Schemas ? "schemas"
                                                            : "explorer"};
  const auto tree_root{this->index_directory_ / tree_segment};
  auto directory{tree_root};
  if (!input.empty()) {
    auto resolved{uri_to_relative_path(input, this->server_uri_)};
    if (!resolved.has_value()) {
      auto stripped{sourcemeta::core::URI::strip_path_prefix(
          input, this->server_uri_base_path_)};
      if (!stripped.has_value()) {
        return std::nullopt;
      }
      resolved = std::filesystem::path{std::move(stripped).value()};
    }
    if (!resolved.value().empty()) {
      directory /= std::move(resolved).value();
    }
  }
  directory /= "%";
  directory /= std::string{artifact_name} + ".metapack";
  auto canonical{sourcemeta::core::weakly_canonical(directory)};
  if (!sourcemeta::core::is_under_path(canonical, tree_root)) {
    return std::nullopt;
  }
  if (!std::filesystem::exists(canonical)) {
    return std::nullopt;
  }
  return canonical;
}

auto RouterAction::artifact_resolve_path(
    std::string_view credential, const std::string_view input, const Tree tree,
    const std::string_view artifact_name) const -> ArtifactResolution {
  // The gate consults the registry path, not the filesystem, and runs
  // before the existence check so a denial reveals nothing about
  // whether the artifact exists. The relative-path computation here
  // mirrors the locator below, and the two converge once the gate is
  // real
  std::filesystem::path relative;
  if (!input.empty()) {
    auto resolved{uri_to_relative_path(input, this->server_uri_)};
    if (!resolved.has_value()) {
      auto stripped{sourcemeta::core::URI::strip_path_prefix(
          input, this->server_uri_base_path_)};
      if (stripped.has_value()) {
        resolved = std::filesystem::path{std::move(stripped).value()};
      }
    }
    if (resolved.has_value()) {
      relative = std::move(resolved).value();
    }
  }

  const auto &authentication{this->dispatcher_.authentication()};
  const auto verdict{
      authentication.admits(relative.generic_string(), credential)};
  if (!verdict.allowed) {
    return {.outcome = ArtifactResolution::Outcome::Denied,
            .path = std::nullopt};
  }

  auto located{this->artifact_locate(input, tree, artifact_name)};
  if (!located.has_value()) {
    return {.outcome = ArtifactResolution::Outcome::NotFound,
            .path = std::nullopt};
  }

  return {.outcome = ArtifactResolution::Outcome::Found,
          .path = ResolvedArtifact{std::move(located).value()}};
}

auto RouterAction::artifact_resolve_path_unauthenticated(
    const std::string_view input, const Tree tree,
    const std::string_view artifact_name) const
    -> std::optional<ResolvedArtifact> {
  auto located{this->artifact_locate(input, tree, artifact_name)};
  if (!located.has_value()) {
    return std::nullopt;
  }
  return ResolvedArtifact{std::move(located).value()};
}

auto RouterAction::artifact_resolve_static(
    const std::filesystem::path &root, const std::string_view relative) const
    -> ArtifactResolution {
  auto canonical{sourcemeta::core::weakly_canonical(root / relative)};
  if (!sourcemeta::core::is_under_path(canonical, root)) {
    return {.outcome = ArtifactResolution::Outcome::NotFound,
            .path = std::nullopt};
  }
  return {.outcome = ArtifactResolution::Outcome::Found,
          .path = ResolvedArtifact{std::move(canonical)}};
}

auto RouterAction::artifact_read_json(const ResolvedArtifact &artifact) const
    -> std::optional<sourcemeta::core::JSON> {
  return sourcemeta::one::metapack_read_json(artifact.path());
}

auto RouterAction::artifact_serve(
    const ResolvedArtifact &artifact,
    const sourcemeta::core::HTTPStatus &status, const bool enable_cors,
    const std::string_view mime, const std::string_view link,
    const BrowserSecurityHeaders &browser_security, HTTPRequest &request,
    HTTPResponse &response, const std::string_view error_schema,
    const std::string_view cache_control, const std::string_view vary,
    const bool authentication_challenge) const -> void {
  const auto &absolute_path{artifact.path()};
  // Caller must pick the cache scope explicitly so no surface
  // silently inherits an inappropriate default. Cacheability is a
  // per-surface policy decision (RFC 9111 §5.2.2): static assets
  // pin for a year, JSON API surfaces revalidate every hit, MCP and
  // health probe responses are uncacheable, etc.
  assert(!cache_control.empty());
  // Same discipline for `Vary`: the negotiation axes that select a
  // representation differ by surface (schema fetch is UA-branched,
  // the default action is Accept-branched, others just gzip), so
  // the caller has to pick the right stack.
  assert(!vary.empty());
  // OPTIONS preflight is a per-surface concern (the Allow-Headers axis
  // varies by action), so each caller short-circuits it before reaching
  // here via `cors_preflight`. If this assertion fires, a caller is
  // missing that early branch and the 405 emitted below would advertise
  // `Allow: GET, HEAD, OPTIONS` while simultaneously refusing OPTIONS,
  // which is internally inconsistent.
  assert(request.method() != "options");
  if (request.method() != "get" && request.method() != "head") {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
        "urn:sourcemeta:one:method-not-allowed",
        "This HTTP method is invalid for this URL", error_schema,
        enable_cors ? "*" : "", "GET, HEAD, OPTIONS");
    return;
  }

  if (!std::filesystem::exists(absolute_path)) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
        "urn:sourcemeta:one:not-found", "There is nothing at this URL",
        error_schema, enable_cors ? "*" : "");
    return;
  }

  sourcemeta::core::FileView view{absolute_path};
  if (view.size() <
      sizeof(sourcemeta::one::MetapackHeader) + sizeof(std::uint32_t)) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
        "urn:sourcemeta:one:not-found", "There is nothing at this URL",
        error_schema, enable_cors ? "*" : "");
    return;
  }

  const auto info{sourcemeta::one::metapack_info(view)};
  if (!info.has_value()) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
        "urn:sourcemeta:one:not-found", "There is nothing at this URL",
        error_schema, enable_cors ? "*" : "");
    return;
  }

  // Our checksum is computed over the identity (uncompressed) payload at
  // index time. When the wire response is gzip-encoded the wire bytes are
  // not what the checksum covers, so per RFC 9110 §8.8.1 the validator must
  // be marked weak:
  //
  //   "if the origin server sends the same validator for a representation
  //   with a gzip content coding applied as it does for a representation
  //   with no content coding, then that validator is weak."
  //
  // https://datatracker.ietf.org/doc/html/rfc9110#section-8.8.1
  //
  // When the wire response is identity, the wire bytes exactly match what
  // the checksum covers, so the validator can be strong.
  const auto &checksum{info->checksum_hex};
  const auto etag_strong{std::format("\"{}\"", checksum)};
  const auto etag_weak{std::format("W/\"{}\"", checksum)};

  // RFC 9110 §13.2.2 (Precedence of Preconditions): If-None-Match is
  // evaluated before If-Modified-Since. RFC 9110 §13.1.3: "A recipient
  // MUST ignore If-Modified-Since if the request contains an If-None-Match
  // header field; the condition in If-None-Match is considered to be a
  // more accurate replacement for the condition in If-Modified-Since, and
  // the two are only combined for the sake of interoperating with older
  // intermediaries that might not implement If-None-Match." Header
  // *presence* (not match outcome, and not non-empty value) is what
  // suppresses If-Modified-Since: even a malformed `If-None-Match:` with
  // an empty value still triggers the MUST per spec letter.
  // https://datatracker.ietf.org/doc/html/rfc9110#section-13.2.2
  // https://datatracker.ietf.org/doc/html/rfc9110#section-13.1.3
  if (request.header_exists("if-none-match")) {
    if (sourcemeta::core::http_field_list_contains_any(
            request.header("if-none-match"), {"*", etag_strong, etag_weak})) {
      response.write_status(sourcemeta::core::HTTP_STATUS_NOT_MODIFIED);
      if (enable_cors) {
        response.write_header("Access-Control-Allow-Origin", "*");
        response.write_header("Access-Control-Expose-Headers", "Link, ETag");
      }
      // RFC 9111 §4.3.4: a 304 response that updates a stored
      // response in the cache propagates fresh metadata to the
      // cached representation, so the caching directives the
      // origin would have sent on the 200 must travel on the 304
      // too. Otherwise the cache may keep evaluating against a
      // stale, more permissive `Cache-Control`.
      response.write_header("Cache-Control", cache_control);
      response.write_header("Vary", vary);
      // RFC 9110 §15.4.5: "The server generating a 304 response MUST
      // generate any of the following header fields that would have been
      // sent in a 200 (OK) response to the same request: Cache-Control,
      // Content-Location, Date, ETag, Expires, and Vary." Last-Modified
      // travels too so caches refresh both validators at once.
      // https://datatracker.ietf.org/doc/html/rfc9110#section-15.4.5
      response.write_header("Last-Modified", sourcemeta::core::to_imf_fixdate(
                                                 info->last_modified));
      response.write_header("ETag", request.response_encoding() ==
                                            sourcemeta::one::Encoding::GZIP
                                        ? etag_weak
                                        : etag_strong);

      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NOT_MODIFIED,
                                     request, response);
      return;
    }
  } else {
    // Per RFC 9110 §13.1.3 If-Modified-Since is only evaluated when
    // If-None-Match is absent. It can only be used with GET or HEAD; this
    // function is reached only on those methods (checked at the entry).
    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/If-Modified-Since
    const auto if_modified_since{request.header_gmt("if-modified-since")};
    // Time comparison can be flaky, but adding a bit of tolerance leads
    // to more consistent behavior.
    if (if_modified_since.has_value() &&
        (if_modified_since.value() + std::chrono::seconds(1)) >=
            info->last_modified) {
      response.write_status(sourcemeta::core::HTTP_STATUS_NOT_MODIFIED);
      if (enable_cors) {
        response.write_header("Access-Control-Allow-Origin", "*");
        response.write_header("Access-Control-Expose-Headers", "Link, ETag");
      }
      // See the matching comments on the If-None-Match branch.
      response.write_header("Cache-Control", cache_control);
      response.write_header("Vary", vary);
      response.write_header("Last-Modified", sourcemeta::core::to_imf_fixdate(
                                                 info->last_modified));
      response.write_header("ETag", request.response_encoding() ==
                                            sourcemeta::one::Encoding::GZIP
                                        ? etag_weak
                                        : etag_strong);

      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NOT_MODIFIED,
                                     request, response);
      return;
    }
  }

  response.write_status(status);

  // RFC 9110 §15.5.2: a 401 response MUST carry WWW-Authenticate. This serves
  // the HTML unauthorized page, so it travels with the same bearer challenge
  // the JSON error envelope emits. The conditional-request short-circuits above
  // answer 304, never 401, so they need no challenge.
  // https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.2
  if (authentication_challenge) {
    response.write_header("WWW-Authenticate", "Bearer realm=\"registry\"");
  }

  // To support requests from web browsers
  if (enable_cors) {
    response.write_header("Access-Control-Allow-Origin", "*");
    // WHATWG Fetch §3.2.2 (CORS-safelisted response-header name): Link and
    // ETag are not in the safelist, so cross-origin browser scripts cannot
    // read them without an explicit Access-Control-Expose-Headers grant.
    // https://fetch.spec.whatwg.org/#cors-safelisted-response-header-name
    response.write_header("Access-Control-Expose-Headers", "Link, ETag");
  }

  response.write_header("Content-Type", mime.empty() ? info->mime : mime);

  if (!browser_security.referrer_policy.empty()) {
    response.write_header("Referrer-Policy", browser_security.referrer_policy);
  }
  if (!browser_security.frame_ancestors.empty()) {
    response.write_header("Content-Security-Policy",
                          std::string{"frame-ancestors "}.append(
                              browser_security.frame_ancestors));
  }
  if (!browser_security.x_frame_options.empty()) {
    response.write_header("X-Frame-Options", browser_security.x_frame_options);
  }

  response.write_header("Last-Modified",
                        sourcemeta::core::to_imf_fixdate(info->last_modified));

  response.write_header("ETag", request.response_encoding() ==
                                        sourcemeta::one::Encoding::GZIP
                                    ? etag_weak
                                    : etag_strong);

  // RFC 9110 §12.5.5: emit Vary on cacheable responses so shared caches
  // key the entry by the request headers that select the representation.
  // Every surface includes `Accept-Encoding` (the wire encoding and the
  // ETag form per RFC 9110 §8.8.1 both vary with it); surfaces with
  // additional negotiation axes stack them on top.
  // https://datatracker.ietf.org/doc/html/rfc9110#section-12.5.5
  response.write_header("Vary", vary);

  // RFC 9111 §5.2.2 — Cache-Control. Each surface picks its own
  // value; we just emit verbatim.
  response.write_header("Cache-Control", cache_control);

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
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
        "urn:sourcemeta:one:not-found", "There is nothing at this URL",
        error_schema, enable_cors ? "*" : "");
    return;
  }

  const auto payload_size{view.size() - payload_start.value()};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string contents{reinterpret_cast<const char *>(
                                 view.as<std::uint8_t>(payload_start.value())),
                             payload_size};

  if (info->encoding == sourcemeta::one::MetapackEncoding::GZIP) {
    sourcemeta::one::send_response(status, request, response, contents,
                                   sourcemeta::one::Encoding::GZIP);
  } else {
    // The header carries the compressed size as a fixed-width `uint64_t`
    // to keep the metapack format portable across architectures. Narrow
    // to `std::size_t` for the uWS API. On 64-bit hosts this is a no-op,
    // and the assert guards a hypothetical 32-bit build from truncating
    // a >4 GB artifact (which the indexer would never produce on
    // realistic schema inputs).
    assert(info->compressed_bytes <= std::numeric_limits<std::size_t>::max());
    sourcemeta::one::send_response(
        status, request, response, contents,
        sourcemeta::one::Encoding::Identity,
        static_cast<std::size_t>(info->compressed_bytes));
  }
}

} // namespace sourcemeta::one
