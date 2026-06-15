#include <sourcemeta/one/search.h>

#include <sourcemeta/core/json.h>

#include <gtest/gtest.h>

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

TEST(Search_query, empty_payload_nullptr) {
  const auto result{sourcemeta::one::search(
      nullptr, 0, "anything", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, empty_payload_zero_size) {
  const std::uint8_t byte{0};
  const auto result{sourcemeta::one::search(
      &byte, 0, "anything", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, no_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "Title", "Desc", 80, 100, 0,
       0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "zzzzz", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, match_in_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "Title", "Desc", 80, 100, 0,
       0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "foo", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "Title", "Desc");
}

TEST(Search_query, match_in_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "Special Title", "Desc", 80,
       100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Special", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "Special Title", "Desc");
}

TEST(Search_query, match_in_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "Title",
       "Unique description here", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Unique", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "Title", "Unique description here");
}

TEST(Search_query, case_insensitive) {
  std::vector<sourcemeta::one::SearchEntry> entries_lower{
      {"/foo/bar", "http://example.com/foo/bar", "Hello World", "desc", 80, 100,
       0, 0}};
  const auto payload_lower{
      sourcemeta::one::make_search(std::move(entries_lower))};
  const auto result_lower{sourcemeta::one::search(
      payload_lower.data(), payload_lower.size(), "hello", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result_lower.size(), 1);
  EXPECT_SEARCH_RESULT(result_lower, 0, "/foo/bar",
                       "http://example.com/foo/bar", "Hello World", "desc");

  std::vector<sourcemeta::one::SearchEntry> entries_upper{
      {"/foo/bar", "http://example.com/foo/bar", "Hello World", "desc", 80, 100,
       0, 0}};
  const auto payload_upper{
      sourcemeta::one::make_search(std::move(entries_upper))};
  const auto result_upper{sourcemeta::one::search(
      payload_upper.data(), payload_upper.size(), "HELLO", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result_upper.size(), 1);
  EXPECT_SEARCH_RESULT(result_upper, 0, "/foo/bar",
                       "http://example.com/foo/bar", "Hello World", "desc");

  std::vector<sourcemeta::one::SearchEntry> entries_mixed{
      {"/foo/bar", "http://example.com/foo/bar", "Hello World", "desc", 80, 100,
       0, 0}};
  const auto payload_mixed{
      sourcemeta::one::make_search(std::move(entries_mixed))};
  const auto result_mixed{sourcemeta::one::search(
      payload_mixed.data(), payload_mixed.size(), "hElLo", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result_mixed.size(), 1);
  EXPECT_SEARCH_RESULT(result_mixed, 0, "/foo/bar",
                       "http://example.com/foo/bar", "Hello World", "desc");
}

TEST(Search_query, multiple_matches) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/address", "http://example.com/schemas/address",
       "Address Schema", "For addresses", 80, 100, 0, 0},
      {"/schemas/person", "http://example.com/schemas/person", "Person Schema",
       "For people", 80, 100, 0, 0},
      {"/schemas/email", "http://example.com/schemas/email", "Email Schema",
       "For emails", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schema", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
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

TEST(Search_query, limit_10) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/test0", "http://example.com/schemas/test0", "Test 0", "", 80,
       100, 0, 0},
      {"/schemas/test1", "http://example.com/schemas/test1", "Test 1", "", 80,
       100, 0, 0},
      {"/schemas/test2", "http://example.com/schemas/test2", "Test 2", "", 80,
       100, 0, 0},
      {"/schemas/test3", "http://example.com/schemas/test3", "Test 3", "", 80,
       100, 0, 0},
      {"/schemas/test4", "http://example.com/schemas/test4", "Test 4", "", 80,
       100, 0, 0},
      {"/schemas/test5", "http://example.com/schemas/test5", "Test 5", "", 80,
       100, 0, 0},
      {"/schemas/test6", "http://example.com/schemas/test6", "Test 6", "", 80,
       100, 0, 0},
      {"/schemas/test7", "http://example.com/schemas/test7", "Test 7", "", 80,
       100, 0, 0},
      {"/schemas/test8", "http://example.com/schemas/test8", "Test 8", "", 80,
       100, 0, 0},
      {"/schemas/test9", "http://example.com/schemas/test9", "Test 9", "", 80,
       100, 0, 0},
      {"/schemas/test10", "http://example.com/schemas/test10", "Test 10", "",
       80, 100, 0, 0},
      {"/schemas/test11", "http://example.com/schemas/test11", "Test 11", "",
       80, 100, 0, 0},
      {"/schemas/test12", "http://example.com/schemas/test12", "Test 12", "",
       80, 100, 0, 0},
      {"/schemas/test13", "http://example.com/schemas/test13", "Test 13", "",
       80, 100, 0, 0},
      {"/schemas/test14", "http://example.com/schemas/test14", "Test 14", "",
       80, 100, 0, 0}};

  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
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

TEST(Search_query, round_trip_data_fidelity) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/a/b/c", "http://example.com/a/b/c", "My Title", "My Description", 80,
       100, 0, 0},
      {"/x/y/z", "http://example.com/x/y/z", "", "Only description", 80, 100, 0,
       0},
      {"/p/q", "http://example.com/p/q", "Only title", "", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "/", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/a/b/c", "http://example.com/a/b/c",
                       "My Title", "My Description");
  EXPECT_SEARCH_RESULT(result, 1, "/p/q", "http://example.com/p/q",
                       "Only title", "");
  EXPECT_SEARCH_RESULT(result, 2, "/x/y/z", "http://example.com/x/y/z", "",
                       "Only description");
}

TEST(Search_query, single_entry_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/only", "http://example.com/only", "One", "Entry", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "One", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/only", "http://example.com/only", "One",
                       "Entry");
}

TEST(Search_query, single_entry_no_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/only", "http://example.com/only", "One", "Entry", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "nope", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, empty_title_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/path/only", "http://example.com/path/only", "", "", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "path", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/path/only", "http://example.com/path/only",
                       "", "");
}

