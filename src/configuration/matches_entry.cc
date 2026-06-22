#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/io.h>

#include <filesystem> // std::filesystem::path
#include <variant>    // std::holds_alternative

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
    // The scope is at or above the entry, so it governs a namespace that
    // contains it
    if (sourcemeta::core::is_lexically_under_path(entry.first, scope)) {
      return true;
    }

    // The scope reaches inside the entry. Only a collection is an open
    // namespace whose schemas may not exist yet, so scoping below it is
    // meaningful. A page is a fixed resource, and reaching below it would let a
    // typo masquerade as a path within it
    if (std::holds_alternative<Collection>(entry.second) &&
        sourcemeta::core::is_lexically_under_path(scope, entry.first)) {
      return true;
    }
  }

  return false;
}

} // namespace sourcemeta::one
