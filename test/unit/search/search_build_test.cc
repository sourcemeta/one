#include <sourcemeta/one/search.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <cstring>     // std::memcpy
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

TEST(empty) {
  std::vector<sourcemeta::one::SearchEntry> entries;
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_TRUE(payload.empty());
}

TEST(single_entry) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "My Title",
       .description = "A description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_FALSE(payload.empty());
  EXPECT_GE(payload.size(), sizeof(sourcemeta::one::SearchIndexHeader));
}

TEST(header_single_entry) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo",
       .identifier = "http://example.com/foo",
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
  EXPECT_EQ(header.records_offset,
            sizeof(sourcemeta::one::SearchIndexHeader) + sizeof(std::uint32_t));
}

TEST(header_multiple_entries) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/a",
       .identifier = "http://example.com/a",
       .title = "A",
       .description = "Desc A",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/b",
       .identifier = "http://example.com/b",
       .title = "B",
       .description = "Desc B",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/c",
       .identifier = "http://example.com/c",
       .title = "C",
       .description = "Desc C",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 3);
  EXPECT_EQ(header.records_offset, sizeof(sourcemeta::one::SearchIndexHeader) +
                                       3 * sizeof(std::uint32_t));
}

TEST(offset_table) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/a",
       .identifier = "http://example.com/a",
       .title = "A",
       .description = "D",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/b",
       .identifier = "http://example.com/b",
       .title = "BB",
       .description = "DD",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));

  std::uint32_t offset_first{0};
  std::uint32_t offset_second{0};
  std::memcpy(&offset_first,
              payload.data() + sizeof(sourcemeta::one::SearchIndexHeader),
              sizeof(std::uint32_t));
  std::memcpy(&offset_second,
              payload.data() + sizeof(sourcemeta::one::SearchIndexHeader) +
                  sizeof(std::uint32_t),
              sizeof(std::uint32_t));

  EXPECT_EQ(offset_first, header.records_offset);
  const auto first_record_size{
      sizeof(sourcemeta::one::SearchRecordHeader) + std::string{"/a"}.size() +
      std::string{"http://example.com/a"}.size() + std::string{"A"}.size() +
      std::string{"D"}.size()};
  EXPECT_EQ(offset_second, offset_first + first_record_size);
}

TEST(record_fields) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/test/path",
       .identifier = "http://example.com/test/path",
       .title = "My Title",
       .description = "My Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 4096,
       .bytes_bundled = 8192}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));

  sourcemeta::one::SearchRecordHeader record_header{};
  std::memcpy(&record_header, payload.data() + header.records_offset,
              sizeof(sourcemeta::one::SearchRecordHeader));
  EXPECT_EQ(record_header.path_length, 10);
  EXPECT_EQ(record_header.identifier_length, 28);
  EXPECT_EQ(record_header.title_length, 8);
  EXPECT_EQ(record_header.description_length, 14);
  EXPECT_EQ(record_header.bytes_raw, 4096);
  EXPECT_EQ(record_header.bytes_bundled, 8192);

  const auto *field_data{payload.data() + header.records_offset +
                         sizeof(sourcemeta::one::SearchRecordHeader)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string path(reinterpret_cast<const char *>(field_data),
                         record_header.path_length);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string identifier(
      reinterpret_cast<const char *>(field_data + record_header.path_length),
      record_header.identifier_length);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string title(
      reinterpret_cast<const char *>(field_data + record_header.path_length +
                                     record_header.identifier_length),
      record_header.title_length);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string description(
      reinterpret_cast<const char *>(field_data + record_header.path_length +
                                     record_header.identifier_length +
                                     record_header.title_length),
      record_header.description_length);
  EXPECT_EQ(path, "/test/path");
  EXPECT_EQ(identifier, "http://example.com/test/path");
  EXPECT_EQ(title, "My Title");
  EXPECT_EQ(description, "My Description");
}

