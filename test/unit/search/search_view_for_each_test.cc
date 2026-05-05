#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/search.h>

#include <gtest/gtest.h>

#include <chrono>      // std::chrono
#include <cstdint>     // std::uint32_t, std::uint64_t
#include <cstring>     // std::memcpy
#include <filesystem>  // std::filesystem
#include <limits>      // std::numeric_limits
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

struct VisitedEntry {
  std::string path;
  std::string title;
  std::string description;
  std::uint64_t bytes_raw;
  std::uint64_t bytes_bundled;
  auto operator==(const VisitedEntry &) const -> bool = default;
};

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{SEARCH_TEST_DIRECTORY} / name;
}

static auto write_search_file(const std::filesystem::path &path,
                              std::vector<sourcemeta::one::SearchEntry> entries)
    -> void {
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const std::string_view payload_view{
      payload.empty()
          ? std::string_view{}
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          : std::string_view{reinterpret_cast<const char *>(payload.data()),
                             payload.size()}};
  sourcemeta::one::metapack_write_text(
      path, payload_view, "application/octet-stream",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});
}

static auto write_raw_search_file(const std::filesystem::path &path,
                                  const std::vector<std::uint8_t> &payload)
    -> void {
  const std::string_view payload_view{
      payload.empty()
          ? std::string_view{}
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          : std::string_view{reinterpret_cast<const char *>(payload.data()),
                             payload.size()}};
  sourcemeta::one::metapack_write_text(
      path, payload_view, "application/octet-stream",
      sourcemeta::one::MetapackEncoding::Identity, {},
      std::chrono::milliseconds{0});
}

static auto collect(sourcemeta::one::SearchView &view, std::size_t offset,
                    std::size_t count) -> std::vector<VisitedEntry> {
  std::vector<VisitedEntry> visited;
  view.for_each(offset, count,
                [&](const sourcemeta::one::SearchListEntry &entry) -> void {
                  visited.push_back(
                      {.path = std::string{entry.path},
                       .title = std::string{entry.title},
                       .description = std::string{entry.description},
                       .bytes_raw = entry.bytes_raw,
                       .bytes_bundled = entry.bytes_bundled});
                });
  return visited;
}

TEST(Search_view_for_each, visits_full_range) {
  const auto path{test_path("for_each_full.metapack")};
  write_search_file(path,
                    {{"/zebra", "Zebra Title", "Zebra Desc", 80, 11, 22},
                     {"/apple", "Apple Title", "Apple Desc", 80, 33, 44},
                     {"/mango", "Mango Title", "Mango Desc", 80, 55, 66}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 0, 3),
            (std::vector<VisitedEntry>{{.path = "/apple",
                                        .title = "Apple Title",
                                        .description = "Apple Desc",
                                        .bytes_raw = 33,
                                        .bytes_bundled = 44},
                                       {.path = "/mango",
                                        .title = "Mango Title",
                                        .description = "Mango Desc",
                                        .bytes_raw = 55,
                                        .bytes_bundled = 66},
                                       {.path = "/zebra",
                                        .title = "Zebra Title",
                                        .description = "Zebra Desc",
                                        .bytes_raw = 11,
                                        .bytes_bundled = 22}}));
}

TEST(Search_view_for_each, visits_subset_with_offset) {
  const auto path{test_path("for_each_offset.metapack")};
  write_search_file(path, {{"/a", "A Title", "A Desc", 80, 1, 2},
                           {"/b", "B Title", "B Desc", 80, 3, 4},
                           {"/c", "C Title", "C Desc", 80, 5, 6},
                           {"/d", "D Title", "D Desc", 80, 7, 8}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 1, 2),
            (std::vector<VisitedEntry>{{.path = "/b",
                                        .title = "B Title",
                                        .description = "B Desc",
                                        .bytes_raw = 3,
                                        .bytes_bundled = 4},
                                       {.path = "/c",
                                        .title = "C Title",
                                        .description = "C Desc",
                                        .bytes_raw = 5,
                                        .bytes_bundled = 6}}));
}

TEST(Search_view_for_each, clamps_count_to_total) {
  const auto path{test_path("for_each_clamp.metapack")};
  write_search_file(path, {{"/a", "A Title", "A Desc", 80, 1, 2},
                           {"/b", "B Title", "B Desc", 80, 3, 4}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 1, 100),
            (std::vector<VisitedEntry>{{.path = "/b",
                                        .title = "B Title",
                                        .description = "B Desc",
                                        .bytes_raw = 3,
                                        .bytes_bundled = 4}}));
}

TEST(Search_view_for_each, skips_when_offset_at_end) {
  const auto path{test_path("for_each_end.metapack")};
  write_search_file(path, {{"/a", "A Title", "A Desc", 80, 1, 2},
                           {"/b", "B Title", "B Desc", 80, 3, 4}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 2, 10), std::vector<VisitedEntry>{});
}

TEST(Search_view_for_each, skips_when_offset_past_end) {
  const auto path{test_path("for_each_past_end.metapack")};
  write_search_file(path, {{"/a", "A Title", "A Desc", 80, 1, 2}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 99, 10), std::vector<VisitedEntry>{});
}

