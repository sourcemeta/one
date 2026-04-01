#ifndef SOURCEMETA_ONE_ACTIONS_SCHEMA_SEARCH_H
#define SOURCEMETA_ONE_ACTIONS_SCHEMA_SEARCH_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>
#include <sourcemeta/one/search.h>

#include <charconv>     // std::from_chars
#include <cstdint>      // std::uint8_t
#include <filesystem>   // std::filesystem
#include <span>         // std::span
#include <sstream>      // std::ostringstream
#include <string_view>  // std::string_view
#include <system_error> // std::errc

class ActionSchemaSearch {
public:
  explicit ActionSchemaSearch(const std::filesystem::path &base)
      : search_view_{base / "explorer" / "%" / "search.metapack"} {}

  auto
  run(const std::filesystem::path &, const std::span<std::string_view>,
      sourcemeta::one::HTTPRequest &request,
      sourcemeta::one::HTTPResponse &response,
      const std::span<const sourcemeta::core::URITemplateRouter::ArgumentValue>)
      -> void {

    if (request.method() != "get") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
          "method-not-allowed", "This HTTP method is invalid for this URL",
          "/self/v1/schemas/api/error");
      return;
    }

    const auto query{request.query("q")};
    if (query.empty()) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST,
          "missing-query", "You must provide a query parameter to search for",
          "/self/v1/schemas/api/error");
      return;
    }

    constexpr std::size_t MAXIMUM_QUERY_LENGTH{256};
    if (query.size() > MAXIMUM_QUERY_LENGTH) {
      sourcemeta::one::json_error(
          request, response, sourcemeta::one::STATUS_BAD_REQUEST,
          "invalid-search-query",
          "The search query must not exceed 256 characters",
          "/self/v1/schemas/api/error");
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
            "/self/v1/schemas/api/error");
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
              "/self/v1/schemas/api/error");
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
            "/self/v1/schemas/api/error");
        return;
      }
    }

    auto result{this->search_view_.search(query, limit, scope)};
    response.write_status(sourcemeta::one::STATUS_OK);
    response.write_header("Access-Control-Allow-Origin", "*");
    response.write_header("Content-Type", "application/json");
    sourcemeta::one::write_link_header(
        response, "/self/v1/schemas/api/schemas/search/response");
    std::ostringstream output;
    sourcemeta::core::prettify(result, output);
    sourcemeta::one::send_response(sourcemeta::one::STATUS_OK, request,
                                   response, output.str(),
                                   sourcemeta::one::Encoding::Identity);
  }

private:
  sourcemeta::one::SearchView search_view_;
};

#endif
