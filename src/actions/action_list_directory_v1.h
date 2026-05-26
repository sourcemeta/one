#ifndef SOURCEMETA_ONE_ACTIONS_LIST_DIRECTORY_V1_H
#define SOURCEMETA_ONE_ACTIONS_LIST_DIRECTORY_V1_H

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include "action_serve_metapack_file_v1.h"

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionListDirectory_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "List the contents of a directory in the catalog"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionListDirectory_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "responseSchema") {
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
    const auto explorer_root{this->base() / "explorer"};
    auto absolute_path{explorer_root};
    if (!matches.empty() && !matches.front().empty()) {
      absolute_path /= matches.front();
    }
    absolute_path /= "%";
    absolute_path /= "directory.metapack";

    const auto safe_path{sourcemeta::core::weakly_canonical(absolute_path)};
    if (!sourcemeta::core::is_under_path(safe_path, explorer_root)) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", this->error_schema_);
      return;
    }

    ActionServeMetapackFile_v1::serve(safe_path, sourcemeta::one::STATUS_OK,
                                      true, {}, this->response_schema_, request,
                                      response, this->error_schema_);
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion version,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments, const std::string_view)
      -> sourcemeta::core::JSON override {
    if (auto output{this->validate_standard(this->rpc_schema_, arguments)};
        output.has_value()) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(
          request_id, std::move(output));
    }

    static const sourcemeta::core::JSON EMPTY_STRING{""};
    const auto explorer_root{this->base() / "explorer"};

    auto absolute_path{explorer_root};
    const auto relative_path{
        std::filesystem::path{arguments.at_or("path", EMPTY_STRING).to_string()}
            .relative_path()};
    if (!relative_path.empty()) {
      absolute_path /= relative_path;
    }
    absolute_path /= "%";
    absolute_path /= "directory.metapack";

    const auto safe_path{sourcemeta::core::weakly_canonical(absolute_path)};
    if (!sourcemeta::core::is_under_path(safe_path, explorer_root)) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(
          request_id,
          sourcemeta::core::JSON{"The path must not escape the catalog root"});
    }

    auto contents{sourcemeta::one::metapack_read_json(safe_path)};
    if (!contents.has_value()) {
      return sourcemeta::core::mcp_make_tool_error(request_id,
                                                   "Directory not found");
    }

    auto &result{contents.value()};
    auto content{sourcemeta::core::JSON::make_array()};

    std::ostringstream payload;
    sourcemeta::core::prettify(result, payload);
    content.push_back(sourcemeta::core::mcp_make_text_block(payload.str()));

    if (const auto *entries{result.try_at("entries")};
        entries != nullptr && entries->is_array()) {
      for (const auto &entry : entries->as_array()) {
        if (entry.at_or("type", EMPTY_STRING).to_string() != "schema") {
          continue;
        }
        if (!entry.defines("identifier")) {
          continue;
        }
        const auto &title{entry.at_or("title", EMPTY_STRING).to_string()};
        content.push_back(sourcemeta::core::mcp_make_resource_link(
            version, entry.at("identifier").to_string(),
            "application/schema+json",
            title.empty() ? entry.at("path").to_string() : title,
            entry.at_or("description", EMPTY_STRING).to_string()));
      }
    }

    return sourcemeta::core::mcp_make_tool_success(
        version, request_id, std::move(result), std::move(content));
  }

private:
  std::string_view response_schema_;
  std::string_view rpc_schema_;
  std::string_view error_schema_;
};

#endif
