#include <sourcemeta/one/search.h>

#include <sourcemeta/core/json.h>

#include <gtest/gtest.h>

#include <cstdint> // std::uint8_t, std::uint32_t
#include <cstring> // std::memcpy
#include <utility> // std::move
#include <vector>  // std::vector

#define EXPECT_SEARCH_RESULT(result, index, expected_path, expected_title,     \
                             expected_description)                             \
  EXPECT_EQ((result).at(index).at("path").to_string(), (expected_path));       \
  EXPECT_EQ((result).at(index).at("title").to_string(), (expected_title));     \
  EXPECT_EQ((result).at(index).at("description").to_string(),                  \
            (expected_description));

TEST(Search_query, empty_payload_nullptr) {
  const auto result{sourcemeta::one::search(
      nullptr, 0, "anything", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, empty_payload_zero_size) {
  const std::uint8_t byte{0};
  const auto result{sourcemeta::one::search(
      &byte, 0, "anything", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, no_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Title", "Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "zzzzz", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, match_in_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Title", "Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "foo", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "Title", "Desc");
}

TEST(Search_query, match_in_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Special Title", "Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Special", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "Special Title", "Desc");
}

TEST(Search_query, match_in_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Title", "Unique description here", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Unique", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "Title",
                       "Unique description here");
}

TEST(Search_query, case_insensitive) {
  std::vector<sourcemeta::one::SearchEntry> entries_lower{
      {"/foo/bar", "Hello World", "desc", 80}};
  const auto payload_lower{
      sourcemeta::one::make_search(std::move(entries_lower))};
  const auto result_lower{sourcemeta::one::search(
      payload_lower.data(), payload_lower.size(), "hello", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result_lower.size(), 1);
  EXPECT_SEARCH_RESULT(result_lower, 0, "/foo/bar", "Hello World", "desc");

  std::vector<sourcemeta::one::SearchEntry> entries_upper{
      {"/foo/bar", "Hello World", "desc", 80}};
  const auto payload_upper{
      sourcemeta::one::make_search(std::move(entries_upper))};
  const auto result_upper{sourcemeta::one::search(
      payload_upper.data(), payload_upper.size(), "HELLO", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result_upper.size(), 1);
  EXPECT_SEARCH_RESULT(result_upper, 0, "/foo/bar", "Hello World", "desc");

  std::vector<sourcemeta::one::SearchEntry> entries_mixed{
      {"/foo/bar", "Hello World", "desc", 80}};
  const auto payload_mixed{
      sourcemeta::one::make_search(std::move(entries_mixed))};
  const auto result_mixed{sourcemeta::one::search(
      payload_mixed.data(), payload_mixed.size(), "hElLo", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result_mixed.size(), 1);
  EXPECT_SEARCH_RESULT(result_mixed, 0, "/foo/bar", "Hello World", "desc");
}

TEST(Search_query, multiple_matches) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/address", "Address Schema", "For addresses", 80},
      {"/schemas/person", "Person Schema", "For people", 80},
      {"/schemas/email", "Email Schema", "For emails", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schema", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/address", "Address Schema",
                       "For addresses");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/email", "Email Schema",
                       "For emails");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/person", "Person Schema",
                       "For people");
}

TEST(Search_query, limit_10) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/test0", "Test 0", "", 80},
      {"/schemas/test1", "Test 1", "", 80},
      {"/schemas/test2", "Test 2", "", 80},
      {"/schemas/test3", "Test 3", "", 80},
      {"/schemas/test4", "Test 4", "", 80},
      {"/schemas/test5", "Test 5", "", 80},
      {"/schemas/test6", "Test 6", "", 80},
      {"/schemas/test7", "Test 7", "", 80},
      {"/schemas/test8", "Test 8", "", 80},
      {"/schemas/test9", "Test 9", "", 80},
      {"/schemas/test10", "Test 10", "", 80},
      {"/schemas/test11", "Test 11", "", 80},
      {"/schemas/test12", "Test 12", "", 80},
      {"/schemas/test13", "Test 13", "", 80},
      {"/schemas/test14", "Test 14", "", 80}};

  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 10);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/test0", "Test 0", "");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/test1", "Test 1", "");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/test10", "Test 10", "");
  EXPECT_SEARCH_RESULT(result, 3, "/schemas/test11", "Test 11", "");
  EXPECT_SEARCH_RESULT(result, 4, "/schemas/test12", "Test 12", "");
  EXPECT_SEARCH_RESULT(result, 5, "/schemas/test13", "Test 13", "");
  EXPECT_SEARCH_RESULT(result, 6, "/schemas/test14", "Test 14", "");
  EXPECT_SEARCH_RESULT(result, 7, "/schemas/test2", "Test 2", "");
  EXPECT_SEARCH_RESULT(result, 8, "/schemas/test3", "Test 3", "");
  EXPECT_SEARCH_RESULT(result, 9, "/schemas/test4", "Test 4", "");
}

