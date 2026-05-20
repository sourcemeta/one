#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/blaze/evaluator.h>

#include <sourcemeta/one/dispatcher.h>
#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::one::enterprise {

class ActionMCP_v1 : public sourcemeta::one::Action {
public:
  static constexpr std::string_view DESCRIPTION{
      "Handle Model Context Protocol JSON-RPC requests"};

  ActionMCP_v1(const std::filesystem::path &base,
               const sourcemeta::core::URITemplateRouterView &router,
               sourcemeta::core::URITemplateRouter::Identifier identifier);

  auto rest(const std::span<std::string_view> matches,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override;

private:
  std::string_view allowed_origin_;
  std::string_view response_schema_;
  sourcemeta::blaze::Template request_schema_template_;
  sourcemeta::core::JSON mcp_metadata_{nullptr};
};

} // namespace sourcemeta::one::enterprise

#endif
