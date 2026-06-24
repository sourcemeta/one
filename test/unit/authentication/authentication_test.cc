#include <sourcemeta/one/authentication.h>

#include <gtest/gtest.h>

#include <array>       // std::array
#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{AUTHENTICATION_TEST_DIRECTORY} / name;
}

TEST(Authentication, admits_every_path_without_a_credential) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("", "").allowed);
  EXPECT_TRUE(authentication.admits("/acme/foo/bar", "").allowed);
}

TEST(Authentication, admits_every_path_with_any_credential) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_TRUE(authentication.admits("/internal", "anything").allowed);
  EXPECT_TRUE(authentication.admits("/internal/foo", "another").allowed);
}

TEST(Authentication, save_emits_an_empty_artifact_that_admits_everything) {
  const std::array<std::string_view, 1> paths{{"/"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, {}}}};
  const auto path{test_path("community_public_root.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(std::filesystem::file_size(path), 0);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("/internal/foo", "").allowed);
}

TEST(Authentication, permits_every_reference) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_TRUE(authentication.reference_permitted("/one", "/two"));
  EXPECT_TRUE(
      authentication.reference_permitted("/public/one", "/private/two"));
  EXPECT_TRUE(authentication.reference_permitted("/internal/a", "/internal/a"));
}
