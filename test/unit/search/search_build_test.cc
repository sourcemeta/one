#include <sourcemeta/one/search.h>

#include <gtest/gtest.h>

#include <cstring> // std::memcpy
#include <string>  // std::string
#include <utility> // std::move
#include <vector>  // std::vector

TEST(Search_build, empty) {
  std::vector<sourcemeta::one::SearchEntry> entries;
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_TRUE(payload.empty());
}

TEST(Search_build, single_entry) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "My Title", "A description",
       80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_FALSE(payload.empty());
  EXPECT_GE(payload.size(), sizeof(sourcemeta::one::SearchIndexHeader));
}

TEST(Search_build, header_single_entry) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "http://example.com/foo", "Title", "Desc", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
  EXPECT_EQ(header.records_offset,
            sizeof(sourcemeta::one::SearchIndexHeader) + sizeof(std::uint32_t));
}

TEST(Search_build, header_multiple_entries) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/a", "http://example.com/a", "A", "Desc A", 80, 100, 0, 0},
      {"/b", "http://example.com/b", "B", "Desc B", 80, 100, 0, 0},
      {"/c", "http://example.com/c", "C", "Desc C", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 3);
  EXPECT_EQ(header.records_offset, sizeof(sourcemeta::one::SearchIndexHeader) +
                                       3 * sizeof(std::uint32_t));
}

TEST(Search_build, offset_table) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/a", "http://example.com/a", "A", "D", 80, 100, 0, 0},
      {"/b", "http://example.com/b", "BB", "DD", 80, 100, 0, 0}};
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

TEST(Search_build, record_fields) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/test/path", "http://example.com/test/path", "My Title",
       "My Description", 80, 100, 4096, 8192}};
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

TEST(Search_build, total_size) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/a", "http://example.com/a", "T", "D", 80, 100, 0, 0},
      {"/bb", "http://example.com/bb", "TT", "DD", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  const auto first_fields{std::string{"/a"}.size() +
                          std::string{"http://example.com/a"}.size() +
                          std::string{"T"}.size() + std::string{"D"}.size()};
  const auto second_fields{std::string{"/bb"}.size() +
                           std::string{"http://example.com/bb"}.size() +
                           std::string{"TT"}.size() + std::string{"DD"}.size()};
  const auto expected_size{
      sizeof(sourcemeta::one::SearchIndexHeader) + 2 * sizeof(std::uint32_t) +
      sizeof(sourcemeta::one::SearchRecordHeader) + first_fields +
      sizeof(sourcemeta::one::SearchRecordHeader) + second_fields};
  EXPECT_EQ(payload.size(), expected_size);
}

TEST(Search_build, skips_entry_with_oversized_path) {
  const std::string oversized_path(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {oversized_path, "http://example.com/oversized", "Title", "Desc", 80, 100,
       0, 0},
      {"/normal", "http://example.com/normal", "Normal", "Desc", 80, 100, 0,
       0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
}

TEST(Search_build, skips_entry_with_oversized_identifier) {
  const std::string oversized_identifier(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", oversized_identifier, "Title", "Desc", 80, 100, 0, 0},
      {"/normal", "http://example.com/normal", "Normal", "Desc", 80, 100, 0,
       0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
}

TEST(Search_build, skips_entry_with_oversized_title) {
  const std::string oversized_title(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "http://example.com/foo", oversized_title, "Desc", 80, 100, 0,
       0},
      {"/normal", "http://example.com/normal", "Normal", "Desc", 80, 100, 0,
       0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
}

TEST(Search_build, skips_entry_with_oversized_description) {
  const std::string oversized_description(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "http://example.com/foo", "Title", oversized_description, 80,
       100, 0, 0},
      {"/normal", "http://example.com/normal", "Normal", "Desc", 80, 100, 0,
       0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
}

TEST(Search_build, all_entries_oversized_returns_empty) {
  const std::string oversized(70000, 'x');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {oversized, "http://example.com/oversized", "Title", "Desc", 80, 100, 0,
       0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_TRUE(payload.empty());
}

TEST(Search_build, entry_at_exact_uint16_max_is_kept) {
  const std::string max_path(65535, 'a');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {max_path, "http://example.com/x", "", "", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_FALSE(payload.empty());

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
}

TEST(Search_build, entry_at_uint16_max_plus_one_is_skipped) {
  const std::string too_long_path(65536, 'a');
  std::vector<sourcemeta::one::SearchEntry> entries{
      {too_long_path, "http://example.com/x", "", "", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_TRUE(payload.empty());
}

TEST(Search_build, priority_is_primary_sort_key) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/low/rich", "http://example.com/low/rich", "Rich Title", "Rich Desc",
       100, 0, 0, 0},
      {"/high/bare", "http://example.com/high/bare", "", "", 0, 100, 0, 0},
      {"/mid/rich", "http://example.com/mid/rich", "Mid Title", "Mid Desc", 90,
       50, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(payload.data(), payload.size(), "/",
                                            10,
                                            sourcemeta::one::SearchScopePath)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0).at("path").to_string(), "/high/bare");
  EXPECT_EQ(result.at(1).at("path").to_string(), "/mid/rich");
  EXPECT_EQ(result.at(2).at("path").to_string(), "/low/rich");
}

TEST(Search_build, priority_does_not_surface_in_search_output) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "http://example.com/foo", "Title", "Desc", 80, 50, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(payload.data(), payload.size(),
                                            "foo", 10,
                                            sourcemeta::one::SearchScopePath)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_FALSE(result.at(0).defines("priority"));
}
