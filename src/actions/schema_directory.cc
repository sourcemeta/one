#include <sourcemeta/one/actions.h>

namespace sourcemeta::one {

auto schema_directory(const std::filesystem::path &output,
                      const std::string_view base_path,
                      const std::string_view uri)
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

  if (!base_path.empty() && uri_path.starts_with(base_path)) {
    uri_path = uri_path.substr(base_path.size());
  }

  if (uri_path.starts_with('/')) {
    uri_path = uri_path.substr(1);
  }

  if (uri_path.empty()) {
    return std::nullopt;
  }

  return output / "schemas" / uri_path / "%";
}

} // namespace sourcemeta::one
