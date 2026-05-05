#include <sourcemeta/one/enterprise_server.h>

#include <sourcemeta/core/json.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared_version.h>

#include <cassert>     // assert
#include <exception>   // std::exception_ptr, std::rethrow_exception
#include <filesystem>  // std::filesystem::path
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

constexpr std::string_view MCP_PROTOCOL_VERSION{"2025-11-25"};
constexpr std::string_view MCP_LEGACY_PROTOCOL_VERSION{"2025-03-26"};
constexpr std::string_view MCP_SERVER_NAME{"sourcemeta-one-enterprise"};
constexpr std::string_view MCP_SERVER_TITLE{"Sourcemeta One Enterprise"};

auto envelope_with_id(const sourcemeta::core::JSON &id)
    -> sourcemeta::core::JSON {
  auto envelope{sourcemeta::core::JSON::make_object()};
  envelope.assign("jsonrpc", sourcemeta::core::JSON{"2.0"});
  envelope.assign("id", id);
  return envelope;
}

auto envelope_without_id() -> sourcemeta::core::JSON {
  auto envelope{sourcemeta::core::JSON::make_object()};
  envelope.assign("jsonrpc", sourcemeta::core::JSON{"2.0"});
  return envelope;
}

auto error_object(const std::int64_t code, const std::string_view message)
    -> sourcemeta::core::JSON {
  auto error{sourcemeta::core::JSON::make_object()};
  error.assign("code", sourcemeta::core::JSON{code});
  error.assign("message", sourcemeta::core::JSON{std::string{message}});
  return error;
}

auto write_envelope(sourcemeta::one::HTTPRequest &request,
                    sourcemeta::one::HTTPResponse &response,
                    const std::string_view allowed_origin,
                    const std::string_view response_schema, const char *status,
                    const sourcemeta::core::JSON &envelope) -> void {
  response.write_status(status);
  response.write_header("Content-Type", "application/json");
  response.write_header("Access-Control-Allow-Origin",
                        std::string{allowed_origin});
  if (!response_schema.empty()) {
    sourcemeta::one::write_link_header(response, response_schema);
  }
  std::ostringstream payload;
  sourcemeta::core::prettify(envelope, payload);
  sourcemeta::one::send_response(status, request, response, payload.str(),
                                 sourcemeta::one::Encoding::Identity);
}

auto write_json_envelope(sourcemeta::one::HTTPRequest &request,
                         sourcemeta::one::HTTPResponse &response,
                         const std::string_view allowed_origin,
                         const std::string_view response_schema,
                         const sourcemeta::core::JSON &envelope) -> void {
  write_envelope(request, response, allowed_origin, response_schema,
                 sourcemeta::one::STATUS_OK, envelope);
}

auto write_accepted(sourcemeta::one::HTTPRequest &request,
                    sourcemeta::one::HTTPResponse &response,
                    const std::string_view allowed_origin) -> void {
  response.write_status(sourcemeta::one::STATUS_ACCEPTED);
  response.write_header("Access-Control-Allow-Origin",
                        std::string{allowed_origin});
  sourcemeta::one::send_response(sourcemeta::one::STATUS_ACCEPTED, request,
                                 response);
}

auto method_not_allowed() -> sourcemeta::core::JSON {
  auto envelope{envelope_without_id()};
  envelope.assign("error", error_object(4, "Method not allowed"));
  return envelope;
}

auto handle_initialize(const sourcemeta::core::JSON &request_json)
    -> sourcemeta::core::JSON {
  auto envelope{envelope_with_id(request_json.at("id"))};
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("protocolVersion",
                sourcemeta::core::JSON{std::string{MCP_PROTOCOL_VERSION}});
  auto capabilities{sourcemeta::core::JSON::make_object()};
  capabilities.assign("resources", sourcemeta::core::JSON::make_object());
  result.assign("capabilities", std::move(capabilities));
  auto server_info{sourcemeta::core::JSON::make_object()};
  server_info.assign("name",
                     sourcemeta::core::JSON{std::string{MCP_SERVER_NAME}});
  server_info.assign("title",
                     sourcemeta::core::JSON{std::string{MCP_SERVER_TITLE}});
  server_info.assign("version", sourcemeta::core::JSON{
                                    std::string{sourcemeta::one::version()}});
  result.assign("serverInfo", std::move(server_info));
  envelope.assign("result", std::move(result));
  return envelope;
}

