#include <sourcemeta/core/test.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/search.h>

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

TEST(count_single_entry) {
  const auto path{test_path("count_single.metapack")};
  write_search_file(path, {{.path = "/foo",
                            .identifier = "http://example.com/foo",
                            .title = "Title",
                            .description = "Desc",
                            .health = 80,
                            .priority = 100,
                            .bytes_raw = 100,
                            .bytes_bundled = 200}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(view.count(), 1);
}

TEST(count_multiple_entries) {
  const auto path{test_path("count_multiple.metapack")};
  write_search_file(path, {{.path = "/a",
                            .identifier = "http://example.com/a",
                            .title = "A",
                            .description = "Da",
                            .health = 80,
                            .priority = 100,
                            .bytes_raw = 1,
                            .bytes_bundled = 2},
                           {.path = "/b",
                            .identifier = "http://example.com/b",
                            .title = "B",
                            .description = "Db",
                            .health = 80,
                            .priority = 100,
                            .bytes_raw = 3,
                            .bytes_bundled = 4},
                           {.path = "/c",
                            .identifier = "http://example.com/c",
                            .title = "C",
                            .description = "Dc",
                            .health = 80,
                            .priority = 100,
                            .bytes_raw = 5,
                            .bytes_bundled = 6}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(view.count(), 3);
}

TEST(at_returns_field_data) {
  const auto path{test_path("at_fields.metapack")};
  write_search_file(path, {{.path = "/foo/bar",
                            .identifier = "http://example.com/foo/bar",
                            .title = "My Title",
                            .description = "My Description",
                            .health = 80,
                            .priority = 100,
                            .bytes_raw = 100,
                            .bytes_bundled = 200}});
  sourcemeta::one::SearchView view{path};
  const auto entry{view.at(0)};
  EXPECT_EQ(entry.path, "/foo/bar");
  EXPECT_EQ(entry.identifier, "http://example.com/foo/bar");
  EXPECT_EQ(entry.title, "My Title");
  EXPECT_EQ(entry.description, "My Description");
  EXPECT_EQ(entry.bytes_raw, 100);
  EXPECT_EQ(entry.bytes_bundled, 200);
}

TEST(at_walks_in_sorted_order) {
  const auto path{test_path("at_sorted.metapack")};
  write_search_file(path, {{.path = "/zebra",
                            .identifier = "http://example.com/zebra",
                            .title = "Title",
                            .description = "Desc",
                            .health = 80,
                            .priority = 100,
                            .bytes_raw = 11,
                            .bytes_bundled = 22},
                           {.path = "/apple",
                            .identifier = "http://example.com/apple",
                            .title = "Title",
                            .description = "Desc",
                            .health = 80,
                            .priority = 100,
                            .bytes_raw = 33,
                            .bytes_bundled = 44},
                           {.path = "/mango",
                            .identifier = "http://example.com/mango",
                            .title = "Title",
                            .description = "Desc",
                            .health = 80,
                            .priority = 100,
                            .bytes_raw = 55,
                            .bytes_bundled = 66}});
  sourcemeta::one::SearchView view{path};
  EXPECT_EQ(view.count(), 3);
  EXPECT_EQ(view.at(0).path, "/apple");
  EXPECT_EQ(view.at(0).identifier, "http://example.com/apple");
  EXPECT_EQ(view.at(0).bytes_raw, 33);
  EXPECT_EQ(view.at(0).bytes_bundled, 44);
  EXPECT_EQ(view.at(1).path, "/mango");
  EXPECT_EQ(view.at(1).identifier, "http://example.com/mango");
  EXPECT_EQ(view.at(1).bytes_raw, 55);
  EXPECT_EQ(view.at(1).bytes_bundled, 66);
  EXPECT_EQ(view.at(2).path, "/zebra");
  EXPECT_EQ(view.at(2).identifier, "http://example.com/zebra");
  EXPECT_EQ(view.at(2).bytes_raw, 11);
  EXPECT_EQ(view.at(2).bytes_bundled, 22);
}

TEST(at_returns_empty_strings_for_empty_metadata) {
  const auto path{test_path("at_empty_meta.metapack")};
  write_search_file(path, {{.path = "/only/path",
                            .identifier = "http://example.com/only/path",
                            .title = "",
                            .description = "",
                            .health = 80,
                            .priority = 100,
                            .bytes_raw = 7,
                            .bytes_bundled = 8}});
  sourcemeta::one::SearchView view{path};
  const auto entry{view.at(0)};
  EXPECT_EQ(entry.path, "/only/path");
  EXPECT_EQ(entry.identifier, "http://example.com/only/path");
  EXPECT_EQ(entry.title, "");
  EXPECT_EQ(entry.description, "");
  EXPECT_EQ(entry.bytes_raw, 7);
  EXPECT_EQ(entry.bytes_bundled, 8);
}