TEST(Search_view_for_each, skips_when_count_zero) {
  const auto path{test_path("for_each_zero.metapack")};
  write_search_file(path, {{"/a", "A Title", "A Desc", 80, 1, 2},
                           {"/b", "B Title", "B Desc", 80, 3, 4}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 0, 0), std::vector<VisitedEntry>{});
}

TEST(Search_view_for_each, visit_order_matches_at) {
  const auto path{test_path("for_each_matches_at.metapack")};
  write_search_file(path, {{"/zebra", "", "", 80, 11, 22},
                           {"/apple", "", "", 80, 33, 44},
                           {"/mango", "", "", 80, 55, 66}});
  sourcemeta::one::SearchView view{path};
  const auto from_for_each{collect(view, 0, view.count())};
  std::vector<VisitedEntry> from_at;
  for (std::size_t index{0}; index < view.count(); ++index) {
    const auto entry{view.at(index)};
    from_at.push_back({.path = std::string{entry.path},
                       .title = std::string{entry.title},
                       .description = std::string{entry.description},
                       .bytes_raw = entry.bytes_raw,
                       .bytes_bundled = entry.bytes_bundled});
  }
  EXPECT_EQ(from_for_each, from_at);
}

TEST(Search_view_for_each, empty_strings_for_empty_metadata) {
  const auto path{test_path("for_each_empty_meta.metapack")};
  write_search_file(path, {{"/only/path", "", "", 80, 7, 8}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 0, 1),
            (std::vector<VisitedEntry>{{.path = "/only/path",
                                        .title = "",
                                        .description = "",
                                        .bytes_raw = 7,
                                        .bytes_bundled = 8}}));
}

TEST(Search_view_for_each, count_size_max_does_not_overflow) {
  const auto path{test_path("for_each_count_max.metapack")};
  write_search_file(path, {{"/a", "A Title", "A Desc", 80, 1, 2},
                           {"/b", "B Title", "B Desc", 80, 3, 4}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 0, std::numeric_limits<std::size_t>::max()),
            (std::vector<VisitedEntry>{{.path = "/a",
                                        .title = "A Title",
                                        .description = "A Desc",
                                        .bytes_raw = 1,
                                        .bytes_bundled = 2},
                                       {.path = "/b",
                                        .title = "B Title",
                                        .description = "B Desc",
                                        .bytes_raw = 3,
                                        .bytes_bundled = 4}}));
}

TEST(Search_view_for_each, count_size_max_with_offset_does_not_overflow) {
  const auto path{test_path("for_each_count_max_offset.metapack")};
  write_search_file(path, {{"/a", "A Title", "A Desc", 80, 1, 2},
                           {"/b", "B Title", "B Desc", 80, 3, 4}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 1, std::numeric_limits<std::size_t>::max()),
            (std::vector<VisitedEntry>{{.path = "/b",
                                        .title = "B Title",
                                        .description = "B Desc",
                                        .bytes_raw = 3,
                                        .bytes_bundled = 4}}));
}

TEST(Search_view_for_each, malformed_offset_table_too_large_returns_nothing) {
  const auto path{test_path("for_each_malformed_offset_table.metapack")};
  sourcemeta::one::SearchIndexHeader header{};
  header.entry_count = 1000;
  header.records_offset =
      static_cast<std::uint32_t>(sizeof(sourcemeta::one::SearchIndexHeader) +
                                 1000 * sizeof(std::uint32_t));
  std::vector<std::uint8_t> payload(sizeof(sourcemeta::one::SearchIndexHeader));
  std::memcpy(payload.data(), &header,
              sizeof(sourcemeta::one::SearchIndexHeader));
  write_raw_search_file(path, payload);

  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 0, 100), std::vector<VisitedEntry>{});
}

TEST(Search_view_for_each, malformed_record_offset_out_of_bounds_stops) {
  const auto path{test_path("for_each_malformed_record_offset.metapack")};
  sourcemeta::one::SearchIndexHeader header{};
  header.entry_count = 1;
  header.records_offset = static_cast<std::uint32_t>(
      sizeof(sourcemeta::one::SearchIndexHeader) + sizeof(std::uint32_t));
  std::vector<std::uint8_t> payload(sizeof(sourcemeta::one::SearchIndexHeader) +
                                    sizeof(std::uint32_t));
  std::memcpy(payload.data(), &header,
              sizeof(sourcemeta::one::SearchIndexHeader));
  const std::uint32_t bad_record_offset{99999};
  std::memcpy(payload.data() + sizeof(sourcemeta::one::SearchIndexHeader),
              &bad_record_offset, sizeof(std::uint32_t));
  write_raw_search_file(path, payload);

  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 0, 100), std::vector<VisitedEntry>{});
}

TEST(Search_view_for_each, malformed_record_field_lengths_stops) {
  const auto path{test_path("for_each_malformed_record_field.metapack")};
  auto payload{
      sourcemeta::one::make_search({{"/foo", "Title", "Desc", 80, 1, 2}})};
  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  sourcemeta::one::SearchRecordHeader bad_record{};
  bad_record.path_length = 60000;
  bad_record.title_length = 60000;
  bad_record.description_length = 60000;
  std::memcpy(payload.data() + header.records_offset, &bad_record,
              sizeof(sourcemeta::one::SearchRecordHeader));
  write_raw_search_file(path, payload);

  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(collect(view, 0, 100), std::vector<VisitedEntry>{});
}
