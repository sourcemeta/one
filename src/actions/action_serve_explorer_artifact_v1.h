#ifndef SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_V1_H
#define SOURCEMETA_ONE_ACTIONS_SERVE_EXPLORER_ARTIFACT_V1_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/metapack.h>

#include "action_serve_metapack_file_v1.h"
#include "mcp.h"

#include <cassert>     // assert
#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionServeExplorerArtifact_v1 : public sourcemeta::one::Action {
public:
  ActionServeExplorerArtifact_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "artifact") {
        this->artifact_ = std::get<std::string_view>(value);
      } else if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

  auto run(const std::span<std::string_view> matches,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    auto absolute_path{this->base() / "explorer"};
    if (!matches.empty()) {
      absolute_path /= matches.front();
    }
    absolute_path /= "%";
    absolute_path /= std::string{this->artifact_} + ".metapack";
    ActionServeMetapackFile_v1::serve(absolute_path, sourcemeta::one::STATUS_OK,
                                      true, {}, this->response_schema_, request,
                                      response, this->error_schema_);
  }

  auto mcp(const sourcemeta::core::JSON &input)
      -> sourcemeta::core::JSON override {
    assert(input.is_object());

    auto absolute_path{this->base() / "explorer"};
    if (input.defines("path")) {
      assert(input.at("path").is_string());
      absolute_path /= input.at("path").to_string();
    }
    absolute_path /= "%";
    absolute_path /= std::string{this->artifact_} + ".metapack";

    auto payload{sourcemeta::one::metapack_read_json(absolute_path)};
    if (!payload.has_value()) {
      return sourcemeta::one::mcp_error("not-found",
                                        "There is nothing at this URL");
    }
    return sourcemeta::one::mcp_json(std::move(payload).value());
  }

private:
  std::string_view artifact_;
  std::string_view response_schema_;
  std::string_view error_schema_;
};

#endif
