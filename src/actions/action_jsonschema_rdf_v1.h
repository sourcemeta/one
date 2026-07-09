#ifndef SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_RDF_V1_H
#define SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_RDF_V1_H

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonrpc.h>
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
#include <utility>     // std::move, std::unreachable
#include <variant>     // std::get, std::holds_alternative

class ActionJSONSchemaRDF_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Validate a JSON instance against a schema in the catalog and "
      "promote it to JSON-LD using the x-jsonld-* annotations of the "
      "schema"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionJSONSchemaRDF_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "requestSchema") {
            this->request_schema_ = std::get<std::string_view>(value);
          } else if (key == "responseSchema") {
            this->response_schema_ = std::get<std::string_view>(value);
          } else if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          }
        });
  }

  auto rest(const std::span<std::string_view> matches,
            std::string_view credential, sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
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
      sourcemeta::one::json_error(request, response,
                                  sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                                  "urn:sourcemeta:one:invalid-schema-uri",
                                  "The schema URI must not contain a fragment",
                                  this->error_schema_, "*");
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

    const auto base_dialect{this->schema_base_dialect(credential, schema_uri)};
    if (!base_dialect.has_value() ||
        !supports_annotations(base_dialect.value())) {
      sourcemeta::one::json_error(
          request, response,
          sourcemeta::core::HTTP_STATUS_UNPROCESSABLE_CONTENT,
          "urn:sourcemeta:one:jsonld-unsupported-dialect",
          "This schema does not declare JSON Schema 2019-09 or newer",
          this->error_schema_, "*");
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
          "urn:sourcemeta:one:payload-too-large",
          "The request body is too large", this->error_schema_, "*");
      return;
    }

    request.body(
        // A throw here is intended and caught by the surrounding error
        // handler
        // NOLINTNEXTLINE(bugprone-exception-escape)
        [this, schema_uri = std::move(schema_uri),
         credential = std::string{credential}](
            sourcemeta::one::HTTPRequest &callback_request,
            sourcemeta::one::HTTPResponse &callback_response,
            std::string &&body, bool too_big) -> void {
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
            sourcemeta::one::json_error(
                callback_request, callback_response,
                sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
                "urn:sourcemeta:one:invalid-json",
                "The request body is not valid JSON", this->error_schema_, "*");
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
            auto outcome{this->schema_jsonld(credential, schema_uri, instance)};

            if (std::holds_alternative<sourcemeta::blaze::JSONLDInvalid>(
                    outcome)) {
              auto payload{sourcemeta::core::http_make_problem_details(
                  {.status =
                       sourcemeta::core::HTTP_STATUS_UNPROCESSABLE_CONTENT,
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

            if (std::holds_alternative<
                    sourcemeta::blaze::JSONLDResolutionError>(outcome)) {
              const auto &error{
                  std::get<sourcemeta::blaze::JSONLDResolutionError>(outcome)};
              auto payload{sourcemeta::core::http_make_problem_details(
                  {.status =
                       sourcemeta::core::HTTP_STATUS_UNPROCESSABLE_CONTENT,
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
            callback_response.write_header("Content-Type",
                                           "application/ld+json");
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

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &, std::string_view)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(request_id);
  }

private:
  static auto supports_annotations(
      const sourcemeta::core::JSON::String &base_dialect) noexcept -> bool {
    return base_dialect == "https://json-schema.org/draft/2020-12/schema" ||
           base_dialect ==
               "https://json-schema.org/draft/2020-12/hyper-schema" ||
           base_dialect == "https://json-schema.org/draft/2019-09/schema" ||
           base_dialect == "https://json-schema.org/draft/2019-09/hyper-schema";
  }

  static auto facet_name(const sourcemeta::blaze::JSONLDFacet facet)
      -> std::string_view {
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

  auto send_problem(sourcemeta::one::HTTPRequest &request,
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

  std::string_view request_schema_;
  std::string_view response_schema_;
  std::string_view error_schema_;
};

#endif
