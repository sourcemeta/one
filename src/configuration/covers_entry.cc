#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/io.h>

#include <filesystem> // std::filesystem::path

namespace sourcemeta::one {

auto Configuration::covers_entry(const std::string_view registry_path) const
    -> bool {
  // Entries are keyed without a leading slash, so compare on equal terms
  const std::filesystem::path scope{
      registry_path.starts_with('/') ? registry_path.substr(1) : registry_path};
  // The root is above every entry
  if (scope.empty()) {
    return true;
  }

  for (const auto &entry : this->entries) {
    if (sourcemeta::core::is_lexically_under_path(entry.first, scope)) {
      return true;
    }
  }

  return false;
}

} // namespace sourcemeta::one
