#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/io.h>

#include <filesystem> // std::filesystem::path

namespace sourcemeta::one {

auto Configuration::matches_entry(const std::string_view registry_path) const
    -> bool {
  // Entries are keyed by relative paths, so drop the leading slash to compare
  // the policy path against them on equal terms
  const std::filesystem::path scope{
      registry_path.starts_with('/') ? registry_path.substr(1) : registry_path};
  // A scope with no segments spans the entire registry
  if (scope.empty()) {
    return true;
  }

  for (const auto &entry : this->entries) {
    if (sourcemeta::core::is_lexically_under_path(entry.first, scope) ||
        sourcemeta::core::is_lexically_under_path(scope, entry.first)) {
      return true;
    }
  }

  return false;
}

} // namespace sourcemeta::one