TEST(total_size) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/a",
       .identifier = "http://example.com/a",
       .title = "T",
       .description = "D",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/bb",
       .identifier = "http://example.com/bb",
       .title = "TT",
       .description = "DD",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  const auto first_fields{std::string{"/a"}.size() +
                          std::string{"http://example.com/a"}.size() +
                          std::string{"T"}.size() + std::string{"D"}.size()};
  const auto second_fields{std::string{"/bb"}.size() +
                           std::string{"http://example.com/bb"}.size() +
                           std::string{"TT"}.size() + std::string{"DD"}.size()};
  const auto expected_size{
      sizeof(sourcemeta::one::SearchIndexHeader) + (2 * sizeof(std::uint32_t)) +
      sizeof(sourcemeta::one::SearchRecordHeader) + first_fields +
      sizeof(sourcemeta::one::SearchRecordHeader) + second_fields};
  EXPECT_EQ(payload.size(), expected_size);
}

TEST(skips_entry_with_oversized_path) {
  const std::string oversized_path(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = oversized_path,
       .identifier = "http://example.com/oversized",
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/normal",
       .identifier = "http://example.com/normal",
       .title = "Normal",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
}

TEST(skips_entry_with_oversized_identifier) {
  const std::string oversized_identifier(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo",
       .identifier = oversized_identifier,
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/normal",
       .identifier = "http://example.com/normal",
       .title = "Normal",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
}

TEST(truncates_oversized_title_with_ellipsis) {
  const std::string oversized_title(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo",
       .identifier = "http://example.com/foo",
       .title = oversized_title,
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);

  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "foo", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH)};
  EXPECT_EQ(result.size(), 1);
  const auto &title{result.at(0).at("title").to_string()};
  EXPECT_EQ(title.size(), 65535);
  EXPECT_TRUE(title.starts_with("xxxxx"));
  EXPECT_TRUE(title.ends_with("..."));
}

TEST(truncates_oversized_description_with_ellipsis) {
  const std::string oversized_description(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo",
       .identifier = "http://example.com/foo",
       .title = "Title",
       .description = oversized_description,
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);

  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "foo", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH)};
  EXPECT_EQ(result.size(), 1);
  const auto &description{result.at(0).at("description").to_string()};
  EXPECT_EQ(description.size(), 65535);
  EXPECT_TRUE(description.starts_with("xxxxx"));
  EXPECT_TRUE(description.ends_with("..."));
}

TEST(all_entries_oversized_returns_empty) {
  const std::string oversized(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = oversized,
       .identifier = "http://example.com/oversized",
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_TRUE(payload.empty());
}

TEST(entry_at_exact_uint16_max_is_kept) {
  const std::string max_path(65535, 'a');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = max_path,
       .identifier = "http://example.com/x",
       .title = "",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_FALSE(payload.empty());

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
}

TEST(entry_at_uint16_max_plus_one_is_skipped) {
  const std::string too_long_path(65536, 'a');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = too_long_path,
       .identifier = "http://example.com/x",
       .title = "",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_TRUE(payload.empty());
}

TEST(priority_is_primary_sort_key) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/low/rich",
       .identifier = "http://example.com/low/rich",
       .title = "Rich Title",
       .description = "Rich Desc",
       .health = 100,
       .priority = 0,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/high/bare",
       .identifier = "http://example.com/high/bare",
       .title = "",
       .description = "",
       .health = 0,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/mid/rich",
       .identifier = "http://example.com/mid/rich",
       .title = "Mid Title",
       .description = "Mid Desc",
       .health = 90,
       .priority = 50,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "/", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0).at("path").to_string(), "/high/bare");
  EXPECT_EQ(result.at(1).at("path").to_string(), "/mid/rich");
  EXPECT_EQ(result.at(2).at("path").to_string(), "/low/rich");
}

TEST(priority_and_health_surface_in_search_output) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo",
       .identifier = "http://example.com/foo",
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 50,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "foo", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).defines("priority"));
  EXPECT_EQ(result.at(0).at("priority").to_integer(), 50);
  EXPECT_TRUE(result.at(0).defines("health"));
  EXPECT_EQ(result.at(0).at("health").to_integer(), 80);
}
