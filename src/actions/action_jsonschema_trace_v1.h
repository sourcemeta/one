#ifndef SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_TRACE_V1_H
#define SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_TRACE_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/dispatcher.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include "action_jsonschema_evaluate_v1.h"

#include <cassert>       // assert
#include <exception>     // std::exception
#include <filesystem>    // std::filesystem::path
#include <span>          // std::span
#include <stdexcept>     // std::runtime_error
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <utility>       // std::move, std::to_underlying

class ActionJSONSchemaTrace_v1 : public sourcemeta::one::Action {
public:
  static constexpr std::string_view DESCRIPTION{
      "Validate a JSON instance against a schema and return detailed "
      "information about every step of the evaluation process"};

  ActionJSONSchemaTrace_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::ActionDispatcher &)
      : sourcemeta::one::Action{base, router.base_path(), router.base_url()} {
    std::string_view request_schema;
    router.arguments(identifier, [this, &request_schema](const auto &key,
                                                         const auto &value) {
      if (key == "requestSchema") {
        request_schema = std::get<std::string_view>(value);
      } else if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "rpcSchema") {
        this->rpc_schema_ = std::get<std::string_view>(value);
      } else if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });

    this->request_schema_template_ = this->blaze_template(
        request_schema, sourcemeta::blaze::Mode::FastValidation);
  }

  auto rest(const std::span<std::string_view> matches,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    ActionJSONSchemaEvaluate_v1::serve_post(
        matches, request, response, this->base(), this->response_schema_,
        this->error_schema_, this->request_schema_template_,
        [this](const std::filesystem::path &template_path,
               const std::string &body) -> sourcemeta::core::JSON {
          return this->evaluate(template_path, body);
        });
  }

  auto mcp(const sourcemeta::core::JSON &envelope)
      -> sourcemeta::core::JSON override {
    const auto *id{sourcemeta::core::jsonrpc_request_id(envelope)};
    const sourcemeta::core::JSON request_id{
        id ? *id : sourcemeta::core::JSON{nullptr}};

    const auto *params{sourcemeta::core::jsonrpc_params(envelope)};
    if (params == nullptr || !params->is_object() ||
        !params->defines("arguments")) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(request_id);
    }

    const auto &arguments{params->at("arguments")};
    // TODO: Cache the compiled template across invocations
    const auto rpc_schema_template{this->blaze_template(
        this->rpc_schema_, sourcemeta::blaze::Mode::FastValidation)};
    sourcemeta::blaze::Evaluator rpc_evaluator;
    if (!rpc_evaluator.validate(rpc_schema_template, arguments)) {
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

    try {
      // TODO: Wire instance source positions through the MCP path so trace
      // steps can carry `instancePositions`. The model can't see the wire
      // bytes today, so positions are omitted for now.
      auto result{this->evaluate(template_path, arguments.at("instance"))};
      return sourcemeta::one::mcp_make_tool_success(request_id,
                                                    std::move(result));
    } catch (const std::exception &exception) {
      return sourcemeta::one::mcp_make_tool_error(request_id, exception.what());
    }
  }

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
        const auto directory{this->schema_directory(keyword_location_string)};
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
      return sourcemeta::core::to_json(
          sourcemeta::core::to_string(walker_result.vocabulary.value()));
    }

    return sourcemeta::core::JSON{nullptr};
  }

  auto trace(sourcemeta::blaze::Evaluator &evaluator,
             const sourcemeta::blaze::Template &schema_template,
             const sourcemeta::core::JSON &instance_json,
             const std::filesystem::path &template_path,
             const sourcemeta::core::PointerPositionTracker *tracker)
      -> sourcemeta::core::JSON {
    auto steps{sourcemeta::core::JSON::make_array()};

    auto locations_path{template_path.parent_path() / "locations.metapack"};
    // TODO: Cache loaded locations across trace requests for performance
    const auto locations_option{
        sourcemeta::one::metapack_read_json(locations_path)};
    if (!locations_option.has_value()) {
      throw std::runtime_error{"Failed to read schema locations metadata"};
    }
    const auto &locations{locations_option.value()};
    if (!locations.is_object() || !locations.defines("static")) {
      throw std::runtime_error{"Failed to read schema locations metadata"};
    }
    const auto &static_locations{locations.at("static")};

    // When tracing through $ref, keyword locations may point into referenced
    // schemas whose locations are in separate metapack files. This map holds
    // lazily loaded locations for referenced schemas, keyed by schema URI.
    // TODO: Cache loaded locations across trace requests for performance
    std::unordered_map<std::string, sourcemeta::core::JSON>
        referenced_locations;

    const auto result{evaluator.validate(
        schema_template, instance_json,
        [this, &steps, tracker, &static_locations, &instance_json,
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
          if (tracker != nullptr) {
            auto instance_positions{tracker->get(
                // TODO: Can we avoid converting the weak pointer into a pointer
                // here?
                sourcemeta::core::to_pointer(instance_location))};
            if (!instance_positions.has_value()) {
              throw std::runtime_error{"Failed to resolve instance positions"};
            }
            step.assign("instancePositions",
                        sourcemeta::core::to_json(
                            std::move(instance_positions).value()));
          }
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
                const std::string &instance) -> sourcemeta::core::JSON {
    const auto schema_template{
        ActionJSONSchemaTrace_v1::compile_template(template_path)};
    sourcemeta::core::PointerPositionTracker tracker;
    sourcemeta::core::JSON instance_json{nullptr};
    sourcemeta::core::parse_json(instance, instance_json, std::ref(tracker));
    sourcemeta::blaze::Evaluator evaluator;
    return this->trace(evaluator, schema_template, instance_json, template_path,
                       &tracker);
  }

  auto evaluate(const std::filesystem::path &template_path,
                const sourcemeta::core::JSON &instance)
      -> sourcemeta::core::JSON {
    const auto schema_template{
        ActionJSONSchemaTrace_v1::compile_template(template_path)};
    sourcemeta::blaze::Evaluator evaluator;
    return this->trace(evaluator, schema_template, instance, template_path,
                       nullptr);
  }

  static auto compile_template(const std::filesystem::path &template_path)
      -> sourcemeta::blaze::Template {
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
    auto schema_template{sourcemeta::blaze::from_json(template_json)};
    if (!schema_template.has_value()) {
      throw std::runtime_error{"Failed to parse schema template"};
    }
    return std::move(schema_template).value();
  }

  std::string_view response_schema_;
  std::string_view rpc_schema_;
  std::string_view error_schema_;
  sourcemeta::blaze::Template request_schema_template_;
};

#endif
