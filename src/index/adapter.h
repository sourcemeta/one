#ifndef SOURCEMETA_ONE_INDEX_ADAPTER_H_
#define SOURCEMETA_ONE_INDEX_ADAPTER_H_

#include <sourcemeta/core/build.h>

#include <filesystem> // std::filesystem
#include <optional>   // std::optional

namespace sourcemeta::one {

class Adapter : public sourcemeta::core::BuildAdapterFilesystem {
public:
  Adapter(std::filesystem::path output_root)
      : root{std::filesystem::canonical(output_root)} {}

  [[nodiscard]] auto read_dependencies(const node_type &path) const
      -> std::optional<sourcemeta::core::BuildDependencies<node_type>> {
    auto result{BuildAdapterFilesystem::read_dependencies(path)};
    if (result.has_value()) {
      for (auto &dependency : result.value()) {
        if (!dependency.is_absolute()) {
          dependency =
              std::filesystem::weakly_canonical(this->root / dependency);
        }
      }
    }

    return result;
  }

  auto write_dependencies(
      const node_type &path,
      const sourcemeta::core::BuildDependencies<node_type> &dependencies)
      -> void {
    sourcemeta::core::BuildDependencies<node_type> relativized;
    relativized.reserve(dependencies.size());
    for (const auto &dependency : dependencies) {
      const auto relative{dependency.lexically_relative(this->root)};
      if (!relative.empty() && *relative.begin() != "..") {
        relativized.emplace_back(relative);
      } else {
        relativized.emplace_back(dependency);
      }
    }

    BuildAdapterFilesystem::write_dependencies(path, relativized);
  }

private:
  std::filesystem::path root;
};

} // namespace sourcemeta::one

#endif
