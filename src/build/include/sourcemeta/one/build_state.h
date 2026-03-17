#ifndef SOURCEMETA_ONE_BUILD_STATE_H_
#define SOURCEMETA_ONE_BUILD_STATE_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

#include <sourcemeta/core/io.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint32_t
#include <filesystem>  // std::filesystem::path, std::filesystem::file_time_type
#include <functional>  // std::function
#include <memory>      // std::unique_ptr
#include <mutex>       // std::recursive_mutex, std::unique_lock
#include <set>         // std::set
#include <string>      // std::string
#include <string_view> // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

namespace sourcemeta::one {

class SOURCEMETA_ONE_BUILD_EXPORT BuildState {
public:
  struct Entry {
    std::filesystem::file_time_type file_mark;
    std::vector<std::filesystem::path> dependencies;
  };

  struct ResolverEntry {
    std::filesystem::file_time_type file_mark;
    std::string new_identifier;
    std::string original_identifier;
    std::string dialect;
    std::string relative_path;
  };

  BuildState() = default;
  ~BuildState() = default;
  BuildState(BuildState &&) = delete;
  auto operator=(BuildState &&) -> BuildState & = delete;
  BuildState(const BuildState &) = delete;
  auto operator=(const BuildState &) -> BuildState & = delete;

  [[nodiscard]] auto take_lock() const
      -> std::unique_lock<std::recursive_mutex>;

  auto load(const std::filesystem::path &path) -> void;
  auto save(const std::filesystem::path &path) const -> void;

  [[nodiscard]] auto empty() const -> bool { return this->entry_count == 0; }
  [[nodiscard]] auto size() const -> std::size_t { return this->entry_count; }

  [[nodiscard]] auto contains(const std::string &key) const -> bool;
  [[nodiscard]] auto entry(const std::string &key) const -> const Entry *;
  [[nodiscard]] auto
  is_stale(const std::string &key,
           std::filesystem::file_time_type source_mtime) const -> bool;
  auto commit(const std::filesystem::path &path,
              std::vector<std::filesystem::path> dependencies) -> void;
  auto commit(const std::string &source_path, ResolverEntry entry) -> void;
  auto forget(const std::string &key) -> void;
  auto emplace(const std::filesystem::path &path, Entry entry) -> void;
  [[nodiscard]] auto keys() const -> const std::vector<std::string_view> &;

  [[nodiscard]] auto resolve(const std::string &source_path,
                             std::filesystem::file_time_type mtime) const
      -> const ResolverEntry *;

  using DependentCallback =
      std::function<void(const std::string &from, const std::string &to,
                         const std::filesystem::path &source)>;

  auto dependents_of(std::string_view uri, const std::string &server_url,
                     const DependentCallback &callback) const -> void;

private:
  struct TransparentHash {
    using is_transparent = void;
    auto operator()(std::string_view value) const noexcept -> std::size_t {
      return std::hash<std::string_view>{}(value);
    }
  };

  struct TransparentEqual {
    using is_transparent = void;
    auto operator()(std::string_view left,
                    std::string_view right) const noexcept -> bool {
      return left == right;
    }
  };

  auto probe_slot(std::string_view key, std::uint8_t kind) const
      -> const std::uint8_t *;
  auto parse_slot_entry(const std::uint8_t *slot) const -> const Entry &;
  auto parse_slot_resolver_entry(const std::uint8_t *slot) const
      -> const ResolverEntry &;

  std::unique_ptr<sourcemeta::core::FileView> view;
  const std::uint8_t *view_data{nullptr};

  std::uint32_t table_capacity{0};
  const std::uint8_t *table_slots{nullptr};
  const std::uint8_t *string_pool{nullptr};

  std::unordered_map<std::string, Entry, TransparentHash, TransparentEqual>
      overlay;
  std::unordered_set<std::string, TransparentHash, TransparentEqual> deleted;
  mutable std::unordered_map<std::string, Entry, TransparentHash,
                             TransparentEqual>
      lazy_cache;

  std::unordered_map<std::string, ResolverEntry, TransparentHash,
                     TransparentEqual>
      resolver_overlay;
  mutable std::unordered_map<std::string, ResolverEntry, TransparentHash,
                             TransparentEqual>
      resolver_lazy_cache;

  struct DependentEntry {
    std::string from;
    std::string to;
    std::filesystem::path source;
    auto operator<(const DependentEntry &other) const -> bool {
      if (this->from != other.from) {
        return this->from < other.from;
      }

      return this->to < other.to;
    }
  };

  mutable std::recursive_mutex mutex_;
  mutable bool dependents_computed_{false};
  mutable std::unordered_map<std::string, std::set<DependentEntry>>
      dependents_cache_;
  std::filesystem::path loaded_path;
  std::size_t entry_count{0};
  std::size_t resolver_entry_count{0};
  bool dirty{false};
  mutable bool keys_stale{true};
  mutable std::vector<std::string_view> cached_keys;
};

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_STATE_H_
