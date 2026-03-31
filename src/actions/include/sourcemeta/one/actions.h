#ifndef SOURCEMETA_ONE_ACTIONS_H
#define SOURCEMETA_ONE_ACTIONS_H

#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/http.h>

#include <cstdint>     // std::uint16_t
#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::one {

constexpr auto HANDLER_SELF_V1_API_LIST = 1;
constexpr auto HANDLER_SELF_V1_API_LIST_PATH = 2;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_DEPENDENCIES = 3;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_DEPENDENTS = 4;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_HEALTH = 5;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_LOCATIONS = 6;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_POSITIONS = 7;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_STATS = 8;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_METADATA = 9;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_EVALUATE = 10;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_TRACE = 11;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_SEARCH = 12;
constexpr auto HANDLER_SELF_V1_API_DEFAULT = 13;
constexpr auto HANDLER_SELF_STATIC = 14;
constexpr auto HANDLER_SELF_V1_HEALTH = 15;

auto dispatch_action(const std::uint16_t identifier,
                     const core::URITemplateRouterView &router,
                     const std::filesystem::path &base,
                     const std::span<std::string_view> matches,
                     HTTPRequest &request, HTTPResponse &response) -> void;

} // namespace sourcemeta::one

#endif
