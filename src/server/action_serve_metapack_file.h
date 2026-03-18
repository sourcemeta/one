#ifndef SOURCEMETA_ONE_SERVER_ACTION_SERVE_METAPACK_FILE_H
#define SOURCEMETA_ONE_SERVER_ACTION_SERVE_METAPACK_FILE_H

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/time.h>

#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include "helpers.h"
#include "request.h"
#include "response.h"

#include <chrono>     // std::chrono::seconds
#include <filesystem> // std::filesystem
#include <optional>   // std::optional
#include <sstream>    // std::ostringstream
#include <string>     // std::string

#pragma pack(push, 1)
struct MetapackDialectExtension {
  std::uint16_t dialect_length;
};
#pragma pack(pop)

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

  if (!std::filesystem::exists(absolute_path)) {
    json_error(request, response, sourcemeta::one::STATUS_NOT_FOUND,
               "not-found", "There is nothing at this URL");
    return;
  }

  sourcemeta::core::FileView view{absolute_path};
  if (view.size() <
      sizeof(sourcemeta::one::MetapackHeader) + sizeof(std::uint32_t)) {
    json_error(request, response, sourcemeta::one::STATUS_NOT_FOUND,
               "not-found", "There is nothing at this URL");
    return;
  }

  const auto info{sourcemeta::one::metapack_info(view)};

  // Note that `If-Modified-Since` can only be used with a `GET` or `HEAD`.
  // See
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/If-Modified-Since
  const auto if_modified_since{request.header_gmt("if-modified-since")};
  // Time comparison can be flaky, but adding a bit of tolerance leads
  // to more consistent behavior.
  if (if_modified_since.has_value() &&
      (if_modified_since.value() + std::chrono::seconds(1)) >=
          info.last_modified) {
    response.write_status(sourcemeta::one::STATUS_NOT_MODIFIED);
    if (enable_cors) {
      response.write_header("Access-Control-Allow-Origin", "*");
    }

    send_response(sourcemeta::one::STATUS_NOT_MODIFIED, request, response);
    return;
  }

  const auto &checksum{info.checksum_hex};
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
    response.write_header("Content-Type", info.mime);
  }

  response.write_header("Last-Modified",
                        sourcemeta::core::to_gmt(info.last_modified));

  std::ostringstream etag;
  etag << '"' << checksum << '"';
  response.write_header("ETag", std::move(etag).str());

  // See
  // https://json-schema.org/draft/2020-12/json-schema-core.html#section-9.5.1.1
  if (link.has_value()) {
    write_link_header(response, link.value());
  } else {
    const auto *dialect_ext{
        sourcemeta::one::metapack_extension<MetapackDialectExtension>(view)};
    const std::string_view dialect =
        (dialect_ext != nullptr && dialect_ext->dialect_length > 0)
            ? std::string_view{reinterpret_cast<
                                   const char *>(view.as<std::uint8_t>(
                                   sourcemeta::one::metapack_extension_offset(
                                       view) +
                                   sizeof(MetapackDialectExtension))),
                               dialect_ext->dialect_length}
            : std::string_view{};
    if (!dialect.empty()) {
      write_link_header(response, std::string{dialect});
    }
  }

  const auto payload_start{sourcemeta::one::metapack_payload_offset(view)};
  const auto payload_size{view.size() - payload_start};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string contents{
      reinterpret_cast<const char *>(view.as<std::uint8_t>(payload_start)),
      payload_size};

  if (info.encoding == sourcemeta::one::MetapackEncoding::GZIP) {
    send_response(code, request, response, contents,
                  sourcemeta::one::Encoding::GZIP);
  } else {
    send_response(code, request, response, contents,
                  sourcemeta::one::Encoding::Identity);
  }
}

#endif
