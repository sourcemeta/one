#include <sourcemeta/one/enterprise_server.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/shared.h>

#include <exception> // std::exception, std::exception_ptr, std::rethrow_exception
#include <sstream>   // std::ostringstream
#include <string>    // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::unreachable
#include <variant>     // std::get, std::holds_alternative

namespace sourcemeta::one::enterprise {

auto ActionJSONSchemaRDF_v1::rest(const std::span<std::string_view> matches,
                                  const std::string_view credential,
                                  sourcemeta::one::HTTPRequest &request,
                                  sourcemeta::one::HTTPResponse &response)
    -> void {
  const auto &path{matches.front()};
  if (request.method() == "options") {
    response.write_status(sourcemeta::core::HTTP_STATUS_NO_CONTENT);
    response.write_header("Access-Control-Allow-Origin", "*");
    response.write_header("Access-Control-Expose-Headers", "Link, ETag");
    response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
    response.write_header("Access-Control-Allow-Headers", "Content-Type");
    response.write_header("Access-Control-Max-Age", "3600");
    // Browser preflight cache is governed by `Access-Control-Max-Age`;
    // `no-store` keeps shared HTTP caches from storing this response.
    response.write_header("Cache-Control", "no-store");
    // RFC 9110 §9.3.7: OPTIONS responses SHOULD include Allow. Different
    // audience than Access-Control-Allow-Methods (HTTP vs CORS preflight).
    // https://datatracker.ietf.org/doc/html/rfc9110#section-9.3.7
    response.write_header("Allow", "POST, OPTIONS");
    sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_NO_CONTENT,
                                   request, response);
    return;
  }

