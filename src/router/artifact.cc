#include <sourcemeta/core/http.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/metapack.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/time.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/router.h>

#include <algorithm>   // std::ranges::copy
#include <array>       // std::array
#include <cassert>     // assert
#include <chrono>      // std::chrono::seconds
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint16_t, std::uint32_t
#include <filesystem>  // std::filesystem
#include <limits>      // std::numeric_limits
#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

#pragma pack(push, 1)
struct DialectExtension {
  std::uint16_t dialect_length;
};
#pragma pack(pop)

auto dialect_of(const std::span<const std::uint8_t> extension)
    -> std::string_view {
  const auto *header{
      sourcemeta::core::metapack_extension_as<DialectExtension>(extension)};
  if (header == nullptr || header->dialect_length == 0 ||
      sizeof(DialectExtension) + header->dialect_length > extension.size()) {
    return {};
  }

  return std::string_view{
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<const char *>(extension.data() +
                                     sizeof(DialectExtension)),
      header->dialect_length};
}

// The longest form a validator takes here, which is the weak prefix, the
// quoted checksum, and nothing else
constexpr std::size_t ETAG_CAPACITY{
    2 + 1 + sourcemeta::core::METAPACK_CHECKSUM_HEX_LENGTH + 1};

// RFC 9110 §8.8.3 gives the weak form the `W/` prefix
// https://datatracker.ietf.org/doc/html/rfc9110#section-8.8.3
auto write_etag(std::array<char, ETAG_CAPACITY> &storage,
                const std::string_view checksum_hex, const bool weak)
    -> std::string_view {
  auto *cursor{storage.data()};
  if (weak) {
    *cursor++ = 'W';
    *cursor++ = '/';
  }

  *cursor++ = '"';
  cursor = std::ranges::copy(checksum_hex, cursor).out;
  *cursor++ = '"';
  return {storage.data(), static_cast<std::size_t>(cursor - storage.data())};
}

} // namespace

namespace sourcemeta::one {

auto RouterAction::canonical_path(const std::string_view input) const
    -> std::optional<Authentication::Path> {
  return Authentication::Path::parse(input, this->server_uri_);
}

auto RouterAction::artifact_locate(const Authentication::Path &path,
                                   const Tree tree, const std::string_view view,
                                   const std::string_view artifact_name) const
    -> std::optional<std::filesystem::path> {
  // The unit tree holds one answer whoever asks, so only the view tree carries
  // the segment naming who is being served
  const auto tree_root{tree == Tree::Schemas
                           ? this->index_directory_ / "schemas"
                           : this->index_directory_ / "explorer" / view};
  auto directory{tree_root};
  // The tree stores a resource once, whatever representation was asked for
  const auto location{
      sourcemeta::core::remove_suffix_ignore_case(path.value(), ".json")};
  if (!location.empty()) {
    directory /= std::filesystem::path{location};
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

auto RouterAction::caller_from(const Authentication::Credentials &credentials)
    const -> Authentication::Caller {
  return this->dispatcher_.authentication().caller(credentials);
}

auto RouterAction::artifact_resolve_path(
    const Authentication::Caller &caller, const std::string_view input,
    const Tree tree, const std::string_view artifact_name) const
    -> ArtifactResolution {
  // The gate and the locator have to agree on where a request points, so both
  // read the one canonical path rather than each deriving its own. A path that
  // names nowhere in this instance is refused without consulting the gate,
  // since there is nothing to be admitted to
  const auto path{this->canonical_path(input)};
  if (!path.has_value()) {
    return {.path = std::nullopt, .is_public = false};
  }

  // The gate consults the registry path, not the filesystem, and runs before
  // the existence check, so a path held back from this caller and a path that
  // was never here are indistinguishable from the outside
  const auto &authentication{this->dispatcher_.authentication()};
  if (!authentication.permits(path.value(), caller)) {
    return {.path = std::nullopt, .is_public = false};
  }

  auto located{
      this->artifact_locate(path.value(), tree, caller.view(), artifact_name)};
  if (!located.has_value()) {
    // Nothing is served on a miss, so report the path as not public and let a
    // caller that skips the check fall back to a private caching directive
    // rather than a public one
    return {.path = std::nullopt, .is_public = false};
  }

  // Reaching a location anonymously is not the same as every caller being
  // served the same thing there. A shared cache keys on the URL, and one URL in
  // the view tree answers differently per view, so storing one caller's copy
  // would hand it to the next, which is the disclosure this closed by another
  // route.
  //
  // Only the anonymous answer may be stored for reuse, since what it holds is
  // what anybody may read by construction, and it is the answer any other
  // anonymous caller would have been given anyway.
  //
  // The two trees answer this differently because they are shaped differently.
  // A view tree answer is shareable exactly when it came from the anonymous
  // view, since reaching one at all means the location is ungoverned. The unit
  // tree holds one answer whoever asks, so what decides it there is whether
  // anybody at all may read the location, asked of the anonymous view
  const auto is_public{
      tree == Tree::Schemas
          ? authentication.permits(path.value(), authentication.caller({}))
          : caller.view() == VIEW_PUBLIC};
  return {.path = ResolvedArtifact{std::move(located).value()},
          .is_public = is_public};
}

auto RouterAction::artifact_resolve_path_unauthenticated(
    const std::string_view view, const std::string_view input, const Tree tree,
    const std::string_view artifact_name) const
    -> std::optional<ResolvedArtifact> {
  const auto path{this->canonical_path(input)};
  if (!path.has_value()) {
    return std::nullopt;
  }

  auto located{this->artifact_locate(path.value(), tree, view, artifact_name)};
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
    return {.path = std::nullopt};
  }
  return {.path = ResolvedArtifact{std::move(canonical)}};
}

auto RouterAction::artifact_read_json(const ResolvedArtifact &artifact) const
    -> std::optional<sourcemeta::core::JSON> {
  return sourcemeta::core::metapack_read_json(artifact.path());
}

auto RouterAction::artifact_dialect(const ResolvedArtifact &artifact) const
    -> std::optional<sourcemeta::core::JSON::String> {
  sourcemeta::core::FileView view{artifact.path()};
  const auto parsed{sourcemeta::core::metapack_parse(view)};
  if (!parsed.has_value()) {
    return std::nullopt;
  }

  const auto dialect{dialect_of(parsed->extension)};
  if (dialect.empty()) {
    return std::nullopt;
  }

  return sourcemeta::core::JSON::String{dialect};
}

auto RouterAction::artifact_serve(
    const ResolvedArtifact &artifact,
    const sourcemeta::core::HTTPStatus &status, const bool enable_cors,
    const std::string_view mime, const std::string_view link,
    const BrowserSecurityHeaders &browser_security, HTTPRequest &request,
    HTTPResponse &response, const std::string_view error_schema,
    const std::string_view cache_control, const std::string_view vary) const
    -> void {
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

  // Not every way of naming an artifact establishes that there is a file to
  // serve, and mapping one that is not there throws, so this is the single
  // gate every caller passes through
  if (!std::filesystem::exists(absolute_path)) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
        "urn:sourcemeta:one:not-found", "There is nothing at this URL",
        error_schema, enable_cors ? "*" : "");
    return;
  }

