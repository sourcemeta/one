#ifndef SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_SERVE_V1_H
#define SOURCEMETA_ONE_ACTIONS_JSONSCHEMA_SERVE_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/shared.h>

#include <algorithm>   // std::ranges::transform
#include <cctype>      // std::tolower
#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionJSONSchemaServe_v1 : public sourcemeta::one::RouterAction {
public:
  ActionJSONSchemaServe_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

  static auto serve(const sourcemeta::one::RouterAction &self,
                    std::string_view schema_path,
                    sourcemeta::one::HTTPRequest &request,
                    sourcemeta::one::HTTPResponse &response,
                    std::string_view error_schema) -> void {
    if (schema_path.find('#') != std::string_view::npos ||
        schema_path.find("%23") != std::string_view::npos) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST,
          "invalid-schema-uri", "The schema URI must not contain a fragment",
          error_schema);
      return;
    }

    // Because Visual Studio Code famously does not support `$id` or `id`
    // See
    // https://github.com/microsoft/vscode-json-languageservice/issues/224
    const auto &user_agent{request.header("user-agent")};
    const auto is_vscode{user_agent.starts_with("Visual Studio Code") ||
                         user_agent.starts_with("VSCodium")};
    const auto is_deno{user_agent.starts_with("Deno/")};
    const auto bundle{request.has_query("bundle")};

    const std::string_view artifact{is_vscode ? std::string_view{"editor"}
                                    : (bundle || is_deno)
                                        ? std::string_view{"bundle"}
                                        : std::string_view{"schema"}};
    const auto path{self.artifact_resolve_path(
        schema_path, sourcemeta::one::RouterAction::Tree::Schemas, artifact)};
    if (!path.has_value()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
          "There is nothing at this URL", error_schema);
      return;
    }
    self.artifact_serve(path.value(), sourcemeta::one::STATUS_OK, true,
                        is_deno ? std::string_view{"application/json"}
                                : std::string_view{},
                        {}, request, response, error_schema);
  }

  auto rest(const std::span<std::string_view> matches,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    serve(*this, matches.front(), request, response, this->error_schema_);
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  std::string_view error_schema_;
};

#endif
