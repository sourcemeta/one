#include <sourcemeta/core/test.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/search.h>

#include <chrono>      // std::chrono
#include <cstdint>     // std::uint8_t, std::uint32_t
#include <filesystem>  // std::filesystem
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace {

class SearchCorruption {
protected:
  static auto test_path(const std::string &name) -> std::filesystem::path {
    return std::filesystem::path{SEARCH_TEST_DIRECTORY} / name;
  }

  static auto append_u32(std::vector<std::uint8_t> &bytes,
                         const std::uint32_t value) -> void {
    for (std::uint32_t index{0}; index < 4; ++index) {
      bytes.push_back(static_cast<std::uint8_t>(value >> (index * 8)));
    }
  }

  static auto write_raw_search_file(const std::filesystem::path &path,
                                    const std::vector<std::uint8_t> &payload)
      -> void {
    const std::string_view payload_view{
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<const char *>(payload.data()), payload.size()};
    sourcemeta::one::metapack_write_text(
        path, payload_view, "application/octet-stream",
        sourcemeta::one::MetapackEncoding::Identity, {},
        std::chrono::milliseconds{0});
  }
};

} // namespace

TEST_F(SearchCorruption, count_rejects_oversized_entry_count) {
  std::vector<std::uint8_t> payload;
  append_u32(payload, 1000);
  append_u32(payload, 8);
  const auto path{test_path("corrupt_oversized_count.metapack")};
  write_raw_search_file(path, payload);
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(view.count(), 0);
}

TEST_F(SearchCorruption, at_rejects_oversized_entry_count) {
  std::vector<std::uint8_t> payload;
  append_u32(payload, 1000);
  append_u32(payload, 8);
  const auto path{test_path("corrupt_at_oversized_count.metapack")};
  write_raw_search_file(path, payload);
  sourcemeta::one::SearchView view{path};
  const auto entry{view.at(0)};
  EXPECT_TRUE(entry.path.empty());
  EXPECT_TRUE(entry.identifier.empty());
  EXPECT_TRUE(entry.title.empty());
  EXPECT_TRUE(entry.description.empty());
}

TEST_F(SearchCorruption, at_rejects_out_of_bounds_record_offset) {
  std::vector<std::uint8_t> payload;
  append_u32(payload, 1);
  append_u32(payload, 12);
  append_u32(payload, 9999);
  const auto path{test_path("corrupt_record_offset.metapack")};
  write_raw_search_file(path, payload);
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(view.count(), 1);
  const auto entry{view.at(0)};
  EXPECT_TRUE(entry.path.empty());
  EXPECT_TRUE(entry.identifier.empty());
}

TEST_F(SearchCorruption, at_rejects_index_past_count) {
  const auto path{test_path("corrupt_index_past_count.metapack")};
  write_raw_search_file(path, sourcemeta::one::make_search(
                                  {{"/foo", "http://example.com/foo", "Title",
                                    "Desc", 80, 100, 100, 200}}));
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(view.count(), 1);
  const auto entry{view.at(5)};
  EXPECT_TRUE(entry.path.empty());
  EXPECT_TRUE(entry.identifier.empty());
}