TEST(Search_query, health_higher_scores_first) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/low", "http://example.com/schemas/low", "Low Health", "Desc",
       20, 100, 0, 0},
      {"/schemas/high", "http://example.com/schemas/high", "High Health",
       "Desc", 100, 100, 0, 0},
      {"/schemas/mid", "http://example.com/schemas/mid", "Mid Health", "Desc",
       60, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Health", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/high",
                       "http://example.com/schemas/high", "High Health",
                       "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mid",
                       "http://example.com/schemas/mid", "Mid Health", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/low",
                       "http://example.com/schemas/low", "Low Health", "Desc");
}

TEST(Search_query, health_100_before_50) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/beta", "http://example.com/schemas/beta", "Beta", "Desc", 50,
       100, 0, 0},
      {"/schemas/alpha", "http://example.com/schemas/alpha", "Alpha", "Desc",
       100, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/alpha",
                       "http://example.com/schemas/alpha", "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/beta",
                       "http://example.com/schemas/beta", "Beta", "Desc");
}

TEST(Search_query, health_0_ranks_last) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/zero", "http://example.com/schemas/zero", "Zero", "Desc", 0,
       100, 0, 0},
      {"/schemas/perfect", "http://example.com/schemas/perfect", "Perfect",
       "Desc", 100, 100, 0, 0},
      {"/schemas/okay", "http://example.com/schemas/okay", "Okay", "Desc", 50,
       100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/perfect",
                       "http://example.com/schemas/perfect", "Perfect", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/okay",
                       "http://example.com/schemas/okay", "Okay", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/zero",
                       "http://example.com/schemas/zero", "Zero", "Desc");
}