auto handle_ping(const sourcemeta::core::JSON &request_json)
    -> sourcemeta::core::JSON {
  auto envelope{envelope_with_id(request_json.at("id"))};
  envelope.assign("result", sourcemeta::core::JSON::make_object());
  return envelope;
}

auto invalid_request(const sourcemeta::core::JSON *id)
    -> sourcemeta::core::JSON {
  auto envelope{id == nullptr ? envelope_without_id() : envelope_with_id(*id)};
  envelope.assign("error", error_object(-32600, "Invalid Request"));
  return envelope;
}

auto method_not_found(const sourcemeta::core::JSON &id)
    -> sourcemeta::core::JSON {
  auto envelope{envelope_with_id(id)};
  envelope.assign("error", error_object(-32601, "Method not found"));
  return envelope;
}

auto parse_error() -> sourcemeta::core::JSON {
  auto envelope{envelope_without_id()};
  envelope.assign("error", error_object(-32700, "Parse error"));
  return envelope;
}

auto forbidden_origin() -> sourcemeta::core::JSON {
  auto envelope{envelope_without_id()};
  envelope.assign("error", error_object(2, "Forbidden origin"));
  return envelope;
}

auto unsupported_protocol_version() -> sourcemeta::core::JSON {
  auto envelope{envelope_without_id()};
  envelope.assign("error", error_object(3, "Unsupported protocol version"));
  return envelope;
}

auto request_too_large() -> sourcemeta::core::JSON {
  auto envelope{envelope_without_id()};
  envelope.assign("error", error_object(5, "Request too large"));
  return envelope;
}

auto internal_error() -> sourcemeta::core::JSON {
  auto envelope{envelope_without_id()};
  envelope.assign("error", error_object(-32603, "Internal error"));
  return envelope;
}

auto request_id(const sourcemeta::core::JSON &request_json)
    -> const sourcemeta::core::JSON * {
  if (!request_json.is_object() || !request_json.defines("id")) {
    return nullptr;
  }
  const auto &id{request_json.at("id")};
  if (!id.is_string() && !id.is_integer()) {
    return nullptr;
  }
  return &id;
}

auto looks_like_notification(const sourcemeta::core::JSON &request_json)
    -> bool {
  return request_json.is_object() && !request_json.defines("id") &&
         request_json.defines("method") &&
         request_json.at("method").is_string();
}

auto looks_like_request(const sourcemeta::core::JSON &request_json) -> bool {
  return request_id(request_json) != nullptr &&
         request_json.defines("method") &&
         request_json.at("method").is_string();
}

auto matches_request_schema(const sourcemeta::blaze::Template &schema_template,
                            const sourcemeta::core::JSON &instance) -> bool {
  sourcemeta::blaze::Evaluator evaluator;
  return evaluator.validate(schema_template, instance);
}

auto handle_jsonrpc_message(
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response,
    const std::string_view allowed_origin,
    const std::string_view response_schema,
    const sourcemeta::blaze::Template &request_schema_template,
    std::string &&body) -> void {
  sourcemeta::core::JSON request_json{nullptr};
  try {
    request_json = sourcemeta::core::parse_json(body);
  } catch (const std::exception &) {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        parse_error());
    return;
  }

  if (!request_json.is_object()) {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        invalid_request(nullptr));
    return;
  }

  if (looks_like_notification(request_json)) {
    write_accepted(request, response, allowed_origin);
    return;
  }

  if (!looks_like_request(request_json)) {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        invalid_request(nullptr));
    return;
  }

  if (!request_json.defines("jsonrpc") ||
      !request_json.at("jsonrpc").is_string() ||
      request_json.at("jsonrpc").to_string() != "2.0") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        invalid_request(&request_json.at("id")));
    return;
  }

  const auto method{request_json.at("method").to_string()};
  if (method != "initialize" && method != "ping") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        method_not_found(request_json.at("id")));
    return;
  }

  if (!matches_request_schema(request_schema_template, request_json)) {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        invalid_request(&request_json.at("id")));
    return;
  }

  if (method == "initialize") {
    write_json_envelope(request, response, allowed_origin, response_schema,
                        handle_initialize(request_json));
    return;
  }

  write_json_envelope(request, response, allowed_origin, response_schema,
                      handle_ping(request_json));
}

} // namespace

