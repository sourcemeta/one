#include <sourcemeta/one/actions.h>

namespace sourcemeta::one {

auto Action::schema_directory(const std::string_view uri) const
    -> std::optional<std::filesystem::path> {
  const auto scheme_end{uri.find("://")};
  if (scheme_end == std::string_view::npos) {
    return std::nullopt;
  }

  const auto path_start{uri.find('/', scheme_end + 3)};
  if (path_start == std::string_view::npos) {
    return std::nullopt;
  }

  const auto fragment_position{uri.find('#', path_start)};
  auto uri_path{uri.substr(path_start, fragment_position - path_start)};

  if (uri_path.empty() || uri_path == "/") {
    return std::nullopt;
  }

  if (!this->base_path_.empty() && uri_path.starts_with(this->base_path_)) {
    uri_path = uri_path.substr(this->base_path_.size());
  }

  if (uri_path.starts_with('/')) {
    uri_path = uri_path.substr(1);
  }

  if (uri_path.empty()) {
    return std::nullopt;
  }

  return this->base_ / "schemas" / uri_path / "%";
}

} // namespace sourcemeta::one