  if (path.find('#') != std::string_view::npos ||
      path.find("%23") != std::string_view::npos) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
        "urn:sourcemeta:one:invalid-schema-uri",
        "The schema URI must not contain a fragment", this->error_schema_, "*");
    return;
  }

  if (request.method() != "post") {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
        "urn:sourcemeta:one:method-not-allowed",
        "This HTTP method is invalid for this URL", this->error_schema_, "*",
        "POST, OPTIONS");
    return;
  }

  std::string schema_uri{this->server_uri()};
  schema_uri.push_back('/');
  schema_uri.append(path);
  const auto schema_present{this->artifact_resolve_path(
      credential, schema_uri, Tree::Schemas, "schema")};
  const auto evaluation_enabled{this->artifact_resolve_path(
      credential, schema_uri, Tree::Schemas, "blaze-exhaustive")};
  if (schema_present.outcome ==
          sourcemeta::one::ArtifactResolution::Outcome::Denied ||
      evaluation_enabled.outcome ==
          sourcemeta::one::ArtifactResolution::Outcome::Denied) {
    sourcemeta::one::json_error_unauthorized(request, response,
                                             this->error_schema_, "*");
    return;
  }
  if (schema_present.outcome !=
      sourcemeta::one::ArtifactResolution::Outcome::Found) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_NOT_FOUND,
        "urn:sourcemeta:one:not-found", "There is nothing at this URL",
        this->error_schema_, "*");
    return;
  }

  if (evaluation_enabled.outcome !=
      sourcemeta::one::ArtifactResolution::Outcome::Found) {
    // RFC 9110 §15.5.6: Allow lists the methods this specific target
    // resource currently supports. POST hits this very branch (returns
    // 405) when the schema was not precompiled, so only OPTIONS is
    // actually supported on this URL.
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
        "urn:sourcemeta:one:no-schema-template",
        "This schema was not precompiled for schema evaluation",
        this->error_schema_, "*", "OPTIONS");
    return;
  }

  // RFC 9110 §10.1.1: refuse unrecognised expectations with 417 before
  // touching the body. uWS already auto-acknowledged `100-continue`
  // upstream, so anything left here is a value we cannot honour.
  if (sourcemeta::one::expect_header_unrecognised(request)) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_EXPECTATION_FAILED,
        "urn:sourcemeta:one:expectation-failed",
        "The Expect header carries an unsupported expectation",
        this->error_schema_, "*");
    return;
  }

  // RFC 9110 §15.5.14: when the client declares a `Content-Length`
  // beyond the cap, fast-fail with 413 before scheduling the read.
  if (sourcemeta::one::request_body_too_large(request)) {
    sourcemeta::one::json_error(
        request, response, sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE,
        "urn:sourcemeta:one:payload-too-large", "The request body is too large",
        this->error_schema_, "*");
    return;
  }

  const auto schema_template{
      this->dispatcher().blaze_template(evaluation_enabled.path.value())};

  request.body(
      // A throw here is intended and caught by the surrounding error
      // handler
      // NOLINTNEXTLINE(bugprone-exception-escape)
      [this, schema_template, schema_uri = std::move(schema_uri),
       credential = std::string{credential}](
          sourcemeta::one::HTTPRequest &callback_request,
          sourcemeta::one::HTTPResponse &callback_response, std::string &&body,
          bool too_big) -> void {
        if (too_big) {
          sourcemeta::one::json_error(
              callback_request, callback_response,
              sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE,
              "urn:sourcemeta:one:payload-too-large",
              "The request body is too large", this->error_schema_, "*");
          return;
        }

        if (body.empty()) {
          sourcemeta::one::json_error(
              callback_request, callback_response,
              sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
              "urn:sourcemeta:one:no-instance",
              "You must pass an instance to validate against",
              this->error_schema_, "*");
          return;
        }

        sourcemeta::core::JSON envelope{nullptr};
        try {
          envelope = sourcemeta::core::parse_json(body);
        } catch (const std::exception &) {
          sourcemeta::one::json_error(callback_request, callback_response,
                                      sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                                      "urn:sourcemeta:one:invalid-json",
                                      "The request body is not valid JSON",
                                      this->error_schema_, "*");
          return;
        }

        if (!this->schema_evaluate_fast(credential, this->request_schema_,
                                        envelope)) {
          sourcemeta::one::json_error(
              callback_request, callback_response,
              sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
              "urn:sourcemeta:one:invalid-request",
              "The request body does not match the expected schema",
              this->error_schema_, "*");
          return;
        }

        const auto &instance{envelope.at("instance")};
        const auto flatten{envelope.defines("flatten") &&
                           envelope.at("flatten").to_boolean()};
        const auto *context{
            envelope.defines("context") ? &envelope.at("context") : nullptr};

        try {
          sourcemeta::blaze::Evaluator evaluator;
          auto outcome{
              sourcemeta::blaze::jsonld(evaluator, *schema_template, instance)};

          if (std::holds_alternative<sourcemeta::blaze::JSONLDInvalid>(
                  outcome)) {
            auto payload{sourcemeta::core::http_make_problem_details(
                {.status = sourcemeta::core::HTTP_STATUS_UNPROCESSABLE_CONTENT,
                 .type = "urn:sourcemeta:one:invalid-instance",
                 .detail = "The instance does not conform to the schema"})};
            payload.assign(
                "errors",
                this->schema_evaluate(credential, schema_uri, instance,
                                      sourcemeta::blaze::Mode::Exhaustive)
                    .second.at("errors"));
            this->send_problem(
                callback_request, callback_response,
                sourcemeta::core::HTTP_STATUS_UNPROCESSABLE_CONTENT,
                std::move(payload));
            return;
          }

          if (std::holds_alternative<sourcemeta::blaze::JSONLDResolutionError>(
                  outcome)) {
            const auto &error{
                std::get<sourcemeta::blaze::JSONLDResolutionError>(outcome)};
            auto payload{sourcemeta::core::http_make_problem_details(
                {.status = sourcemeta::core::HTTP_STATUS_UNPROCESSABLE_CONTENT,
                 .type = "urn:sourcemeta:one:jsonld-resolution-error",
                 .detail = error.message})};
            payload.assign("facet",
                           sourcemeta::core::JSON{facet_name(error.facet)});
            payload.assign("instanceLocation",
                           sourcemeta::core::JSON{sourcemeta::core::to_string(
                               error.instance_location)});
            this->send_problem(
                callback_request, callback_response,
                sourcemeta::core::HTTP_STATUS_UNPROCESSABLE_CONTENT,
                std::move(payload));
            return;
          }

          auto document{std::get<sourcemeta::core::JSON>(std::move(outcome))};
          if (context != nullptr) {
            try {
              document =
                  flatten
                      ? sourcemeta::core::jsonld_flatten(document, *context)
                      : sourcemeta::core::jsonld_compact(document, *context);
            } catch (const sourcemeta::core::JSONLDError &error) {
              auto payload{sourcemeta::core::http_make_problem_details(
                  {.status = sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                   .type = "urn:sourcemeta:one:invalid-context",
                   .detail = error.what()})};
              this->send_problem(callback_request, callback_response,
                                 sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                                 std::move(payload));
              return;
            }
          } else if (flatten) {
            document = sourcemeta::core::jsonld_flatten(document);
          }

          callback_response.write_status(sourcemeta::core::HTTP_STATUS_OK);
          callback_response.write_header("Content-Type", "application/ld+json");
          callback_response.write_header("Access-Control-Allow-Origin", "*");
          callback_response.write_header("Access-Control-Expose-Headers",
                                         "Link, ETag");
          // The response is fully determined by the POST body. A
          // shared cache cannot use this for any other request, so
          // skip caching altogether.
          callback_response.write_header("Cache-Control", "no-store");
          sourcemeta::one::write_link_header(callback_response,
                                             this->response_schema_);
          std::ostringstream payload;
          sourcemeta::core::prettify(document, payload);
          sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_OK,
                                         callback_request, callback_response,
                                         payload.str(),
                                         sourcemeta::one::Encoding::Identity);
        } catch (const std::exception &exception) {
          sourcemeta::one::json_error(
              callback_request, callback_response,
              sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
              "urn:sourcemeta:one:schema-evaluation-error", exception.what(),
              this->error_schema_, "*");
        }
      },
      [this](sourcemeta::one::HTTPRequest &callback_request,
             sourcemeta::one::HTTPResponse &callback_response,
             const std::exception_ptr &error) -> void {
        try {
          std::rethrow_exception(error);
        } catch (const std::exception &exception) {
          sourcemeta::one::json_error(
              callback_request, callback_response,
              sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR,
              "urn:sourcemeta:one:uncaught-error", exception.what(),
              this->error_schema_, "*");
        }
      });
}

