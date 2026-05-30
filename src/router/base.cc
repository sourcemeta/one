#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include <algorithm>   // std::ranges::transform
#include <cassert>     // assert
#include <cctype>      // std::tolower
#include <exception>   // std::exception
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::one {

auto RouterAction::uri_to_relative_path(const std::string_view uri) const
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

  std::filesystem::path result{path_option.value()};
  sourcemeta::core::to_lowercase(result);
  return result;
}

auto RouterAction::schema_directory(const std::string_view uri) const
    -> std::optional<std::filesystem::path> {
  auto path{this->uri_to_relative_path(uri)};
  if (!path.has_value()) {
    return std::nullopt;
  }
  return this->base_ / "schemas" / std::move(path).value() / "%";
}

auto RouterAction::resolve_schema_path(
    const std::string_view schema_relative_path, const bool bundle) const
    -> std::filesystem::path {
  std::string lowercase_path{schema_relative_path};
  std::ranges::transform(
      lowercase_path, lowercase_path.begin(),
      [](const unsigned char character) { return std::tolower(character); });
  auto absolute_path{this->base_ / "schemas" / lowercase_path / "%"};
  if (bundle) {
    absolute_path /= "bundle.metapack";
  } else {
    absolute_path /= "schema.metapack";
  }
  return absolute_path;
}

auto RouterAction::validate(const std::string_view schema_uri,
                            const sourcemeta::core::JSON &instance) const
    -> bool {
  // TODO: Cache the compiled template across invocations, keyed by schema URI
  const auto schema_template{this->blaze_template(
      schema_uri, sourcemeta::blaze::Mode::FastValidation)};
  sourcemeta::blaze::Evaluator evaluator;
  return evaluator.validate(schema_template, instance);
}

auto RouterAction::validate_standard(const std::string_view schema_uri,
                                     const sourcemeta::core::JSON &instance)
    const -> std::optional<sourcemeta::core::JSON> {
  const auto schema_template{
      this->blaze_template(schema_uri, sourcemeta::blaze::Mode::Exhaustive)};
  sourcemeta::blaze::Evaluator evaluator;
  auto result{
      sourcemeta::blaze::standard(evaluator, schema_template, instance,
                                  sourcemeta::blaze::StandardOutput::Basic)};
  const auto *valid{result.try_at("valid")};
  if (valid != nullptr && valid->is_boolean() && valid->to_boolean()) {
    return std::nullopt;
  }
  return result;
}

auto RouterAction::blaze_template(const std::string_view schema_uri,
                                  const sourcemeta::blaze::Mode mode) const
    -> sourcemeta::blaze::Template {
  const auto stripped{
      sourcemeta::core::URI::strip_path_prefix(schema_uri, this->base_path_)};
  assert(stripped.has_value());

  const auto *filename{mode == sourcemeta::blaze::Mode::FastValidation
                           ? "blaze-fast.metapack"
                           : "blaze-exhaustive.metapack"};
  const auto template_path{this->base_ / "schemas" / stripped.value() / "%" /
                           filename};
  const auto template_json{sourcemeta::one::metapack_read_json(template_path)};
  assert(template_json.has_value());
  auto compiled{sourcemeta::blaze::from_json(template_json.value())};
  assert(compiled.has_value());
  return std::move(compiled.value());
}

} // namespace sourcemeta::one
