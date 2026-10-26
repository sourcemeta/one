#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_MCP_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_MCP_V1_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/metapack.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/enterprise_conversion.h>
#include <sourcemeta/one/enterprise_server_conversion.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>

#include <algorithm> // std::ranges::find
#include <array>     // std::array
#include <cassert>   // assert
#include <cstddef>   // std::size_t, std::ptrdiff_t
#include <cstdint>   // std::uint64_t
#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <filesystem>    // std::filesystem
#include <iterator>      // std::ranges::distance
#include <memory>        // std::make_unique, std::unique_ptr
#include <optional>      // std::optional, std::nullopt
#include <span>          // std::span
#include <sstream>       // std::ostringstream
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <utility>       // std::move, std::pair
#include <vector>        // std::vector

// The protocol revisions this instance serves. A client asking for anything
// else is refused rather than answered under rules this does not implement
inline constexpr std::array<sourcemeta::core::JSON::StringView, 4>
    MCP_SUPPORTED_VERSIONS{{sourcemeta::core::MCP_PROTOCOL_VERSION_2025_03_26,
                            sourcemeta::core::MCP_PROTOCOL_VERSION_2025_06_18,
                            sourcemeta::core::MCP_PROTOCOL_VERSION_2025_11_25,
                            sourcemeta::core::MCP_PROTOCOL_VERSION_2026_07_28}};

// How long a caller may keep an answer. What a process serves is settled before
// it starts and cannot change under it, so what bounds this is how soon a
// caller should notice a deployment rather than anything the catalog might do
inline constexpr std::int64_t MCP_CACHE_TTL_MILLISECONDS{300000};

// Which of the methods a revision names this instance answers. A method the
// revision defines but nothing here serves is missing rather than malformed,
// and saying which it is tells a caller whether to fix the request or stop
// asking
[[nodiscard]] constexpr auto
mcp_serves_method(const sourcemeta::core::MCPProtocolVersion version,
                  const sourcemeta::core::JSON::StringView method) noexcept
    -> bool {
  if (version == sourcemeta::core::MCPProtocolVersion::V_2026_07_28) {
    return method == sourcemeta::core::MCP_METHOD_SERVER_DISCOVER ||
           method == sourcemeta::core::MCP_METHOD_TOOLS_LIST ||
           method == sourcemeta::core::MCP_METHOD_TOOLS_CALL ||
           method == sourcemeta::core::MCP_METHOD_RESOURCES_LIST ||
           method == sourcemeta::core::MCP_METHOD_RESOURCES_READ ||
           method == sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST;
  }

  return method == sourcemeta::core::MCP_METHOD_INITIALIZE ||
         method == sourcemeta::core::MCP_METHOD_PING ||
         method == sourcemeta::core::MCP_METHOD_TOOLS_LIST ||
         method == sourcemeta::core::MCP_METHOD_TOOLS_CALL ||
         method == sourcemeta::core::MCP_METHOD_RESOURCES_LIST ||
         method == sourcemeta::core::MCP_METHOD_RESOURCES_READ ||
         method == sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST;
}

// What a caller is answered with when an error carries a status of its own, and
// what it is answered with otherwise. A rejection reached before a method was
// resolved speaks about the request, so it is a bad one, while a refusal that
// came out of answering speaks about what was asked for and travels on a
// successful exchange
[[nodiscard]] inline auto
mcp_error_status(const sourcemeta::core::MCPProtocolVersion version,
                 const sourcemeta::core::JSON &envelope, const bool transport)
    -> sourcemeta::core::HTTPStatus {
  const auto *error{envelope.try_at("error")};
  if (error != nullptr) {
    const auto *code{error->try_at("code")};
    if (code != nullptr && code->is_integer()) {
      const auto mandated{sourcemeta::core::mcp_error_code_to_http_status(
          version, code->to_integer())};
      if (mandated.has_value()) {
        return mandated.value();
      }
    }
  }

  return transport ? sourcemeta::core::HTTP_STATUS_BAD_REQUEST
                   : sourcemeta::core::HTTP_STATUS_OK;
}

// What answers a rejection settled before any identifier could be read. Such a
// rejection names none, so it is left out on a revision that allows an error
// response without one and carried as the transport fallback on a revision
// that allows nothing else
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http
[[nodiscard]] inline auto
mcp_transport_error(const sourcemeta::core::MCPProtocolVersion version,
                    const std::int64_t code,
                    const sourcemeta::core::JSON::StringView message,
                    std::optional<sourcemeta::core::JSON> data = std::nullopt)
    -> sourcemeta::core::JSON {
  const sourcemeta::core::JSON unreadable{nullptr};
  return sourcemeta::core::mcp_make_error(
      version, &unreadable, code, message, std::move(data),
      sourcemeta::core::MCPErrorContext::Transport);
}

