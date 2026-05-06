#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTIONS_H_

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/blaze/evaluator.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/search.h>

#include <filesystem>  // std::filesystem::path
#include <string>      // std::string
#include <string_view> // std::string_view

class EnterpriseMCP {
public:
  EnterpriseMCP(const std::filesystem::path &base,
                const sourcemeta::core::URITemplateRouterView &router,
                sourcemeta::core::URITemplateRouter::Identifier identifier);

  // To avoid mistakes
  EnterpriseMCP(const EnterpriseMCP &) = delete;
  EnterpriseMCP(EnterpriseMCP &&) = delete;
  auto operator=(const EnterpriseMCP &) -> EnterpriseMCP & = delete;
  auto operator=(EnterpriseMCP &&) -> EnterpriseMCP & = delete;
  ~EnterpriseMCP() = default;

  auto run(sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void;

private:
  std::filesystem::path base_;
  std::string allowed_origin_;
  std::string registry_url_;
  std::string_view response_schema_;
  sourcemeta::blaze::Template request_schema_template_;
  sourcemeta::one::SearchView search_view_;
};

#endif
