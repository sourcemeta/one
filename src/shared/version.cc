#include <sourcemeta/one/shared_version.h>

#include <sourcemeta/core/crypto.h>

#include "configure.h"

namespace sourcemeta::one {

auto stamp() noexcept -> std::string_view {
  static auto CACHE_STAMP{sourcemeta::core::uuidv4()};
  return CACHE_STAMP;
}

auto version() noexcept -> std::string_view { return PROJECT_VERSION; }

auto edition() noexcept -> std::string_view {
#if defined(SOURCEMETA_ONE_ENTERPRISE)
  return "Enterprise";
#else
  return "Community";
#endif
}

} // namespace sourcemeta::one