TEST(Search_query, health_same_score_sorts_by_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/zebra", "http://example.com/schemas/zebra", "Zebra", "Desc",
       75, 100, 0, 0},
      {"/schemas/apple", "http://example.com/schemas/apple", "Apple", "Desc",
       75, 100, 0, 0},
      {"/schemas/mango", "http://example.com/schemas/mango", "Mango", "Desc",
       75, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/apple",
                       "http://example.com/schemas/apple", "Apple", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mango",
                       "http://example.com/schemas/mango", "Mango", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/zebra",
                       "http://example.com/schemas/zebra", "Zebra", "Desc");
}

TEST(Search_query, metadata_score_beats_health) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/healthy", "http://example.com/schemas/healthy", "", "", 100,
       100, 0, 0},
      {"/schemas/complete", "http://example.com/schemas/complete", "Title",
       "Description", 30, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/complete",
                       "http://example.com/schemas/complete", "Title",
                       "Description");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/healthy",
                       "http://example.com/schemas/healthy", "", "");
}

TEST(Search_query, metadata_score_beats_health_title_only) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/no-meta", "http://example.com/schemas/no-meta", "", "", 100,
       100, 0, 0},
      {"/schemas/has-title", "http://example.com/schemas/has-title", "A Title",
       "", 10, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/has-title",
                       "http://example.com/schemas/has-title", "A Title", "");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/no-meta",
                       "http://example.com/schemas/no-meta", "", "");
}

TEST(Search_query, health_tiebreaker_within_same_metadata) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/low-health", "http://example.com/schemas/low-health", "Title",
       "", 25, 100, 0, 0},
      {"/schemas/high-health", "http://example.com/schemas/high-health",
       "Title", "", 90, 100, 0, 0},
      {"/schemas/mid-health", "http://example.com/schemas/mid-health", "Title",
       "", 50, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/high-health",
                       "http://example.com/schemas/high-health", "Title", "");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mid-health",
                       "http://example.com/schemas/mid-health", "Title", "");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/low-health",
                       "http://example.com/schemas/low-health", "Title", "");
}

TEST(Search_query, health_fine_grained_ordering) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/d", "http://example.com/schemas/d", "Title", "Desc", 70, 100,
       0, 0},
      {"/schemas/a", "http://example.com/schemas/a", "Title", "Desc", 100, 100,
       0, 0},
      {"/schemas/c", "http://example.com/schemas/c", "Title", "Desc", 85, 100,
       0, 0},
      {"/schemas/e", "http://example.com/schemas/e", "Title", "Desc", 55, 100,
       0, 0},
      {"/schemas/b", "http://example.com/schemas/b", "Title", "Desc", 95, 100,
       0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
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

TEST(Search_query, health_mixed_metadata_and_health) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/full-low", "http://example.com/schemas/full-low", "Title",
       "Desc", 30, 100, 0, 0},
      {"/schemas/title-high", "http://example.com/schemas/title-high", "Title",
       "", 95, 100, 0, 0},
      {"/schemas/full-high", "http://example.com/schemas/full-high", "Title",
       "Desc", 90, 100, 0, 0},
      {"/schemas/none-perfect", "http://example.com/schemas/none-perfect", "",
       "", 100, 100, 0, 0},
      {"/schemas/title-low", "http://example.com/schemas/title-low", "Title",
       "", 40, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
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

TEST(Search_query, invalid_payload_too_small_for_header) {
  const std::vector<std::uint8_t> garbage{0x01, 0x02, 0x03};
  const auto result{sourcemeta::one::search(
      garbage.data(), garbage.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
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
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
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
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_record_field_lengths_exceed_payload) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "http://example.com/foo", "Title", "Desc", 80, 100, 0, 0}};
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
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
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
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_all_zeros) {
  const std::vector<std::uint8_t> payload(64, 0);
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "test", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
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
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_truncated_after_header) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "http://example.com/foo", "Title", "Desc", 80, 100, 0, 0}};
  const auto full_payload{sourcemeta::one::make_search(std::move(entries))};
  const auto truncated_size{sizeof(sourcemeta::one::SearchIndexHeader)};
  const auto result{sourcemeta::one::search(
      full_payload.data(), truncated_size, "foo", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, invalid_payload_truncated_mid_record) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo", "http://example.com/foo", "Title", "Desc", 80, 100, 0, 0}};
  const auto full_payload{sourcemeta::one::make_search(std::move(entries))};
  const auto truncated_size{sizeof(sourcemeta::one::SearchIndexHeader) +
                            sizeof(std::uint32_t) +
                            sizeof(sourcemeta::one::SearchRecordHeader) + 2};
  const auto result{sourcemeta::one::search(
      full_payload.data(), truncated_size, "foo", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, limit_1_returns_single_result) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "http://example.com/schemas/a", "Alpha", "Desc", 100, 100,
       0, 0},
      {"/schemas/b", "http://example.com/schemas/b", "Beta", "Desc", 90, 100, 0,
       0},
      {"/schemas/c", "http://example.com/schemas/c", "Gamma", "Desc", 80, 100,
       0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 1,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "http://example.com/schemas/a",
                       "Alpha", "Desc");
}

