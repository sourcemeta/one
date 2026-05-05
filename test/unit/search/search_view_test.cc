#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/search.h>

#include <gtest/gtest.h>

#include <chrono>      // std::chrono
#include <filesystem>  // std::filesystem
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

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

TEST(Search_view, count_single_entry) {
  const auto path{test_path("count_single.metapack")};
  write_search_file(path, {{"/foo", "Title", "Desc", 80, 100, 200}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(view.count(), 1);
}

TEST(Search_view, count_multiple_entries) {
  const auto path{test_path("count_multiple.metapack")};
  write_search_file(path, {{"/a", "A", "Da", 80, 1, 2},
                           {"/b", "B", "Db", 80, 3, 4},
                           {"/c", "C", "Dc", 80, 5, 6}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(view.count(), 3);
}

TEST(Search_view, at_returns_field_data) {
  const auto path{test_path("at_fields.metapack")};
  write_search_file(path,
                    {{"/foo/bar", "My Title", "My Description", 80, 100, 200}});
  sourcemeta::one::SearchView view{path};
  const auto entry{view.at(0)};
  EXPECT_EQ(entry.path, "/foo/bar");
  EXPECT_EQ(entry.title, "My Title");
  EXPECT_EQ(entry.description, "My Description");
  EXPECT_EQ(entry.bytes_raw, 100);
  EXPECT_EQ(entry.bytes_bundled, 200);
}

TEST(Search_view, at_walks_in_sorted_order) {
  const auto path{test_path("at_sorted.metapack")};
  write_search_file(path, {{"/zebra", "Title", "Desc", 80, 11, 22},
                           {"/apple", "Title", "Desc", 80, 33, 44},
                           {"/mango", "Title", "Desc", 80, 55, 66}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(view.count(), 3);
  EXPECT_EQ(view.at(0).path, "/apple");
  EXPECT_EQ(view.at(0).bytes_raw, 33);
  EXPECT_EQ(view.at(0).bytes_bundled, 44);
  EXPECT_EQ(view.at(1).path, "/mango");
  EXPECT_EQ(view.at(1).bytes_raw, 55);
  EXPECT_EQ(view.at(1).bytes_bundled, 66);
  EXPECT_EQ(view.at(2).path, "/zebra");
  EXPECT_EQ(view.at(2).bytes_raw, 11);
  EXPECT_EQ(view.at(2).bytes_bundled, 22);
}

TEST(Search_view, at_returns_empty_strings_for_empty_metadata) {
  const auto path{test_path("at_empty_meta.metapack")};
  write_search_file(path, {{"/only/path", "", "", 80, 7, 8}});
  sourcemeta::one::SearchView view{path};
  const auto entry{view.at(0)};
  EXPECT_EQ(entry.path, "/only/path");
  EXPECT_EQ(entry.title, "");
  EXPECT_EQ(entry.description, "");
  EXPECT_EQ(entry.bytes_raw, 7);
  EXPECT_EQ(entry.bytes_bundled, 8);
}

TEST(Search_view, for_each_visits_full_range) {
  const auto path{test_path("for_each_full.metapack")};
  write_search_file(path, {{"/zebra", "Title", "Desc", 80, 11, 22},
                           {"/apple", "Title", "Desc", 80, 33, 44},
                           {"/mango", "Title", "Desc", 80, 55, 66}});
  sourcemeta::one::SearchView view{path};
  std::vector<std::string> visited_paths;
  std::vector<std::uint64_t> visited_bytes_raw;
  std::vector<std::uint64_t> visited_bytes_bundled;
  view.for_each(0, 3,
                [&](const sourcemeta::one::SearchListEntry &entry) -> void {
                  visited_paths.emplace_back(entry.path);
                  visited_bytes_raw.push_back(entry.bytes_raw);
                  visited_bytes_bundled.push_back(entry.bytes_bundled);
                });
  EXPECT_EQ(visited_paths,
            (std::vector<std::string>{"/apple", "/mango", "/zebra"}));
  EXPECT_EQ(visited_bytes_raw, (std::vector<std::uint64_t>{33, 55, 11}));
  EXPECT_EQ(visited_bytes_bundled, (std::vector<std::uint64_t>{44, 66, 22}));
}

TEST(Search_view, for_each_visits_subset_with_offset) {
  const auto path{test_path("for_each_offset.metapack")};
  write_search_file(path, {{"/a", "Title", "Desc", 80, 1, 2},
                           {"/b", "Title", "Desc", 80, 3, 4},
                           {"/c", "Title", "Desc", 80, 5, 6},
                           {"/d", "Title", "Desc", 80, 7, 8}});
  sourcemeta::one::SearchView view{path};
  std::vector<std::string> visited;
  view.for_each(1, 2,
                [&](const sourcemeta::one::SearchListEntry &entry) -> void {
                  visited.emplace_back(entry.path);
                });
  EXPECT_EQ(visited, (std::vector<std::string>{"/b", "/c"}));
}

TEST(Search_view, for_each_clamps_count_to_total) {
  const auto path{test_path("for_each_clamp.metapack")};
  write_search_file(path, {{"/a", "Title", "Desc", 80, 1, 2},
                           {"/b", "Title", "Desc", 80, 3, 4}});
  sourcemeta::one::SearchView view{path};
  std::vector<std::string> visited;
  view.for_each(1, 100,
                [&](const sourcemeta::one::SearchListEntry &entry) -> void {
                  visited.emplace_back(entry.path);
                });
  EXPECT_EQ(visited, (std::vector<std::string>{"/b"}));
}

TEST(Search_view, for_each_skips_when_offset_at_end) {
  const auto path{test_path("for_each_end.metapack")};
  write_search_file(path, {{"/a", "Title", "Desc", 80, 1, 2},
                           {"/b", "Title", "Desc", 80, 3, 4}});
  sourcemeta::one::SearchView view{path};
  std::size_t calls{0};
  view.for_each(2, 10, [&](const sourcemeta::one::SearchListEntry &) -> void {
    ++calls;
  });
  EXPECT_EQ(calls, 0);
}

TEST(Search_view, for_each_skips_when_offset_past_end) {
  const auto path{test_path("for_each_past_end.metapack")};
  write_search_file(path, {{"/a", "Title", "Desc", 80, 1, 2}});
  sourcemeta::one::SearchView view{path};
  std::size_t calls{0};
  view.for_each(99, 10, [&](const sourcemeta::one::SearchListEntry &) -> void {
    ++calls;
  });
  EXPECT_EQ(calls, 0);
}

TEST(Search_view, for_each_skips_when_count_zero) {
  const auto path{test_path("for_each_zero.metapack")};
  write_search_file(path, {{"/a", "Title", "Desc", 80, 1, 2},
                           {"/b", "Title", "Desc", 80, 3, 4}});
  sourcemeta::one::SearchView view{path};
  std::size_t calls{0};
  view.for_each(
      0, 0, [&](const sourcemeta::one::SearchListEntry &) -> void { ++calls; });
  EXPECT_EQ(calls, 0);
}

TEST(Search_view, for_each_visit_order_matches_at) {
  const auto path{test_path("for_each_matches_at.metapack")};
  write_search_file(path, {{"/zebra", "", "", 80, 11, 22},
                           {"/apple", "", "", 80, 33, 44},
                           {"/mango", "", "", 80, 55, 66}});
  sourcemeta::one::SearchView view{path};
  std::vector<std::string> from_for_each;
  view.for_each(0, view.count(),
                [&](const sourcemeta::one::SearchListEntry &entry) -> void {
                  from_for_each.emplace_back(entry.path);
                });
  std::vector<std::string> from_at;
  for (std::size_t index{0}; index < view.count(); ++index) {
    from_at.emplace_back(view.at(index).path);
  }
  EXPECT_EQ(from_for_each, from_at);
}
