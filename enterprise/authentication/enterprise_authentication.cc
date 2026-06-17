#include <sourcemeta/one/enterprise_authentication.h>

namespace sourcemeta::one {

auto authentication_admits_enterprise(const std::string_view) -> bool {
  // Policy types beyond public are introduced in a later step. Until then
  // there are none to evaluate, so no caller is admitted through this path
  return false;
}

} // namespace sourcemeta::one