TEST(Search_query, round_trip_data_fidelity) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/a/b/c", "My Title", "My Description", 80},
      {"/x/y/z", "", "Only description", 80},
      {"/p/q", "Only title", "", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "/", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/a/b/c", "My Title", "My Description");
  EXPECT_SEARCH_RESULT(result, 1, "/p/q", "Only title", "");
  EXPECT_SEARCH_RESULT(result, 2, "/x/y/z", "", "Only description");
}

TEST(Search_query, single_entry_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/only", "One", "Entry", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "One", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/only", "One", "Entry");
}

TEST(Search_query, single_entry_no_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/only", "One", "Entry", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "nope", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, empty_title_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{{"/path/only", "", "", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "path", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/path/only", "", "");
}

TEST(Search_query, health_higher_scores_first) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/low", "Low Health", "Desc", 20},
      {"/schemas/high", "High Health", "Desc", 100},
      {"/schemas/mid", "Mid Health", "Desc", 60}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Health", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/high", "High Health", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mid", "Mid Health", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/low", "Low Health", "Desc");
}

TEST(Search_query, health_100_before_50) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/beta", "Beta", "Desc", 50},
      {"/schemas/alpha", "Alpha", "Desc", 100}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/alpha", "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/beta", "Beta", "Desc");
}

TEST(Search_query, health_0_ranks_last) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/zero", "Zero", "Desc", 0},
      {"/schemas/perfect", "Perfect", "Desc", 100},
      {"/schemas/okay", "Okay", "Desc", 50}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/perfect", "Perfect", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/okay", "Okay", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/zero", "Zero", "Desc");
}

TEST(Search_query, health_same_score_sorts_by_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/zebra", "Zebra", "Desc", 75},
      {"/schemas/apple", "Apple", "Desc", 75},
      {"/schemas/mango", "Mango", "Desc", 75}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/apple", "Apple", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mango", "Mango", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/zebra", "Zebra", "Desc");
}

TEST(Search_query, metadata_score_beats_health) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/healthy", "", "", 100},
      {"/schemas/complete", "Title", "Description", 30}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/complete", "Title", "Description");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/healthy", "", "");
}

TEST(Search_query, metadata_score_beats_health_title_only) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/no-meta", "", "", 100},
      {"/schemas/has-title", "A Title", "", 10}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/has-title", "A Title", "");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/no-meta", "", "");
}

TEST(Search_query, health_tiebreaker_within_same_metadata) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/low-health", "Title", "", 25},
      {"/schemas/high-health", "Title", "", 90},
      {"/schemas/mid-health", "Title", "", 50}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/high-health", "Title", "");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mid-health", "Title", "");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/low-health", "Title", "");
}

TEST(Search_query, health_fine_grained_ordering) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/d", "Title", "Desc", 70},
      {"/schemas/a", "Title", "Desc", 100},
      {"/schemas/c", "Title", "Desc", 85},
      {"/schemas/e", "Title", "Desc", 55},
      {"/schemas/b", "Title", "Desc", 95}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 5);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/c", "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 3, "/schemas/d", "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 4, "/schemas/e", "Title", "Desc");
}

TEST(Search_query, health_mixed_metadata_and_health) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/full-low", "Title", "Desc", 30},
      {"/schemas/title-high", "Title", "", 95},
      {"/schemas/full-high", "Title", "Desc", 90},
      {"/schemas/none-perfect", "", "", 100},
      {"/schemas/title-low", "Title", "", 40}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 5);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/full-high", "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/full-low", "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/title-high", "Title", "");
  EXPECT_SEARCH_RESULT(result, 3, "/schemas/title-low", "Title", "");
  EXPECT_SEARCH_RESULT(result, 4, "/schemas/none-perfect", "", "");
}

