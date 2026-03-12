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

  auto load(const std::filesystem::path &path) -> bool;
  auto save(const std::filesystem::path &path) const -> void;

  [[nodiscard]] auto empty() const -> bool { return this->data.empty(); }
  [[nodiscard]] auto size() const -> std::size_t { return this->data.size(); }
  auto clear() -> void { this->data.clear(); }
  auto erase(const std::string &key) -> void { this->data.erase(key); }
  [[nodiscard]] auto contains(const std::string &key) const -> bool {
    return this->data.contains(key);
  }

  [[nodiscard]] auto find(const std::string &key) -> Container::iterator {
    return this->data.find(key);
  }

  [[nodiscard]] auto find(const std::string &key) const
      -> Container::const_iterator {
    return this->data.find(key);
  }

  [[nodiscard]] auto at(const std::string &key) const -> const Entry & {
    return this->data.at(key);
  }

  auto add(const std::filesystem::path &path,
           const std::filesystem::file_time_type mark) -> void {
    this->data[path.string()].file_mark = mark;
  }

  auto add(const std::filesystem::path &path, Entry entry) -> void {
    this->data[path.string()] = std::move(entry);
  }

  auto operator[](const std::string &key) -> Entry & { return this->data[key]; }

  [[nodiscard]] auto begin() const -> Container::const_iterator {
    return this->data.begin();
  }

  [[nodiscard]] auto end() const -> Container::const_iterator {
    return this->data.end();
  }

private:
  Container data;
};

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_STATE_H_
