#ifndef SOURCEMETA_ONE_SERVER_HELPERS_H
#define SOURCEMETA_ONE_SERVER_HELPERS_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/time.h>

#include <sourcemeta/one/shared.h>

#include "request.h"
#include "response.h"

#include <cassert>     // assert
#include <chrono>      // std::chrono::system_clock
#include <iostream>    // std::cerr
#include <mutex>       // std::mutex, std::lock_guard
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <thread>      // std::this_thread
#include <utility>     // std::move

constexpr auto SENTINEL{"%"};

static auto write_link_header(sourcemeta::one::HTTPResponse &response,
                              const std::string_view schema_path) -> void {
  std::ostringstream link;
  link << "<" << schema_path << ">; rel=\"describedby\"";
  response.write_header("Link", std::move(link).str());
}

static auto log(std::string_view message) -> void {
  // Otherwise we can get messed up output interleaved from multiple threads
  static std::mutex log_mutex;
  std::lock_guard<std::mutex> guard{log_mutex};
  std::cerr << "[" << sourcemeta::core::to_gmt(std::chrono::system_clock::now())
            << "] " << std::this_thread::get_id() << ' ' << message << "\n";
}

static auto send_response(const char *const code,
                          const sourcemeta::one::HTTPRequest &request,
                          sourcemeta::one::HTTPResponse &response) -> void {
  response.send_without_content();
  std::ostringstream line;
  assert(code);
  line << code << ' ' << request.method() << ' ' << request.path();
  log(std::move(line).str());
}

static auto send_response(const char *const code,
                          const sourcemeta::one::HTTPRequest &request,
                          sourcemeta::one::HTTPResponse &response,
                          const std::string &message,
                          const sourcemeta::one::Encoding current_encoding)
    -> void {
  response.send(request, message, current_encoding);
  std::ostringstream line;
  assert(code);
  line << code << ' ' << request.method() << ' ' << request.path();
  log(std::move(line).str());
}

// See https://www.rfc-editor.org/rfc/rfc7807
static auto json_error(const sourcemeta::one::HTTPRequest &request,
                       sourcemeta::one::HTTPResponse &response,
                       const char *const code, std::string &&id,
                       std::string &&message) -> void {
  auto object{sourcemeta::core::JSON::make_object()};
  object.assign("title",
                // A URI with a custom scheme
                sourcemeta::core::JSON{"sourcemeta:one/" + std::move(id)});
  object.assign("status", sourcemeta::core::JSON{std::atoi(code)});
  object.assign("detail", sourcemeta::core::JSON{std::move(message)});
  response.write_status(code);
  response.write_header("Content-Type", "application/problem+json");
  response.write_header("Access-Control-Allow-Origin", "*");
  write_link_header(response, "/self/v1/schemas/api/error");

  std::ostringstream output;
  sourcemeta::core::prettify(object, output);
  send_response(code, request, response, output.str(),
                sourcemeta::one::Encoding::Identity);
}

#endif
