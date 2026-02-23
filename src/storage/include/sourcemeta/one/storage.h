#ifndef SOURCEMETA_ONE_STORAGE_H_
#define SOURCEMETA_ONE_STORAGE_H_

#include <sourcemeta/core/json.h>

#include <sourcemeta/one/shared.h>

#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem
#include <fstream>     // std::ifstream
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

class Storage {
public:
  using Key = std::string_view;

  explicit Storage(std::filesystem::path base);

  template <typename... Parts>
  [[nodiscard]] static auto key(const std::string_view first,
                                const Parts &...rest) -> std::string {
    static_assert(sizeof...(rest) >= 1,
                  "A storage key must have at least two components");
    std::size_t total{first.size()};
    ((total += 1 + std::string_view{rest}.size()), ...);
    std::string result;
    result.reserve(total);
    result.append(first);
    ((result.push_back('/'), result.append(std::string_view{rest})), ...);
    return result;
  }

  [[nodiscard]] auto exists(const Key key) const -> bool;
  [[nodiscard]] auto sibling_exists(const Key key,
                                    const std::string_view filename) const
      -> bool;
  [[nodiscard]] auto read_json(const Key key) const -> sourcemeta::core::JSON;
  [[nodiscard]] auto read_sibling_json(const Key key,
                                       const std::string_view filename) const
      -> sourcemeta::core::JSON;
  [[nodiscard]] auto read_raw(const Key key) const
      -> std::optional<File<std::ifstream>>;

private:
  std::filesystem::path base_;
};

} // namespace sourcemeta::one

#endif
