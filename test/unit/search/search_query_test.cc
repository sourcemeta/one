#include <sourcemeta/one/search.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint32_t
#include <cstring>     // std::memcpy
#include <string>      // std::string
#include <string_view> // std::string_view_literals
#include <utility>     // std::move
#include <vector>      // std::vector

#define EXPECT_SEARCH_RESULT(result, index, expected_path,                     \
                             expected_identifier, expected_title,              \
                             expected_description)                             \
  EXPECT_EQ((result).at(index).at("path").to_string(), (expected_path));       \
  EXPECT_EQ((result).at(index).at("identifier").to_string(),                   \
            (expected_identifier));                                            \
  EXPECT_EQ((result).at(index).at("title").to_string(), (expected_title));     \
  EXPECT_EQ((result).at(index).at("description").to_string(),                  \
            (expected_description));

TEST(empty_payload_nullptr) {
  const auto result{sourcemeta::one::search(
      nullptr, 0, "anything", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(empty_payload_zero_size) {
  const std::uint8_t byte{0};
  const auto result{sourcemeta::one::search(
      &byte, 0, "anything", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(no_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "zzzzz", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(match_in_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "foo", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "Title", "Desc");
}

TEST(match_in_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Special Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Special", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "Special Title", "Desc");
}

TEST(match_in_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Title",
       .description = "Unique description here",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Unique", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "Title", "Unique description here");
}

TEST(case_insensitive) {
  std::vector<sourcemeta::one::SearchEntry> entries_lower{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Hello World",
       .description = "desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload_lower{
      sourcemeta::one::make_search(std::move(entries_lower))};
  const auto result_lower{sourcemeta::one::search(
      payload_lower.data(), payload_lower.size(), "hello", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result_lower.size(), 1);
  EXPECT_SEARCH_RESULT(result_lower, 0, "/foo/bar",
                       "http://example.com/foo/bar", "Hello World", "desc");

  std::vector<sourcemeta::one::SearchEntry> entries_upper{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Hello World",
       .description = "desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload_upper{
      sourcemeta::one::make_search(std::move(entries_upper))};
  const auto result_upper{sourcemeta::one::search(
      payload_upper.data(), payload_upper.size(), "HELLO", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result_upper.size(), 1);
  EXPECT_SEARCH_RESULT(result_upper, 0, "/foo/bar",
                       "http://example.com/foo/bar", "Hello World", "desc");

  std::vector<sourcemeta::one::SearchEntry> entries_mixed{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Hello World",
       .description = "desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload_mixed{
      sourcemeta::one::make_search(std::move(entries_mixed))};
  const auto result_mixed{sourcemeta::one::search(
      payload_mixed.data(), payload_mixed.size(), "hElLo", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result_mixed.size(), 1);
  EXPECT_SEARCH_RESULT(result_mixed, 0, "/foo/bar",
                       "http://example.com/foo/bar", "Hello World", "desc");
}

TEST(multiple_matches) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/address",
       .identifier = "http://example.com/schemas/address",
       .title = "Address Schema",
       .description = "For addresses",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/person",
       .identifier = "http://example.com/schemas/person",
       .title = "Person Schema",
       .description = "For people",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/email",
       .identifier = "http://example.com/schemas/email",
       .title = "Email Schema",
       .description = "For emails",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schema", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/address",
                       "http://example.com/schemas/address", "Address Schema",
                       "For addresses");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/email",
                       "http://example.com/schemas/email", "Email Schema",
                       "For emails");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/person",
                       "http://example.com/schemas/person", "Person Schema",
                       "For people");
}

TEST(limit_10) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/test0",
       .identifier = "http://example.com/schemas/test0",
       .title = "Test 0",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test1",
       .identifier = "http://example.com/schemas/test1",
       .title = "Test 1",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test2",
       .identifier = "http://example.com/schemas/test2",
       .title = "Test 2",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test3",
       .identifier = "http://example.com/schemas/test3",
       .title = "Test 3",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test4",
       .identifier = "http://example.com/schemas/test4",
       .title = "Test 4",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test5",
       .identifier = "http://example.com/schemas/test5",
       .title = "Test 5",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test6",
       .identifier = "http://example.com/schemas/test6",
       .title = "Test 6",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test7",
       .identifier = "http://example.com/schemas/test7",
       .title = "Test 7",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test8",
       .identifier = "http://example.com/schemas/test8",
       .title = "Test 8",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test9",
       .identifier = "http://example.com/schemas/test9",
       .title = "Test 9",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test10",
       .identifier = "http://example.com/schemas/test10",
       .title = "Test 10",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test11",
       .identifier = "http://example.com/schemas/test11",
       .title = "Test 11",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test12",
       .identifier = "http://example.com/schemas/test12",
       .title = "Test 12",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test13",
       .identifier = "http://example.com/schemas/test13",
       .title = "Test 13",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/test14",
       .identifier = "http://example.com/schemas/test14",
       .title = "Test 14",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};

  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 10);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/test0",
                       "http://example.com/schemas/test0", "Test 0", "");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/test1",
                       "http://example.com/schemas/test1", "Test 1", "");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/test10",
                       "http://example.com/schemas/test10", "Test 10", "");
  EXPECT_SEARCH_RESULT(result, 3, "/schemas/test11",
                       "http://example.com/schemas/test11", "Test 11", "");
  EXPECT_SEARCH_RESULT(result, 4, "/schemas/test12",
                       "http://example.com/schemas/test12", "Test 12", "");
  EXPECT_SEARCH_RESULT(result, 5, "/schemas/test13",
                       "http://example.com/schemas/test13", "Test 13", "");
  EXPECT_SEARCH_RESULT(result, 6, "/schemas/test14",
                       "http://example.com/schemas/test14", "Test 14", "");
  EXPECT_SEARCH_RESULT(result, 7, "/schemas/test2",
                       "http://example.com/schemas/test2", "Test 2", "");
  EXPECT_SEARCH_RESULT(result, 8, "/schemas/test3",
                       "http://example.com/schemas/test3", "Test 3", "");
  EXPECT_SEARCH_RESULT(result, 9, "/schemas/test4",
                       "http://example.com/schemas/test4", "Test 4", "");
}

