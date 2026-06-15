#include <sourcemeta/one/authentication.h>

#include <gtest/gtest.h>

#include <filesystem> // std::filesystem::path

TEST(Authentication_context, default_constructed_is_anonymous) {
  const sourcemeta::one::Authentication::Context context;
  EXPECT_FALSE(context.credential().has_value());
}

TEST(Authentication_context, from_header_canonical_bearer) {
  const auto context{sourcemeta::one::Authentication::Context::from_header(
      "Bearer my-secret-key")};
  EXPECT_TRUE(context.credential().has_value());
  EXPECT_EQ(context.credential().value(), "my-secret-key");
}

TEST(Authentication_context, from_header_lowercase_scheme) {
  const auto context{sourcemeta::one::Authentication::Context::from_header(
      "bearer my-secret-key")};
  EXPECT_TRUE(context.credential().has_value());
  EXPECT_EQ(context.credential().value(), "my-secret-key");
}

TEST(Authentication_context, from_header_uppercase_scheme) {
  const auto context{sourcemeta::one::Authentication::Context::from_header(
      "BEARER my-secret-key")};
  EXPECT_TRUE(context.credential().has_value());
  EXPECT_EQ(context.credential().value(), "my-secret-key");
}

TEST(Authentication_context, from_header_multiple_spaces_before_token) {
  const auto context{sourcemeta::one::Authentication::Context::from_header(
      "Bearer   my-secret-key")};
  EXPECT_TRUE(context.credential().has_value());
  EXPECT_EQ(context.credential().value(), "my-secret-key");
}

TEST(Authentication_context, from_header_trims_trailing_whitespace) {
  const auto context{sourcemeta::one::Authentication::Context::from_header(
      "Bearer my-secret-key  ")};
  EXPECT_TRUE(context.credential().has_value());
  EXPECT_EQ(context.credential().value(), "my-secret-key");
}

TEST(Authentication_context, from_header_empty_is_anonymous) {
  const auto context{sourcemeta::one::Authentication::Context::from_header("")};
  EXPECT_FALSE(context.credential().has_value());
}

TEST(Authentication_context, from_header_scheme_without_token_is_anonymous) {
  const auto context{
      sourcemeta::one::Authentication::Context::from_header("Bearer ")};
  EXPECT_FALSE(context.credential().has_value());
}

TEST(Authentication_context, from_header_scheme_without_space_is_anonymous) {
  const auto context{
      sourcemeta::one::Authentication::Context::from_header("Bearerabc")};
  EXPECT_FALSE(context.credential().has_value());
}

TEST(Authentication_context, from_header_other_scheme_is_anonymous) {
  const auto context{sourcemeta::one::Authentication::Context::from_header(
      "Basic dXNlcjpwYXNz")};
  EXPECT_FALSE(context.credential().has_value());
}

TEST(Authentication_context, copies_preserve_credential) {
  const auto context{sourcemeta::one::Authentication::Context::from_header(
      "Bearer my-secret-key")};
  const auto copy{context};
  EXPECT_TRUE(copy.credential().has_value());
  EXPECT_EQ(copy.credential().value(), "my-secret-key");
}

TEST(Authentication_view, absent_policy_is_inactive) {
  const sourcemeta::one::Authentication view{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_FALSE(view.active());
}

TEST(Authentication_view, absent_policy_matches_nothing) {
  const sourcemeta::one::Authentication view{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_EQ(view.match("acme/foo"), 0);
  EXPECT_EQ(view.match("/acme/foo"), 0);
  EXPECT_EQ(view.match(""), 0);
  EXPECT_EQ(view.match("/"), 0);
}

TEST(Authentication_view, absent_policy_admits_anonymous) {
  const sourcemeta::one::Authentication view{
      std::filesystem::path{"/no/such/authentication.bin"}};
  sourcemeta::one::Authentication::Context context;
  const auto verdict{view.admits(0, context)};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.key_name.empty());
}

TEST(Authentication_view, absent_policy_admits_credentialed) {
  const sourcemeta::one::Authentication view{
      std::filesystem::path{"/no/such/authentication.bin"}};
  auto context{sourcemeta::one::Authentication::Context::from_header(
      "Bearer my-secret-key")};
  const auto verdict{view.admits(0, context)};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.key_name.empty());
}