TEST(Search_query, invalid_payload_too_small_for_header) {
  const std::vector<std::uint8_t> garbage{0x01, 0x02, 0x03};
  const auto result{sourcemeta::one::search(
      garbage.data(), garbage.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_header_claims_too_many_entries) {
  sourcemeta::one::SearchIndexHeader header{};
  header.entry_count = 1000;
  header.records_offset =
      sizeof(sourcemeta::one::SearchIndexHeader) + 1000 * sizeof(std::uint32_t);
  std::vector<std::uint8_t> payload(sizeof(sourcemeta::one::SearchIndexHeader));
  std::memcpy(payload.data(), &header,
              sizeof(sourcemeta::one::SearchIndexHeader));
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_offset_points_beyond_payload) {
  sourcemeta::one::SearchIndexHeader header{};
  header.entry_count = 1;
  header.records_offset =
      sizeof(sourcemeta::one::SearchIndexHeader) + sizeof(std::uint32_t);
  std::vector<std::uint8_t> payload(sizeof(sourcemeta::one::SearchIndexHeader) +
                                    sizeof(std::uint32_t));
  std::memcpy(payload.data(), &header,
              sizeof(sourcemeta::one::SearchIndexHeader));
  const std::uint32_t bad_offset{99999};
  std::memcpy(payload.data() + sizeof(sourcemeta::one::SearchIndexHeader),
              &bad_offset, sizeof(std::uint32_t));
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_record_field_lengths_exceed_payload) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "Title", "Desc", 80}};
  auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));

  sourcemeta::one::SearchRecordHeader bad_record{};
  bad_record.path_length = 60000;
  bad_record.title_length = 60000;
  bad_record.description_length = 60000;
  std::memcpy(payload.data() + header.records_offset, &bad_record,
              sizeof(sourcemeta::one::SearchRecordHeader));

  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_zero_entry_count) {
  sourcemeta::one::SearchIndexHeader header{};
  header.entry_count = 0;
  header.records_offset = sizeof(sourcemeta::one::SearchIndexHeader);
  std::vector<std::uint8_t> payload(sizeof(sourcemeta::one::SearchIndexHeader));
  std::memcpy(payload.data(), &header,
              sizeof(sourcemeta::one::SearchIndexHeader));
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_all_zeros) {
  const std::vector<std::uint8_t> payload(64, 0);
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_random_garbage) {
  const std::vector<std::uint8_t> payload{0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA,
                                          0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4,
                                          0xF3, 0xF2, 0xF1, 0xF0};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_truncated_after_header) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "Title", "Desc", 80}};
  const auto full_payload{sourcemeta::one::make_search(std::move(entries))};
  const auto truncated_size{sizeof(sourcemeta::one::SearchIndexHeader)};
  const auto result{sourcemeta::one::search(
      full_payload.data(), truncated_size, "foo", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_truncated_mid_record) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "Title", "Desc", 80}};
  const auto full_payload{sourcemeta::one::make_search(std::move(entries))};
  const auto truncated_size{sizeof(sourcemeta::one::SearchIndexHeader) +
                            sizeof(std::uint32_t) +
                            sizeof(sourcemeta::one::SearchRecordHeader) + 2};
  const auto result{sourcemeta::one::search(
      full_payload.data(), truncated_size, "foo", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, limit_1_returns_single_result) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "Alpha", "Desc", 100},
      {"/schemas/b", "Beta", "Desc", 90},
      {"/schemas/c", "Gamma", "Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 1,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "Alpha", "Desc");
}

TEST(Search_query, limit_2_returns_two_results) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "Alpha", "Desc", 100},
      {"/schemas/b", "Beta", "Desc", 90},
      {"/schemas/c", "Gamma", "Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 2,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "Beta", "Desc");
}

TEST(Search_query, limit_larger_than_matches_returns_all) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "Alpha", "Desc", 100}, {"/schemas/b", "Beta", "Desc", 90}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 100,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "Beta", "Desc");
}

