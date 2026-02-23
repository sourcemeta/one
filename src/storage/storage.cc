#include <sourcemeta/one/storage.h>

#include <cassert>     // assert
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

auto resolve(const std::filesystem::path &base,
             const sourcemeta::one::Storage::Key key) -> std::filesystem::path {
  return base / std::string{key};
}

auto resolve_sibling(const std::filesystem::path &base,
                     const sourcemeta::one::Storage::Key key,
                     const std::string_view filename) -> std::filesystem::path {
  const auto last_slash{key.rfind('/')};
  assert(last_slash != std::string_view::npos);
  const auto parent{key.substr(0, last_slash)};
  return base / std::string{parent} / std::string{filename};
}

} // namespace

namespace sourcemeta::one {

Storage::Storage(std::filesystem::path base) : base_{std::move(base)} {
  assert(this->base_.is_absolute());
}

auto Storage::exists(const Key key) const -> bool {
  return std::filesystem::exists(resolve(this->base_, key));
}

auto Storage::sibling_exists(const Key key,
                             const std::string_view filename) const -> bool {
  return std::filesystem::exists(resolve_sibling(this->base_, key, filename));
}

auto Storage::read_json(const Key key) const -> sourcemeta::core::JSON {
  return sourcemeta::one::read_json(resolve(this->base_, key));
}

auto Storage::read_sibling_json(const Key key,
                                const std::string_view filename) const
    -> sourcemeta::core::JSON {
  return sourcemeta::one::read_json(
      resolve_sibling(this->base_, key, filename));
}

auto Storage::read_raw(const Key key) const
    -> std::optional<File<std::ifstream>> {
  return sourcemeta::one::read_stream_raw(resolve(this->base_, key));
}

} // namespace sourcemeta::one
