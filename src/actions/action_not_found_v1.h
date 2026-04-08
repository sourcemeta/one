#ifndef SOURCEMETA_ONE_ACTIONS_NOT_FOUND_V1_H
#define SOURCEMETA_ONE_ACTIONS_NOT_FOUND_V1_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionNotFound_v1 : public sourcemeta::one::Action {
public:
  ActionNotFound_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "errorSchema") {
        this->error_schema_ = std::get<std::string_view>(value);
      }
    });
  }

  auto run(const std::span<std::string_view>,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_NOT_FOUND, "not-found",
        "There is nothing at this URL", this->error_schema_);
  }

private:
  std::string_view error_schema_;
};

#endif
