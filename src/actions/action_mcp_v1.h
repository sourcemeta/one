#ifndef SOURCEMETA_ONE_ACTIONS_MCP_V1_H
#define SOURCEMETA_ONE_ACTIONS_MCP_V1_H

#if defined(SOURCEMETA_ONE_ENTERPRISE)
#include <sourcemeta/one/enterprise_server.h>
#else

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>

#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

class ActionMCP_v1 : public sourcemeta::one::Action {
public:
  ActionMCP_v1(const std::filesystem::path &base,
               const sourcemeta::core::URITemplateRouterView &router,
               const sourcemeta::core::URITemplateRouter::Identifier identifier)
      : sourcemeta::one::Action{base, router.base_path()} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      }
    });

    const auto metadata{
        sourcemeta::core::read_json(this->base() / "metadata.json")};
    this->allowed_origin_ = metadata.at("origin").to_string();
  }

  auto run(const std::span<std::string_view>,
           sourcemeta::one::HTTPRequest &request,
           sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      response.write_status(sourcemeta::one::STATUS_NO_CONTENT);
      response.write_header("Access-Control-Allow-Origin",
                            this->allowed_origin_);
      response.write_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      response.write_header("Access-Control-Allow-Headers",
                            "Content-Type, MCP-Protocol-Version");
      response.write_header("Access-Control-Max-Age", "3600");
      sourcemeta::one::send_response(sourcemeta::one::STATUS_NO_CONTENT,
                                     request, response);
      return;
    }

    sourcemeta::core::JSON envelope{sourcemeta::core::JSON::make_object()};
    envelope.assign("jsonrpc", sourcemeta::core::JSON{"2.0"});
    sourcemeta::core::JSON error{sourcemeta::core::JSON::make_object()};

    const char *status{sourcemeta::one::STATUS_OK};
    if (request.method() != "post") {
      status = sourcemeta::one::STATUS_METHOD_NOT_ALLOWED;
      error.assign("code", sourcemeta::core::JSON{4});
      error.assign("message", sourcemeta::core::JSON{"Method not allowed"});
    } else {
      error.assign("code", sourcemeta::core::JSON{1});
      error.assign("message",
                   sourcemeta::core::JSON{"Enterprise edition required"});
      error.assign(
          "data",
          sourcemeta::core::JSON{
              "MCP support is only available in the Enterprise edition of "
              "Sourcemeta One"});
    }
    envelope.assign("error", std::move(error));

    response.write_status(status);
    response.write_header("Content-Type", "application/json");
    response.write_header("Access-Control-Allow-Origin", this->allowed_origin_);
    if (!this->response_schema_.empty()) {
      sourcemeta::one::write_link_header(response, this->response_schema_);
    }

    std::ostringstream payload;
    sourcemeta::core::prettify(envelope, payload);
    sourcemeta::one::send_response(status, request, response, payload.str(),
                                   sourcemeta::one::Encoding::Identity);
  }

private:
  std::string allowed_origin_;
  std::string_view response_schema_;
};

#endif

#endif
