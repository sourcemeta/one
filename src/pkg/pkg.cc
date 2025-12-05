#include <sourcemeta/one/pkg.h>

#include <sourcemeta/one/shared.h>

#include <cassert> // assert
#include <fstream> // std::ofstream

namespace sourcemeta::one {

auto GENERATE_PKG_EXTRACT_JSON::handler(
    const std::filesystem::path &destination,
    const sourcemeta::core::BuildDependencies<std::filesystem::path>
        &dependencies,
    const sourcemeta::core::BuildDynamicCallback<std::filesystem::path> &,
    const Context &) -> void {
  auto document{sourcemeta::one::read_json(dependencies.front())};
  assert(destination.is_absolute());
  std::filesystem::create_directories(destination.parent_path());
  std::ofstream stream{destination};
  assert(!stream.fail());
  sourcemeta::core::prettify(document, stream);
}

auto GENERATE_PKG_COPY_FILE::handler(
    const std::filesystem::path &destination,
    const sourcemeta::core::BuildDependencies<std::filesystem::path>
        &dependencies,
    const sourcemeta::core::BuildDynamicCallback<std::filesystem::path> &,
    const Context &) -> void {
  assert(destination.is_absolute());
  std::filesystem::create_directories(destination.parent_path());
  std::filesystem::copy_file(dependencies.front(), destination,
                             std::filesystem::copy_options::overwrite_existing);
}

} // namespace sourcemeta::one
