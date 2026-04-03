#ifndef SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_EVALUATE_H
#define SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_EVALUATE_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <cassert>       // assert
#include <cstdint>       // std::uint8_t
#include <filesystem>    // std::filesystem::path
#include <span>          // std::span
#include <sstream>       // std::ostringstream
#include <stdexcept>     // std::runtime_error
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <utility>       // std::move, std::to_underlying

class ActionJSONSchemaEvaluate {
public:
  ActionJSONSchemaEvaluate(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : base_{base}, server_url_path_{router.base_path()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "mode") {
        this->mode_ = static_cast<EvaluateMode>(std::get<std::int64_t>(value));
      }
    });
  }

  auto run(const std::span<std::string_view> matches,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void {
    const auto &mode{this->mode_};
    const auto &path{matches.front()};
    // A CORS pre-flight request
    if (request.method() == "options") {
      response.write_status(sourcemeta::one::STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin", "*");
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      response.write_header("Access-Control-Allow-Headers", "Content-Type");
      response.write_header("Access-Control-Max-Age", "3600");
      sourcemeta::one::send_response(sourcemeta::one::STATUS_NO_CONTENT,
                                     request, response);
    } else if (request.method() == "post") {
      auto template_path{this->base_ / "schemas"};
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
              "/self/v1/schemas/api/error");
        } else {
          sourcemeta::one::json_error(
              request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
              "There is nothing at this URL", "/self/v1/schemas/api/error");
        }

        return;
      }

      request.body(
          [this, mode, template_path = std::move(template_path)](
              sourcemeta::one::HTTPRequest &callback_request,
              sourcemeta::one::HTTPResponse &callback_response,
              std::string &&body, bool too_big) {
            this->body_callback(template_path, mode, callback_request,
                                callback_response, std::move(body), too_big);
          },
          [](sourcemeta::one::HTTPRequest &callback_request,
             sourcemeta::one::HTTPResponse &callback_response,
             const std::exception_ptr &error) {
            try {
              std::rethrow_exception(error);
            } catch (const std::exception &exception) {
              sourcemeta::one::json_error(
                  callback_request, callback_response,
                  sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
                  "uncaught-error", exception.what(),
                  "/self/v1/schemas/api/error");
            }
          });
    } else {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
          "method-not-allowed", "This HTTP method is invalid for this URL",
          "/self/v1/schemas/api/error");
    }
  }

  enum class EvaluateMode : std::uint8_t { Standard, Trace };

