#ifndef SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_EVALUATE_V1_H
#define SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_EVALUATE_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/shared.h>

#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionJSONSchemaEvaluate_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Validate a JSON instance against a schema and return whether it "
      "is valid plus any errors"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionJSONSchemaEvaluate_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "requestSchema") {
        this->request_schema_ = std::get<std::string_view>(value);
      } else if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "mcpRequestSchema") {
        this->rpc_request_schema_ = std::get<std::string_view>(value);
      } else if (key == "mcpResponseSchema") {
        this->rpc_response_schema_ = std::get<std::string_view>(value);
      } else if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

  auto rest(const std::span<std::string_view> matches,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    ActionJSONSchemaEvaluate_v1::serve_post(
        matches, request, response, *this, this->response_schema_,
        this->error_schema_, this->request_schema_,
        [this](const std::string_view schema_uri,
               const std::string &body) -> sourcemeta::core::JSON {
          return this
              ->schema_evaluate(schema_uri, sourcemeta::core::parse_json(body),
                                sourcemeta::blaze::Mode::Exhaustive)
              .second;
        });
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion version,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments)
      -> sourcemeta::core::JSON override {
    auto [request_valid, request_output]{
        this->schema_evaluate(this->rpc_request_schema_, arguments,
                              sourcemeta::blaze::Mode::Exhaustive)};
    if (!request_valid) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Params fail against the tool request schema",
          std::move(request_output));
    }

    const auto &schema_uri{arguments.at("schema").to_string()};
    if (!sourcemeta::core::URI::is_uri(schema_uri) ||
        schema_uri.find('#') != std::string::npos ||
        schema_uri.find('?') != std::string::npos) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Invalid tool input schema URI",
          sourcemeta::core::JSON{"The schema must be an absolute URI without "
                                 "a fragment or query parameters"});
    }

    const auto schema_present{this->artifact_resolve_path(
        schema_uri, InputKind::URI, Tree::Schemas, "schema")};
    const auto evaluation_enabled{this->artifact_resolve_path(
        schema_uri, InputKind::URI, Tree::Schemas, "blaze-exhaustive")};
    if (!schema_present.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Schema not found");
    }

    if (!evaluation_enabled.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(
          request_id, "This schema was not precompiled for schema evaluation");
    }

    sourcemeta::core::JSON parsed_instance{nullptr};
    try {
      parsed_instance = sourcemeta::core::parse_json(
          arguments.at("stringifiedInstance").to_string());
    } catch (const std::exception &) {
      return sourcemeta::core::mcp_make_tool_error(
          request_id, "The instance is not valid JSON");
    } catch (...) {
      return sourcemeta::core::mcp_make_tool_error(
          request_id, "The instance is not valid JSON");
    }

    return sourcemeta::core::mcp_make_tool_success(
        version, request_id,
        this->schema_evaluate(schema_uri, parsed_instance,
                              sourcemeta::blaze::Mode::Exhaustive)
            .second);
  }

  template <typename Perform>
  static auto serve_post(const std::span<std::string_view> matches,
                         sourcemeta::one::HTTPRequest &request,
                         sourcemeta::one::HTTPResponse &response,
                         const sourcemeta::one::RouterAction &self,
                         const std::string_view response_schema,
                         const std::string_view error_schema,
                         const std::string_view request_schema, Perform perform)
      -> void {
    const auto &path{matches.front()};
    if (request.method() == "options") {
      response.write_status(sourcemeta::one::STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin", "*");
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      response.write_header("Access-Control-Allow-Headers", "Content-Type");
      response.write_header("Access-Control-Max-Age", "3600");
      sourcemeta::one::send_response(sourcemeta::one::STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    if (path.find('#') != std::string_view::npos ||
        path.find("%23") != std::string_view::npos) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST,
          "invalid-schema-uri", "The schema URI must not contain a fragment",
          error_schema);
      return;
    }

    if (request.method() != "post") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
          "method-not-allowed", "This HTTP method is invalid for this URL",
          error_schema);
      return;
    }

    std::string schema_uri{self.server_uri()};
    schema_uri.push_back('/');
    schema_uri.append(path);
    const auto schema_present{self.artifact_resolve_path(
        schema_uri, sourcemeta::one::RouterAction::InputKind::URI,
        sourcemeta::one::RouterAction::Tree::Schemas, "schema")};
    const auto evaluation_enabled{self.artifact_resolve_path(
        schema_uri, sourcemeta::one::RouterAction::InputKind::URI,
        sourcemeta::one::RouterAction::Tree::Schemas, "blaze-exhaustive")};
    if (!schema_present.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", error_schema);
      return;
    }

    if (!evaluation_enabled.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
          "no-schema-template",
          "This schema was not precompiled for schema evaluation",
          error_schema);
      return;
    }

    request.body(
        [response_schema, error_schema, schema_uri = std::move(schema_uri),
         &self, request_schema, perform = std::move(perform)](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            std::string &&body, bool too_big) {
          if (too_big) {
            sourcemeta::one::json_error(
                callback_request, callback_response,
                sourcemeta::one::STATUS_PAYLOAD_TOO_LARGE, "payload-too-large",
                "The request body is too large", error_schema);
            return;
          }

          if (body.empty()) {
            sourcemeta::one::json_error(
                callback_request, callback_response,
                sourcemeta::one::STATUS_BAD_REQUEST, "no-instance",
                "You must pass an instance to validate against", error_schema);
            return;
          }

          sourcemeta::core::JSON instance{nullptr};
          try {
            instance = sourcemeta::core::parse_json(body);
          } catch (const std::exception &) {
            sourcemeta::one::json_error(
                callback_request, callback_response,
                sourcemeta::one::STATUS_BAD_REQUEST, "invalid-json",
                "The request body is not valid JSON", error_schema);
            return;
          }

          if (!self.schema_evaluate_fast(request_schema, instance)) {
            sourcemeta::one::json_error(
                callback_request, callback_response,
                sourcemeta::one::STATUS_BAD_REQUEST, "invalid-request",
                "The request body does not match the expected schema",
                error_schema);
            return;
          }

          try {
            const auto result{perform(schema_uri, body)};
            callback_response.write_status(sourcemeta::one::STATUS_OK);
            callback_response.write_header("Content-Type", "application/json");
            callback_response.write_header("Access-Control-Allow-Origin", "*");
            sourcemeta::one::write_link_header(callback_response,
                                               response_schema);
            std::ostringstream payload;
            sourcemeta::core::prettify(result, payload);
            sourcemeta::one::send_response(
                sourcemeta::one::STATUS_OK, callback_request, callback_response,
                payload.str(), sourcemeta::one::Encoding::Identity);
          } catch (const std::exception &exception) {
            sourcemeta::one::json_error(
                callback_request, callback_response,
                sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
                "schema-evaluation-error", exception.what(), error_schema);
          }
        },
        [error_schema](sourcemeta::one::HTTPRequest &callback_request,
                       sourcemeta::one::HTTPResponse &callback_response,
                       const std::exception_ptr &error) {
          try {
            std::rethrow_exception(error);
          } catch (const std::exception &exception) {
            sourcemeta::one::json_error(
                callback_request, callback_response,
                sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR, "uncaught-error",
                exception.what(), error_schema);
          }
        });
  }

private:
  std::string_view request_schema_;
  std::string_view response_schema_;
  std::string_view rpc_request_schema_;
  std::string_view rpc_response_schema_;
  std::string_view error_schema_;
};

#endif
