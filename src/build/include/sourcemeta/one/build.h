#ifndef SOURCEMETA_ONE_BUILD_H_
#define SOURCEMETA_ONE_BUILD_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

#include <sourcemeta/core/json.h>

#include <array>         // std::array
#include <atomic>        // std::atomic
#include <filesystem>    // std::filesystem
#include <functional>    // std::function, std::reference_wrapper, std::cref
#include <mutex>         // std::mutex, std::lock_guard
#include <optional>      // std::optional
#include <string>        // std::string
#include <type_traits>   // std::is_same_v
#include <unordered_map> // std::unordered_map
#include <utility>       // std::move
#include <vector>        // std::vector

namespace sourcemeta::one {

class SOURCEMETA_ONE_BUILD_EXPORT Build {
public:
  using Dependencies =
      std::vector<std::reference_wrapper<const std::filesystem::path>>;

  using DynamicCallback = std::function<void(const std::filesystem::path &)>;

  template <typename Context>
  using Handler =
      std::function<void(const std::filesystem::path &, const Dependencies &,
                         const DynamicCallback &, const Context &)>;

  using mark_type = std::filesystem::file_time_type;

  Build(const std::filesystem::path &output_root);

  auto path() const -> const std::filesystem::path & { return this->root; }

  [[nodiscard]] auto has_dependencies(const std::filesystem::path &path) const
      -> bool;
  auto finish() -> void;
  auto refresh(const std::filesystem::path &path) -> void;

  template <typename Context>
  auto dispatch(const Handler<Context> &handler,
                const std::filesystem::path &destination,
                const Context &context,
                const std::vector<std::filesystem::path> &dependencies)
      -> bool {
    const auto &destination_string{destination.native()};
    const auto *entry{this->find_entry(destination_string)};
    if (entry != nullptr && entry->file_mark.has_value() &&
        (!entry->static_dependencies.empty() ||
         !entry->dynamic_dependencies.empty())) {
      bool static_dependencies_match{dependencies.size() ==
                                     entry->static_dependencies.size()};
      if (static_dependencies_match) {
        for (std::size_t index = 0; index < dependencies.size(); ++index) {
          if (dependencies[index] != entry->static_dependencies[index]) {
            static_dependencies_match = false;
            break;
          }
        }
      }

      if (this->dispatch_is_cached(*entry, static_dependencies_match)) {
        this->track_unsafe(destination);
        return false;
      }
    }

    Dependencies static_dependency_references;
    static_dependency_references.reserve(dependencies.size());
    for (const auto &dependency : dependencies) {
      static_dependency_references.emplace_back(std::cref(dependency));
    }

    std::vector<std::filesystem::path> dynamic_dependencies;
    handler(
        destination, static_dependency_references,
        [&](const auto &new_dependency) {
          dynamic_dependencies.emplace_back(new_dependency);
        },
        context);

    this->dispatch_commit(destination,
                          std::vector<std::filesystem::path>(
                              dependencies.begin(), dependencies.end()),
                          std::move(dynamic_dependencies));
    return true;
  }

  template <typename Context, typename... DependencyTypes>
    requires(sizeof...(DependencyTypes) == 0 ||
             (std::is_same_v<DependencyTypes, std::filesystem::path> && ...))
  auto dispatch(const Handler<Context> &handler,
                const std::filesystem::path &destination,
                const Context &context, const DependencyTypes &...dependencies)
      -> bool {
    const auto &destination_string{destination.native()};
    const auto *entry{this->find_entry(destination_string)};
    if (entry != nullptr && entry->file_mark.has_value() &&
        (!entry->static_dependencies.empty() ||
         !entry->dynamic_dependencies.empty())) {
      const std::array<const std::filesystem::path *,
                       sizeof...(DependencyTypes)>
          dependency_pointers{{&dependencies...}};
      constexpr auto dependency_count{sizeof...(DependencyTypes)};
      bool static_dependencies_match{entry->static_dependencies.size() ==
                                     dependency_count};
      if (static_dependencies_match) {
        for (std::size_t index = 0; index < dependency_count; ++index) {
          if (*dependency_pointers[index] !=
              entry->static_dependencies[index]) {
            static_dependencies_match = false;
            break;
          }
        }
      }

      if (this->dispatch_is_cached(*entry, static_dependencies_match)) {
        this->track_unsafe(destination);
        return false;
      }
    }

    const Dependencies static_dependency_references{std::cref(dependencies)...};

    std::vector<std::filesystem::path> dynamic_dependencies;
    handler(
        destination, static_dependency_references,
        [&](const auto &new_dependency) {
          dynamic_dependencies.emplace_back(new_dependency);
        },
        context);

    std::vector<std::filesystem::path> stored_static_dependencies;
    stored_static_dependencies.reserve(sizeof...(DependencyTypes));
    ((void)stored_static_dependencies.emplace_back(dependencies), ...);

    this->dispatch_commit(destination, std::move(stored_static_dependencies),
                          std::move(dynamic_dependencies));
    return true;
  }

  struct Entry {
    std::optional<mark_type> file_mark;
    std::vector<std::filesystem::path> static_dependencies;
    std::vector<std::filesystem::path> dynamic_dependencies;
    std::atomic<bool> tracked{false};
    std::atomic<bool> is_directory{false};

    Entry() = default;
    ~Entry() = default;
    Entry(Entry &&other) noexcept
        : file_mark{other.file_mark},
          static_dependencies{std::move(other.static_dependencies)},
          dynamic_dependencies{std::move(other.dynamic_dependencies)},
          tracked{other.tracked.load(std::memory_order_relaxed)},
          is_directory{other.is_directory.load(std::memory_order_relaxed)} {}
    auto operator=(Entry &&other) noexcept -> Entry & {
      file_mark = other.file_mark;
      static_dependencies = std::move(other.static_dependencies);
      dynamic_dependencies = std::move(other.dynamic_dependencies);
      tracked.store(other.tracked.load(std::memory_order_relaxed),
                    std::memory_order_relaxed);
      is_directory.store(other.is_directory.load(std::memory_order_relaxed),
                         std::memory_order_relaxed);
      return *this;
    }
    Entry(const Entry &) = delete;
    auto operator=(const Entry &) -> Entry & = delete;
  };

  auto track(const std::filesystem::path &path) -> void;
  [[nodiscard]] auto is_untracked_file(const std::filesystem::path &path) const
      -> bool;

  auto entries() -> const std::unordered_map<std::string, Entry> &;

  auto write_json_if_different(const std::filesystem::path &path,
                               const sourcemeta::core::JSON &document) -> void;

private:
  [[nodiscard]] auto find_entry(const std::string &key) const -> const Entry *;
  [[nodiscard]] auto find_mark(const std::filesystem::path &path) const
      -> std::optional<mark_type>;
  [[nodiscard]] auto dispatch_is_cached(const Entry &entry,
                                        bool static_dependencies_match) const
      -> bool;
  auto
  dispatch_commit(const std::filesystem::path &destination,
                  std::vector<std::filesystem::path> &&static_dependencies,
                  std::vector<std::filesystem::path> &&dynamic_dependencies)
      -> void;
  auto track_unsafe(const std::filesystem::path &path) -> void;
  auto merge_writes() -> void;

  std::filesystem::path root;
  std::string root_string;
  std::unordered_map<std::string, Entry> entries_;
  std::unordered_map<std::string, Entry> writes_;
  mutable std::mutex write_mutex_;
  std::atomic<bool> has_writes_{false};
  bool has_previous_run{false};
};

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_H_
