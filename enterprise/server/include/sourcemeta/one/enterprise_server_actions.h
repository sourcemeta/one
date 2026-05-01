#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/blaze/evaluator.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionMCP_v1 : public sourcemeta::one::Action {
public:
  ActionMCP_v1(const std::filesystem::path &base,
               const sourcemeta::core::URITemplateRouterView &router,
               sourcemeta::core::URITemplateRouter::Identifier identifier);

  auto run(const std::span<std::string_view> matches,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override;

private:
  std::string allowed_origin_;
  std::string_view response_schema_;
  sourcemeta::blaze::Template request_schema_template_;
};

#endif
