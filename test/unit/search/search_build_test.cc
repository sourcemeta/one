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
      {"/foo/bar", "My Title", "A description", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_FALSE(payload.empty());
  EXPECT_GE(payload.size(), sizeof(sourcemeta::one::SearchIndexHeader));
}

TEST(Search_build, header_single_entry) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "Title", "Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 1);
  EXPECT_EQ(header.records_offset,
            sizeof(sourcemeta::one::SearchIndexHeader) + sizeof(std::uint32_t));
}

TEST(Search_build, header_multiple_entries) {
  std::vector<sourcemeta::one::SearchEntry> entries{{"/a", "A", "Desc A", 80},
                                                    {"/b", "B", "Desc B", 80},
                                                    {"/c", "C", "Desc C", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));
  EXPECT_EQ(header.entry_count, 3);
  EXPECT_EQ(header.records_offset, sizeof(sourcemeta::one::SearchIndexHeader) +
                                       3 * sizeof(std::uint32_t));
}

TEST(Search_build, offset_table) {
  std::vector<sourcemeta::one::SearchEntry> entries{{"/a", "A", "D", 80},
                                                    {"/b", "BB", "DD", 80}};
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
  const auto first_record_size{sizeof(sourcemeta::one::SearchRecordHeader) + 2 +
                               1 + 1};
  EXPECT_EQ(offset_second, offset_first + first_record_size);
}

TEST(Search_build, record_fields) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/test/path", "My Title", "My Description", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));

  sourcemeta::one::SearchRecordHeader record_header{};
  std::memcpy(&record_header, payload.data() + header.records_offset,
              sizeof(sourcemeta::one::SearchRecordHeader));
  EXPECT_EQ(record_header.path_length, 10);
  EXPECT_EQ(record_header.title_length, 8);
  EXPECT_EQ(record_header.description_length, 14);

  const auto *field_data{payload.data() + header.records_offset +
                         sizeof(sourcemeta::one::SearchRecordHeader)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string path(reinterpret_cast<const char *>(field_data),
                         record_header.path_length);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string title(
      reinterpret_cast<const char *>(field_data + record_header.path_length),
      record_header.title_length);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string description(
      reinterpret_cast<const char *>(field_data + record_header.path_length +
                                     record_header.title_length),
      record_header.description_length);
  EXPECT_EQ(path, "/test/path");
  EXPECT_EQ(title, "My Title");
  EXPECT_EQ(description, "My Description");
}

TEST(Search_build, total_size) {
  std::vector<sourcemeta::one::SearchEntry> entries{{"/a", "T", "D", 80},
                                                    {"/bb", "TT", "DD", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  const auto expected_size{
      sizeof(sourcemeta::one::SearchIndexHeader) + 2 * sizeof(std::uint32_t) +
      sizeof(sourcemeta::one::SearchRecordHeader) + 2 + 1 + 1 +
      sizeof(sourcemeta::one::SearchRecordHeader) + 3 + 2 + 2};
  EXPECT_EQ(payload.size(), expected_size);
}
