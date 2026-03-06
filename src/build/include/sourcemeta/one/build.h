#ifndef SOURCEMETA_ONE_BUILD_H_
#define SOURCEMETA_ONE_BUILD_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

#include <sourcemeta/core/json.h>

#include <algorithm>     // std::ranges::none_of
#include <cassert>       // assert
#include <cstdint>       // std::uint8_t
#include <filesystem>    // std::filesystem
#include <functional>    // std::function
#include <mutex>         // std::unique_lock
#include <optional>      // std::optional
#include <shared_mutex>  // std::shared_mutex, std::shared_lock
#include <string>        // std::string
#include <unordered_map> // std::unordered_map
#include <utility>       // std::pair, std::move
#include <vector>        // std::vector

namespace sourcemeta::one {

class SOURCEMETA_ONE_BUILD_EXPORT Build {
public:
  enum class DependencyKind : std::uint8_t { Static, Dynamic };

  using Dependencies =
      std::vector<std::pair<DependencyKind, std::filesystem::path>>;

  using DynamicCallback = std::function<void(const std::filesystem::path &)>;

  template <typename Context>
  using Handler =
      std::function<void(const std::filesystem::path &, const Dependencies &,
                         const DynamicCallback &, const Context &)>;

  using mark_type = std::filesystem::file_time_type;

  Build(const std::filesystem::path &output_root);

  auto path() const -> const std::filesystem::path & { return this->root; }

  static auto dependency(std::filesystem::path node)
      -> std::pair<DependencyKind, std::filesystem::path>;

  [[nodiscard]] auto has_dependencies(const std::filesystem::path &path) const
      -> bool;
  auto finish() -> void;
  auto refresh(const std::filesystem::path &path) -> void;
  [[nodiscard]] auto mark(const std::filesystem::path &path)
      -> std::optional<mark_type>;

  template <typename Context>
  auto dispatch(const Handler<Context> &handler,
                const std::filesystem::path &destination,
                const Dependencies &dependencies, const Context &context)
      -> bool {
    const auto destination_mark{this->mark(destination)};
    const auto destination_key{
        destination.lexically_relative(this->root).string()};
    std::shared_lock dependencies_lock{this->dependencies_mutex};
    const auto cached_match{this->dependencies_map.find(destination_key)};
    const auto has_cached{cached_match != this->dependencies_map.end() &&
                          !cached_match->second.empty()};
    if (destination_mark.has_value() && has_cached) {
      const auto &cached{cached_match->second};
      std::size_t static_index{0};
      bool static_dependencies_match{true};
      for (const auto &entry : cached) {
        if (entry.first == DependencyKind::Static) {
          if (static_index >= dependencies.size() ||
              dependencies[static_index].second != entry.second) {
            static_dependencies_match = false;
            break;
          }

          static_index += 1;
        }
      }

      if (static_dependencies_match && static_index != dependencies.size()) {
        static_dependencies_match = false;
      }

      if (static_dependencies_match &&
          std::ranges::none_of(
              cached, [this, &destination_mark](const auto &dependency) {
                const auto dependency_mark{this->mark(dependency.second)};
                return !dependency_mark.has_value() ||
                       dependency_mark.value() > destination_mark.value();
              })) {
        this->track(destination);
        return false;
      }
    }

    dependencies_lock.unlock();

    Dependencies output_dependencies;
    for (const auto &dependency : dependencies) {
      assert(this->mark(dependency.second).has_value());
      output_dependencies.emplace_back(DependencyKind::Static,
                                       dependency.second);
    }

    handler(
        destination, dependencies,
        [&](const auto &new_dependency) {
          assert(this->mark(new_dependency).has_value());
          output_dependencies.emplace_back(DependencyKind::Dynamic,
                                           new_dependency);
        },
        context);

    assert(destination.is_absolute());
    assert(std::filesystem::exists(destination));
    this->refresh(destination);
    this->track(destination);
    const auto key{destination.lexically_relative(this->root).string()};
    {
      std::unique_lock lock{this->dependencies_mutex};
      this->dependencies_map.insert_or_assign(key,
                                              std::move(output_dependencies));
    }

    return true;
  }

  // TODO: Continue refactoring and cleaning up these interfaces

  auto track(const std::filesystem::path &path)
      -> const std::filesystem::path &;
  [[nodiscard]] auto is_untracked_file(const std::filesystem::path &path) const
      -> bool;

  // TODO: Turn into a proper DISPATCH on index.cc
  auto output_write_json_if_different(const std::filesystem::path &path,
                                      const sourcemeta::core::JSON &document)
      -> void;

private:
  auto output_write_json(const std::filesystem::path &path,
                         const sourcemeta::core::JSON &document)
      -> const std::filesystem::path &;

  std::filesystem::path root;
  std::unordered_map<std::filesystem::path, mark_type> marks;
  std::shared_mutex mutex;
  std::unordered_map<std::string, Dependencies> dependencies_map;
  mutable std::shared_mutex dependencies_mutex;
  bool has_previous_run{false};
  std::unordered_map<std::filesystem::path, bool> tracker;
  mutable std::shared_mutex tracker_mutex;
};

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_H_
