#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/uri.h>

namespace sourcemeta::one {

auto Configuration::resolve_path(const sourcemeta::core::URI &input) const
    -> std::optional<std::filesystem::path> {
  sourcemeta::core::URI relative{input};
  relative.canonicalize();
  relative.relative_to(sourcemeta::core::URI{this->url});
  if (relative.is_absolute()) {
    return std::nullopt;
  }

  const auto path_value{relative.path()};
  if (!path_value.has_value() || path_value->empty()) {
    return std::nullopt;
  }

  const std::filesystem::path relative_path{
      std::filesystem::path{path_value.value()}.relative_path()};
  if (relative_path.empty()) {
    return std::nullopt;
  }

  auto candidate{relative_path};
  while (!candidate.empty()) {
    const auto match{this->entries.find(candidate)};
    if (match != this->entries.cend()) {
      const auto *collection{std::get_if<Collection>(&match->second)};
      if (collection) {
        return (collection->absolute_path /
                relative_path.lexically_relative(candidate))
            .lexically_normal();
      }
    }

    candidate = candidate.parent_path();
  }

  return std::nullopt;
}

} // namespace sourcemeta::one
