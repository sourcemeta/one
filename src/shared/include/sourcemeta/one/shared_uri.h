#ifndef SOURCEMETA_ONE_SHARED_URI_H_
#define SOURCEMETA_ONE_SHARED_URI_H_

#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <string_view> // std::string_view

namespace sourcemeta::one {

auto request_registry_path(std::string_view uri, std::string_view server_uri)
    -> std::optional<std::filesystem::path>;

} // namespace sourcemeta::one

#endif
