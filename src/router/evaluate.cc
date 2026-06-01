#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/blaze/output.h>

#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/router.h>

#include <cassert>     // assert
#include <string_view> // std::string_view
#include <utility>     // std::move, std::pair

namespace sourcemeta::one {

auto RouterAction::blaze_compile(const std::string_view schema_uri,
                                 const sourcemeta::blaze::Mode mode) const
    -> sourcemeta::blaze::Template {
  const auto template_path{this->artifact_resolve_path(
      schema_uri, InputKind::URI, Tree::Schemas,
      mode == sourcemeta::blaze::Mode::FastValidation ? "blaze-fast"
                                                      : "blaze-exhaustive")};
  assert(template_path.has_value());
  const auto template_json{
      sourcemeta::one::metapack_read_json(template_path.value())};
  assert(template_json.has_value());
  auto compiled{sourcemeta::blaze::from_json(template_json.value())};
  assert(compiled.has_value());
  return std::move(compiled).value();
}

auto RouterAction::schema_evaluate_fast(
    const std::string_view schema_uri,
    const sourcemeta::core::JSON &instance) const -> bool {
  const auto schema_template{
      this->blaze_compile(schema_uri, sourcemeta::blaze::Mode::FastValidation)};
  sourcemeta::blaze::Evaluator evaluator;
  return evaluator.validate(schema_template, instance);
}

auto RouterAction::schema_evaluate(const std::string_view schema_uri,
                                   const sourcemeta::core::JSON &instance,
                                   const sourcemeta::blaze::Mode mode) const
    -> std::pair<bool, sourcemeta::core::JSON> {
  const auto schema_template{this->blaze_compile(schema_uri, mode)};
  sourcemeta::blaze::Evaluator evaluator;
  auto result{
      sourcemeta::blaze::standard(evaluator, schema_template, instance,
                                  sourcemeta::blaze::StandardOutput::Basic)};
  const auto *valid{result.try_at("valid")};
  const bool is_valid{valid != nullptr && valid->is_boolean() &&
                      valid->to_boolean()};
  return {is_valid, std::move(result)};
}

auto RouterAction::schema_evaluate_with_tracing(
    const std::string_view schema_uri, const sourcemeta::core::JSON &instance,
    const sourcemeta::blaze::Callback &callback) const -> bool {
  const auto schema_template{
      this->blaze_compile(schema_uri, sourcemeta::blaze::Mode::Exhaustive)};
  sourcemeta::blaze::Evaluator evaluator;
  return evaluator.validate(schema_template, instance, callback);
}

} // namespace sourcemeta::one