EnterpriseMCP::EnterpriseMCP(
    const std::filesystem::path &base,
    const sourcemeta::core::URITemplateRouterView &router,
    const sourcemeta::core::URITemplateRouter::Identifier identifier) {
  std::string_view request_schema;
  router.arguments(
      identifier, [this, &request_schema](const auto &key, const auto &value) {
        if (key == "responseSchema") {
          this->response_schema_ = std::get<std::string_view>(value);
        } else if (key == "requestSchema") {
          request_schema = std::get<std::string_view>(value);
        }
      });

  const auto metadata{sourcemeta::core::read_json(base / "metadata.json")};
  this->allowed_origin_ = metadata.at("origin").to_string();

  const auto base_path{router.base_path()};
  std::string_view request_schema_path{request_schema};
  if (!base_path.empty() && request_schema_path.starts_with(base_path)) {
    request_schema_path.remove_prefix(base_path.size());
  }
  if (request_schema_path.starts_with('/')) {
    request_schema_path.remove_prefix(1);
  }

  const auto template_path{base / "schemas" / request_schema_path / "%" /
                           "blaze-fast.metapack"};
  const auto template_json{sourcemeta::one::metapack_read_json(template_path)};
  assert(template_json.has_value());
  auto compiled{sourcemeta::blaze::from_json(template_json.value())};
  assert(compiled.has_value());
  this->request_schema_template_ = std::move(compiled.value());
}

auto EnterpriseMCP::run(sourcemeta::one::HTTPRequest &request,
                        sourcemeta::one::HTTPResponse &response) -> void {
  const auto origin_header{request.header("origin")};
  if (!origin_header.empty() && origin_header != this->allowed_origin_) {
    write_envelope(request, response, this->allowed_origin_,
                   this->response_schema_, sourcemeta::one::STATUS_FORBIDDEN,
                   forbidden_origin());
    return;
  }

  if (request.method() == "options") {
    response.write_status(sourcemeta::one::STATUS_NO_CONTENT);
    response.write_header("Access-Control-Allow-Origin",
                          std::string{this->allowed_origin_});
    response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
    response.write_header("Access-Control-Allow-Headers",
                          "Content-Type, MCP-Protocol-Version");
    response.write_header("Access-Control-Max-Age", "3600");
    sourcemeta::one::send_response(sourcemeta::one::STATUS_NO_CONTENT, request,
                                   response);
    return;
  }

  if (request.method() != "post") {
    write_envelope(
        request, response, this->allowed_origin_, this->response_schema_,
        sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, method_not_allowed());
    return;
  }

  const auto protocol_version_header{request.header("mcp-protocol-version")};
  if (!protocol_version_header.empty() &&
      protocol_version_header != MCP_PROTOCOL_VERSION &&
      protocol_version_header != MCP_LEGACY_PROTOCOL_VERSION) {
    write_envelope(request, response, this->allowed_origin_,
                   this->response_schema_, sourcemeta::one::STATUS_BAD_REQUEST,
                   unsupported_protocol_version());
    return;
  }

  request.body(
      [allowed_origin = std::string_view{this->allowed_origin_},
       response_schema = this->response_schema_,
       &request_schema_template = this->request_schema_template_](
          sourcemeta::one::HTTPRequest &callback_request,
          sourcemeta::one::HTTPResponse &callback_response, std::string &&body,
          bool too_big) {
        if (too_big) {
          write_envelope(callback_request, callback_response, allowed_origin,
                         response_schema,
                         sourcemeta::one::STATUS_PAYLOAD_TOO_LARGE,
                         request_too_large());
          return;
        }
        handle_jsonrpc_message(callback_request, callback_response,
                               allowed_origin, response_schema,
                               request_schema_template, std::move(body));
      },
      [allowed_origin = std::string_view{this->allowed_origin_},
       response_schema = this->response_schema_](
          sourcemeta::one::HTTPRequest &callback_request,
          sourcemeta::one::HTTPResponse &callback_response,
          const std::exception_ptr &error) {
        try {
          std::rethrow_exception(error);
        } catch (const std::exception &) {
          write_envelope(callback_request, callback_response, allowed_origin,
                         response_schema,
                         sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
                         internal_error());
        }
      });
}
