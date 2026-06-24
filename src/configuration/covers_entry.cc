#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/io.h>

#include <filesystem> // std::filesystem::path

namespace sourcemeta::one {

auto Configuration::covers_entry(const std::string_view registry_path) const
    -> bool {
  // Entries are keyed without a leading slash, so compare on equal terms
  const std::string_view relative{
      registry_path.starts_with('/') ? registry_path.substr(1) : registry_path};
  // The root is above every entry
  if (relative.empty()) {
    return true;
  }

  // The route table and the matcher have no trailing slash in their path
  // grammar, so neither does an entry path. Reject one here too, so this check
  // accepts exactly what the route table does for any given policy path
  if (relative.back() == '/') {
    return false;
  }

  const std::filesystem::path scope{relative};
  for (const auto &entry : this->entries) {
    if (sourcemeta::core::is_lexically_under_path(entry.first, scope)) {
      return true;
    }
  }

  return false;
}

} // namespace sourcemeta::one
