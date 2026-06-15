#include <sourcemeta/one/authentication.h>

#include <gtest/gtest.h>

#include <filesystem> // std::filesystem::path

TEST(Authentication, absent_policy_matches_nothing) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_EQ(authentication.match("acme/foo"), 0);
  EXPECT_EQ(authentication.match("/acme/foo"), 0);
  EXPECT_EQ(authentication.match(""), 0);
  EXPECT_EQ(authentication.match("/"), 0);
}

TEST(Authentication, absent_policy_admits_anonymous) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  const auto verdict{authentication.admits(0, "")};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.key_name.empty());
}

TEST(Authentication, absent_policy_admits_credentialed) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  const auto verdict{authentication.admits(0, "my-secret-key")};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.key_name.empty());
}