TEST(Search_query, limit_0_returns_empty) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "Alpha", "Desc", 100}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 0,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, limit_exact_match_count) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "Alpha", "Desc", 100},
      {"/schemas/b", "Beta", "Desc", 90},
      {"/schemas/c", "Gamma", "Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 3,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "Beta", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/c", "Gamma", "Desc");
}

TEST(Search_query, limit_respects_health_ordering) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/low", "Low", "Desc", 20},
      {"/schemas/high", "High", "Desc", 100},
      {"/schemas/mid", "Mid", "Desc", 60}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 2,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/high", "High", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mid", "Mid", "Desc");
}

TEST(Search_query, scope_path_only_matches_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/unique/path", "Title", "Description", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(payload.data(), payload.size(),
                                            "unique", 10,
                                            sourcemeta::one::SearchScopePath)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/unique/path", "Title", "Description");
}

TEST(Search_query, scope_path_only_misses_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "UniqueTitle", "Description", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(payload.data(), payload.size(),
                                            "UniqueTitle", 10,
                                            sourcemeta::one::SearchScopePath)};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_path_only_misses_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Title", "UniqueDesc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(payload.data(), payload.size(),
                                            "UniqueDesc", 10,
                                            sourcemeta::one::SearchScopePath)};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_title_only_matches_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "UniqueTitle", "Description", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(payload.data(), payload.size(),
                                            "UniqueTitle", 10,
                                            sourcemeta::one::SearchScopeTitle)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "UniqueTitle", "Description");
}

TEST(Search_query, scope_title_only_misses_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/unique/path", "Title", "Description", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(payload.data(), payload.size(),
                                            "unique", 10,
                                            sourcemeta::one::SearchScopeTitle)};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_description_only_matches_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Title", "UniqueDesc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "UniqueDesc", 10,
                              sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "Title", "UniqueDesc");
}

TEST(Search_query, scope_description_only_misses_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/unique/path", "Title", "Description", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "unique", 10,
                              sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_path_and_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/xyz/path", "Needle In Title", "Other", 80},
      {"/needle/path", "Other", "Other", 80},
      {"/abc/path", "Other", "Needle In Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Needle", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/needle/path", "Other", "Other");
  EXPECT_SEARCH_RESULT(result, 1, "/xyz/path", "Needle In Title", "Other");
}

TEST(Search_query, scope_title_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/abc/path", "Needle In Title", "Other", 80},
      {"/def/path", "Other", "Needle In Desc", 80},
      {"/needle/path", "Other", "Other", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Needle", 10,
                              sourcemeta::one::SearchScopeTitle |
                                  sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/abc/path", "Needle In Title", "Other");
  EXPECT_SEARCH_RESULT(result, 1, "/def/path", "Other", "Needle In Desc");
}

TEST(Search_query, scope_path_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/needle/path", "Other", "Other", 80},
      {"/abc/path", "Needle In Title", "Other", 80},
      {"/def/path", "Other", "Needle In Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Needle", 10,
                              sourcemeta::one::SearchScopePath |
                                  sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/def/path", "Other", "Needle In Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/needle/path", "Other", "Other");
}

TEST(Search_query, scope_0_matches_nothing) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Title", "Description", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "foo", 10, 0)};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_all_matches_any_field) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/unique/path", "NormalTitle", "NormalDesc", 80},
      {"/normal/path", "UniqueTitle", "NormalDesc", 80},
      {"/normal/path2", "NormalTitle", "UniqueDesc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Unique", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/normal/path", "UniqueTitle", "NormalDesc");
  EXPECT_SEARCH_RESULT(result, 1, "/normal/path2", "NormalTitle", "UniqueDesc");
  EXPECT_SEARCH_RESULT(result, 2, "/unique/path", "NormalTitle", "NormalDesc");
}

TEST(Search_query, scope_combined_with_limit) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/a", "Match A", "Desc", 100},
      {"/b", "Match B", "Desc", 90},
      {"/c", "Match C", "Desc", 80}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(payload.data(), payload.size(),
                                            "Match", 2,
                                            sourcemeta::one::SearchScopeTitle)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/a", "Match A", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/b", "Match B", "Desc");
}
