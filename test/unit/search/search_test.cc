#include <sourcemeta/one/search.h>

#include <sourcemeta/core/json.h>

#include <gtest/gtest.h>

#include <string>  // std::string
#include <utility> // std::move
#include <vector>  // std::vector

TEST(Search, make_search_empty) {
  std::vector<sourcemeta::one::SearchEntry> entries;
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_TRUE(payload.empty());
}

TEST(Search, make_search_single_entry) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "My Title", "A description"}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  EXPECT_FALSE(payload.empty());

  const std::string payload_string(payload.begin(), payload.end());
  EXPECT_NE(payload_string.find("/foo/bar"), std::string::npos);
  EXPECT_NE(payload_string.find("My Title"), std::string::npos);
  EXPECT_NE(payload_string.find("A description"), std::string::npos);
}

TEST(Search, search_empty_payload) {
  const auto result{sourcemeta::one::search(nullptr, 0, "anything")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search, search_no_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Title", "Desc"}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "zzzzz")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
}

TEST(Search, search_match_in_path) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Title", "Desc"}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "foo")};
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).at("path").to_string(), "/foo/bar");
  EXPECT_EQ(result.at(0).at("title").to_string(), "Title");
  EXPECT_EQ(result.at(0).at("description").to_string(), "Desc");
}

TEST(Search, search_match_in_title) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Special Title", "Desc"}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Special")};
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).at("title").to_string(), "Special Title");
}

TEST(Search, search_match_in_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Title", "Unique description here"}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "Unique")};
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).at("description").to_string(),
            "Unique description here");
}

TEST(Search, search_case_insensitive) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/foo/bar", "Hello World", "desc"}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  const auto result_lower{
      sourcemeta::one::search(payload.data(), payload.size(), "hello")};
  EXPECT_EQ(result_lower.size(), 1);

  const auto result_upper{
      sourcemeta::one::search(payload.data(), payload.size(), "HELLO")};
  EXPECT_EQ(result_upper.size(), 1);

  const auto result_mixed{
      sourcemeta::one::search(payload.data(), payload.size(), "hElLo")};
  EXPECT_EQ(result_mixed.size(), 1);
}

TEST(Search, search_multiple_matches) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/address", "Address Schema", "For addresses"},
      {"/schemas/person", "Person Schema", "For people"},
      {"/schemas/email", "Email Schema", "For emails"}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "schema")};
  EXPECT_EQ(result.size(), 3);
}

TEST(Search, search_limit_10) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/schemas/test0", "Test 0", ""},   {"/schemas/test1", "Test 1", ""},
      {"/schemas/test2", "Test 2", ""},   {"/schemas/test3", "Test 3", ""},
      {"/schemas/test4", "Test 4", ""},   {"/schemas/test5", "Test 5", ""},
      {"/schemas/test6", "Test 6", ""},   {"/schemas/test7", "Test 7", ""},
      {"/schemas/test8", "Test 8", ""},   {"/schemas/test9", "Test 9", ""},
      {"/schemas/test10", "Test 10", ""}, {"/schemas/test11", "Test 11", ""},
      {"/schemas/test12", "Test 12", ""}, {"/schemas/test13", "Test 13", ""},
      {"/schemas/test14", "Test 14", ""}};

  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "test")};
  EXPECT_EQ(result.size(), 10);
}

TEST(Search, search_round_trip_data_fidelity) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/a/b/c", "My Title", "My Description"},
      {"/x/y/z", "", "Only description"},
      {"/p/q", "Only title", ""}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};

  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "/")};
  EXPECT_EQ(result.size(), 3);

  EXPECT_EQ(result.at(0).at("path").to_string(), "/a/b/c");
  EXPECT_EQ(result.at(0).at("title").to_string(), "My Title");
  EXPECT_EQ(result.at(0).at("description").to_string(), "My Description");

  EXPECT_EQ(result.at(1).at("path").to_string(), "/p/q");
  EXPECT_EQ(result.at(1).at("title").to_string(), "Only title");
  EXPECT_EQ(result.at(1).at("description").to_string(), "");

  EXPECT_EQ(result.at(2).at("path").to_string(), "/x/y/z");
  EXPECT_EQ(result.at(2).at("title").to_string(), "");
  EXPECT_EQ(result.at(2).at("description").to_string(), "Only description");
}

TEST(Search, search_single_entry_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/only", "One", "Entry"}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "One")};
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).at("path").to_string(), "/only");
}

TEST(Search, search_single_entry_no_match) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/only", "One", "Entry"}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "nope")};
  EXPECT_EQ(result.size(), 0);
}

TEST(Search, search_empty_title_and_description) {
  std::vector<sourcemeta::one::SearchEntry> entries{
      {"/path/only", "", ""}};
  const auto payload{sourcemeta::one::make_search(std::move(entries))};
  const auto result{
      sourcemeta::one::search(payload.data(), payload.size(), "path")};
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).at("title").to_string(), "");
  EXPECT_EQ(result.at(0).at("description").to_string(), "");
}
