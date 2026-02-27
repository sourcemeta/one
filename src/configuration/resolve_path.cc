#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/uri.h>

namespace sourcemeta::one {

auto Configuration::resolve_path(const sourcemeta::core::URI &input) const
    -> std::optional<std::filesystem::path> {
  sourcemeta::core::URI relative{input.recompose()};
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

  for (const auto &entry : this->entries) {
    const auto *collection{std::get_if<Collection>(&entry.second)};
    if (!collection ||
        !sourcemeta::core::starts_with(relative_path, entry.first)) {
      continue;
    }

    return (collection->absolute_path /
            relative_path.lexically_relative(entry.first))
        .lexically_normal();
  }

  return std::nullopt;
}

} // namespace sourcemeta::one
