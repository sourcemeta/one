#ifndef SOURCEMETA_ONE_JSONRPC_H
#define SOURCEMETA_ONE_JSONRPC_H

#include <sourcemeta/core/json.h>

#include <cstdint>     // std::int64_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

// TODO: Eventually elevate to Core

namespace sourcemeta::one {

// See https://www.jsonrpc.org/specification#error_object
constexpr std::int64_t JSONRPC_CODE_PARSE = -32700;
constexpr std::int64_t JSONRPC_CODE_INVALID_REQUEST = -32600;
constexpr std::int64_t JSONRPC_CODE_METHOD_NOT_FOUND = -32601;
constexpr std::int64_t JSONRPC_CODE_INVALID_PARAMS = -32602;
constexpr std::int64_t JSONRPC_CODE_INTERNAL = -32603;
constexpr std::int64_t JSONRPC_CODE_SERVER_ERROR_MIN = -32099;
constexpr std::int64_t JSONRPC_CODE_SERVER_ERROR_MAX = -32000;

inline auto jsonrpc_is_server_error(const std::int64_t code) -> bool {
  return code >= JSONRPC_CODE_SERVER_ERROR_MIN &&
         code <= JSONRPC_CODE_SERVER_ERROR_MAX;
}

inline const auto JSONRPC_HASH_ID{
    sourcemeta::core::JSON::make_object().as_object().hash("id")};
inline const auto JSONRPC_HASH_JSONRPC{
    sourcemeta::core::JSON::make_object().as_object().hash("jsonrpc")};
inline const auto JSONRPC_HASH_METHOD{
    sourcemeta::core::JSON::make_object().as_object().hash("method")};
inline const auto JSONRPC_HASH_RESULT{
    sourcemeta::core::JSON::make_object().as_object().hash("result")};
inline const auto JSONRPC_HASH_ERROR{
    sourcemeta::core::JSON::make_object().as_object().hash("error")};
inline const auto JSONRPC_HASH_CODE{
    sourcemeta::core::JSON::make_object().as_object().hash("code")};
inline const auto JSONRPC_HASH_MESSAGE{
    sourcemeta::core::JSON::make_object().as_object().hash("message")};
inline const auto JSONRPC_HASH_DATA{
    sourcemeta::core::JSON::make_object().as_object().hash("data")};
inline const auto JSONRPC_HASH_PARAMS{
    sourcemeta::core::JSON::make_object().as_object().hash("params")};

inline auto jsonrpc_request_id(const sourcemeta::core::JSON &request)
    -> const sourcemeta::core::JSON * {
  if (!request.is_object()) {
    return nullptr;
  }
  const auto *identifier{request.try_at("id", JSONRPC_HASH_ID)};
  if (identifier == nullptr ||
      (!identifier->is_string() && !identifier->is_integer())) {
    return nullptr;
  }
  return identifier;
}

inline auto jsonrpc_is_request(const sourcemeta::core::JSON &request) -> bool {
  if (!request.is_object()) {
    return false;
  }
  const auto *jsonrpc_field{request.try_at("jsonrpc", JSONRPC_HASH_JSONRPC)};
  if (jsonrpc_field == nullptr || !jsonrpc_field->is_string() ||
      jsonrpc_field->to_string() != "2.0" ||
      jsonrpc_request_id(request) == nullptr) {
    return false;
  }
  const auto *method_field{request.try_at("method", JSONRPC_HASH_METHOD)};
  return method_field != nullptr && method_field->is_string();
}

inline auto jsonrpc_method(const sourcemeta::core::JSON &request)
    -> std::string_view {
  if (!request.is_object()) {
    return {};
  }
  const auto *method_field{request.try_at("method", JSONRPC_HASH_METHOD)};
  if (method_field == nullptr || !method_field->is_string()) {
    return {};
  }
  return method_field->to_string();
}

inline auto jsonrpc_params(const sourcemeta::core::JSON &request)
    -> const sourcemeta::core::JSON * {
  if (!request.is_object()) {
    return nullptr;
  }
  const auto *params{request.try_at("params", JSONRPC_HASH_PARAMS)};
  if (params == nullptr || (!params->is_object() && !params->is_array())) {
    return nullptr;
  }
  return params;
}

inline auto jsonrpc_is_notification(const sourcemeta::core::JSON &request)
    -> bool {
  if (!request.is_object()) {
    return false;
  }
  const auto *jsonrpc_field{request.try_at("jsonrpc", JSONRPC_HASH_JSONRPC)};
  if (jsonrpc_field == nullptr || !jsonrpc_field->is_string() ||
      jsonrpc_field->to_string() != "2.0" ||
      request.try_at("id", JSONRPC_HASH_ID) != nullptr) {
    return false;
  }
  const auto *method_field{request.try_at("method", JSONRPC_HASH_METHOD)};
  return method_field != nullptr && method_field->is_string();
}

inline auto jsonrpc_make_success(const sourcemeta::core::JSON &id,
                                 sourcemeta::core::JSON result)
    -> sourcemeta::core::JSON {
  auto envelope{sourcemeta::core::JSON::make_object()};
  envelope.assign_assume_new(std::string{"jsonrpc"},
                             sourcemeta::core::JSON{"2.0"},
                             JSONRPC_HASH_JSONRPC);
  envelope.assign_assume_new(std::string{"id"}, sourcemeta::core::JSON{id},
                             JSONRPC_HASH_ID);
  envelope.assign_assume_new(std::string{"result"}, std::move(result),
                             JSONRPC_HASH_RESULT);
  return envelope;
}

inline auto jsonrpc_make_success_empty(const sourcemeta::core::JSON &id)
    -> sourcemeta::core::JSON {
  return jsonrpc_make_success(id, sourcemeta::core::JSON::make_object());
}

inline auto
jsonrpc_make_error(const sourcemeta::core::JSON *id, const std::int64_t code,
                   const std::string_view message,
                   std::optional<sourcemeta::core::JSON> data = std::nullopt)
    -> sourcemeta::core::JSON {
  auto envelope{sourcemeta::core::JSON::make_object()};
  envelope.assign_assume_new(std::string{"jsonrpc"},
                             sourcemeta::core::JSON{"2.0"},
                             JSONRPC_HASH_JSONRPC);
  if (id != nullptr) {
    envelope.assign_assume_new(std::string{"id"}, sourcemeta::core::JSON{*id},
                               JSONRPC_HASH_ID);
  }
  auto error{sourcemeta::core::JSON::make_object()};
  error.assign_assume_new(std::string{"code"}, sourcemeta::core::JSON{code},
                          JSONRPC_HASH_CODE);
  error.assign_assume_new(std::string{"message"},
                          sourcemeta::core::JSON{std::string{message}},
                          JSONRPC_HASH_MESSAGE);
  if (data.has_value()) {
    error.assign_assume_new(std::string{"data"}, std::move(data.value()),
                            JSONRPC_HASH_DATA);
  }
  envelope.assign_assume_new(std::string{"error"}, std::move(error),
                             JSONRPC_HASH_ERROR);
  return envelope;
}

inline auto jsonrpc_make_error_parse() -> const sourcemeta::core::JSON & {
  static const auto envelope{
      jsonrpc_make_error(nullptr, JSONRPC_CODE_PARSE, "Parse error")};
  return envelope;
}

inline auto
jsonrpc_make_error_invalid_request(const sourcemeta::core::JSON *id = nullptr)
    -> sourcemeta::core::JSON {
  return jsonrpc_make_error(id, JSONRPC_CODE_INVALID_REQUEST,
                            "Invalid Request");
}

inline auto
jsonrpc_make_error_method_not_found(const sourcemeta::core::JSON &id)
    -> sourcemeta::core::JSON {
  return jsonrpc_make_error(&id, JSONRPC_CODE_METHOD_NOT_FOUND,
                            "Method not found");
}

inline auto jsonrpc_make_error_invalid_params(const sourcemeta::core::JSON &id)
    -> sourcemeta::core::JSON {
  return jsonrpc_make_error(&id, JSONRPC_CODE_INVALID_PARAMS, "Invalid params");
}

inline auto
jsonrpc_make_error_internal(const sourcemeta::core::JSON *id = nullptr)
    -> sourcemeta::core::JSON {
  return jsonrpc_make_error(id, JSONRPC_CODE_INTERNAL, "Internal error");
}

} // namespace sourcemeta::one

#endif
