#ifndef SOURCEMETA_ONE_SHARED_VERSION_H_
#define SOURCEMETA_ONE_SHARED_VERSION_H_

#include <string_view> // std::string_view

namespace sourcemeta::one {

auto version() noexcept -> std::string_view;

// What distinguishes this build from another that produced artifacts earlier.
// Unlike the version, this is never shown, so it is free to tell two builds
// apart that the version alone cannot
auto build_identity() noexcept -> std::string_view;
auto edition() noexcept -> std::string_view;

} // namespace sourcemeta::one

#endif