TEST(round_trip_data_fidelity) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/a/b/c",
       .identifier = "http://example.com/a/b/c",
       .title = "My Title",
       .description = "My Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/x/y/z",
       .identifier = "http://example.com/x/y/z",
       .title = "",
       .description = "Only description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/p/q",
       .identifier = "http://example.com/p/q",
       .title = "Only title",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "/", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/a/b/c", "http://example.com/a/b/c",
                       "My Title", "My Description");
  EXPECT_SEARCH_RESULT(result, 1, "/p/q", "http://example.com/p/q",
                       "Only title", "");
  EXPECT_SEARCH_RESULT(result, 2, "/x/y/z", "http://example.com/x/y/z", "",
                       "Only description");
}

TEST(single_entry_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/only",
       .identifier = "http://example.com/only",
       .title = "One",
       .description = "Entry",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "One", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/only", "http://example.com/only", "One",
                       "Entry");
}

TEST(single_entry_no_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/only",
       .identifier = "http://example.com/only",
       .title = "One",
       .description = "Entry",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "nope", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 0);
}

TEST(empty_title_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/path/only",
       .identifier = "http://example.com/path/only",
       .title = "",
       .description = "",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "path", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/path/only", "http://example.com/path/only",
                       "", "");
}

TEST(health_higher_scores_first) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/low",
       .identifier = "http://example.com/schemas/low",
       .title = "Low Health",
       .description = "Desc",
       .health = 20,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/high",
       .identifier = "http://example.com/schemas/high",
       .title = "High Health",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/mid",
       .identifier = "http://example.com/schemas/mid",
       .title = "Mid Health",
       .description = "Desc",
       .health = 60,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Health", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/high",
                       "http://example.com/schemas/high", "High Health",
                       "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mid",
                       "http://example.com/schemas/mid", "Mid Health", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/low",
                       "http://example.com/schemas/low", "Low Health", "Desc");
}

TEST(health_100_before_50) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/beta",
       .identifier = "http://example.com/schemas/beta",
       .title = "Beta",
       .description = "Desc",
       .health = 50,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/alpha",
       .identifier = "http://example.com/schemas/alpha",
       .title = "Alpha",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/alpha",
                       "http://example.com/schemas/alpha", "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/beta",
                       "http://example.com/schemas/beta", "Beta", "Desc");
}

