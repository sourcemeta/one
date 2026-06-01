#ifndef SOURCEMETA_ONE_ACTIONS_SCHEMA_SEARCH_V1_H
#define SOURCEMETA_ONE_ACTIONS_SCHEMA_SEARCH_V1_H

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
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
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  ActionSchemaSearch_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_path(),
                                      router.base_url(), dispatcher},
        search_view_{base / "explorer" / "%" / "search.metapack"} {
    router.arguments(identifier, [this](const auto &key, const auto &value) {
      if (key == "responseSchema") {
        this->response_schema_ = std::get<std::string_view>(value);
      } else if (key == "mcpRequestSchema") {
        this->rpc_request_schema_ = std::get<std::string_view>(value);
      } else if (key == "mcpResponseSchema") {
        this->rpc_response_schema_ = std::get<std::string_view>(value);
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
          "missing-search-query",
          "You must provide a query parameter to search for",
          this->error_schema_);
      return;
    }

    if (query.find_first_not_of(" \t\n\r\f\v") == std::string_view::npos) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST,
          "invalid-search-query",
          "The search query must contain at least one non-whitespace character",
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

  auto mcp(const sourcemeta::core::MCPProtocolVersion version,
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &arguments)
      -> sourcemeta::core::JSON override {
    auto [request_valid, request_output]{
        this->schema_evaluate(this->rpc_request_schema_, arguments,
                              sourcemeta::blaze::Mode::Exhaustive)};
    if (!request_valid) {
      return sourcemeta::core::jsonrpc_make_error(
          &request_id, -32602, "Params fail against the tool request schema",
          std::move(request_output));
    }

    constexpr std::size_t DEFAULT_LIMIT{10};
    std::size_t limit{DEFAULT_LIMIT};
    if (arguments.defines("limit")) {
      const auto &limit_argument{arguments.at("limit")};
      limit = limit_argument.is_string()
                  ? static_cast<std::size_t>(sourcemeta::core::to_uint64_t(
                                                 limit_argument.to_string())
                                                 .value())
                  : static_cast<std::size_t>(limit_argument.to_integer());
    }

    constexpr std::uint8_t DEFAULT_SCOPE{
        sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
        sourcemeta::one::SearchScopeDescription};
    std::uint8_t scope{DEFAULT_SCOPE};
    const auto add_scope_token{[&scope](const std::string_view token) -> bool {
      if (token == "path") {
        scope |= sourcemeta::one::SearchScopePath;
      } else if (token == "title") {
        scope |= sourcemeta::one::SearchScopeTitle;
      } else if (token == "description") {
        scope |= sourcemeta::one::SearchScopeDescription;
      } else {
        return false;
      }
      return true;
    }};
    if (arguments.defines("scope")) {
      scope = 0;
      const auto &scope_argument{arguments.at("scope")};
      sourcemeta::core::JSON parsed_scope{nullptr};
      const sourcemeta::core::JSON *scope_array{nullptr};
      if (scope_argument.is_string()) {
        try {
          parsed_scope =
              sourcemeta::core::parse_json(scope_argument.to_string());
        } catch (const std::exception &) {
          return sourcemeta::core::mcp_make_tool_error(
              request_id, "The scope is not valid JSON");
        }
        if (!parsed_scope.is_array()) {
          return sourcemeta::core::mcp_make_tool_error(
              request_id, "The scope must be a JSON-encoded array of strings");
        }
        if (parsed_scope.empty()) {
          return sourcemeta::core::mcp_make_tool_error(
              request_id, "The scope must contain at least one item");
        }
        scope_array = &parsed_scope;
      } else {
        scope_array = &scope_argument;
      }
      for (const auto &item : scope_array->as_array()) {
        if (!item.is_string() || !add_scope_token(item.to_string())) {
          return sourcemeta::core::mcp_make_tool_error(
              request_id, "The scope items must be \"path\", \"title\", or "
                          "\"description\"");
        }
      }
    }

    auto results{
        this->search_view_.search(arguments.at("q").to_string(), limit, scope)};
    auto envelope{sourcemeta::core::JSON::make_object()};
    envelope.assign_assume_new("results", std::move(results));

    auto content{sourcemeta::core::JSON::make_array()};

    std::ostringstream payload;
    sourcemeta::core::prettify(envelope, payload);
    content.push_back(sourcemeta::core::mcp_make_text_block(payload.str()));

    for (const auto &entry : envelope.at("results").as_array()) {
      const auto &title{entry.at("title").to_string()};
      content.push_back(sourcemeta::core::mcp_make_resource_link(
          version, entry.at("identifier").to_string(),
          "application/schema+json",
          title.empty() ? entry.at("path").to_string() : title,
          entry.at("description").to_string()));
    }

    return sourcemeta::core::mcp_make_tool_success(
        version, request_id, std::move(envelope), std::move(content));
  }

private:
  sourcemeta::one::SearchView search_view_;
  std::string_view response_schema_;
  std::string_view rpc_request_schema_;
  std::string_view rpc_response_schema_;
  std::string_view error_schema_;
};

#endif