class ActionMCPV1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Handle Model Context Protocol JSON-RPC requests"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionMCPV1(const std::filesystem::path &base,
              const sourcemeta::core::URITemplateRouterView &router,
              const sourcemeta::core::URITemplateRouter::Identifier identifier,
              sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "responseSchema") {
            this->response_schema_ = std::get<std::string_view>(value);
          } else if (key == "metadataPath") {
            this->metadata_path_ = std::get<std::string_view>(value);
          } else if (key == "requestSchema") {
            this->request_schema_ = std::get<std::string_view>(value);
          }
        });

    const auto mcp_metadata_path{this->artifact_resolve_path_unauthenticated(
        sourcemeta::one::VIEW_PUBLIC, "", Tree::Explorer, "mcp")};
    assert(mcp_metadata_path.has_value());
    auto mcp_metadata_option{
        this->artifact_read_json(mcp_metadata_path.value())};
    assert(mcp_metadata_option.has_value());
    this->mcp_metadata_ = std::move(mcp_metadata_option.value());
    this->allowed_origin_ = this->mcp_metadata_.at("origin").to_string();
    // Indexing canonicalizes the configured origin URL, lowercasing scheme
    // and host per RFC 3986 §3.1 and §3.2.2. RFC 6454 §4 origin equality
    // assumes both sides are already in this form, so per-request compares
    // only need to fold the inbound header. Catch a stale metapack that
    // somehow carried a mixed-case origin through.
    // https://datatracker.ietf.org/doc/html/rfc6454#section-4
    assert(sourcemeta::core::is_lowercase(this->allowed_origin_));

    this->resource_identifier_ =
        this->mcp_metadata_.at("resourceIdentifier").to_string();

    // RFC 9728 Section 5.1. A client derives this location from the resource
    // identifier anyway, but only after being refused once. It is named here
    // only when there is a document to find
    if (this->mcp_metadata_.defines("protectedResourceMetadata")) {
      this->metadata_url_.append(this->server_uri());
      this->metadata_url_.append(this->metadata_path_);
      this->challenge_.emplace_back("resource_metadata", this->metadata_url_);
    }

    // What a caller is told this endpoint offers follows from their view, so
    // each one's copy is read here rather than assembled per request. The
    // anonymous copy above still answers whatever is decided before a
    // credential is known, which is the origin a preflight is checked against
    // and the metadata a refusal points at
    const auto &authentication{dispatcher.authentication()};
    for (const auto &recorded : authentication.table().views()) {
      const auto path{this->artifact_resolve_path_unauthenticated(
          recorded.name(), "", Tree::Explorer, "mcp")};
      if (!path.has_value()) {
        continue;
      }

      auto metadata{this->artifact_read_json(path.value())};
      if (metadata.has_value()) {
        this->metadata_views_.emplace(recorded.name(),
                                      std::move(metadata).value());
      }
    }
  }

  // A caller whose view this instance no longer serves is answered from the
  // anonymous one, which is a build behind a running server rather than a
  // caller to be trusted with more
  [[nodiscard]] auto metadata_for(const std::string_view view) const
      -> const sourcemeta::core::JSON & {
    const auto match{this->metadata_views_.find(view)};
    return match == this->metadata_views_.cend() ? this->mcp_metadata_
                                                 : match->second;
  }

  [[nodiscard]] auto authentication_challenge() const noexcept -> std::span<
      const std::pair<std::string_view, std::string_view>> override {
    return this->challenge_;
  }

  // This endpoint is a protected resource in its own right, so a token that
  // names only something wider was not issued for it
  [[nodiscard]] auto required_audience() const noexcept
      -> std::string_view override {
    return this->resource_identifier_;
  }

  auto rest(const std::span<std::string_view>,
            const sourcemeta::one::Authentication::Caller &,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    // MCP Streamable HTTP transport / Protocol Version Header:
    // - Client MUST send `MCP-Protocol-Version` on every request after init
    // - If header is absent, server SHOULD assume 2025-03-26, which is the
    //   revision that predates the header
    // - If header is invalid/unsupported, server MUST respond with 400
    // Presence is what the fallback turns on rather than whether the header
    // carries anything, since one that is there but empty names no revision
    // and is refused rather than read as the one that predates it
    // https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#protocol-version-header
    const auto requested_version{request.header("mcp-protocol-version")};
    const auto negotiated_version{
        request.header_exists("mcp-protocol-version")
            ? sourcemeta::core::mcp_resolve_protocol_version(requested_version)
            : std::optional<sourcemeta::core::MCPProtocolVersion>{
                  sourcemeta::core::MCPProtocolVersion::V_2025_03_26}};
    // A revision this library models is not automatically one this instance
    // serves, so what it settles on is checked against what is advertised.
    // Nothing is refused here: a refusal has to name the request it answers,
    // and the identifier that names it has not been read yet
    const auto served_version{
        negotiated_version.has_value() &&
                std::ranges::find(MCP_SUPPORTED_VERSIONS,
                                  sourcemeta::core::mcp_protocol_version_string(
                                      negotiated_version.value())) !=
                    MCP_SUPPORTED_VERSIONS.cend()
            ? negotiated_version
            : std::optional<sourcemeta::core::MCPProtocolVersion>{}};
    // What a rejection decided before the body arrives echoes back, which is
    // the revision that predates the header whenever none was settled on
    const auto echo_version{served_version.value_or(
        sourcemeta::core::MCPProtocolVersion::V_2025_03_26)};

    // MCP Streamable HTTP transport / Security Warning:
    // "Servers MUST validate the Origin header on all incoming connections
    // to prevent DNS rebinding attacks." Mandatory per the MCP spec, not
    // defensive-only. Do not remove this check.
    // https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#security-warning
    //
    // RFC 6454 §7.3: user agents send `Origin: null` from privacy-sensitive
    // contexts (sandboxed iframes, file://, data:). The literal "null" is
    // never a real allowlisted origin, so the byte-compare below correctly
    // rejects it. https://datatracker.ietf.org/doc/html/rfc6454#section-7.3
    const auto origin_header{request.header("origin")};
    if (!origin_header.empty()) {
      std::string origin_canonical{origin_header};
      sourcemeta::core::to_lowercase(origin_canonical);
      if (origin_canonical != this->allowed_origin_) {
        this->write_envelope(
            request, response, sourcemeta::core::HTTP_STATUS_FORBIDDEN,
            mcp_transport_error(
                echo_version, -32001, "Forbidden origin",
                sourcemeta::core::JSON{std::string{origin_header}}),
            echo_version);
        return;
      }
    }

    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin",
                            this->allowed_origin_);
      response.write_header("Access-Control-Expose-Headers", "Link, ETag");
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      // Without `Authorization` a browser withholds the credential at
      // preflight, so no browser-resident client could authenticate at all,
      // whatever it holds. The routing fields are named for the same reason:
      // a revision that requires them on every request puts them beyond a
      // browser's reach unless they are allowed here
      response.write_header("Access-Control-Allow-Headers",
                            "Authorization, Content-Type, "
                            "MCP-Protocol-Version, Mcp-Method, Mcp-Name");
      response.write_header("Access-Control-Max-Age", "3600");
      // Browser preflight cache is governed by `Access-Control-Max-Age`;
      // `no-store` keeps shared HTTP caches from storing this response.
      response.write_header("Cache-Control",
                            sourcemeta::one::cache_control_no_store());
      // RFC 9110 §9.3.7: OPTIONS responses SHOULD include Allow. Different
      // audience than Access-Control-Allow-Methods (HTTP vs CORS preflight).
      // https://datatracker.ietf.org/doc/html/rfc9110#section-9.3.7
      response.write_header("Allow", "POST, OPTIONS");
      // Debuggability echo. A preflight carries no message to settle a
      // revision against, so it names the one that predates the header.
      response.write_header(
          "MCP-Protocol-Version",
          sourcemeta::core::mcp_protocol_version_string(
              sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    if (request.method() != "post") {
      this->write_envelope(
          request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
          mcp_transport_error(echo_version, -32004, "Method not allowed"),
          echo_version);
      return;
    }

    // MCP Streamable HTTP transport / Sending Messages, item 2: "The client
    // MUST include an Accept header, listing both application/json and
    // text/event-stream as supported content types." The MUST is on the
    // client. Defensively rejecting clients that omit either media type
    // catches integration bugs before SSE lands. Both types must be
    // individually acceptable per RFC 9110 §12.5.1 (q-aware, wildcard-aware).
    // https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#sending-messages-to-the-server
    const auto accept{request.header("accept")};
    if (accept.empty() ||
        !sourcemeta::core::http_accept_includes_all(
            accept, {"application/json", "text/event-stream"})) {
      this->write_envelope(
          request, response, sourcemeta::core::HTTP_STATUS_NOT_ACCEPTABLE,
          mcp_transport_error(echo_version, -32006,
                              "Accept header must list application/json and "
                              "text/event-stream"),
          echo_version);
      return;
    }

    // MCP requests carry a JSON-RPC body. Reject other media types up front
    // rather than letting the parser surface a confusing -32700 (parse
    // error). The MUST is implicit in MCP's wire spec. Parameters do not
    // change the media-type identity, so any params (including
    // `charset=utf-8`) are accepted.
    if (!sourcemeta::core::http_content_type_matches(
            request.header("content-type"), "application/json")) {
      this->write_envelope(
          request, response,
          sourcemeta::core::HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE,
          mcp_transport_error(echo_version, -32008,
                              "Content-Type must be application/json"),
          echo_version);
      return;
    }

    // RFC 9110 §10.1.1: refuse unrecognised expectations with 417 before
    // touching the body. uWS already auto-acknowledged `100-continue`
    // upstream, so anything left here is a value we cannot honour.
    if (sourcemeta::one::expect_header_unrecognised(request)) {
      this->write_envelope(
          request, response, sourcemeta::core::HTTP_STATUS_EXPECTATION_FAILED,
          mcp_transport_error(
              echo_version, -32009,
              "The Expect header carries an unsupported expectation"),
          echo_version);
      return;
    }

    // RFC 9110 §15.5.14: when the client declares a `Content-Length`
    // beyond the cap, fast-fail with 413 before scheduling the read.
    if (sourcemeta::one::request_body_too_large(request)) {
      this->write_envelope(request, response,
                           sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE,
                           mcp_transport_error(echo_version, -32005,
                                               "Request body is too large"),
                           echo_version);
      return;
    }

    request.body(
        [this,
         bearer = std::string{sourcemeta::core::http_parse_bearer(
             request.header("authorization"))},
         cookies = sourcemeta::one::owned_cookies(request),
         // Taken now for the same reason as the credential above, and read
         // against the body once there is a body to read it against
         routing = sourcemeta::one::mcp_owned_routing_headers(request),
         requested = std::string{requested_version}, served = served_version,
         echo = echo_version](sourcemeta::one::HTTPRequest &callback_request,
                              sourcemeta::one::HTTPResponse &callback_response,
                              std::string &&body, bool too_big) -> void {
          if (too_big) {
            this->write_envelope(
                callback_request, callback_response,
                sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE,
                mcp_transport_error(echo, -32005, "Request body is too large"),
                echo);
            return;
          }
          const sourcemeta::one::RequestCookies fields{cookies};
          // Placed again rather than captured, since the caller this action
          // was handed points at a request that is gone by now
          const auto deferred_caller{
              this->caller_from({.bearer = bearer, .cookies = fields})};
          this->on_message(served, requested, routing, callback_request,
                           callback_response, std::move(body), deferred_caller);
        },
        [this, version = echo_version](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            const std::exception_ptr &error) -> void {
          try {
            std::rethrow_exception(error);
          } catch (const std::exception &) {
            this->write_envelope(
                callback_request, callback_response,
                sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
                sourcemeta::core::jsonrpc_make_error_internal(), version);
          }
        });
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &,
           const sourcemeta::one::Authentication::Caller &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(request_id);
  }

private:
  static constexpr std::string_view MCP_TEMPLATE_MIME_TYPE{
      "application/schema+json"};

  auto
  write_envelope(sourcemeta::one::HTTPRequest &request,
                 sourcemeta::one::HTTPResponse &response,
                 const sourcemeta::core::HTTPStatus &status,
                 const sourcemeta::core::JSON &envelope,
                 const sourcemeta::core::MCPProtocolVersion version =
                     sourcemeta::core::MCPProtocolVersion::V_2025_03_26) const
      -> void {
    response.write_status(status);
    response.write_header("Content-Type", "application/json");
    response.write_header("Access-Control-Allow-Origin", this->allowed_origin_);
    response.write_header("Access-Control-Expose-Headers", "Link, ETag");
    // The envelope embeds the echoed JSON-RPC id from the request, so
    // the response is request-specific and never a sound cache hit
    // for any other request.
    response.write_header("Cache-Control",
                          sourcemeta::one::cache_control_no_store());
    // RFC 9110 §15.5.6: 405 responses MUST carry Allow listing supported
    // methods. The MCP endpoint accepts POST and OPTIONS only.
    // https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.6
    if (status == sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED) {
      response.write_header("Allow", "POST, OPTIONS");
    }
    // Debuggability echo: not mandated by MCP, but lets clients confirm
    // which protocol revision the server interpreted. For error paths
    // where no negotiation succeeded, we echo the spec-default `2025-03-26`.
    // https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#protocol-version-header
    response.write_header(
        "MCP-Protocol-Version",
        sourcemeta::core::mcp_protocol_version_string(version));
    if (!this->response_schema_.empty()) {
      sourcemeta::one::write_link_header(response, this->response_schema_);
    }
    std::ostringstream payload;
    sourcemeta::core::prettify(envelope, payload);
    sourcemeta::one::send_response(status, request, response, payload.str(),
                                   sourcemeta::one::Encoding::Identity);
  }

  // Who may keep an answer assembled for this view, and for how long. The view
  // settles the first, which is also what holds every page of one listing to
  // the same answer, and a deployment settles the second
  [[nodiscard]] auto cache_policy(const sourcemeta::core::JSON &metadata) const
      -> sourcemeta::core::MCPCachePolicy {
    const auto scope{sourcemeta::core::mcp_resolve_cache_scope(
        metadata.at("cacheScope").to_string())};
    assert(scope.has_value());
    return {.ttl_ms = MCP_CACHE_TTL_MILLISECONDS, .scope = scope.value()};
  }

  auto on_resources_list(const sourcemeta::core::MCPProtocolVersion version,
                         const sourcemeta::core::JSON &request_json,
                         const std::string_view view) const
      -> sourcemeta::core::JSON {
    const auto &request_id{request_json.at("id")};
    const auto &metadata{this->metadata_for(view)};
    const auto &pages{metadata.at(sourcemeta::core::MCP_METHOD_RESOURCES_LIST)};
    // The pages were cut somewhere, and the artifact says where, so a cursor is
    // read against how these very pages were written
    const auto page_size{static_cast<std::uint64_t>(
        metadata.at("resourcePageSize").to_integer())};
    assert(page_size > 0);

    std::uint64_t offset{0};
    const auto *params{sourcemeta::core::jsonrpc_params(request_json)};
    if (params != nullptr && params->defines("cursor")) {
      const auto cursor_input{params->at("cursor").to_string()};
      if (!cursor_input.empty()) {
        const auto parsed{sourcemeta::core::to_uint64_t(cursor_input)};
        // A cursor names a page boundary, so one that parses to anything else
        // names no page at all
        if (!parsed.has_value() || parsed.value() % page_size != 0) {
          return sourcemeta::core::jsonrpc_make_error(
              &request_id, -32602, "Invalid resource list cursor",
              sourcemeta::core::JSON{
                  "Use the `nextCursor` returned by a prior resources/list "
                  "response, or omit it to start from the beginning"});
        }
        offset = parsed.value();
      }
    }

    // The pages were written for this view, so answering is finding the one
    // asked for. A catalog holding nothing still has a first page, which is why
    // the beginning is always a page and anything past the end is not
    const auto index{offset / page_size};
    if (index >= pages.size()) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Invalid resource list cursor",
          sourcemeta::core::JSON{
              "Use the `nextCursor` returned by a prior resources/list "
              "response, or omit it to start from the beginning"});
    }

    return sourcemeta::core::jsonrpc_make_success(
        request_id, sourcemeta::core::mcp_decorate_cacheable_result(
                        version, pages.at(static_cast<std::size_t>(index)),
                        this->cache_policy(metadata),
                        sourcemeta::core::MCP_METHOD_RESOURCES_LIST));
  }

  auto
  on_resource_templates_list(const sourcemeta::core::MCPProtocolVersion version,
                             const sourcemeta::core::JSON &request_json,
                             const std::string_view view) const
      -> sourcemeta::core::JSON {
    const auto &metadata{this->metadata_for(view)};
    return sourcemeta::core::jsonrpc_make_success(
        request_json.at("id"),
        sourcemeta::core::mcp_decorate_cacheable_result(
            version,
            metadata.at(sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST),
            this->cache_policy(metadata),
            sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST));
  }

  [[nodiscard]] auto
  on_server_discover(const sourcemeta::core::MCPProtocolVersion version,
                     const sourcemeta::core::JSON &request_json,
                     const std::string_view view) const
      -> sourcemeta::core::JSON {
    const auto &metadata{this->metadata_for(view)};
    const auto &parts{metadata.at("advertisement")};
    return sourcemeta::core::mcp_make_server_discover_response(
        version, request_json.at("id"),
        sourcemeta::core::MCPServerCapabilities{
            .prompts = parts.at(0).to_boolean(),
            .resources = parts.at(1).to_boolean(),
            .tools = parts.at(2).to_boolean(),
            .logging = parts.at(3).to_boolean(),
            .completions = parts.at(4).to_boolean(),
        },
        sourcemeta::core::MCPImplementation{
            .name = parts.at(5).to_string(),
            .version = parts.at(6).to_string(),
            .title = parts.at(7).to_string(),
            .description = parts.at(8).to_string(),
            .website_url = parts.at(9).to_string(),
        },
        MCP_SUPPORTED_VERSIONS, parts.at(10).to_string(),
        this->cache_policy(metadata));
  }

  auto on_initialize(const sourcemeta::core::JSON &request_json,
                     const std::string_view view) const
      -> sourcemeta::core::JSON {
    const auto &parts{this->metadata_for(view).at("advertisement")};
    return sourcemeta::core::mcp_make_initialize_response(
        request_json,
        sourcemeta::core::MCPServerCapabilities{
            .prompts = parts.at(0).to_boolean(),
            .resources = parts.at(1).to_boolean(),
            .tools = parts.at(2).to_boolean(),
            .logging = parts.at(3).to_boolean(),
            .completions = parts.at(4).to_boolean(),
        },
        sourcemeta::core::MCPImplementation{
            .name = parts.at(5).to_string(),
            .version = parts.at(6).to_string(),
            .title = parts.at(7).to_string(),
            .description = parts.at(8).to_string(),
            .website_url = parts.at(9).to_string(),
        },
        MCP_SUPPORTED_VERSIONS, parts.at(10).to_string());
  }

  auto on_tools_list(const sourcemeta::core::MCPProtocolVersion version,
                     const sourcemeta::core::JSON &request_json,
                     const std::string_view view) const
      -> sourcemeta::core::JSON {
    const auto &precomputed{
        this->metadata_for(view).at(sourcemeta::core::MCP_METHOD_TOOLS_LIST)};

    auto tools{sourcemeta::core::JSON::make_array()};
    for (const auto &tool : precomputed.as_array()) {
      std::optional<sourcemeta::core::JSON> output_schema;
      if (!tool.at(3).is_null()) {
        output_schema = tool.at(3);
      }
      tools.push_back(sourcemeta::core::mcp_make_tool_descriptor(
          version, tool.at(0).to_string(), tool.at(1).to_string(), tool.at(2),
          std::move(output_schema),
          sourcemeta::core::MCPToolAnnotations{
              .title = tool.at(4).to_string(),
              .read_only = tool.at(5).to_boolean(),
              .destructive = tool.at(6).to_boolean(),
              .idempotent = tool.at(7).to_boolean(),
              .open_world = tool.at(8).to_boolean(),
          },
          sourcemeta::core::MCPDescriptorPresentation{
              .title = tool.at(4).to_string(),
          }));
    }
    // The listing follows the router's own order, which is settled at build
    // time, so what a caller is handed twice is the same list in the same order
    return sourcemeta::core::jsonrpc_make_success(
        request_json.at("id"),
        sourcemeta::core::mcp_make_tools_list_result(
            version, std::move(tools), std::nullopt,
            this->cache_policy(this->metadata_for(view))));
  }

  auto
  on_resources_read(const sourcemeta::core::MCPProtocolVersion version,
                    const sourcemeta::core::JSON &request_json,
                    const sourcemeta::one::Authentication::Caller &caller) const
      -> sourcemeta::core::JSON {
    const auto &request_id{request_json.at("id")};
    const auto &uri{request_json.at("params").at("uri").to_string()};

    bool bundle{false};
    std::optional<std::string> conversion;
    try {
      sourcemeta::core::URI request{uri};
      if (request.fragment().has_value()) {
        return sourcemeta::core::jsonrpc_make_error(
            &request_id, -32602, "Invalid resource schema URI",
            sourcemeta::core::JSON{
                "URIs accepted by resources/read must not contain a fragment "
                "and may only carry optional `bundle` or `as` query "
                "parameters"});
      }
      request.canonicalize();
      // The MCP `resources/read` URI must match the `{+path}{?bundle,as}`
      // resource template exactly. Any query parameter other than `bundle` and
      // `as` is outside the template and must be rejected to keep the input
      // shape predictable for clients
      if (const auto query_view{request.query()}; query_view.has_value()) {
        const auto conversion_value{query_view->at("as")};
        const auto expected_size{
            static_cast<std::ptrdiff_t>(query_view->at("bundle").has_value()) +
            static_cast<std::ptrdiff_t>(conversion_value.has_value())};
        if (std::ranges::distance(*query_view) != expected_size) {
          return sourcemeta::core::jsonrpc_make_error(
              &request_id, -32602, "Invalid resource schema URI",
              sourcemeta::core::JSON{
                  "URIs accepted by resources/read must not contain a "
                  "fragment and may only carry optional `bundle` or `as` "
                  "query parameters"});
        }

        if (conversion_value.has_value()) {
          conversion.emplace(conversion_value.value());
        }
      }
      request.relative_to(sourcemeta::core::URI{this->server_uri()});
      if (request.is_absolute()) {
        return sourcemeta::core::jsonrpc_make_error(
            &request_id, -32007, "Foreign URI",
            sourcemeta::core::JSON{"This URI lies outside this catalog's "
                                   "namespace. Query the appropriate registry "
                                   "instead"});
      }
      bundle = request.query().has_value() &&
               request.query()->at("bundle").has_value();
    } catch (const std::exception &) {
      return sourcemeta::core::mcp_make_error_resource_not_found(version,
                                                                 &request_id);
    }

    auto resolution{this->artifact_resolve_path(caller, uri, Tree::Schemas,
                                                bundle ? "bundle" : "schema")};
    if (!resolution.path.has_value()) {
      return sourcemeta::core::mcp_make_error_resource_not_found(version,
                                                                 &request_id);
    }

    if (conversion.has_value()) {
      const auto target{sourcemeta::one::conversion_target(conversion.value())};
      if (target.has_value()) {
        auto converted{this->artifact_resolve_path(
            caller, uri, Tree::Schemas,
            sourcemeta::one::conversion_artifact(target.value()))};
        if (!converted.path.has_value()) {
          const auto declared{sourcemeta::one::declared_schema_dialect(
              *this, resolution.path.value())};
          if (!declared.has_value()) {
            return sourcemeta::core::jsonrpc_make_error(
                &request_id, -32602, "Invalid resource schema URI",
                sourcemeta::core::JSON{
                    "Schemas with custom dialects cannot be converted, as "
                    "their meta-schemas would need to be converted too"});
          }

          // Every schema on the upgrade ladder has an artifact for every
          // dialect newer than it, unless it is a meta-schema, which gets none
          if (sourcemeta::one::conversion_applies(declared.value(),
                                                  target.value())) {
            return sourcemeta::core::jsonrpc_make_error(
                &request_id, -32602, "Invalid resource schema URI",
                sourcemeta::core::JSON{
                    "Meta-schemas are not converted, as restating the dialect "
                    "that a meta-schema describes is not always safe"});
          }
        }

        resolution = std::move(converted);
      }

      if (!target.has_value() || !resolution.path.has_value()) {
        return sourcemeta::core::jsonrpc_make_error(
            &request_id, -32602, "Invalid resource schema URI",
            sourcemeta::core::JSON{
                "The schema cannot be converted into this dialect"});
      }
    }

    const auto schema{this->artifact_read_json(resolution.path.value())};
    if (!schema.has_value()) {
      return sourcemeta::core::mcp_make_error_resource_not_found(version,
                                                                 &request_id);
    }

    std::ostringstream payload;
    sourcemeta::core::prettify(schema.value(), payload);

    auto contents{sourcemeta::core::JSON::make_array()};
    contents.push_back(sourcemeta::core::mcp_make_resource_text_content(
        uri, MCP_TEMPLATE_MIME_TYPE, payload.str()));
    // Whether this schema is one anybody reaches, which is what decides who
    // may keep a copy of it. A listing is settled by the view it was built
    // for, whereas a single schema is settled by itself
    return sourcemeta::core::jsonrpc_make_success(
        request_id,
        sourcemeta::core::mcp_make_resources_read_result(
            version, std::move(contents),
            {.ttl_ms = MCP_CACHE_TTL_MILLISECONDS,
             .scope = resolution.is_public
                          ? sourcemeta::core::MCPCacheScope::Public
                          : sourcemeta::core::MCPCacheScope::Private}));
  }

  auto on_tools_call(const sourcemeta::core::MCPProtocolVersion version,
                     const sourcemeta::core::JSON &request_json,
                     const sourcemeta::one::Authentication::Caller &caller)
      -> sourcemeta::core::JSON {
    const auto view{caller.view()};
    const auto &request_id{request_json.at("id")};
    const auto &name{request_json.at("params").at("name").to_string()};
    const auto &tool_routes{this->metadata_for(view).at("toolRoutes")};
    if (!tool_routes.defines(name)) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Invalid tool name",
          sourcemeta::core::JSON{
              "Use `tools/list` to discover the available tools"});
    }
    const auto identifier{
        static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
            tool_routes.at(name).to_integer())};
    auto *instance{this->dispatcher().action(identifier)};
    if (instance == nullptr) [[unlikely]] {
      return sourcemeta::core::jsonrpc_make_error_internal(&request_id);
    }
    const auto *arguments{
        sourcemeta::core::mcp_tool_call_arguments(request_json)};
    const auto empty_arguments{sourcemeta::core::JSON::make_object()};
    try {
      return instance->mcp(version, request_id,
                           arguments == nullptr ? empty_arguments : *arguments,
                           caller);
    } catch (const std::exception &error) {
      return sourcemeta::core::mcp_make_tool_error(version, request_id,
                                                   error.what());
    }
  }

  // What the newest revision adds to whatever a handler produced: the kind of
  // result it is, and who produced it. Settled in one place, so that a handler
  // answers its own question and nothing else
  [[nodiscard]] auto
  decorated(const sourcemeta::core::MCPProtocolVersion version,
            sourcemeta::core::JSON envelope) const -> sourcemeta::core::JSON {
    if (version != sourcemeta::core::MCPProtocolVersion::V_2026_07_28) {
      return envelope;
    }

    auto *result{envelope.try_at("result")};
    if (result == nullptr || !result->is_object()) {
      return envelope;
    }

    const auto &parts{this->mcp_metadata_.at("advertisement")};
    envelope.assign("result", sourcemeta::core::mcp_decorate_result(
                                  version, std::move(*result),
                                  sourcemeta::core::MCPImplementation{
                                      .name = parts.at(5).to_string(),
                                      .version = parts.at(6).to_string(),
                                      .title = parts.at(7).to_string(),
                                      .description = parts.at(8).to_string(),
                                      .website_url = parts.at(9).to_string(),
                                  }));
    return envelope;
  }

  auto
  on_message(const std::optional<sourcemeta::core::MCPProtocolVersion> served,
             const std::string_view requested,
             const std::span<const std::pair<std::string, std::string>> routing,
             sourcemeta::one::HTTPRequest &request,
             sourcemeta::one::HTTPResponse &response, std::string &&body,
             const sourcemeta::one::Authentication::Caller &caller) -> void {
    // A refusal decided before a revision was settled speaks in the one that
    // predates the header, since there is nothing else to speak in
    const auto version{
        served.value_or(sourcemeta::core::MCPProtocolVersion::V_2025_03_26)};

    sourcemeta::core::JSON request_json{nullptr};
    try {
      request_json = sourcemeta::core::parse_json(body);
    } catch (const std::exception &) {
      // A body that cannot be read names no request either, so a revision
      // this instance does not serve is still the first thing to say about it
      if (!served.has_value()) {
        this->write_envelope(
            request, response, sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
            sourcemeta::core::mcp_make_error_unsupported_protocol_version(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, nullptr,
                requested, MCP_SUPPORTED_VERSIONS),
            version);
        return;
      }

      this->write_envelope(
          request, response,
          version == sourcemeta::core::MCPProtocolVersion::V_2026_07_28
              ? sourcemeta::core::HTTP_STATUS_BAD_REQUEST
              : sourcemeta::core::HTTP_STATUS_OK,
          mcp_transport_error(version, sourcemeta::core::JSONRPC_CODE_PARSE,
                              "Parse error"),
          version);
      return;
    }

    // Held back until the body was read so that the refusal names the request
    // it answers. A revision this instance does not serve leaves the request
    // perfectly readable, so there is an identifier to echo and no excuse for
    // leaving it out
    if (!served.has_value()) {
      this->write_envelope(
          request, response, sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
          sourcemeta::core::mcp_make_error_unsupported_protocol_version(
              sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
              sourcemeta::core::jsonrpc_request_id(request_json), requested,
              MCP_SUPPORTED_VERSIONS),
          version);
      return;
    }

    // The fields beside the body are the request's own account of where it is
    // going, and a component that routes on one while another acts on the
    // other is the thing this prevents, so they agree before either is used
    if (version == sourcemeta::core::MCPProtocolVersion::V_2026_07_28) {
      std::vector<std::pair<sourcemeta::core::JSON::StringView,
                            sourcemeta::core::JSON::StringView>>
          fields;
      fields.reserve(routing.size());
      for (const auto &field : routing) {
        fields.emplace_back(field.first, field.second);
      }

      const auto refusal{sourcemeta::core::mcp_validate_request_headers(
          version, fields, request_json, MCP_SUPPORTED_VERSIONS)};
      if (refusal.has_value()) {
        this->write_envelope(request, response,
                             mcp_error_status(version, refusal.value(), true),
                             refusal.value(), version);
        return;
      }
    }

    if (sourcemeta::core::jsonrpc_is_batch(request_json)) {
      if (!sourcemeta::core::mcp_supports_jsonrpc_batching(version)) {
        // MCP 2025-06-18 removed JSON-RPC batching. Any array body on a
        // protocol version that doesn't support batching is treated as an
        // unrecognized batch shape: per JSON-RPC 2.0 §6, an unrecognized
        // batch yields a single Invalid Request response object.
        // https://www.jsonrpc.org/specification#batch
        this->write_envelope(
            request, response, sourcemeta::core::HTTP_STATUS_OK,
            sourcemeta::core::jsonrpc_make_error_invalid_request(nullptr),
            version);
        return;
      }
      if (!sourcemeta::core::jsonrpc_is_valid_batch(request_json)) {
        // JSON-RPC 2.0 §6: "If the batch rpc call itself fails to be
        // recognized as a valid JSON or as an Array with at least one value,
        // the response from the Server MUST be a single Response object."
        // Empty array body falls here.
        // https://www.jsonrpc.org/specification#batch
        this->write_envelope(
            request, response, sourcemeta::core::HTTP_STATUS_OK,
            sourcemeta::core::jsonrpc_make_error_invalid_request(nullptr),
            version);
        return;
      }
      auto responses{sourcemeta::core::JSON::make_array()};
      for (const auto &sub : request_json.as_array()) {
        sourcemeta::core::JSON sub_id{nullptr};
        if (const auto *parsed_id{sourcemeta::core::jsonrpc_request_id(sub)};
            parsed_id != nullptr) {
          sub_id = *parsed_id;
        }
        try {
          auto envelope{this->process_one(version, sub, caller)};
          if (envelope.has_value()) {
            responses.push_back(std::move(envelope).value());
          }
        } catch (const std::exception &) {
          // One sub-element throwing cannot poison the rest of the batch
          responses.push_back(
              sourcemeta::core::jsonrpc_make_error_internal(&sub_id));
        }
      }
      if (responses.empty()) {
        // JSON-RPC 2.0 §6: "If there are no Response objects contained within
        // the Response array as it is to be sent to the client, the server
        // MUST NOT return an empty Array and should return nothing at all."
        // A batch of pure notifications falls here.
        // https://www.jsonrpc.org/specification#batch
        response.write_status(sourcemeta::core::HTTP_STATUS_ACCEPTED);
        response.write_header("Access-Control-Allow-Origin",
                              this->allowed_origin_);
        response.write_header("Access-Control-Expose-Headers", "Link, ETag");
        response.write_header("Cache-Control",
                              sourcemeta::one::cache_control_no_store());
        response.write_header(
            "MCP-Protocol-Version",
            sourcemeta::core::mcp_protocol_version_string(version));
        sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_ACCEPTED,
                                       request, response);
        return;
      }
      this->write_envelope(request, response, sourcemeta::core::HTTP_STATUS_OK,
                           responses, version);
      return;
    }

    std::optional<sourcemeta::core::JSON> envelope;
    try {
      envelope = this->process_one(version, request_json, caller);
      if (envelope.has_value()) {
        envelope = this->decorated(version, std::move(envelope).value());
      }
    } catch (const std::exception &) {
      // Whatever a handler would not build, the caller is still owed an answer
      // that names the request it asked about
      this->write_envelope(
          request, response,
          sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
          sourcemeta::core::jsonrpc_make_error_internal(
              sourcemeta::core::jsonrpc_request_id(request_json)),
          version);
      return;
    }

    if (envelope.has_value()) {
      this->write_envelope(request, response,
                           mcp_error_status(version, envelope.value(), false),
                           envelope.value(), version);
      return;
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_ACCEPTED);
    response.write_header("Access-Control-Allow-Origin", this->allowed_origin_);
    response.write_header("Access-Control-Expose-Headers", "Link, ETag");
    response.write_header("Cache-Control",
                          sourcemeta::one::cache_control_no_store());
    response.write_header(
        "MCP-Protocol-Version",
        sourcemeta::core::mcp_protocol_version_string(version));
    sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_ACCEPTED,
                                   request, response);
  }

  auto process_one(const sourcemeta::core::MCPProtocolVersion version,
                   const sourcemeta::core::JSON &request_json,
                   const sourcemeta::one::Authentication::Caller &caller)
      -> std::optional<sourcemeta::core::JSON> {
    if (sourcemeta::core::jsonrpc_is_notification(request_json)) {
      return std::nullopt;
    }
    if (!sourcemeta::core::jsonrpc_is_request(request_json)) {
      return sourcemeta::core::jsonrpc_make_error_invalid_request(
          sourcemeta::core::jsonrpc_request_id(request_json));
    }
    const auto *request_id{sourcemeta::core::jsonrpc_request_id(request_json)};
    assert(request_id != nullptr);
    const auto method{sourcemeta::core::jsonrpc_method(request_json)};
    // A method nothing here answers is missing rather than malformed, whether
    // the revision never named it or named it and this instance declined it
    if (!mcp_serves_method(version, method)) {
      return sourcemeta::core::jsonrpc_make_error_method_not_found(*request_id);
    }
    const auto view{caller.view()};
    // Schema validation enforces MCP's stricter rules on top of JSON-RPC 2.0.
    // Most notably, MCP requires `id` MUST NOT be null even though JSON-RPC
    // §4 only calls null-id "discouraged" (technically valid). Sourcemeta One
    // follows MCP's tighter rule and rejects null-id requests here.
    // https://www.jsonrpc.org/specification (§4)
    if (!this->structural_evaluate_fast(this->request_schema_, request_json)) {
      return sourcemeta::core::jsonrpc_make_error_invalid_request(request_id);
    }
    // Resolved once here, since a request reaches one method and placing the
    // caller again inside each of them would repeat the reading
    if (method == sourcemeta::core::MCP_METHOD_SERVER_DISCOVER) {
      return this->on_server_discover(version, request_json, view);
    }
    if (method == sourcemeta::core::MCP_METHOD_INITIALIZE) {
      return this->on_initialize(request_json, view);
    }
    if (method == sourcemeta::core::MCP_METHOD_TOOLS_LIST) {
      return this->on_tools_list(version, request_json, view);
    }
    if (method == sourcemeta::core::MCP_METHOD_RESOURCES_LIST) {
      return this->on_resources_list(version, request_json, view);
    }
    if (method == sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST) {
      return this->on_resource_templates_list(version, request_json, view);
    }
    if (method == sourcemeta::core::MCP_METHOD_RESOURCES_READ) {
      return this->on_resources_read(version, request_json, caller);
    }
    if (method == sourcemeta::core::MCP_METHOD_TOOLS_CALL) {
      return this->on_tools_call(version, request_json, caller);
    }
    // Everything the gate admits is answered above, and what remains is the
    // liveness check, which carries nothing to answer with
    assert(method == sourcemeta::core::MCP_METHOD_PING);
    return sourcemeta::core::jsonrpc_make_success_empty(*request_id);
  }

  std::string_view allowed_origin_;
  std::string_view response_schema_;
  std::string_view request_schema_;
  std::string_view metadata_path_;
  sourcemeta::core::JSON mcp_metadata_{nullptr};
  std::unordered_map<std::string_view, sourcemeta::core::JSON> metadata_views_;
  // The parameter points into the URL beside it, which is composed once and
  // never moves again
  std::string metadata_url_;
  std::vector<std::pair<std::string_view, std::string_view>> challenge_;
  std::string resource_identifier_;
};

#endif