TEST(health_0_ranks_last) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/zero",
       .identifier = "http://example.com/schemas/zero",
       .title = "Zero",
       .description = "Desc",
       .health = 0,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/perfect",
       .identifier = "http://example.com/schemas/perfect",
       .title = "Perfect",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/okay",
       .identifier = "http://example.com/schemas/okay",
       .title = "Okay",
       .description = "Desc",
       .health = 50,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/perfect",
                       "http://example.com/schemas/perfect", "Perfect", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/okay",
                       "http://example.com/schemas/okay", "Okay", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/zero",
                       "http://example.com/schemas/zero", "Zero", "Desc");
}

TEST(health_same_score_sorts_by_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/zebra",
       .identifier = "http://example.com/schemas/zebra",
       .title = "Zebra",
       .description = "Desc",
       .health = 75,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/apple",
       .identifier = "http://example.com/schemas/apple",
       .title = "Apple",
       .description = "Desc",
       .health = 75,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/mango",
       .identifier = "http://example.com/schemas/mango",
       .title = "Mango",
       .description = "Desc",
       .health = 75,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/apple",
                       "http://example.com/schemas/apple", "Apple", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mango",
                       "http://example.com/schemas/mango", "Mango", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/zebra",
                       "http://example.com/schemas/zebra", "Zebra", "Desc");
}

TEST(metadata_score_beats_health) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/healthy",
       .identifier = "http://example.com/schemas/healthy",
       .title = "",
       .description = "",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/complete",
       .identifier = "http://example.com/schemas/complete",
       .title = "Title",
       .description = "Description",
       .health = 30,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/complete",
                       "http://example.com/schemas/complete", "Title",
                       "Description");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/healthy",
                       "http://example.com/schemas/healthy", "", "");
}

TEST(metadata_score_beats_health_title_only) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/no-meta",
       .identifier = "http://example.com/schemas/no-meta",
       .title = "",
       .description = "",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/has-title",
       .identifier = "http://example.com/schemas/has-title",
       .title = "A Title",
       .description = "",
       .health = 10,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/has-title",
                       "http://example.com/schemas/has-title", "A Title", "");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/no-meta",
                       "http://example.com/schemas/no-meta", "", "");
}

TEST(health_tiebreaker_within_same_metadata) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/low-health",
       .identifier = "http://example.com/schemas/low-health",
       .title = "Title",
       .description = "",
       .health = 25,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/high-health",
       .identifier = "http://example.com/schemas/high-health",
       .title = "Title",
       .description = "",
       .health = 90,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/mid-health",
       .identifier = "http://example.com/schemas/mid-health",
       .title = "Title",
       .description = "",
       .health = 50,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/high-health",
                       "http://example.com/schemas/high-health", "Title", "");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mid-health",
                       "http://example.com/schemas/mid-health", "Title", "");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/low-health",
                       "http://example.com/schemas/low-health", "Title", "");
}

TEST(health_fine_grained_ordering) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/d",
       .identifier = "http://example.com/schemas/d",
       .title = "Title",
       .description = "Desc",
       .health = 70,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/a",
       .identifier = "http://example.com/schemas/a",
       .title = "Title",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/c",
       .identifier = "http://example.com/schemas/c",
       .title = "Title",
       .description = "Desc",
       .health = 85,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/e",
       .identifier = "http://example.com/schemas/e",
       .title = "Title",
       .description = "Desc",
       .health = 55,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/b",
       .identifier = "http://example.com/schemas/b",
       .title = "Title",
       .description = "Desc",
       .health = 95,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 5);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "http://example.com/schemas/a",
                       "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "http://example.com/schemas/b",
                       "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/c", "http://example.com/schemas/c",
                       "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 3, "/schemas/d", "http://example.com/schemas/d",
                       "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 4, "/schemas/e", "http://example.com/schemas/e",
                       "Title", "Desc");
}

TEST(health_mixed_metadata_and_health) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/full-low",
       .identifier = "http://example.com/schemas/full-low",
       .title = "Title",
       .description = "Desc",
       .health = 30,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/title-high",
       .identifier = "http://example.com/schemas/title-high",
       .title = "Title",
       .description = "",
       .health = 95,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/full-high",
       .identifier = "http://example.com/schemas/full-high",
       .title = "Title",
       .description = "Desc",
       .health = 90,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/none-perfect",
       .identifier = "http://example.com/schemas/none-perfect",
       .title = "",
       .description = "",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/title-low",
       .identifier = "http://example.com/schemas/title-low",
       .title = "Title",
       .description = "",
       .health = 40,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 5);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/full-high",
                       "http://example.com/schemas/full-high", "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/full-low",
                       "http://example.com/schemas/full-low", "Title", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/title-high",
                       "http://example.com/schemas/title-high", "Title", "");
  EXPECT_SEARCH_RESULT(result, 3, "/schemas/title-low",
                       "http://example.com/schemas/title-low", "Title", "");
  EXPECT_SEARCH_RESULT(result, 4, "/schemas/none-perfect",
                       "http://example.com/schemas/none-perfect", "", "");
}