auto ActionJSONSchemaRDF_v1::mcp(
    const sourcemeta::core::MCPProtocolVersion version,
    const sourcemeta::core::JSON &request_id,
    const sourcemeta::core::JSON &arguments, const std::string_view credential)
    -> sourcemeta::core::JSON {
  auto [request_valid, request_output]{
      this->schema_evaluate(credential, this->rpc_request_schema_, arguments,
                            sourcemeta::blaze::Mode::Exhaustive)};
  if (!request_valid) {
    return sourcemeta::core::jsonrpc_make_error(
        &request_id, -32602, "Params fail against the tool request schema",
        std::move(request_output));
  }

  const auto &schema_uri{arguments.at("schema").to_string()};
  const auto schema_present{this->artifact_resolve_path(
      credential, schema_uri, Tree::Schemas, "schema")};
  const auto evaluation_enabled{this->artifact_resolve_path(
      credential, schema_uri, Tree::Schemas, "blaze-exhaustive")};
  if (schema_present.outcome ==
          sourcemeta::one::ArtifactResolution::Outcome::Denied ||
      evaluation_enabled.outcome ==
          sourcemeta::one::ArtifactResolution::Outcome::Denied) {
    return sourcemeta::core::mcp_make_tool_error(request_id,
                                                 "Authentication required");
  }
  if (schema_present.outcome !=
      sourcemeta::one::ArtifactResolution::Outcome::Found) {
    return sourcemeta::core::mcp_make_tool_error(request_id,
                                                 "Schema not found");
  }

  if (evaluation_enabled.outcome !=
      sourcemeta::one::ArtifactResolution::Outcome::Found) {
    return sourcemeta::core::mcp_make_tool_error(
        request_id, "This schema was not precompiled for schema evaluation");
  }

  sourcemeta::core::JSON instance{nullptr};
  try {
    instance = sourcemeta::core::parse_json(
        arguments.at("stringifiedInstance").to_string());
  } catch (const std::exception &) {
    return sourcemeta::core::mcp_make_tool_error(
        request_id, "The instance is not valid JSON");
  } catch (...) {
    return sourcemeta::core::mcp_make_tool_error(
        request_id, "The instance is not valid JSON");
  }

  sourcemeta::core::JSON context{nullptr};
  const auto with_context{arguments.defines("stringifiedContext")};
  if (with_context) {
    try {
      context = sourcemeta::core::parse_json(
          arguments.at("stringifiedContext").to_string());
    } catch (const std::exception &) {
      return sourcemeta::core::mcp_make_tool_error(
          request_id, "The context is not valid JSON");
    } catch (...) {
      return sourcemeta::core::mcp_make_tool_error(
          request_id, "The context is not valid JSON");
    }
  }

  const auto flatten{arguments.defines("flatten") &&
                     arguments.at("flatten").to_boolean()};
  const auto schema_template{
      this->dispatcher().blaze_template(evaluation_enabled.path.value())};
  sourcemeta::blaze::Evaluator evaluator;
  auto outcome{
      sourcemeta::blaze::jsonld(evaluator, *schema_template, instance)};

  if (std::holds_alternative<sourcemeta::blaze::JSONLDInvalid>(outcome)) {
    auto payload{sourcemeta::core::JSON::make_object()};
    payload.assign("valid", sourcemeta::core::JSON{false});
    payload.assign("errors",
                   this->schema_evaluate(credential, schema_uri, instance,
                                         sourcemeta::blaze::Mode::Exhaustive)
                       .second.at("errors"));
    return sourcemeta::core::mcp_make_tool_success(version, request_id,
                                                   std::move(payload));
  }

  if (std::holds_alternative<sourcemeta::blaze::JSONLDResolutionError>(
          outcome)) {
    const auto &error{
        std::get<sourcemeta::blaze::JSONLDResolutionError>(outcome)};
    return sourcemeta::core::mcp_make_tool_error(request_id, error.message);
  }

  auto document{std::get<sourcemeta::core::JSON>(std::move(outcome))};
  if (with_context) {
    try {
      document = flatten ? sourcemeta::core::jsonld_flatten(document, context)
                         : sourcemeta::core::jsonld_compact(document, context);
    } catch (const sourcemeta::core::JSONLDError &error) {
      return sourcemeta::core::mcp_make_tool_error(request_id, error.what());
    }
  } else if (flatten) {
    document = sourcemeta::core::jsonld_flatten(document);
  }

  auto payload{sourcemeta::core::JSON::make_object()};
  payload.assign("valid", sourcemeta::core::JSON{true});
  payload.assign("document", std::move(document));
  return sourcemeta::core::mcp_make_tool_success(version, request_id,
                                                 std::move(payload));
}