TEST(Search_query, limit_2_returns_two_results) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "http://example.com/schemas/a", "Alpha", "Desc", 100, 100,
       0, 0},
      {"/schemas/b", "http://example.com/schemas/b", "Beta", "Desc", 90, 100, 0,
       0},
      {"/schemas/c", "http://example.com/schemas/c", "Gamma", "Desc", 80, 100,
       0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 2,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "http://example.com/schemas/a",
                       "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "http://example.com/schemas/b",
                       "Beta", "Desc");
}

TEST(Search_query, limit_larger_than_matches_returns_all) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "http://example.com/schemas/a", "Alpha", "Desc", 100, 100,
       0, 0},
      {"/schemas/b", "http://example.com/schemas/b", "Beta", "Desc", 90, 100, 0,
       0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 100,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "http://example.com/schemas/a",
                       "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "http://example.com/schemas/b",
                       "Beta", "Desc");
}

TEST(Search_query, limit_0_returns_empty) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "http://example.com/schemas/a", "Alpha", "Desc", 100, 100,
       0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 0,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, limit_exact_match_count) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "http://example.com/schemas/a", "Alpha", "Desc", 100, 100,
       0, 0},
      {"/schemas/b", "http://example.com/schemas/b", "Beta", "Desc", 90, 100, 0,
       0},
      {"/schemas/c", "http://example.com/schemas/c", "Gamma", "Desc", 80, 100,
       0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 3,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 3);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/a", "http://example.com/schemas/a",
                       "Alpha", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/b", "http://example.com/schemas/b",
                       "Beta", "Desc");
  EXPECT_SEARCH_RESULT(result, 2, "/schemas/c", "http://example.com/schemas/c",
                       "Gamma", "Desc");
}

TEST(Search_query, limit_respects_health_ordering) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/low", "http://example.com/schemas/low", "Low", "Desc", 20, 100,
       0, 0},
      {"/schemas/high", "http://example.com/schemas/high", "High", "Desc", 100,
       100, 0, 0},
      {"/schemas/mid", "http://example.com/schemas/mid", "Mid", "Desc", 60, 100,
       0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 2,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/high",
                       "http://example.com/schemas/high", "High", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/mid",
                       "http://example.com/schemas/mid", "Mid", "Desc");
}

TEST(Search_query, scope_path_only_matches_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/unique/path", "http://example.com/unique/path", "Title", "Description",
       80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "unique", 10,
                              sourcemeta::one::SearchScopePath,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/unique/path",
                       "http://example.com/unique/path", "Title",
                       "Description");
}

