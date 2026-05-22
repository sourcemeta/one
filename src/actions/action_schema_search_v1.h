#ifndef SOURCEMETA_ONE_ACTIONS_SCHEMA_SEARCH_V1_H
#define SOURCEMETA_ONE_ACTIONS_SCHEMA_SEARCH_V1_H

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/mcp.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/search.h>

#include <charconv>     // std::from_chars
#include <cstddef>      // std::size_t
#include <cstdint>      // std::uint8_t
#include <filesystem>   // std::filesystem
#include <span>         // std::span
#include <sstream>      // std::ostringstream
#include <string>       // std::string
#include <string_view>  // std::string_view
#include <system_error> // std::errc
#include <utility>      // std::move

class ActionSchemaSearch_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Search for schemas by query term"};

  ActionSchemaSearch_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url()},
        search_view_{base / "explorer" / "%" / "search.metapack"} {
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

  auto rest(const std::span<std::string_view>,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {

    if (request.method() != "get") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
          "method-not-allowed", "This HTTP method is invalid for this URL",
          this->error_schema_);
      return;
    }

    const auto query{request.query("q")};
    if (query.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST,
          "missing-query", "You must provide a query parameter to search for",
          this->error_schema_);
      return;
    }

    constexpr std::size_t MAXIMUM_QUERY_LENGTH{256};
    if (query.size() > MAXIMUM_QUERY_LENGTH) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST,
          "invalid-search-query",
          "The search query must not exceed 256 characters",
          this->error_schema_);
      return;
    }

    constexpr std::size_t DEFAULT_LIMIT{10};
    constexpr std::size_t MAXIMUM_LIMIT{100};
    std::size_t limit{DEFAULT_LIMIT};
    const auto limit_param{request.query("limit")};
    if (!limit_param.empty()) {
      std::size_t parsed_limit{0};
      const auto [pointer, error_code] = std::from_chars(
          limit_param.data(), limit_param.data() + limit_param.size(),
          parsed_limit);
      if (error_code != std::errc{} ||
          pointer != limit_param.data() + limit_param.size() ||
          parsed_limit < 1 || parsed_limit > MAXIMUM_LIMIT) {
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_BAD_REQUEST,
            "invalid-search-limit",
            "The limit must be a positive integer between 1 and 100",
            this->error_schema_);
        return;
      }

      limit = parsed_limit;
    }

    std::uint8_t scope{sourcemeta::one::SearchScopePath |
                       sourcemeta::one::SearchScopeTitle |
                       sourcemeta::one::SearchScopeDescription};
    const auto scope_param{request.query("scope")};
    if (!scope_param.empty()) {
      scope = 0;
      std::string_view remaining{scope_param};
      while (!remaining.empty()) {
        const auto comma{remaining.find(',')};
        const auto token{comma != std::string_view::npos
                             ? remaining.substr(0, comma)
                             : remaining};
        if (token.empty()) {
          // Skip empty tokens from trailing commas
        } else if (token == "path") {
          scope |= sourcemeta::one::SearchScopePath;
        } else if (token == "title") {
          scope |= sourcemeta::one::SearchScopeTitle;
        } else if (token == "description") {
          scope |= sourcemeta::one::SearchScopeDescription;
        } else {
          sourcemeta::one::json_error(
              request, response, sourcemeta::one::STATUS_BAD_REQUEST,
              "invalid-search-scope",
              "The scope must be a comma-separated list of: path, title, "
              "description",
              this->error_schema_);
          return;
        }

        if (comma != std::string_view::npos) {
          remaining = remaining.substr(comma + 1);
        } else {
          break;
        }
      }

      if (scope == 0) {
        sourcemeta::one::json_error(
            request, response, sourcemeta::one::STATUS_BAD_REQUEST,
            "invalid-search-scope",
            "The scope must be a comma-separated list of: path, title, "
            "description",
            this->error_schema_);
        return;
      }
    }

    auto result{this->search_view_.search(query, limit, scope)};
    response.write_status(sourcemeta::one::STATUS_OK);
    response.write_header("Access-Control-Allow-Origin", "*");
    response.write_header("Content-Type", "application/json");
    sourcemeta::one::write_link_header(response, this->response_schema_);
    std::ostringstream output;
    sourcemeta::core::prettify(result, output);
    sourcemeta::one::send_response(sourcemeta::one::STATUS_OK, request,
                                   response, output.str(),
                                   sourcemeta::one::Encoding::Identity);
  }

  auto mcp(const sourcemeta::one::MCPProtocolVersion version,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments, const std::string_view)
      -> sourcemeta::core::JSON override {
    if (!this->validate(this->rpc_schema_, arguments)) {
      return sourcemeta::core::jsonrpc_make_error_invalid_params(request_id);
    }

    constexpr std::size_t DEFAULT_LIMIT{10};
    std::size_t limit{DEFAULT_LIMIT};
    if (arguments.defines("limit")) {
      limit = static_cast<std::size_t>(arguments.at("limit").to_integer());
    }

    std::uint8_t scope{sourcemeta::one::SearchScopePath |
                       sourcemeta::one::SearchScopeTitle |
                       sourcemeta::one::SearchScopeDescription};
    if (arguments.defines("scope")) {
      scope = 0;
      for (const auto &item : arguments.at("scope").as_array()) {
        const auto &token{item.to_string()};
        if (token == "path") {
          scope |= sourcemeta::one::SearchScopePath;
        } else if (token == "title") {
          scope |= sourcemeta::one::SearchScopeTitle;
        } else if (token == "description") {
          scope |= sourcemeta::one::SearchScopeDescription;
        }
      }
    }

    auto result{
        this->search_view_.search(arguments.at("q").to_string(), limit, scope)};

    auto content{sourcemeta::core::JSON::make_array()};

    std::ostringstream payload;
    sourcemeta::core::prettify(result, payload);
    content.push_back(sourcemeta::one::mcp_make_text_block(payload.str()));

    for (std::size_t index{0}; index < result.array_size(); ++index) {
      const auto &entry{result.at(index)};
      content.push_back(sourcemeta::one::mcp_make_resource_link(
          version, entry.at("identifier").to_string(),
          "application/schema+json", entry.at("title").to_string(),
          entry.at("description").to_string()));
    }

    return sourcemeta::one::mcp_make_tool_success(
        version, request_id, std::move(result), std::move(content));
  }

private:
  sourcemeta::one::SearchView search_view_;
  std::string_view response_schema_;
  std::string_view rpc_schema_;
  std::string_view error_schema_;
};

#endif
