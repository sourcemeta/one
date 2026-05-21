#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_V1_H

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include "action_serve_metapack_file_v1.h"

#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionServeExplorerArtifact_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Read a navigation artifact for browsing schemas"};

  ActionServeExplorerArtifact_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "artifact") {
        this->artifact_ = std::get<std::string_view>(value);
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
    auto absolute_path{this->base() / "explorer"};
    if (!matches.empty() && !matches.front().empty()) {
      absolute_path /= matches.front();
    }
    absolute_path /= "%";
    absolute_path /= std::string{this->artifact_} + ".metapack";
    ActionServeMetapackFile_v1::serve(absolute_path, sourcemeta::one::STATUS_OK,
                                      true, {}, this->response_schema_, request,
                                      response, this->error_schema_);
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
    sourcemeta::blaze::Evaluator evaluator;
    if (!evaluator.validate(rpc_schema_template, arguments)) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(request_id);
    }

    auto absolute_path{this->base() / "explorer"};

    if (this->artifact_ == "directory") {
      if (arguments.defines("path")) {
        const auto &path{arguments.at("path").to_string()};
        if (!path.empty()) {
          // Defense in depth: rpc.json's `path` is just `type: string`,
          // so still reject filesystem-traversal attempts before joining.
          const std::filesystem::path relative_path{path};
          if (relative_path.is_absolute()) {
            return sourcemeta::core::jsonrpc_make_error_invalid_params(
                request_id);
          }
          for (const auto &component : relative_path) {
            if (component == ".." || component == ".") {
              return sourcemeta::core::jsonrpc_make_error_invalid_params(
                  request_id);
            }
          }
          absolute_path /= relative_path;
        }
      }
    } else {
      if (!sourcemeta::core::URI::is_uri(arguments.at("schema").to_string())) {
        return sourcemeta::core::jsonrpc_make_error_invalid_params(request_id);
      }
      const auto schema_path{
          this->uri_to_relative_path(arguments.at("schema").to_string())};
      if (!schema_path.has_value()) {
        return sourcemeta::one::mcp_make_tool_error(request_id,
                                                    "Schema not found");
      }
      absolute_path /= schema_path.value();
    }

    absolute_path /= "%";
    absolute_path /= std::string{this->artifact_} + ".metapack";

    auto contents{sourcemeta::one::metapack_read_json(absolute_path)};
    if (!contents.has_value()) {
      return sourcemeta::one::mcp_make_tool_error(
          request_id, this->artifact_ == "directory" ? "Directory not found"
                                                     : "Schema not found");
    }

    if (this->artifact_ == "directory") {
      return this->make_directory_envelope(request_id,
                                           std::move(contents).value());
    }

    return sourcemeta::one::mcp_make_tool_success(request_id,
                                                  std::move(contents).value());
  }

private:
  auto make_directory_envelope(const sourcemeta::core::JSON &request_id,
                               sourcemeta::core::JSON &&result)
      -> sourcemeta::core::JSON {
    auto content{sourcemeta::core::JSON::make_array()};

    std::ostringstream payload;
    sourcemeta::core::prettify(result, payload);
    auto text_block{sourcemeta::core::JSON::make_object()};
    text_block.assign_assume_new(std::string{"type"},
                                 sourcemeta::core::JSON{"text"});
    text_block.assign_assume_new(std::string{"text"},
                                 sourcemeta::core::JSON{payload.str()});
    content.push_back(std::move(text_block));

    if (result.defines("entries") && result.at("entries").is_array()) {
      const auto &entries{result.at("entries")};
      for (std::size_t index{0}; index < entries.array_size(); ++index) {
        const auto &entry{entries.at(index)};
        if (!entry.defines("type") ||
            entry.at("type").to_string() != "schema") {
          continue;
        }
        if (!entry.defines("identifier")) {
          continue;
        }
        auto resource_link{sourcemeta::core::JSON::make_object()};
        resource_link.assign_assume_new(
            std::string{"type"}, sourcemeta::core::JSON{"resource_link"});
        resource_link.assign_assume_new(
            std::string{"uri"},
            sourcemeta::core::JSON{entry.at("identifier").to_string()});
        if (entry.defines("title")) {
          const auto &name{entry.at("title").to_string()};
          if (!name.empty()) {
            resource_link.assign_assume_new(std::string{"name"},
                                            sourcemeta::core::JSON{name});
          }
        }
        if (entry.defines("description")) {
          const auto &description{entry.at("description").to_string()};
          if (!description.empty()) {
            resource_link.assign_assume_new(
                std::string{"description"},
                sourcemeta::core::JSON{description});
          }
        }
        resource_link.assign_assume_new(
            std::string{"mimeType"},
            sourcemeta::core::JSON{"application/schema+json"});
        content.push_back(std::move(resource_link));
      }
    }

    auto envelope_result{sourcemeta::core::JSON::make_object()};
    envelope_result.assign_assume_new(std::string{"content"},
                                      std::move(content));
    envelope_result.assign_assume_new(std::string{"structuredContent"},
                                      std::move(result));
    envelope_result.assign_assume_new(std::string{"isError"},
                                      sourcemeta::core::JSON{false});
    return sourcemeta::core::jsonrpc_make_success(request_id,
                                                  std::move(envelope_result));
  }

  std::string_view artifact_;
  std::string_view response_schema_;
  std::string_view rpc_schema_;
  std::string_view error_schema_;
};

#endif
