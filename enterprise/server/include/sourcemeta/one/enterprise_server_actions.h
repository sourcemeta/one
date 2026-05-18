#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/blaze/evaluator.h>

#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem::path
#include <functional>  // std::function
#include <string>      // std::string
#include <string_view> // std::string_view

class EnterpriseMCP {
public:
  // Handler invoked on `tools/call`. Takes the raw JSON-RPC envelope
  // and the loaded `mcp.metapack`, returns the JSON-RPC response
  // envelope. Set by the owning `ActionMCP_v1` (which has access to
  // the `ActionDispatcher` and can route to sibling actions' `mcp()`
  // methods). The hook is the dependency-inversion seam that lets
  // `enterprise_server` avoid linking against `actions`
  using ToolCallHandler = std::function<sourcemeta::core::JSON(
      const sourcemeta::core::JSON &envelope,
      const sourcemeta::core::JSON &mcp_metadata)>;

  EnterpriseMCP(const std::filesystem::path &base,
                const sourcemeta::core::URITemplateRouterView &router,
                sourcemeta::core::URITemplateRouter::Identifier identifier);

  // To avoid mistakes
  EnterpriseMCP(const EnterpriseMCP &) = delete;
  EnterpriseMCP(EnterpriseMCP &&) = delete;
  auto operator=(const EnterpriseMCP &) -> EnterpriseMCP & = delete;
  auto operator=(EnterpriseMCP &&) -> EnterpriseMCP & = delete;
  ~EnterpriseMCP() = default;

  auto set_tool_call_handler(ToolCallHandler handler) -> void;

  auto rest(sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void;

private:
  std::filesystem::path base_;
  std::string_view allowed_origin_;
  std::string_view registry_url_;
  std::string_view response_schema_;
  sourcemeta::blaze::Template request_schema_template_;
  sourcemeta::core::JSON mcp_metadata_{nullptr};
  ToolCallHandler tool_call_handler_;
};

#endif
