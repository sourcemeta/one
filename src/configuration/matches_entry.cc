#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/shared_path.h>

namespace sourcemeta::one {

auto Configuration::matches_entry(const std::string_view registry_path) const
    -> bool {
  for (const auto &entry : this->entries) {
    const auto target{entry.first.generic_string()};
    if (path_covers(registry_path, target) ||
        path_covers(target, registry_path)) {
      return true;
    }
  }

  // A path with no segments spans the entire registry, so it always has a
  // target
  return path_covers(registry_path, "");
}

} // namespace sourcemeta::one
