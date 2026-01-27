#ifndef SOURCEMETA_ONE_SHARED_VERSION_H_
#define SOURCEMETA_ONE_SHARED_VERSION_H_

#include <string_view> // std::string_view

namespace sourcemeta::one {

auto stamp() noexcept -> std::string_view;
auto version() noexcept -> std::string_view;
auto edition() noexcept -> std::string_view;

} // namespace sourcemeta::one

#endif
