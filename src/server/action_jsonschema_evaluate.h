#ifndef SOURCEMETA_ONE_SERVER_ACTION_JSONSCHEMA_EVALUATE_H
#define SOURCEMETA_ONE_SERVER_ACTION_JSONSCHEMA_EVALUATE_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/shared.h>
#include <sourcemeta/one/storage.h>

#include "helpers.h"
#include "request.h"
#include "response.h"

#include <cassert>     // assert
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <type_traits> // std::underlying_type_t
#include <utility>     // std::move

namespace {

auto trace(sourcemeta::blaze::Evaluator &evaluator,
           const sourcemeta::blaze::Template &schema_template,
           const std::string &instance, const sourcemeta::one::Storage &storage,
           const sourcemeta::one::Storage::Key template_key)
    -> sourcemeta::core::JSON {
  auto steps{sourcemeta::core::JSON::make_array()};

  // TODO: Cache this across runs?
  const auto locations{
      storage.read_sibling_json(template_key, "locations.metapack")};
  assert(locations.defines("static"));
  const auto &static_locations{locations.at("static")};

  sourcemeta::core::PointerPositionTracker tracker;
  const auto instance_json{
      sourcemeta::core::parse_json(instance, std::ref(tracker))};
  const auto result{evaluator.validate(
      schema_template, instance_json,
      [&steps, &tracker, &static_locations, &instance_json](
          const sourcemeta::blaze::EvaluationType type, const bool valid,
          const sourcemeta::blaze::Instruction &instruction,
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

        step.assign("name", sourcemeta::core::JSON{
                                sourcemeta::blaze::InstructionNames
                                    [static_cast<std::underlying_type_t<
                                        sourcemeta::blaze::InstructionIndex>>(
                                        instruction.type)]});
        step.assign("evaluatePath", sourcemeta::core::to_json(evaluate_path));
        step.assign("instanceLocation",
                    sourcemeta::core::to_json(instance_location));
        auto instance_positions{tracker.get(
            // TODO: Can we avoid converting the weak pointer into a pointer
            // here?
            sourcemeta::core::to_pointer(instance_location))};
        assert(instance_positions.has_value());
        step.assign(
            "instancePositions",
            sourcemeta::core::to_json(std::move(instance_positions).value()));
        step.assign("keywordLocation",
                    sourcemeta::core::to_json(instruction.keyword_location));
        step.assign("annotation", annotation);

        if (type == sourcemeta::blaze::EvaluationType::Pre) {
          step.assign("message", sourcemeta::core::JSON{nullptr});
        } else {
          step.assign("message",
                      sourcemeta::core::JSON{sourcemeta::blaze::describe(
                          valid, instruction, evaluate_path, instance_location,
                          instance_json, annotation)});
        }

        // Determine keyword vocabulary
        const auto &current_location{
            static_locations.at(instruction.keyword_location)};
        const auto base_dialect_result{sourcemeta::core::to_base_dialect(
            current_location.at("baseDialect").to_string())};
        assert(base_dialect_result.has_value());
        const auto vocabularies{sourcemeta::core::vocabularies(
            sourcemeta::core::schema_resolver, base_dialect_result.value(),
            current_location.at("dialect").to_string())};
        const auto &walker_result{sourcemeta::core::schema_walker(
            evaluate_path.back().to_property(), vocabularies)};
        if (walker_result.vocabulary.has_value()) {
          step.assign(
              "vocabulary",
              sourcemeta::core::to_json(std::string{sourcemeta::core::to_string(
                  walker_result.vocabulary.value())}));
        } else {
          step.assign("vocabulary", sourcemeta::core::to_json(nullptr));
        }

        steps.push_back(std::move(step));
      })};

  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("valid", sourcemeta::core::JSON{result});
  document.assign("steps", std::move(steps));
  return document;
}

} // namespace