TEST(invalid_payload_too_small_for_header) {
  const std::vector<std::uint8_t> garbage{0x01, 0x02, 0x03};
  const auto result{sourcemeta::one::search(
      garbage.data(), garbage.size(), "test", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(invalid_payload_header_claims_too_many_entries) {
  sourcemeta::one::SearchIndexHeader header{};
  header.entry_count = 1000;
  header.records_offset = sizeof(sourcemeta::one::SearchIndexHeader) +
                          (1000 * sizeof(std::uint32_t));
  std::vector<std::uint8_t> payload(sizeof(sourcemeta::one::SearchIndexHeader));
  std::memcpy(payload.data(), &header,
              sizeof(sourcemeta::one::SearchIndexHeader));
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(invalid_payload_offset_points_beyond_payload) {
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
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(invalid_payload_record_field_lengths_exceed_payload) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo",
       .identifier = "http://example.com/foo",
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  auto payload{sourcemeta::one::make_search(std::move(entries))};

  sourcemeta::one::SearchIndexHeader header{};
  std::memcpy(&header, payload.data(),
              sizeof(sourcemeta::one::SearchIndexHeader));

  sourcemeta::one::SearchRecordHeader bad_record{};
  bad_record.path_length = 60000;
  bad_record.identifier_length = 60000;
  bad_record.title_length = 60000;
  bad_record.description_length = 60000;
  std::memcpy(payload.data() + header.records_offset, &bad_record,
              sizeof(sourcemeta::one::SearchRecordHeader));

  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(invalid_payload_zero_entry_count) {
  sourcemeta::one::SearchIndexHeader header{};
  header.entry_count = 0;
  header.records_offset = sizeof(sourcemeta::one::SearchIndexHeader);
  std::vector<std::uint8_t> payload(sizeof(sourcemeta::one::SearchIndexHeader));
  std::memcpy(payload.data(), &header,
              sizeof(sourcemeta::one::SearchIndexHeader));
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(invalid_payload_all_zeros) {
  const std::vector<std::uint8_t> payload(64, 0);
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(invalid_payload_random_garbage) {
  const std::vector<std::uint8_t> payload{0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA,
                                          0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4,
                                          0xF3, 0xF2, 0xF1, 0xF0};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(invalid_payload_truncated_after_header) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo",
       .identifier = "http://example.com/foo",
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto full_payload{sourcemeta::one::make_search(std::move(entries))};
  const auto truncated_size{sizeof(sourcemeta::one::SearchIndexHeader)};
  const auto result{sourcemeta::one::search(
      full_payload.data(), truncated_size, "foo", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(invalid_payload_truncated_mid_record) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo",
       .identifier = "http://example.com/foo",
       .title = "Title",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto full_payload{sourcemeta::one::make_search(std::move(entries))};
  const auto truncated_size{sizeof(sourcemeta::one::SearchIndexHeader) +
                            sizeof(std::uint32_t) +
                            sizeof(sourcemeta::one::SearchRecordHeader) + 2};
  const auto result{sourcemeta::one::search(
      full_payload.data(), truncated_size, "foo", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(limit_1_returns_single_result) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/a",
       .identifier = "http://example.com/schemas/a",
       .title = "Alpha",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/b",
       .identifier = "http://example.com/schemas/b",
       .title = "Beta",
       .description = "Desc",
       .health = 90,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/c",
       .identifier = "http://example.com/schemas/c",
       .title = "Gamma",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 1,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "http://example.com/schemas/a",
                       "Alpha", "Desc");
}

TEST(limit_2_returns_two_results) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/a",
       .identifier = "http://example.com/schemas/a",
       .title = "Alpha",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/b",
       .identifier = "http://example.com/schemas/b",
       .title = "Beta",
       .description = "Desc",
       .health = 90,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/c",
       .identifier = "http://example.com/schemas/c",
       .title = "Gamma",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 2,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "http://example.com/schemas/a",
                       "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "http://example.com/schemas/b",
                       "Beta", "Desc");
}

TEST(limit_larger_than_matches_returns_all) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/a",
       .identifier = "http://example.com/schemas/a",
       .title = "Alpha",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/b",
       .identifier = "http://example.com/schemas/b",
       .title = "Beta",
       .description = "Desc",
       .health = 90,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 100,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "http://example.com/schemas/a",
                       "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "http://example.com/schemas/b",
                       "Beta", "Desc");
}

TEST(limit_0_returns_empty) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/a",
       .identifier = "http://example.com/schemas/a",
       .title = "Alpha",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 0,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 0);
}

TEST(limit_exact_match_count) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/a",
       .identifier = "http://example.com/schemas/a",
       .title = "Alpha",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/b",
       .identifier = "http://example.com/schemas/b",
       .title = "Beta",
       .description = "Desc",
       .health = 90,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/c",
       .identifier = "http://example.com/schemas/c",
       .title = "Gamma",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 3,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "http://example.com/schemas/a",
                       "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "http://example.com/schemas/b",
                       "Beta", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/c", "http://example.com/schemas/c",
                       "Gamma", "Desc");
}

TEST(limit_respects_health_ordering) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/low",
       .identifier = "http://example.com/schemas/low",
       .title = "Low",
       .description = "Desc",
       .health = 20,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/high",
       .identifier = "http://example.com/schemas/high",
       .title = "High",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/schemas/mid",
       .identifier = "http://example.com/schemas/mid",
       .title = "Mid",
       .description = "Desc",
       .health = 60,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 2,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/high",
                       "http://example.com/schemas/high", "High", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mid",
                       "http://example.com/schemas/mid", "Mid", "Desc");
}

