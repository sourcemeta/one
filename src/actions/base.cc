#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/actions.h>

#include <exception>   // std::exception
#include <optional>    // std::optional
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::one {

auto Action::uri_to_relative_path(const std::string_view uri) const
    -> std::optional<std::filesystem::path> {
  std::optional<sourcemeta::core::URI> parsed;
  try {
    parsed.emplace(uri);
    parsed->canonicalize();
  } catch (const std::exception &) {
    return std::nullopt;
  }

  parsed->relative_to(sourcemeta::core::URI{this->server_uri_});
  if (parsed->is_absolute()) {
    return std::nullopt;
  }

  const auto path_option{parsed->path()};
  if (!path_option.has_value() || path_option.value().empty() ||
      path_option.value().starts_with("..") ||
      path_option.value().starts_with("/")) {
    return std::nullopt;
  }

  return std::filesystem::path{path_option.value()};
}

auto Action::schema_directory(const std::string_view uri) const
    -> std::optional<std::filesystem::path> {
  auto path{this->uri_to_relative_path(uri)};
  if (!path.has_value()) {
    return std::nullopt;
  }
  return this->base_ / "schemas" / std::move(path).value() / "%";
}

} // namespace sourcemeta::one