  sourcemeta::core::FileView view{absolute_path};
  const auto parsed{sourcemeta::core::metapack_parse(view)};
  if (!parsed.has_value()) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
        "urn:sourcemeta:one:not-found", "There is nothing at this URL",
        error_schema, enable_cors ? "*" : "");
    return;
  }

  const auto &header{*parsed->header};
  const auto last_modified{sourcemeta::core::metapack_last_modified(header)};

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
  const auto checksum{sourcemeta::core::metapack_checksum_hex(header)};
  const std::string_view checksum_hex{checksum.data(), checksum.size()};
  const auto wire_is_gzip{request.response_encoding() ==
                          sourcemeta::one::Encoding::GZIP};
  std::array<char, ETAG_CAPACITY> etag_storage{};
  const auto etag{write_etag(etag_storage, checksum_hex, wire_is_gzip)};

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
    // A caller may name either validator whatever this response would carry,
    // so matching considers both forms rather than only the one being sent
    std::array<char, ETAG_CAPACITY> counterpart_storage{};
    const auto etag_counterpart{
        write_etag(counterpart_storage, checksum_hex, !wire_is_gzip)};
    if (sourcemeta::core::http_field_list_contains_any(
            request.header("if-none-match"), {"*", etag, etag_counterpart})) {
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
      response.write_header("Last-Modified",
                            sourcemeta::core::to_imf_fixdate(last_modified));
      response.write_header("ETag", etag);

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
            last_modified) {
      response.write_status(sourcemeta::core::HTTP_STATUS_NOT_MODIFIED);
      if (enable_cors) {
        response.write_header("Access-Control-Allow-Origin", "*");
        response.write_header("Access-Control-Expose-Headers", "Link, ETag");
      }
      // See the matching comments on the If-None-Match branch.
      response.write_header("Cache-Control", cache_control);
      response.write_header("Vary", vary);
      response.write_header("Last-Modified",
                            sourcemeta::core::to_imf_fixdate(last_modified));
      response.write_header("ETag", etag);

      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NOT_MODIFIED,
                                     request, response);
      return;
    }
  }

  response.write_status(status);

  // RFC 9110 §15.5.2: a 401 response MUST carry WWW-Authenticate. The
  // conditional-request short-circuits above answer 304, never 401, so tying
  // the challenge to the status keeps it correct by construction.
  // https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.2
  if (status == sourcemeta::core::HTTP_STATUS_UNAUTHORIZED) {
    sourcemeta::one::write_challenge(response, {});
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

  response.write_header("Content-Type",
                        mime.empty() ? parsed->media_type : mime);

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
                        sourcemeta::core::to_imf_fixdate(last_modified));

  response.write_header("ETag", etag);

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
    const auto dialect{dialect_of(parsed->extension)};
    if (!dialect.empty()) {
      sourcemeta::one::write_link_header(response, dialect);
    }
  }

  const std::string_view contents{
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<const char *>(parsed->payload.data()),
      parsed->payload.size()};

  // The uncompressed size is recorded for every artifact, so one stored
  // compressed and asked for uncompressed is answered without running the
  // codec. No compressed size is recorded, so the opposite direction has to
  // compress to answer
  if (header.encoding == sourcemeta::core::MetapackEncoding::GZIP) {
    assert(header.content_bytes <= std::numeric_limits<std::size_t>::max());
    sourcemeta::one::send_response(
        status, request, response, contents, sourcemeta::one::Encoding::GZIP,
        static_cast<std::size_t>(header.content_bytes));
  } else {
    sourcemeta::one::send_response(status, request, response, contents,
                                   sourcemeta::one::Encoding::Identity);
  }
}

} // namespace sourcemeta::one
