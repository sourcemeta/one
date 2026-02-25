#ifndef SOURCEMETA_ONE_BUILD_ADAPTER_FILESYSTEM_H_
#define SOURCEMETA_ONE_BUILD_ADAPTER_FILESYSTEM_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/one/build_types.h>
// NOLINTEND(misc-include-cleaner)

#include <filesystem>    // std::filesystem
#include <optional>      // std::optional
#include <shared_mutex>  // std::shared_mutex
#include <unordered_map> // std::unordered_map

namespace sourcemeta::one {

class SOURCEMETA_ONE_BUILD_EXPORT BuildAdapterFilesystem {
public:
  using node_type = std::filesystem::path;
  using mark_type = std::filesystem::file_time_type;

  BuildAdapterFilesystem(const std::filesystem::path &output_root);

  [[nodiscard]] auto dependencies_path(const node_type &path) const
      -> node_type;
  [[nodiscard]] auto read_dependencies(const node_type &path) const
      -> std::optional<BuildDependencies<node_type>>;
  auto write_dependencies(const node_type &path,
                          const BuildDependencies<node_type> &dependencies)
      -> void;
  auto refresh(const node_type &path) -> void;
  [[nodiscard]] auto mark(const node_type &path) -> std::optional<mark_type>;
  [[nodiscard]] auto is_newer_than(const mark_type left,
                                   const mark_type right) const -> bool;

private:
  std::filesystem::path root;
  std::unordered_map<node_type, mark_type> marks;
  std::shared_mutex mutex;
};

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_ADAPTER_FILESYSTEM_H_
