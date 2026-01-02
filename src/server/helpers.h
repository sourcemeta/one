#ifndef SOURCEMETA_ONE_SERVER_HELPERS_H
#define SOURCEMETA_ONE_SERVER_HELPERS_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/time.h>

#include <sourcemeta/one/gzip.h>
#include <sourcemeta/one/shared.h>

#include "request.h"
#include "status.h"
#include "uwebsockets.h"

#include <cassert>     // assert
#include <chrono>      // std::chrono::system_clock
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cerr
#include <mutex>       // std::mutex, std::lock_guard
#include <optional>    // std::optional
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <thread>      // std::this_thread
#include <utility>     // std::move

constexpr auto SENTINEL{"%"};

static auto write_link_header(uWS::HttpResponse<true> *response,
                              const std::string_view schema_path) -> void {
  std::ostringstream link;
  link << "<" << schema_path << ">; rel=\"describedby\"";
  response->writeHeader("Link", std::move(link).str());
}

static auto log(std::string_view message) -> void {
  // Otherwise we can get messed up output interleaved from multiple threads
  static std::mutex log_mutex;
  std::lock_guard<std::mutex> guard{log_mutex};
  std::cerr << "[" << sourcemeta::core::to_gmt(std::chrono::system_clock::now())
            << "] " << std::this_thread::get_id() << ' ' << message << "\n";
}

static auto send_response(const char *const code, const std::string_view method,
                          const std::string_view url,
                          uWS::HttpResponse<true> *response) -> void {
  response->end();
  std::ostringstream line;
  assert(code);
  line << code << ' ' << method << ' ' << url;
  log(std::move(line).str());
}

static auto
send_response(const char *const code, const std::string_view method,
              const std::string_view url, uWS::HttpResponse<true> *response,
              const std::string &message,
              const sourcemeta::one::HTTPRequest::Encoding expected_encoding,
              const sourcemeta::one::HTTPRequest::Encoding current_encoding)
    -> void {
  if (expected_encoding == sourcemeta::one::HTTPRequest::Encoding::GZIP) {
    response->writeHeader("Content-Encoding", "gzip");
    if (current_encoding == sourcemeta::one::HTTPRequest::Encoding::Identity) {
      auto effective_message{sourcemeta::one::gzip(message)};
      if (method == "head") {
        response->endWithoutBody(effective_message.size());
        response->end();
      } else {
        response->end(std::move(effective_message));
      }
    } else {
      if (method == "head") {
        response->endWithoutBody(message.size());
        response->end();
      } else {
        response->end(message);
      }
    }
  } else if (expected_encoding ==
             sourcemeta::one::HTTPRequest::Encoding::Identity) {
    if (current_encoding == sourcemeta::one::HTTPRequest::Encoding::GZIP) {
      auto effective_message{sourcemeta::one::gunzip(message)};
      if (method == "head") {
        response->endWithoutBody(effective_message.size());
        response->end();
      } else {
        response->end(effective_message);
      }
    } else {
      if (method == "head") {
        response->endWithoutBody(message.size());
        response->end();
      } else {
        response->end(message);
      }
    }
  }

  std::ostringstream line;
  assert(code);
  line << code << ' ' << method << ' ' << url;
  log(std::move(line).str());
}

// See https://www.rfc-editor.org/rfc/rfc7807
static auto json_error(const std::string_view method,
                       const std::string_view url,
                       uWS::HttpResponse<true> *response,
                       const sourcemeta::one::HTTPRequest::Encoding encoding,
                       const char *const code, std::string &&id,
                       std::string &&message) -> void {
  auto object{sourcemeta::core::JSON::make_object()};
  object.assign("title",
                // A URI with a custom scheme
                sourcemeta::core::JSON{"sourcemeta:one/" + std::move(id)});
  object.assign("status", sourcemeta::core::JSON{std::atoi(code)});
  object.assign("detail", sourcemeta::core::JSON{std::move(message)});
  response->writeStatus(code);
  response->writeHeader("Content-Type", "application/problem+json");
  response->writeHeader("Access-Control-Allow-Origin", "*");
  write_link_header(response, "/self/v1/schemas/api/error");

  std::ostringstream output;
  sourcemeta::core::prettify(object, output);
  send_response(code, method, url, response, output.str(), encoding,
                sourcemeta::one::HTTPRequest::Encoding::Identity);
}

