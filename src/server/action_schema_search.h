#ifndef SOURCEMETA_ONE_SERVER_ACTION_SCHEMA_SEARCH_H
#define SOURCEMETA_ONE_SERVER_ACTION_SCHEMA_SEARCH_H

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>

#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include "helpers.h"
#include "request.h"
#include "response.h"

#include <algorithm>   // std::search
#include <cassert>     // assert
#include <filesystem>  // std::filesystem
#include <sstream>     // std::ostringstream
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

static auto search(const std::filesystem::path &search_index,
                   const std::string_view query) -> sourcemeta::core::JSON {
  if (!std::filesystem::exists(search_index)) {
    throw std::runtime_error("Search index not found");
  }

  assert(search_index.is_absolute());

  sourcemeta::core::FileView view{search_index};
  const auto payload_start_option{metapack_payload_offset(view)};
  assert(payload_start_option.has_value());
  const auto &payload_start{payload_start_option.value()};
  const auto payload_size{view.size() - payload_start};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view payload{
      reinterpret_cast<const char *>(view.as<std::uint8_t>(payload_start)),
      payload_size};

  auto result{sourcemeta::core::JSON::make_array()};
  std::size_t line_start{0};
  while (line_start < payload.size()) {
    auto line_end{payload.find('\n', line_start)};
    if (line_end == std::string_view::npos) {
      line_end = payload.size();
    }

    const auto line{payload.substr(line_start, line_end - line_start)};
    line_start = line_end + 1;

    if (line.empty()) {
      continue;
    }

    if (std::search(line.cbegin(), line.cend(), query.begin(), query.end(),
                    [](const auto left, const auto right) {
                      return std::tolower(left) == std::tolower(right);
                    }) == line.cend()) {
      continue;
    }

    auto entry{sourcemeta::core::JSON::make_object()};
    const std::string line_string{line};
    auto line_json{sourcemeta::core::parse_json(line_string)};
    entry.assign("path", std::move(line_json.at(0)));
    entry.assign("title", std::move(line_json.at(1)));
    entry.assign("description", std::move(line_json.at(2)));
    result.push_back(std::move(entry));

    constexpr auto MAXIMUM_SEARCH_COUNT{10};
    if (result.array_size() >= MAXIMUM_SEARCH_COUNT) {
      break;
    }
  }

  return result;
}

} // namespace sourcemeta::one

static auto action_schema_search(const std::filesystem::path &base,
                                 sourcemeta::one::HTTPRequest &request,
                                 sourcemeta::one::HTTPResponse &response)
    -> void {
  if (request.method() == "get") {
    const auto query{request.query("q")};
    if (query.empty()) {
      json_error(request, response, sourcemeta::one::STATUS_BAD_REQUEST,
                 "missing-query",
                 "You must provide a query parameter to search for");
    } else {
      auto result{sourcemeta::one::search(
          base / "explorer" / SENTINEL / "search.metapack", query)};
      response.write_status(sourcemeta::one::STATUS_OK);
      response.write_header("Access-Control-Allow-Origin", "*");
      response.write_header("Content-Type", "application/json");
      write_link_header(response,
                        "/self/v1/schemas/api/schemas/search/response");
      std::ostringstream output;
      sourcemeta::core::prettify(result, output);
      send_response(sourcemeta::one::STATUS_OK, request, response, output.str(),
                    sourcemeta::one::Encoding::Identity);
    }
  } else {
    json_error(request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
               "method-not-allowed",
               "This HTTP method is invalid for this URL");
  }
}

#endif