TEST(Search_query, scope_path_only_misses_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "UniqueTitle", "Description",
       80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "UniqueTitle", 10,
                              sourcemeta::one::SearchScopePath,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_path_only_misses_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "Title", "UniqueDesc", 80, 100,
       0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "UniqueDesc", 10,
                              sourcemeta::one::SearchScopePath,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_title_only_matches_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "UniqueTitle", "Description",
       80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "UniqueTitle", 10,
                              sourcemeta::one::SearchScopeTitle,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "UniqueTitle", "Description");
}

TEST(Search_query, scope_title_only_misses_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/unique/path", "http://example.com/unique/path", "Title", "Description",
       80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "unique", 10,
                              sourcemeta::one::SearchScopeTitle,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_description_only_matches_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "Title", "UniqueDesc", 80, 100,
       0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "UniqueDesc", 10,
                              sourcemeta::one::SearchScopeDescription,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/foo/bar", "http://example.com/foo/bar",
                       "Title", "UniqueDesc");
}

TEST(Search_query, scope_description_only_misses_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/unique/path", "http://example.com/unique/path", "Title", "Description",
       80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "unique", 10,
                              sourcemeta::one::SearchScopeDescription,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_path_and_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/xyz/path", "http://example.com/xyz/path", "Needle In Title", "Other",
       80, 100, 0, 0},
      {"/needle/path", "http://example.com/needle/path", "Other", "Other", 80,
       100, 0, 0},
      {"/abc/path", "http://example.com/abc/path", "Other", "Needle In Desc",
       80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Needle", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/needle/path",
                       "http://example.com/needle/path", "Other", "Other");
  EXPECT_SEARCH_RESULT(result, 1, "/xyz/path", "http://example.com/xyz/path",
                       "Needle In Title", "Other");
}

TEST(Search_query, scope_title_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/abc/path", "http://example.com/abc/path", "Needle In Title", "Other",
       80, 100, 0, 0},
      {"/def/path", "http://example.com/def/path", "Other", "Needle In Desc",
       80, 100, 0, 0},
      {"/needle/path", "http://example.com/needle/path", "Other", "Other", 80,
       100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Needle", 10,
                              sourcemeta::one::SearchScopeTitle |
                                  sourcemeta::one::SearchScopeDescription,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/abc/path", "http://example.com/abc/path",
                       "Needle In Title", "Other");
  EXPECT_SEARCH_RESULT(result, 1, "/def/path", "http://example.com/def/path",
                       "Other", "Needle In Desc");
}

TEST(Search_query, scope_path_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/needle/path", "http://example.com/needle/path", "Other", "Other", 80,
       100, 0, 0},
      {"/abc/path", "http://example.com/abc/path", "Needle In Title", "Other",
       80, 100, 0, 0},
      {"/def/path", "http://example.com/def/path", "Other", "Needle In Desc",
       80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Needle", 10,
                              sourcemeta::one::SearchScopePath |
                                  sourcemeta::one::SearchScopeDescription,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/def/path", "http://example.com/def/path",
                       "Other", "Needle In Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/needle/path",
                       "http://example.com/needle/path", "Other", "Other");
}

TEST(Search_query, scope_0_matches_nothing) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "http://example.com/foo/bar", "Title", "Description", 80,
       100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "foo", 10, 0,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, scope_all_matches_any_field) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/unique/path", "http://example.com/unique/path", "NormalTitle",
       "NormalDesc", 80, 100, 0, 0},
      {"/normal/path", "http://example.com/normal/path", "UniqueTitle",
       "NormalDesc", 80, 100, 0, 0},
      {"/normal/path2", "http://example.com/normal/path2", "NormalTitle",
       "UniqueDesc", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "Unique", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
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

TEST(Search_query, scope_combined_with_limit) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/a", "http://example.com/a", "Match A", "Desc", 100, 100, 0, 0},
      {"/b", "http://example.com/b", "Match B", "Desc", 90, 100, 0, 0},
      {"/c", "http://example.com/c", "Match C", "Desc", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Match", 2,
                              sourcemeta::one::SearchScopeTitle,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/a", "http://example.com/a", "Match A",
                       "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/b", "http://example.com/b", "Match B",
                       "Desc");
}

