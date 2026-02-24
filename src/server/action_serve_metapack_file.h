#ifndef SOURCEMETA_ONE_SERVER_ACTION_SERVE_METAPACK_FILE_H
#define SOURCEMETA_ONE_SERVER_ACTION_SERVE_METAPACK_FILE_H

#include <sourcemeta/core/time.h>

#include <sourcemeta/one/shared.h>

#include "helpers.h"
#include "request.h"
#include "response.h"

#include <chrono>     // std::chrono::seconds
#include <filesystem> // std::filesystem
#include <optional>   // std::optional
#include <sstream>    // std::ostringstream
#include <string>     // std::string

static auto action_serve_metapack_file(
    const sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response,
    const std::filesystem::path &absolute_path, const char *const code,
    const bool enable_cors = false,
    const std::optional<std::string> &mime = std::nullopt,
    const std::optional<std::string> &link = std::nullopt) -> void {
  if (request.method() != "get" && request.method() != "head") {
    json_error(request, response, sourcemeta::one::STATUS_METHOD_NOT_ALLOWED,
               "method-not-allowed",
               "This HTTP method is invalid for this URL");
    return;
  }

  auto file{sourcemeta::one::read_stream_raw(absolute_path)};
  if (!file.has_value()) {
    json_error(request, response, sourcemeta::one::STATUS_NOT_FOUND,
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
    response.write_status(sourcemeta::one::STATUS_NOT_MODIFIED);
    if (enable_cors) {
      response.write_header("Access-Control-Allow-Origin", "*");
    }

    send_response(sourcemeta::one::STATUS_NOT_MODIFIED, request, response);
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
      response.write_status(sourcemeta::one::STATUS_NOT_MODIFIED);
      if (enable_cors) {
        response.write_header("Access-Control-Allow-Origin", "*");
      }

      send_response(sourcemeta::one::STATUS_NOT_MODIFIED, request, response);
      return;
    }
  }

  response.write_status(code);

  // To support requests from web browsers
  if (enable_cors) {
    response.write_header("Access-Control-Allow-Origin", "*");
  }

  if (mime.has_value()) {
    response.write_header("Content-Type", mime.value());
  } else {
    response.write_header("Content-Type", file.value().mime);
  }

  response.write_header("Last-Modified",
                        sourcemeta::core::to_gmt(file.value().last_modified));

  std::ostringstream etag;
  etag << '"' << checksum << '"';
  response.write_header("ETag", std::move(etag).str());

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
    send_response(code, request, response, contents.str(),
                  sourcemeta::one::Encoding::GZIP);
  } else {
    send_response(code, request, response, contents.str(),
                  sourcemeta::one::Encoding::Identity);
  }
}

#endif
