#ifndef SOURCEMETA_ONE_BUILD_STATE_H_
#define SOURCEMETA_ONE_BUILD_STATE_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

#include <filesystem> // std::filesystem::path, std::filesystem::file_time_type
#include <string>     // std::string
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

namespace sourcemeta::one {

class SOURCEMETA_ONE_BUILD_EXPORT BuildState {
public:
  struct Entry {
    std::filesystem::file_time_type file_mark;
    std::vector<std::filesystem::path> dependencies;
  };

  using Container = std::unordered_map<std::string, Entry>;

  BuildState() = default;

  auto load(const std::filesystem::path &path) -> void;
  auto save(const std::filesystem::path &path) const -> void;

  [[nodiscard]] auto empty() const -> bool { return this->data.empty(); }
  [[nodiscard]] auto size() const -> std::size_t { return this->data.size(); }

  [[nodiscard]] auto contains(const Container::key_type &key) const -> bool {
    return this->data.contains(key);
  }

  [[nodiscard]] auto entry(const Container::key_type &key) const
      -> const Entry * {
    const auto match{this->data.find(key)};
    return match == this->data.end() ? nullptr : &match->second;
  }

  [[nodiscard]] auto
  is_stale(const Container::key_type &key,
           const std::filesystem::file_time_type source_mtime) const -> bool {
    const auto *result{this->entry(key)};
    return result == nullptr || source_mtime > result->file_mark;
  }

  auto commit(const std::filesystem::path &path,
              std::vector<std::filesystem::path> dependencies) -> void {
    auto &result{this->data[path.native()]};
    result.file_mark = std::filesystem::file_time_type::clock::now();
    result.dependencies = std::move(dependencies);
  }

  auto forget(const Container::key_type &key) -> void;

  [[nodiscard]] auto begin() const -> Container::const_iterator {
    return this->data.begin();
  }

  [[nodiscard]] auto end() const -> Container::const_iterator {
    return this->data.end();
  }

  auto emplace(const std::filesystem::path &path, Entry entry) -> void {
    this->data[path.native()] = std::move(entry);
  }

private:
  Container data;
};

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_STATE_H_