private:
  auto
  resolve_vocabulary(const std::string_view keyword_location,
                     const sourcemeta::core::WeakPointer &evaluate_path,
                     const sourcemeta::core::JSON &static_locations,
                     std::unordered_map<std::string, sourcemeta::core::JSON>
                         &referenced_locations) -> sourcemeta::core::JSON {
    const std::string keyword_location_string{keyword_location};
    const sourcemeta::core::JSON *location{nullptr};
    if (static_locations.defines(keyword_location_string)) {
      location = &static_locations.at(keyword_location_string);
    } else {
      const auto fragment_position{keyword_location_string.find('#')};
      const std::string schema_uri{
          keyword_location_string.substr(0, fragment_position)};

      auto cached{referenced_locations.find(schema_uri)};
      if (cached == referenced_locations.end()) {
        const auto directory{sourcemeta::one::schema_directory(
            this->base_, this->server_url_path_, keyword_location_string)};
        if (directory.has_value()) {
          const auto locations_path{directory.value() / "locations.metapack"};
          if (std::filesystem::exists(locations_path)) {
            auto locations{sourcemeta::one::metapack_read_json(locations_path)};
            if (locations.has_value() && locations.value().is_object() &&
                locations.value().defines("static")) {
              cached = referenced_locations
                           .emplace(schema_uri, std::move(locations.value()))
                           .first;
            }
          }
        }
      }

      if (cached != referenced_locations.end()) {
        const auto &ref_static{cached->second.at("static")};
        if (ref_static.defines(keyword_location_string)) {
          location = &ref_static.at(keyword_location_string);
        }
      }
    }

    if (location == nullptr || !location->is_object() ||
        !location->defines("baseDialect") || !location->defines("dialect")) {
      return sourcemeta::core::JSON{nullptr};
    }

    const auto base_dialect{sourcemeta::core::to_base_dialect(
        location->at("baseDialect").to_string())};
    if (!base_dialect.has_value()) {
      return sourcemeta::core::JSON{nullptr};
    }

    const auto vocabularies{sourcemeta::core::vocabularies(
        sourcemeta::core::schema_resolver, base_dialect.value(),
        location->at("dialect").to_string())};
    const auto &walker_result{sourcemeta::core::schema_walker(
        evaluate_path.back().to_property(), vocabularies)};
    if (walker_result.vocabulary.has_value()) {
      return sourcemeta::core::to_json(std::string{
          sourcemeta::core::to_string(walker_result.vocabulary.value())});
    }

    return sourcemeta::core::JSON{nullptr};
  }

  auto trace(sourcemeta::blaze::Evaluator &evaluator,
             const sourcemeta::blaze::Template &schema_template,
             const std::string &instance,
             const std::filesystem::path &template_path)
      -> sourcemeta::core::JSON {
    auto steps{sourcemeta::core::JSON::make_array()};

    auto locations_path{template_path.parent_path() / "locations.metapack"};
    // TODO: Cache loaded locations across trace requests for performance
    const auto locations_option{
        sourcemeta::one::metapack_read_json(locations_path)};
    assert(locations_option.has_value());
    const auto &locations{locations_option.value()};
    if (!locations.is_object() || !locations.defines("static")) {
      throw std::runtime_error("Failed to read schema locations metadata");
    }
    const auto &static_locations{locations.at("static")};

    // When tracing through $ref, keyword locations may point into referenced
    // schemas whose locations are in separate metapack files. This map holds
    // lazily loaded locations for referenced schemas, keyed by schema URI.
    // TODO: Cache loaded locations across trace requests for performance
    std::unordered_map<std::string, sourcemeta::core::JSON>
        referenced_locations;

    sourcemeta::core::PointerPositionTracker tracker;
    sourcemeta::core::JSON instance_json{nullptr};
    sourcemeta::core::parse_json(instance, instance_json, std::ref(tracker));
    const auto result{evaluator.validate(
        schema_template, instance_json,
        [this, &steps, &tracker, &static_locations, &instance_json,
         &referenced_locations](
            const sourcemeta::blaze::EvaluationType type, const bool valid,
            const sourcemeta::blaze::Instruction &instruction,
            const sourcemeta::blaze::InstructionExtra &extra,
            const sourcemeta::core::WeakPointer &evaluate_path,
            const sourcemeta::core::WeakPointer &instance_location,
            const sourcemeta::core::JSON &annotation) {
          auto step{sourcemeta::core::JSON::make_object()};

          if (type == sourcemeta::blaze::EvaluationType::Pre) {
            step.assign("type", sourcemeta::core::JSON{"push"});
          } else if (valid) {
            step.assign("type", sourcemeta::core::JSON{"pass"});
          } else {
            step.assign("type", sourcemeta::core::JSON{"fail"});
          }

          step.assign(
              "name",
              sourcemeta::core::JSON{
                  sourcemeta::blaze::InstructionNames[std::to_underlying(
                      instruction.type)]});
          step.assign("evaluatePath",
                      sourcemeta::core::JSON{
                          sourcemeta::core::to_string(evaluate_path)});
          step.assign("instanceLocation",
                      sourcemeta::core::JSON{
                          sourcemeta::core::to_string(instance_location)});
          auto instance_positions{tracker.get(
              // TODO: Can we avoid converting the weak pointer into a pointer
              // here?
              sourcemeta::core::to_pointer(instance_location))};
          if (!instance_positions.has_value()) {
            throw std::runtime_error("Failed to resolve instance positions");
          }
          step.assign(
              "instancePositions",
              sourcemeta::core::to_json(std::move(instance_positions).value()));
          step.assign("keywordLocation",
                      sourcemeta::core::JSON{
                          sourcemeta::core::to_string(extra.keyword_location)});
          step.assign("annotation", annotation);

          if (type == sourcemeta::blaze::EvaluationType::Pre) {
            step.assign("message", sourcemeta::core::JSON{nullptr});
          } else {
            step.assign("message",
                        sourcemeta::core::JSON{sourcemeta::blaze::describe(
                            valid, instruction, evaluate_path,
                            instance_location, instance_json, annotation)});
          }

          step.assign("vocabulary",
                      this->resolve_vocabulary(
                          sourcemeta::core::to_string(extra.keyword_location),
                          evaluate_path, static_locations,
                          referenced_locations));

          steps.push_back(std::move(step));
        })};

    auto document{sourcemeta::core::JSON::make_object()};
    document.assign("valid", sourcemeta::core::JSON{result});
    document.assign("steps", std::move(steps));
    return document;
  }

  auto evaluate(const std::filesystem::path &template_path,
                const std::string &instance, const EvaluateMode mode)
      -> sourcemeta::core::JSON {
    if (!std::filesystem::exists(template_path)) {
      throw std::runtime_error("Schema template not found");
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
      throw std::runtime_error("Failed to parse schema template");
    }

    sourcemeta::blaze::Evaluator evaluator;

    switch (mode) {
      case EvaluateMode::Standard:
        return sourcemeta::blaze::standard(
            evaluator, schema_template.value(),
            sourcemeta::core::parse_json(instance),
            sourcemeta::blaze::StandardOutput::Basic);
      case EvaluateMode::Trace:
        return this->trace(evaluator, schema_template.value(), instance,
                           template_path);
      default:
        std::unreachable();
    }
  }

  auto body_callback(const std::filesystem::path &template_path,
                     const EvaluateMode mode,
                     sourcemeta::one::HTTPRequest &request,
                     sourcemeta::one::HTTPResponse &response,
                     std::string &&body, bool too_big) -> void {
    if (too_big) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_PAYLOAD_TOO_LARGE,
          "payload-too-large", "The request body is too large",
          "/self/v1/schemas/api/error");
      return;
    }

    if (body.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST, "no-instance",
          "You must pass an instance to validate against",
          "/self/v1/schemas/api/error");
      return;
    }

    try {
      const auto result{this->evaluate(template_path, body, mode)};
      response.write_status(sourcemeta::one::STATUS_OK);
      response.write_header("Content-Type", "application/json");
      response.write_header("Access-Control-Allow-Origin", "*");
      if (mode == EvaluateMode::Trace) {
        sourcemeta::one::write_link_header(
            response, "/self/v1/schemas/api/schemas/trace/response");
      } else {
        sourcemeta::one::write_link_header(
            response, "/self/v1/schemas/api/schemas/evaluate/response");
      }

      std::ostringstream payload;
      sourcemeta::core::prettify(result, payload);
      sourcemeta::one::send_response(sourcemeta::one::STATUS_OK, request,
                                     response, payload.str(),
                                     sourcemeta::one::Encoding::Identity);
    } catch (const std::exception &exception) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
          "evaluation-error", exception.what(), "/self/v1/schemas/api/error");
    }
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::filesystem::path &base_;
  std::string_view server_url_path_;
  EvaluateMode mode_{EvaluateMode::Standard};
};

#endif
