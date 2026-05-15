#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/metapack.h>

#include <cassert>     // assert
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

auto Action::blaze_template(std::string_view schema_uri,
                            const sourcemeta::blaze::Mode mode) const
    -> sourcemeta::blaze::Template {
  if (!this->base_path_.empty() && schema_uri.starts_with(this->base_path_)) {
    schema_uri.remove_prefix(this->base_path_.size());
  }
  if (schema_uri.starts_with('/')) {
    schema_uri.remove_prefix(1);
  }

  const auto *filename{mode == sourcemeta::blaze::Mode::FastValidation
                           ? "blaze-fast.metapack"
                           : "blaze-exhaustive.metapack"};
  const auto template_path{this->base_ / "schemas" / schema_uri / "%" /
                           filename};
  const auto template_json{sourcemeta::one::metapack_read_json(template_path)};
  assert(template_json.has_value());
  auto compiled{sourcemeta::blaze::from_json(template_json.value())};
  assert(compiled.has_value());
  return std::move(compiled.value());
}

} // namespace sourcemeta::one