TEST(scope_path_only_matches_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/unique/path",
       .identifier = "http://example.com/unique/path",
       .title = "Title",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "unique", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/unique/path",
                       "http://example.com/unique/path", "Title",
                       "Description");
}

TEST(scope_path_only_misses_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "UniqueTitle",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "UniqueTitle", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH)};
  EXPECT_EQ(result.size(), 0);
}

TEST(scope_path_only_misses_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Title",
       .description = "UniqueDesc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "UniqueDesc", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH)};
  EXPECT_EQ(result.size(), 0);
}

TEST(scope_title_only_matches_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "UniqueTitle",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "UniqueTitle", 10,
                              sourcemeta::one::SEARCH_SCOPE_TITLE)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "UniqueTitle", "Description");
}

TEST(scope_title_only_misses_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/unique/path",
       .identifier = "http://example.com/unique/path",
       .title = "Title",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "unique", 10,
                              sourcemeta::one::SEARCH_SCOPE_TITLE)};
  EXPECT_EQ(result.size(), 0);
}

TEST(scope_description_only_matches_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Title",
       .description = "UniqueDesc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "UniqueDesc", 10,
                              sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "Title", "UniqueDesc");
}

TEST(scope_description_only_misses_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/unique/path",
       .identifier = "http://example.com/unique/path",
       .title = "Title",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "unique", 10,
                              sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 0);
}

TEST(scope_path_and_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/xyz/path",
       .identifier = "http://example.com/xyz/path",
       .title = "Needle In Title",
       .description = "Other",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/needle/path",
       .identifier = "http://example.com/needle/path",
       .title = "Other",
       .description = "Other",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/abc/path",
       .identifier = "http://example.com/abc/path",
       .title = "Other",
       .description = "Needle In Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Needle", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH |
                                  sourcemeta::one::SEARCH_SCOPE_TITLE)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/needle/path",
                       "http://example.com/needle/path", "Other", "Other");
  EXPECT_SEARCH_RESULT(result, 1, "/xyz/path", "http://example.com/xyz/path",
                       "Needle In Title", "Other");
}

TEST(scope_title_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/abc/path",
       .identifier = "http://example.com/abc/path",
       .title = "Needle In Title",
       .description = "Other",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/def/path",
       .identifier = "http://example.com/def/path",
       .title = "Other",
       .description = "Needle In Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/needle/path",
       .identifier = "http://example.com/needle/path",
       .title = "Other",
       .description = "Other",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Needle", 10,
                              sourcemeta::one::SEARCH_SCOPE_TITLE |
                                  sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/abc/path", "http://example.com/abc/path",
                       "Needle In Title", "Other");
  EXPECT_SEARCH_RESULT(result, 1, "/def/path", "http://example.com/def/path",
                       "Other", "Needle In Desc");
}

