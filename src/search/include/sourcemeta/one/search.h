#ifndef SOURCEMETA_ONE_SEARCH_H_
#define SOURCEMETA_ONE_SEARCH_H_

#ifndef SOURCEMETA_ONE_SEARCH_EXPORT
#include <sourcemeta/one/search_export.h>
#endif

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <functional>  // std::function
#include <memory>      // std::unique_ptr
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace sourcemeta::one {

struct SearchEntry {
  std::string path;
  std::string title;
  std::string description;
  std::uint8_t health;
  std::uint64_t bytes_raw;
  std::uint64_t bytes_bundled;
};

struct SearchListEntry {
  std::string_view path;
  std::string_view title;
  std::string_view description;
  std::uint64_t bytes_raw;
  std::uint64_t bytes_bundled;
};

#pragma pack(push, 1)
struct SearchIndexHeader {
  std::uint32_t entry_count;
  std::uint32_t records_offset;
};

struct SearchRecordHeader {
  std::uint16_t path_length;
  std::uint16_t title_length;
  std::uint16_t description_length;
  std::uint64_t bytes_raw;
  std::uint64_t bytes_bundled;
};
#pragma pack(pop)

// Health is intentionally excluded from this enum as higher-health schemas
// always rank first regardless of scope. This is a non-negotiable aspect of the
// sort order to ensure that higher-quality schemas are always preferred.
enum SearchScope : std::uint8_t {
  SearchScopePath = 0b001,
  SearchScopeTitle = 0b010,
  SearchScopeDescription = 0b100
};

SOURCEMETA_ONE_SEARCH_EXPORT
auto make_search(std::vector<SearchEntry> &&entries)
    -> std::vector<std::uint8_t>;

SOURCEMETA_ONE_SEARCH_EXPORT
auto search(const std::uint8_t *payload, std::size_t payload_size,
            std::string_view query, std::size_t limit, std::uint8_t scope)
    -> sourcemeta::core::JSON;

class SOURCEMETA_ONE_SEARCH_EXPORT SearchView {
public:
  explicit SearchView(const std::filesystem::path &path);
  ~SearchView();

  SearchView(const SearchView &) = delete;
  SearchView(SearchView &&) = delete;
  auto operator=(const SearchView &) -> SearchView & = delete;
  auto operator=(SearchView &&) -> SearchView & = delete;

  auto search(std::string_view query, std::size_t limit, std::uint8_t scope)
      -> sourcemeta::core::JSON;

  auto count() -> std::size_t;
  auto at(std::size_t index) -> SearchListEntry;
  auto for_each(std::size_t offset, std::size_t count,
                const std::function<void(const SearchListEntry &)> &callback)
      -> void;

private:
  std::unique_ptr<sourcemeta::core::FileView> view_;
  const std::uint8_t *payload_{nullptr};
  std::size_t payload_size_{0};
};

} // namespace sourcemeta::one

#endif