auto ActionJSONSchemaRDF_v1::facet_name(
    const sourcemeta::blaze::JSONLDFacet facet) -> std::string_view {
  switch (facet) {
    case sourcemeta::blaze::JSONLDFacet::Type:
      return "type";
    case sourcemeta::blaze::JSONLDFacet::Predicate:
      return "predicate";
    case sourcemeta::blaze::JSONLDFacet::Datatype:
      return "datatype";
    case sourcemeta::blaze::JSONLDFacet::Language:
      return "language";
    case sourcemeta::blaze::JSONLDFacet::Direction:
      return "direction";
    case sourcemeta::blaze::JSONLDFacet::Graph:
      return "graph";
    case sourcemeta::blaze::JSONLDFacet::JSON:
      return "json";
    case sourcemeta::blaze::JSONLDFacet::Container:
      return "container";
    case sourcemeta::blaze::JSONLDFacet::Self:
      return "self";
    default:
      std::unreachable();
  }
}

auto ActionJSONSchemaRDF_v1::send_problem(
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response,
    const sourcemeta::core::HTTPStatus &status,
    sourcemeta::core::JSON &&payload) const -> void {
  response.write_status(status);
  response.write_header("Content-Type", "application/problem+json");
  response.write_header("Cache-Control", "no-store");
  response.write_header("Access-Control-Allow-Origin", "*");
  response.write_header("Access-Control-Expose-Headers", "Link, ETag");
  sourcemeta::one::write_link_header(response, this->error_schema_);
  std::ostringstream output;
  sourcemeta::core::prettify(payload, output);
  sourcemeta::one::send_response(status, request, response, output.str(),
                                 sourcemeta::one::Encoding::Identity);
}

} // namespace sourcemeta::one::enterprise
