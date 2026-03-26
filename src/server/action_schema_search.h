#ifndef SOURCEMETA_ONE_SERVER_ACTION_SCHEMA_SEARCH_H
#define SOURCEMETA_ONE_SERVER_ACTION_SCHEMA_SEARCH_H

#include <sourcemeta/core/json.h>

#include <sourcemeta/one/search.h>

#include "helpers.h"
#include "request.h"
#include "response.h"

#include <sstream>     // std::ostringstream
#include <string_view> // std::string_view

static auto action_schema_search(sourcemeta::one::SearchView &search_view,
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
      // TODO: Allow configuring this
      constexpr std::size_t MAXIMUM_SEARCH_RESULTS{10};
      auto result{search_view.search(query, MAXIMUM_SEARCH_RESULTS)};
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
