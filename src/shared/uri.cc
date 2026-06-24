#include <sourcemeta/one/shared_uri.h>

#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>

#include <exception> // std::exception

namespace sourcemeta::one {

auto request_registry_path(const std::string_view uri,
                           const std::string_view server_uri)
    -> std::optional<std::filesystem::path> {
  std::optional<sourcemeta::core::URI> parsed;
  try {
    parsed.emplace(uri);
    parsed->canonicalize();
  } catch (const std::exception &) {
    return std::nullopt;
  }

  parsed->relative_to(sourcemeta::core::URI{server_uri});
  if (parsed->is_absolute()) {
    return std::nullopt;
  }

  const auto path_option{parsed->path()};
  if (path_option.has_value() && path_option.value().starts_with("/")) {
    return std::nullopt;
  }

  std::string_view normalized{path_option.has_value()
                                  ? std::string_view{path_option.value()}
                                  : std::string_view{}};
  while (normalized.starts_with("../")) {
    normalized.remove_prefix(3);
  }
  if (normalized == "..") {
    normalized = std::string_view{};
  }

  const auto stripped{
      sourcemeta::core::remove_suffix_ignore_case(normalized, ".json")};
  std::filesystem::path result{stripped};
  sourcemeta::core::to_lowercase(result);
  return result;
}

} // namespace sourcemeta::one