static auto
serve_static_file(const sourcemeta::one::HTTPRequest &request,
                  uWS::HttpResponse<true> *response,
                  const std::filesystem::path &absolute_path,
                  const char *const code, const bool enable_cors = false,
                  const std::optional<std::string> &mime = std::nullopt,
                  const std::optional<std::string> &link = std::nullopt)
    -> void {
  if (request.method() != "get" && request.method() != "head") {
    if (std::filesystem::exists(absolute_path)) {
      json_error(request.method(), request.path(), response,
                 request.response_encoding(),
                 sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
                 "method-not-allowed",
                 "This HTTP method is invalid for this URL");
    } else {
      json_error(request.method(), request.path(), response,
                 request.response_encoding(), sourcemeta::one::STATUS_NOT_FOUND,
                 "not-found", "There is nothing at this URL");
    }

    return;
  }

  auto file{sourcemeta::one::read_stream_raw(absolute_path)};
  if (!file.has_value()) {
    json_error(request.method(), request.path(), response,
               request.response_encoding(), sourcemeta::one::STATUS_NOT_FOUND,
               "not-found", "There is nothing at this URL");
    return;
  }

  // Note that `If-Modified-Since` can only be used with a `GET` or `HEAD`.
  // See
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/If-Modified-Since
  const auto if_modified_since{request.header_gmt("if-modified-since")};
  // Time comparison can be flaky, but adding a bit of tolerance leads
  // to more consistent behavior.
  if (if_modified_since.has_value() &&
      (if_modified_since.value() + std::chrono::seconds(1)) >=
          file.value().last_modified) {
    response->writeStatus(sourcemeta::one::STATUS_NOT_MODIFIED);
    if (enable_cors) {
      response->writeHeader("Access-Control-Allow-Origin", "*");
    }

    send_response(sourcemeta::one::STATUS_NOT_MODIFIED, request.method(),
                  request.path(), response);
    return;
  }

  const auto &checksum{file.value().checksum};
  std::ostringstream etag_value_strong;
  std::ostringstream etag_value_weak;
  etag_value_strong << '"' << checksum << '"';
  etag_value_weak << 'W' << '/' << '"' << checksum << '"';
  for (const auto &match : request.header_list("if-none-match")) {
    // Cache hit
    if (match.first == "*" || match.first == etag_value_weak.str() ||
        match.first == etag_value_strong.str()) {
      response->writeStatus(sourcemeta::one::STATUS_NOT_MODIFIED);
      if (enable_cors) {
        response->writeHeader("Access-Control-Allow-Origin", "*");
      }

      send_response(sourcemeta::one::STATUS_NOT_MODIFIED, request.method(),
                    request.path(), response);
      return;
    }
  }

  response->writeStatus(code);

  // To support requests from web browsers
  if (enable_cors) {
    response->writeHeader("Access-Control-Allow-Origin", "*");
  }

  if (mime.has_value()) {
    response->writeHeader("Content-Type", mime.value());
  } else {
    response->writeHeader("Content-Type", file.value().mime);
  }

  response->writeHeader("Last-Modified",
                        sourcemeta::core::to_gmt(file.value().last_modified));

  std::ostringstream etag;
  etag << '"' << checksum << '"';
  response->writeHeader("ETag", std::move(etag).str());

  // See
  // https://json-schema.org/draft/2020-12/json-schema-core.html#section-9.5.1.1
  if (link.has_value()) {
    write_link_header(response, link.value());
  } else {
    const auto &dialect{file.value().extension};
    if (dialect.is_string()) {
      write_link_header(response, dialect.to_string());
    }
  }

  std::ostringstream contents;
  contents << file.value().data.rdbuf();

  if (file.value().encoding == sourcemeta::one::Encoding::GZIP) {
    send_response(code, request.method(), request.path(), response,
                  contents.str(), request.response_encoding(),
                  sourcemeta::one::HTTPRequest::Encoding::GZIP);
  } else {
    send_response(code, request.method(), request.path(), response,
                  contents.str(), request.response_encoding(),
                  sourcemeta::one::HTTPRequest::Encoding::Identity);
  }
}

#endif
