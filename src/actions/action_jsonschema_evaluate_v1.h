#ifndef SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_EVALUATE_V1_H
#define SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_EVALUATE_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/shared.h>

#include <cassert>   // assert
#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionJSONSchemaEvaluate_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Validate a JSON instance against a schema and return whether it "
      "is valid plus any errors"};

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
      } else if (key == "rpcSchema") {
        this->rpc_schema_ = std::get<std::string_view>(value);
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
        [this](const std::filesystem::path &template_path,
               const std::string &body) -> sourcemeta::core::JSON {
          return this->evaluate(template_path,
                                sourcemeta::core::parse_json(body));
        });
  }

  auto mcp(const sourcemeta::one::MCPProtocolVersion version,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments, const std::string_view)
      -> sourcemeta::core::JSON override {
    if (!this->validate(this->rpc_schema_, arguments)) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(request_id);
    }

    if (!sourcemeta::core::URI::is_uri(arguments.at("schema").to_string())) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(request_id);
    }

    const auto directory{
        this->schema_directory(arguments.at("schema").to_string())};
    if (!directory.has_value()) {
      return sourcemeta::one::mcp_make_tool_error(request_id,
                                                  "Schema not found");
    }

    const auto template_path{directory.value() / "blaze-exhaustive.metapack"};
    if (!std::filesystem::exists(template_path)) {
      const auto schema_path{directory.value() / "schema.metapack"};
      if (std::filesystem::exists(schema_path)) {
        return sourcemeta::one::mcp_make_tool_error(
            request_id,
            "This schema was not precompiled for schema evaluation");
      }
      return sourcemeta::one::mcp_make_tool_error(request_id,
                                                  "Schema not found");
    }

    return sourcemeta::one::mcp_make_tool_success(
        version, request_id,
        this->evaluate(template_path, arguments.at("instance")));
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

    if (request.method() != "post") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
          "method-not-allowed", "This HTTP method is invalid for this URL",
          error_schema);
      return;
    }

    auto template_path{self.base() / "schemas"};
    template_path /= path;
    template_path /= "%";
    template_path /= "blaze-exhaustive.metapack";
    if (!std::filesystem::exists(template_path)) {
      const auto schema_path{template_path.parent_path() / "schema.metapack"};
      if (std::filesystem::exists(schema_path)) {
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
            "no-template",
            "This schema was not precompiled for schema evaluation",
            error_schema);
      } else {
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
            "There is nothing at this URL", error_schema);
      }
      return;
    }

    request.body(
        [response_schema, error_schema,
         template_path = std::move(template_path), &self, request_schema,
         perform = std::move(perform)](
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

          if (!self.validate(request_schema, instance)) {
            sourcemeta::one::json_error(
                callback_request, callback_response,
                sourcemeta::one::STATUS_BAD_REQUEST, "invalid-request",
                "The request body does not match the expected schema",
                error_schema);
            return;
          }

          try {
            const auto result{perform(template_path, body)};
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
                "evaluation-error", exception.what(), error_schema);
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
  auto evaluate(const std::filesystem::path &template_path,
                const sourcemeta::core::JSON &instance)
      -> sourcemeta::core::JSON {
    if (!std::filesystem::exists(template_path)) {
      throw std::runtime_error{"Schema template not found"};
    }

    // TODO: Cache this conversion across runs, potentially using the schema
    // file "checksum" as the cache key. This is important as the template
    // might be compressed
    const auto template_json_option{
        sourcemeta::one::metapack_read_json(template_path)};
    assert(template_json_option.has_value());
    const auto &template_json{template_json_option.value()};
    const auto schema_template{sourcemeta::blaze::from_json(template_json)};
    if (!schema_template.has_value()) {
      throw std::runtime_error{"Failed to parse schema template"};
    }

    sourcemeta::blaze::Evaluator evaluator;
    return sourcemeta::blaze::standard(
        evaluator, schema_template.value(), instance,
        sourcemeta::blaze::StandardOutput::Basic);
  }

  std::string_view request_schema_;
  std::string_view response_schema_;
  std::string_view rpc_schema_;
  std::string_view error_schema_;
};

#endif
