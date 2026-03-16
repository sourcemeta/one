#ifndef SOURCEMETA_ONE_BUILD_STATE_H_
#define SOURCEMETA_ONE_BUILD_STATE_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path, std::filesystem::file_time_type
#include <memory>      // std::unique_ptr
#include <string>      // std::string
#include <string_view> // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

namespace sourcemeta::core {
class FileView;
}

namespace sourcemeta::one {

class SOURCEMETA_ONE_BUILD_EXPORT BuildState {
public:
  struct Entry {
    std::filesystem::file_time_type file_mark;
    std::vector<std::filesystem::path> dependencies;
  };

  BuildState();
  ~BuildState();
  BuildState(BuildState &&) noexcept;
  auto operator=(BuildState &&) noexcept -> BuildState &;
  BuildState(const BuildState &) = delete;
  auto operator=(const BuildState &) -> BuildState & = delete;

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

  auto forget(const std::string &key) -> void;

  auto emplace(const std::filesystem::path &path, Entry entry) -> void;

  [[nodiscard]] auto keys() const -> const std::vector<std::string_view> & {
    if (!this->keys_stale) {
      return this->cached_keys;
    }

    this->cached_keys.clear();
    this->cached_keys.reserve(this->entry_count);
    for (const auto &[key, value] : this->overlay) {
      this->cached_keys.push_back(key);
    }

    for (const auto &[key, info] : this->mmap_index) {
      if (!this->deleted.contains(key) && !this->overlay.contains(key)) {
        this->cached_keys.push_back(key);
      }
    }

    this->keys_stale = false;
    return this->cached_keys;
  }

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

  struct MmapEntryInfo {
    std::filesystem::file_time_type file_mark;
    std::size_t data_offset;
    std::size_t data_length;
  };

  auto parse_mmap_entry(std::string_view key, const MmapEntryInfo &info) const
      -> const Entry &;

  std::unique_ptr<sourcemeta::core::FileView> view;
  const std::uint8_t *view_data{nullptr};

  std::unordered_map<std::string_view, MmapEntryInfo, TransparentHash,
                     TransparentEqual>
      mmap_index;

  std::unordered_map<std::string, Entry, TransparentHash, TransparentEqual>
      overlay;

  std::unordered_set<std::string, TransparentHash, TransparentEqual> deleted;

  mutable std::unordered_map<std::string, Entry, TransparentHash,
                             TransparentEqual>
      lazy_cache;

  std::size_t entry_count{0};
  bool dirty{false};
  mutable bool keys_stale{true};
  mutable std::vector<std::string_view> cached_keys;
};

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_STATE_H_