TEST(Search_query, query_with_embedded_null_does_not_match) {
  using namespace std::string_view_literals;
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/test", "http://example.com/schemas/test", "Title",
       "Description", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "sche\0mas"sv, 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, query_with_tab_does_not_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/test", "http://example.com/schemas/test", "Title",
       "Description", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas\ttest", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, query_with_newline_does_not_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/test", "http://example.com/schemas/test", "Title",
       "Description", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas\ntest", 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, entry_with_null_in_path_found_by_other_content) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {std::string("before\0after", 12), "http://example.com/null-path",
       "Title", "Description", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "after", 10,
                              sourcemeta::one::SearchScopePath,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
}

TEST(Search_query, entry_with_null_in_title_found_by_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/test", "http://example.com/schemas/test",
       std::string("Foo\0Bar", 7), "Description", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "schemas", 10,
                              sourcemeta::one::SearchScopePath,
                              [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 1);
}

TEST(Search_query, query_only_null_bytes_matches_nothing) {
  using namespace std::string_view_literals;
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/test", "http://example.com/schemas/test", "Title",
       "Description", 80, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "\0\0\0"sv, 10,
      sourcemeta::one::SearchScopePath | sourcemeta::one::SearchScopeTitle |
          sourcemeta::one::SearchScopeDescription,
      [](const std::string_view) { return true; })};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search_query, filter_excludes_rejected_paths) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/public", "http://example.com/schemas/public", "Public", "Desc",
       100, 100, 0, 0},
      {"/schemas/private", "http://example.com/schemas/private", "Private",
       "Desc", 90, 100, 0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 10,
      sourcemeta::one::SearchScopePath,
      [](const std::string_view path) { return path != "/schemas/private"; })};
  EXPECT_EQ(result.size(), 1);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/public",
                       "http://example.com/schemas/public", "Public", "Desc");
}

TEST(Search_query, filter_rejecting_all_returns_empty) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "http://example.com/schemas/a", "Alpha", "Desc", 100, 100,
       0, 0},
      {"/schemas/b", "http://example.com/schemas/b", "Beta", "Desc", 90, 100, 0,
       0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "schemas", 10,
                              sourcemeta::one::SearchScopePath,
                              [](const std::string_view) { return false; })};
  EXPECT_EQ(result.size(), 0);
}

// Filtering happens before the limit is applied, so rejected entries must
// not consume result slots. With the two highest-ranked entries rejected,
// a limit of two still yields the next two allowed entries rather than
// fewer
TEST(Search_query, filter_excluded_entries_do_not_consume_limit) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/a", "http://example.com/schemas/a", "Alpha", "Desc", 100, 100,
       0, 0},
      {"/schemas/b", "http://example.com/schemas/b", "Beta", "Desc", 90, 100, 0,
       0},
      {"/schemas/c", "http://example.com/schemas/c", "Gamma", "Desc", 80, 100,
       0, 0},
      {"/schemas/d", "http://example.com/schemas/d", "Delta", "Desc", 70, 100,
       0, 0}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{sourcemeta::one::search(
      payload.data(), payload.size(), "schemas", 2,
      sourcemeta::one::SearchScopePath, [](const std::string_view path) {
        return path != "/schemas/a" && path != "/schemas/b";
      })};
  EXPECT_EQ(result.size(), 2);
  EXPECT_SEARCH_RESULT(result, 0, "/schemas/c", "http://example.com/schemas/c",
                       "Gamma", "Desc");
  EXPECT_SEARCH_RESULT(result, 1, "/schemas/d", "http://example.com/schemas/d",
                       "Delta", "Desc");
}