namespace sourcemeta::one {

enum class EvaluateType { Standard, Trace };

auto evaluate(const Storage &storage, const Storage::Key template_key,
              const std::string &instance, const EvaluateType type)
    -> sourcemeta::core::JSON {
  // TODO: Cache this conversion across runs, potentially using the schema file
  // "checksum" as the cache key. This is important as the template might be
  // compressed
  const auto template_json{storage.read_json(template_key)};
  const auto schema_template{sourcemeta::blaze::from_json(template_json)};
  assert(schema_template.has_value());

  sourcemeta::blaze::Evaluator evaluator;

  switch (type) {
    case EvaluateType::Standard:
      return sourcemeta::blaze::standard(
          evaluator, schema_template.value(),
          sourcemeta::core::parse_json(instance),
          sourcemeta::blaze::StandardOutput::Basic);
    case EvaluateType::Trace:
      return trace(evaluator, schema_template.value(), instance, storage,
                   template_key);
    default:
      // We should never get here
      assert(false);
      return sourcemeta::core::JSON{nullptr};
  }
}

} // namespace sourcemeta::one

static auto action_jsonschema_evaluate_callback(
    const sourcemeta::one::Storage &storage,
    const sourcemeta::one::Storage::Key template_key,
    const sourcemeta::one::EvaluateType mode,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response, std::string &&body, bool too_big)
    -> void {
  if (too_big) {
    json_error(request, response, sourcemeta::one::STATUS_PAYLOAD_TOO_LARGE,
               "payload-too-large", "The request body is too large");
    return;
  }

  if (body.empty()) {
    json_error(request, response, sourcemeta::one::STATUS_BAD_REQUEST,
               "no-instance", "You must pass an instance to validate against");
    return;
  }

  try {
    const auto result{
        sourcemeta::one::evaluate(storage, template_key, body, mode)};
    response.write_status(sourcemeta::one::STATUS_OK);
    response.write_header("Content-Type", "application/json");
    response.write_header("Access-Control-Allow-Origin", "*");
    if (mode == sourcemeta::one::EvaluateType::Trace) {
      write_link_header(response,
                        "/self/v1/schemas/api/schemas/trace/response");
    } else {
      write_link_header(response,
                        "/self/v1/schemas/api/schemas/evaluate/response");
    }

    std::ostringstream payload;
    sourcemeta::core::prettify(result, payload);
    send_response(sourcemeta::one::STATUS_OK, request, response, payload.str(),
                  sourcemeta::one::Encoding::Identity);
  } catch (const std::exception &exception) {
    json_error(request, response, sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
               "evaluation-error", exception.what());
  }
}

static auto action_jsonschema_evaluate(const sourcemeta::one::Storage &storage,
                                       const std::string_view &path,
                                       sourcemeta::one::HTTPRequest &request,
                                       sourcemeta::one::HTTPResponse &response,
                                       const sourcemeta::one::EvaluateType mode)
    -> void {
  // A CORS pre-flight request
  if (request.method() == "options") {
    response.write_status(sourcemeta::one::STATUS_NO_CONTENT);
    response.write_header("Access-Control-Allow-Origin", "*");
    response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
    response.write_header("Access-Control-Allow-Headers", "Content-Type");
    response.write_header("Access-Control-Max-Age", "3600");
    send_response(sourcemeta::one::STATUS_NO_CONTENT, request, response);
  } else if (request.method() == "post") {
    const auto template_key{sourcemeta::one::Storage::key(
        "schemas", path, SENTINEL, "blaze-exhaustive.metapack")};
    if (!storage.exists(template_key)) {
      if (storage.sibling_exists(template_key, "schema.metapack")) {
        json_error(request, response,
                   sourcemeta::one::STATUS_METHOD_NOT_ALLOWED, "no-template",
                   "This schema was not precompiled for schema evaluation");
      } else {
        json_error(request, response, sourcemeta::one::STATUS_NOT_FOUND,
                   "not-found", "There is nothing at this URL");
      }

      return;
    }

    request.body(
        [mode, &storage,
         template_key](sourcemeta::one::HTTPRequest &callback_request,
                       sourcemeta::one::HTTPResponse &callback_response,
                       std::string &&body, bool too_big) {
          action_jsonschema_evaluate_callback(
              storage, template_key, mode, callback_request, callback_response,
              std::move(body), too_big);
        },
        [](sourcemeta::one::HTTPRequest &callback_request,
           sourcemeta::one::HTTPResponse &callback_response,
           std::exception_ptr error) {
          try {
            std::rethrow_exception(error);
          } catch (const std::exception &exception) {
            json_error(callback_request, callback_response,
                       sourcemeta::one::STATUS_INTERNAL_SERVER_ERROR,
                       "uncaught-error", exception.what());
          }
        });
  } else {
    json_error(request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
               "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

#endif
