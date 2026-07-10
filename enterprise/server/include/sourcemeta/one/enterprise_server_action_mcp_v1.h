#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_MCP_V1_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_MCP_V1_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/search.h>

#include <cassert>   // assert
#include <cstddef>   // std::size_t, std::ptrdiff_t
#include <cstdint>   // std::uint64_t
#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <filesystem>  // std::filesystem
#include <iterator>    // std::ranges::distance
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionMCP_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Handle Model Context Protocol JSON-RPC requests"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionMCP_v1(const std::filesystem::path &base,
               const sourcemeta::core::URITemplateRouterView &router,
               const sourcemeta::core::URITemplateRouter::Identifier identifier,
               sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher},
        search_view_{base / "explorer" / "%" / "search.metapack"} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "responseSchema") {
            this->response_schema_ = std::get<std::string_view>(value);
          } else if (key == "requestSchema") {
            this->request_schema_ = std::get<std::string_view>(value);
          }
        });

    const auto mcp_metadata_path{
        this->artifact_resolve_path_unauthenticated("", Tree::Explorer, "mcp")};
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
  }

  auto rest(const std::span<std::string_view>, std::string_view credential,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
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
            sourcemeta::core::jsonrpc_make_error(
                nullptr, -32001, "Forbidden origin",
                sourcemeta::core::JSON{std::string{origin_header}}));
        return;
      }
    }

    if (request.method() == "options") {
      response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin",
                            this->allowed_origin_);
      response.write_header("Access-Control-Expose-Headers", "Link, ETag");
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      response.write_header("Access-Control-Allow-Headers",
                            "Content-Type, MCP-Protocol-Version");
      response.write_header("Access-Control-Max-Age", "3600");
      // Browser preflight cache is governed by `Access-Control-Max-Age`;
      // `no-store` keeps shared HTTP caches from storing this response.
      response.write_header("Cache-Control", "no-store");
      // RFC 9110 §9.3.7: OPTIONS responses SHOULD include Allow. Different
      // audience than Access-Control-Allow-Methods (HTTP vs CORS preflight).
      // https://datatracker.ietf.org/doc/html/rfc9110#section-9.3.7
      response.write_header("Allow", "POST, OPTIONS");
      // Debuggability echo (spec default, no negotiation has happened).
      response.write_header(
          "MCP-Protocol-Version",
          sourcemeta::core::mcp_protocol_version_string(
              sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
      sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    if (request.method() != "post") {
      this->write_envelope(request, response,
                           sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
                           sourcemeta::core::jsonrpc_make_error(
                               nullptr, -32004, "Method not allowed"));
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
      this->write_envelope(request, response,
                           sourcemeta::core::HTTP_STATUS_NOT_ACCEPTABLE,
                           sourcemeta::core::jsonrpc_make_error(
                               nullptr, -32006,
                               "Accept header must list application/json and "
                               "text/event-stream"));
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
          sourcemeta::core::jsonrpc_make_error(
              nullptr, -32008, "Content-Type must be application/json"));
      return;
    }

    // MCP Streamable HTTP transport / Protocol Version Header:
    // - Client MUST send `MCP-Protocol-Version` on every request after init
    // - If header is absent, server SHOULD assume 2025-03-26 (handled inside
    //   `mcp_resolve_protocol_version`)
    // - If header is invalid/unsupported, server MUST respond with 400
    // https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#protocol-version-header
    const auto requested_version{request.header("mcp-protocol-version")};
    const auto negotiated_version{
        sourcemeta::core::mcp_resolve_protocol_version(requested_version)};
    if (!negotiated_version.has_value()) {
      auto data{sourcemeta::core::JSON::make_object()};
      data.assign("current",
                  sourcemeta::core::JSON{std::string{requested_version}});
      auto supported{sourcemeta::core::JSON::make_array()};
      supported.push_back(sourcemeta::core::JSON{"2025-03-26"});
      supported.push_back(sourcemeta::core::JSON{"2025-06-18"});
      supported.push_back(sourcemeta::core::JSON{"2025-11-25"});
      data.assign("supported", std::move(supported));
      this->write_envelope(request, response,
                           sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                           sourcemeta::core::jsonrpc_make_error(
                               nullptr, -32003, "Unsupported protocol version",
                               std::move(data)));
      return;
    }

    // RFC 9110 §10.1.1: refuse unrecognised expectations with 417 before
    // touching the body. uWS already auto-acknowledged `100-continue`
    // upstream, so anything left here is a value we cannot honour.
    if (sourcemeta::one::expect_header_unrecognised(request)) {
      this->write_envelope(
          request, response, sourcemeta::core::HTTP_STATUS_EXPECTATION_FAILED,
          sourcemeta::core::jsonrpc_make_error(
              nullptr, -32009,
              "The Expect header carries an unsupported expectation"),
          negotiated_version.value());
      return;
    }

    // RFC 9110 §15.5.14: when the client declares a `Content-Length`
    // beyond the cap, fast-fail with 413 before scheduling the read.
    if (sourcemeta::one::request_body_too_large(request)) {
      this->write_envelope(request, response,
                           sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE,
                           sourcemeta::core::jsonrpc_make_error(
                               nullptr, -32005, "Request body is too large"),
                           negotiated_version.value());
      return;
    }

    request.body(
        [this, credential = std::string{credential},
         version = negotiated_version.value()](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            std::string &&body, bool too_big) {
          if (too_big) {
            this->write_envelope(
                callback_request, callback_response,
                sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE,
                sourcemeta::core::jsonrpc_make_error(
                    nullptr, -32005, "Request body is too large"),
                version);
            return;
          }
          this->on_message(version, callback_request, callback_response,
                           std::move(body), credential);
        },
        [this, version = negotiated_version.value()](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            const std::exception_ptr &error) {
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
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           std::string_view) -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  static constexpr std::string_view MCP_TEMPLATE_MIME_TYPE{
      "application/schema+json"};
  static constexpr std::size_t MCP_RESOURCES_PAGE_SIZE{50};

  static constexpr std::string_view MCP_KEY_RESOURCES{"resources"};
  static constexpr std::string_view MCP_KEY_NEXT_CURSOR{"nextCursor"};
  static inline const auto MCP_HASH_RESOURCES{
      sourcemeta::core::JSON::Object::hash(MCP_KEY_RESOURCES)};
  static inline const auto MCP_HASH_NEXT_CURSOR{
      sourcemeta::core::JSON::Object::hash(MCP_KEY_NEXT_CURSOR)};

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
    response.write_header("Cache-Control", "no-store");
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

  auto on_resources_list(const sourcemeta::core::JSON &request_json,
                         const std::string_view credential)
      -> sourcemeta::core::JSON {
    const auto &id{request_json.at("id")};

    std::uint64_t offset{0};
    const auto *params{sourcemeta::core::jsonrpc_params(request_json)};
    if (params != nullptr && params->defines("cursor")) {
      const auto cursor_input{params->at("cursor").to_string()};
      if (!cursor_input.empty()) {
        const auto parsed{sourcemeta::core::to_uint64_t(cursor_input)};
        // A cursor must parse and align to a page boundary. Reject before the
        // catalog scan so a malformed cursor cannot force the O(N) walk
        if (!parsed.has_value() ||
            parsed.value() % MCP_RESOURCES_PAGE_SIZE != 0) {
          return sourcemeta::core::jsonrpc_make_error(
              &id, -32602, "Invalid resource list cursor",
              sourcemeta::core::JSON{
                  "Use the `nextCursor` returned by a prior resources/list "
                  "response, or omit it to start from the beginning"});
        }
        offset = parsed.value();
      }
    }

    // The pages are not pre-baked: a caller only sees the schemas it is
    // admitted to, so the list is filtered against its credential at request
    // time, exactly as the search surface does
    const auto &authentication{this->dispatcher().authentication()};
    auto resources{sourcemeta::core::JSON::make_array()};
    std::uint64_t admitted{0};
    this->search_view_.for_each(
        0, this->search_view_.count(),
        [this, credential, &authentication, &resources, &admitted,
         offset](const sourcemeta::one::SearchListEntry &entry) -> void {
          if (!authentication.admits(entry.path, credential).allowed) {
            return;
          }

          const auto position{admitted++};
          if (position < offset ||
              position >= offset + MCP_RESOURCES_PAGE_SIZE) {
            return;
          }

          std::string uri{this->allowed_origin_};
          uri.append(entry.path);
          resources.push_back(sourcemeta::core::mcp_make_resource(
              uri, entry.title.empty() ? entry.path : entry.title,
              MCP_TEMPLATE_MIME_TYPE, entry.description,
              static_cast<std::size_t>(entry.bytes_raw),
              static_cast<double>(entry.priority) / 100.0));
        });

    // The alignment of a non-zero cursor is already validated above. A cursor
    // past the end of the filtered catalog is out of range, though zero is
    // always valid even when the catalog is empty
    if (offset != 0 && offset >= admitted) {
      return sourcemeta::core::jsonrpc_make_error(
          &id, -32602, "Invalid resource list cursor",
          sourcemeta::core::JSON{
              "Use the `nextCursor` returned by a prior resources/list "
              "response, or omit it to start from the beginning"});
    }

    auto page{sourcemeta::core::JSON::make_object()};
    page.assign_assume_new("resources", std::move(resources),
                           MCP_HASH_RESOURCES);
    if (offset + MCP_RESOURCES_PAGE_SIZE < admitted) {
      page.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{std::to_string(
                                 offset + MCP_RESOURCES_PAGE_SIZE)},
                             MCP_HASH_NEXT_CURSOR);
    }

    return sourcemeta::core::jsonrpc_make_success(id, std::move(page));
  }

  auto on_initialize(const sourcemeta::core::JSON &request_json) const
      -> sourcemeta::core::JSON {
    const auto &parts{
        this->mcp_metadata_.at(sourcemeta::core::MCP_METHOD_INITIALIZE)};
    return sourcemeta::core::mcp_make_initialize_result(
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
        parts.at(10).to_string());
  }

  auto on_tools_list(const sourcemeta::core::MCPProtocolVersion version,
                     const sourcemeta::core::JSON &request_json) const
      -> sourcemeta::core::JSON {
    const auto &precomputed{
        this->mcp_metadata_.at(sourcemeta::core::MCP_METHOD_TOOLS_LIST)};

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
          }));
    }
    auto result{sourcemeta::core::JSON::make_object()};
    result.assign_assume_new(std::string{"tools"}, std::move(tools));
    return sourcemeta::core::jsonrpc_make_success(request_json.at("id"),
                                                  std::move(result));
  }

  auto on_resources_read(const sourcemeta::core::JSON &request_json,
                         std::string_view credential) const
      -> sourcemeta::core::JSON {
    const auto &id{request_json.at("id")};
    const auto &uri{request_json.at("params").at("uri").to_string()};

    bool bundle{false};
    try {
      sourcemeta::core::URI request{uri};
      if (request.fragment().has_value()) {
        return sourcemeta::core::jsonrpc_make_error(
            &id, -32602, "Invalid resource schema URI",
            sourcemeta::core::JSON{
                "URIs accepted by resources/read must not contain a fragment "
                "and may only carry an optional `bundle` query parameter"});
      }
      request.canonicalize();
      // The MCP `resources/read` URI must match the `{+path}{?bundle}`
      // resource template exactly. Any query parameter other than `bundle` is
      // outside the template and must be rejected to keep the input shape
      // predictable for clients
      if (const auto query_view{request.query()}; query_view.has_value()) {
        const auto expected_size{
            static_cast<std::ptrdiff_t>(query_view->at("bundle").has_value())};
        if (std::ranges::distance(*query_view) != expected_size) {
          return sourcemeta::core::jsonrpc_make_error(
              &id, -32602, "Invalid resource schema URI",
              sourcemeta::core::JSON{
                  "URIs accepted by resources/read must not contain a "
                  "fragment "
                  "and may only carry an optional `bundle` query parameter"});
        }
      }
      request.relative_to(sourcemeta::core::URI{this->server_uri()});
      if (request.is_absolute()) {
        return sourcemeta::core::jsonrpc_make_error(
            &id, -32007, "Foreign URI",
            sourcemeta::core::JSON{"This URI lies outside this catalog's "
                                   "namespace. Query the appropriate registry "
                                   "instead"});
      }
      bundle = request.query().has_value() &&
               request.query()->at("bundle").has_value();
    } catch (const std::exception &) {
      return sourcemeta::core::jsonrpc_make_error(
          &id, sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
          "Resource not found");
    }

    const auto resolution{this->artifact_resolve_path(
        credential, uri, Tree::Schemas, bundle ? "bundle" : "schema")};
    if (resolution.outcome ==
        sourcemeta::one::ArtifactResolution::Outcome::Denied) {
      return sourcemeta::core::jsonrpc_make_error(&id, -32010,
                                                  "Authentication required");
    }
    if (resolution.outcome !=
        sourcemeta::one::ArtifactResolution::Outcome::Found) {
      return sourcemeta::core::jsonrpc_make_error(
          &id, sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
          "Resource not found");
    }
    const auto schema{this->artifact_read_json(resolution.path.value())};
    if (!schema.has_value()) {
      return sourcemeta::core::jsonrpc_make_error(
          &id, sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
          "Resource not found");
    }

    std::ostringstream payload;
    sourcemeta::core::prettify(schema.value(), payload);

    auto contents{sourcemeta::core::JSON::make_array()};
    contents.push_back(sourcemeta::core::mcp_make_resource_text_content(
        uri, MCP_TEMPLATE_MIME_TYPE, payload.str()));
    return sourcemeta::core::jsonrpc_make_success(
        id,
        sourcemeta::core::mcp_make_resources_read_result(std::move(contents)));
  }

  auto on_tools_call(const sourcemeta::core::MCPProtocolVersion version,
                     const sourcemeta::core::JSON &request_json,
                     std::string_view credential) -> sourcemeta::core::JSON {
    const auto &id{request_json.at("id")};
    const auto &name{request_json.at("params").at("name").to_string()};
    const auto &tool_routes{this->mcp_metadata_.at("toolRoutes")};
    if (!tool_routes.defines(name)) {
      return sourcemeta::core::jsonrpc_make_error(
          &id, -32602, "Invalid tool name",
          sourcemeta::core::JSON{
              "Use `tools/list` to discover the available tools"});
    }
    const auto identifier{
        static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
            tool_routes.at(name).to_integer())};
    auto *instance{this->dispatcher().action(identifier)};
    if (instance == nullptr) [[unlikely]] {
      return sourcemeta::core::jsonrpc_make_error_internal(&id);
    }
    const auto *arguments{
        sourcemeta::core::mcp_tool_call_arguments(request_json)};
    const auto empty_arguments{sourcemeta::core::JSON::make_object()};
    try {
      return instance->mcp(version, id,
                           arguments == nullptr ? empty_arguments : *arguments,
                           credential);
    } catch (const std::exception &error) {
      return sourcemeta::core::mcp_make_tool_error(id, error.what());
    }
  }

  auto on_message(const sourcemeta::core::MCPProtocolVersion version,
                  sourcemeta::one::HTTPRequest &request,
                  sourcemeta::one::HTTPResponse &response, std::string &&body,
                  std::string_view credential) -> void {
    sourcemeta::core::JSON request_json{nullptr};
    try {
      request_json = sourcemeta::core::parse_json(body);
    } catch (const std::exception &) {
      this->write_envelope(request, response, sourcemeta::core::HTTP_STATUS_OK,
                           sourcemeta::core::jsonrpc_make_error_parse(),
                           version);
      return;
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
          auto envelope{this->process_one(version, sub, credential)};
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
        response.write_header("Cache-Control", "no-store");
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

    auto envelope{this->process_one(version, request_json, credential)};
    if (envelope.has_value()) {
      this->write_envelope(request, response, sourcemeta::core::HTTP_STATUS_OK,
                           envelope.value(), version);
      return;
    }

    response.write_status(sourcemeta::core::HTTP_STATUS_ACCEPTED);
    response.write_header("Access-Control-Allow-Origin", this->allowed_origin_);
    response.write_header("Access-Control-Expose-Headers", "Link, ETag");
    response.write_header("Cache-Control", "no-store");
    response.write_header(
        "MCP-Protocol-Version",
        sourcemeta::core::mcp_protocol_version_string(version));
    sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_ACCEPTED,
                                   request, response);
  }

  auto process_one(const sourcemeta::core::MCPProtocolVersion version,
                   const sourcemeta::core::JSON &request_json,
                   std::string_view credential)
      -> std::optional<sourcemeta::core::JSON> {
    if (sourcemeta::core::jsonrpc_is_notification(request_json)) {
      return std::nullopt;
    }
    if (!sourcemeta::core::jsonrpc_is_request(request_json)) {
      return sourcemeta::core::jsonrpc_make_error_invalid_request(
          sourcemeta::core::jsonrpc_request_id(request_json));
    }
    const auto *id{sourcemeta::core::jsonrpc_request_id(request_json)};
    assert(id != nullptr);
    const auto method{sourcemeta::core::jsonrpc_method(request_json)};
    if (!sourcemeta::core::mcp_is_request_method(method)) {
      return sourcemeta::core::jsonrpc_make_error_method_not_found(*id);
    }
    // Schema validation enforces MCP's stricter rules on top of JSON-RPC 2.0.
    // Most notably, MCP requires `id` MUST NOT be null even though JSON-RPC
    // §4 only calls null-id "discouraged" (technically valid). Sourcemeta One
    // follows MCP's tighter rule and rejects null-id requests here.
    // https://www.jsonrpc.org/specification (§4)
    if (!this->schema_evaluate_fast(credential, this->request_schema_,
                                    request_json)) {
      return sourcemeta::core::jsonrpc_make_error_invalid_request(id);
    }
    if (method == sourcemeta::core::MCP_METHOD_INITIALIZE) {
      return this->on_initialize(request_json);
    }
    if (method == sourcemeta::core::MCP_METHOD_TOOLS_LIST) {
      return this->on_tools_list(version, request_json);
    }
    if (method == sourcemeta::core::MCP_METHOD_RESOURCES_LIST) {
      return this->on_resources_list(request_json, credential);
    }
    if (method == sourcemeta::core::MCP_METHOD_RESOURCES_READ) {
      return this->on_resources_read(request_json, credential);
    }
    if (method == sourcemeta::core::MCP_METHOD_TOOLS_CALL) {
      return this->on_tools_call(version, request_json, credential);
    }
    if (this->mcp_metadata_.defines(method)) {
      return sourcemeta::core::jsonrpc_make_success(
          *id, this->mcp_metadata_.at(method));
    }
    return sourcemeta::core::jsonrpc_make_success_empty(*id);
  }

  std::string_view allowed_origin_;
  std::string_view response_schema_;
  std::string_view request_schema_;
  sourcemeta::core::JSON mcp_metadata_{nullptr};
  sourcemeta::one::SearchView search_view_;
};

#endif