TEST(scope_path_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/needle/path",
       .identifier = "http://example.com/needle/path",
       .title = "Other",
       .description = "Other",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/abc/path",
       .identifier = "http://example.com/abc/path",
       .title = "Needle In Title",
       .description = "Other",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/def/path",
       .identifier = "http://example.com/def/path",
       .title = "Other",
       .description = "Needle In Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Needle", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH |
                                  sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/def/path", "http://example.com/def/path",
                       "Other", "Needle In Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/needle/path",
                       "http://example.com/needle/path", "Other", "Other");
}

TEST(scope_0_matches_nothing) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/foo/bar",
       .identifier = "http://example.com/foo/bar",
       .title = "Title",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "foo", 10, 0)};
  EXPECT_EQ(result.size(), 0);
}

TEST(scope_all_matches_any_field) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/unique/path",
       .identifier = "http://example.com/unique/path",
       .title = "NormalTitle",
       .description = "NormalDesc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/normal/path",
       .identifier = "http://example.com/normal/path",
       .title = "UniqueTitle",
       .description = "NormalDesc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/normal/path2",
       .identifier = "http://example.com/normal/path2",
       .title = "NormalTitle",
       .description = "UniqueDesc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Unique", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/normal/path",
                       "http://example.com/normal/path", "UniqueTitle",
                       "NormalDesc");
  EXPECT_SEARCH_RESULT(result, 1, "/normal/path2",
                       "http://example.com/normal/path2", "NormalTitle",
                       "UniqueDesc");
  EXPECT_SEARCH_RESULT(result, 2, "/unique/path",
                       "http://example.com/unique/path", "NormalTitle",
                       "NormalDesc");
}

TEST(scope_combined_with_limit) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/a",
       .identifier = "http://example.com/a",
       .title = "Match A",
       .description = "Desc",
       .health = 100,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/b",
       .identifier = "http://example.com/b",
       .title = "Match B",
       .description = "Desc",
       .health = 90,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0},
      {.path = "/c",
       .identifier = "http://example.com/c",
       .title = "Match C",
       .description = "Desc",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Match", 2,
                              sourcemeta::one::SEARCH_SCOPE_TITLE)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/a", "http://example.com/a", "Match A",
                       "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/b", "http://example.com/b", "Match B",
                       "Desc");
}

TEST(query_with_embedded_null_does_not_match) {
  using namespace std::string_view_literals;
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/test",
       .identifier = "http://example.com/schemas/test",
       .title = "Title",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "sche\0mas"sv, 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 0);
}

TEST(query_with_tab_does_not_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/test",
       .identifier = "http://example.com/schemas/test",
       .title = "Title",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas\ttest", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 0);
}

TEST(query_with_newline_does_not_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/test",
       .identifier = "http://example.com/schemas/test",
       .title = "Title",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas\ntest", 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 0);
}

TEST(entry_with_null_in_path_found_by_other_content) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = std::string("before\0after", 12),
       .identifier = "http://example.com/null-path",
       .title = "Title",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "after", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH)};
  EXPECT_EQ(result.size(), 1);
}

TEST(entry_with_null_in_title_found_by_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/test",
       .identifier = "http://example.com/schemas/test",
       .title = std::string("Foo\0Bar", 7),
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "schemas", 10,
                              sourcemeta::one::SEARCH_SCOPE_PATH)};
  EXPECT_EQ(result.size(), 1);
}

TEST(query_only_null_bytes_matches_nothing) {
  using namespace std::string_view_literals;
  std::vector<sourcemeta::one::SearchEntry> entries{
      {.path = "/schemas/test",
       .identifier = "http://example.com/schemas/test",
       .title = "Title",
       .description = "Description",
       .health = 80,
       .priority = 100,
       .bytes_raw = 0,
       .bytes_bundled = 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "\0\0\0"sv, 10,
      sourcemeta::one::SEARCH_SCOPE_PATH | sourcemeta::one::SEARCH_SCOPE_TITLE |
          sourcemeta::one::SEARCH_SCOPE_DESCRIPTION)};
  EXPECT_EQ(result.size(), 0);
}
