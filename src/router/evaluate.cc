#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include <cassert>     // assert
#include <memory>      // std::make_shared, std::shared_ptr
#include <string_view> // std::string_view
#include <utility>     // std::move, std::pair

namespace sourcemeta::one {

auto Router::blaze_template(const ResolvedArtifact &artifact)
    -> std::shared_ptr<const sourcemeta::blaze::Template> {
  return this->template_cache_.get_or_compute(artifact.path(), [&artifact] {
    const auto template_json{
        sourcemeta::one::metapack_read_json(artifact.path())};
    assert(template_json.has_value());
    auto compiled{sourcemeta::blaze::from_json(template_json.value())};
    assert(compiled.has_value());
    return std::move(compiled).value();
  });
}

auto RouterAction::blaze_template(const Authentication::Context &access,
                                  const std::string_view schema_uri,
                                  const sourcemeta::blaze::Mode mode) const
    -> std::shared_ptr<const sourcemeta::blaze::Template> {
  const auto resolution{this->artifact_resolve_path(
      access, schema_uri, Tree::Schemas,
      mode == sourcemeta::blaze::Mode::FastValidation ? "blaze-fast"
                                                      : "blaze-exhaustive")};
  assert(resolution.outcome == ArtifactResolution::Outcome::Found);
  return this->dispatcher_.blaze_template(resolution.path.value());
}

auto RouterAction::schema_evaluate_fast(
    const Authentication::Context &access, const std::string_view schema_uri,
    const sourcemeta::core::JSON &instance) const -> bool {
  const auto schema_template{this->blaze_template(
      access, schema_uri, sourcemeta::blaze::Mode::FastValidation)};
  sourcemeta::blaze::Evaluator evaluator;
  return evaluator.validate(*schema_template, instance);
}

auto RouterAction::schema_evaluate(const Authentication::Context &access,
                                   const std::string_view schema_uri,
                                   const sourcemeta::core::JSON &instance,
                                   const sourcemeta::blaze::Mode mode) const
    -> std::pair<bool, sourcemeta::core::JSON> {
  const auto schema_template{this->blaze_template(access, schema_uri, mode)};
  sourcemeta::blaze::Evaluator evaluator;
  auto result{
      sourcemeta::blaze::standard(evaluator, *schema_template, instance,
                                  sourcemeta::blaze::StandardOutput::Basic)};
  const auto *valid{result.try_at("valid")};
  const bool is_valid{valid != nullptr && valid->is_boolean() &&
                      valid->to_boolean()};
  return {is_valid, std::move(result)};
}

auto RouterAction::schema_evaluate_with_tracing(
    const Authentication::Context &access, const std::string_view schema_uri,
    const sourcemeta::core::JSON &instance,
    const sourcemeta::blaze::Callback &callback) const -> bool {
  const auto schema_template{this->blaze_template(
      access, schema_uri, sourcemeta::blaze::Mode::Exhaustive)};
  sourcemeta::blaze::Evaluator evaluator;
  return evaluator.validate(*schema_template, instance, callback);
}

} // namespace sourcemeta::one
