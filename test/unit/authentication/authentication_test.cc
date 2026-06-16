#include <sourcemeta/one/authentication.h>

#include <gtest/gtest.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem::path
#include <fstream>     // std::ofstream
#include <span>        // std::span
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{AUTHENTICATION_TEST_DIRECTORY} / name;
}

TEST(Authentication, absent_policy_admits_anonymous) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_TRUE(authentication.admits("/acme/foo", "").allowed);
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("", "").allowed);
  EXPECT_TRUE(authentication.admits("/acme/foo", "").key_name.empty());
}

TEST(Authentication, absent_policy_admits_credentialed) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  const auto verdict{authentication.admits("/acme/foo", "my-secret-key")};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.key_name.empty());
}

TEST(Authentication, malformed_artifact_is_rejected) {
  const auto path{test_path("malformed.bin")};
  std::ofstream stream{path, std::ios::binary};
  const std::array<char, 64> garbage{};
  stream.write(garbage.data(), garbage.size());
  stream.close();

  EXPECT_THROW(const sourcemeta::one::Authentication authentication{path},
               std::runtime_error);
}

TEST(Authentication, public_root_admits_every_path) {
  const std::array<std::string_view, 1> paths{"/"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("public_root.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("", "").allowed);
  EXPECT_TRUE(authentication.admits("acme", "").allowed);
  EXPECT_TRUE(authentication.admits("/acme/foo/bar", "").allowed);
  EXPECT_TRUE(authentication.admits("/acme/foo/bar", "").key_name.empty());
}

TEST(Authentication, scoped_prefix_admits_only_its_subtree) {
  const std::array<std::string_view, 1> paths{"/internal"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("scoped_prefix.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/internal", "").allowed);
  EXPECT_TRUE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_TRUE(authentication.admits("/internal/foo/bar", "").allowed);
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/vendor", "").allowed);
}

TEST(Authentication, scoped_prefix_matches_whole_segments_only) {
  const std::array<std::string_view, 1> paths{"/internal"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("segment_boundary.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.admits("/internalish", "").allowed);
  EXPECT_FALSE(authentication.admits("/int", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal-team", "").allowed);
}

TEST(Authentication, distinct_policies_each_admit_their_scope) {
  const std::array<std::string_view, 1> alpha{"/alpha"};
  const std::array<std::string_view, 1> beta{"/beta"};
  const std::array<std::string_view, 1> gamma{"/gamma"};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{sourcemeta::one::Authentication::Type::Public, alpha},
       {sourcemeta::one::Authentication::Type::Public, beta},
       {sourcemeta::one::Authentication::Type::Public, gamma}}};
  const auto path{test_path("distinct_policies.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/alpha/one", "").allowed);
  EXPECT_TRUE(authentication.admits("/beta/two", "").allowed);
  EXPECT_TRUE(authentication.admits("/gamma/three", "").allowed);
  EXPECT_FALSE(authentication.admits("/delta", "").allowed);
}

TEST(Authentication, nested_prefixes_admit_their_subtrees) {
  const std::array<std::string_view, 1> internal{"/internal"};
  const std::array<std::string_view, 1> secret{"/internal/secret"};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, internal},
       {sourcemeta::one::Authentication::Type::Public, secret}}};
  const auto path{test_path("nested_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/internal", "").allowed);
  EXPECT_TRUE(authentication.admits("/internal/other", "").allowed);
  EXPECT_TRUE(authentication.admits("/internal/secret", "").allowed);
  EXPECT_TRUE(authentication.admits("/internal/secret/deep", "").allowed);
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/public", "").allowed);
}

TEST(Authentication, single_policy_with_multiple_prefixes) {
  const std::array<std::string_view, 2> paths{"/internal", "/vendor"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("multiple_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_TRUE(authentication.admits("/vendor/bar", "").allowed);
  EXPECT_FALSE(authentication.admits("/public", "").allowed);
}

TEST(Authentication, supports_the_maximum_number_of_policies) {
  constexpr auto maximum{sourcemeta::one::Authentication::MAXIMUM_POLICIES};
  std::vector<std::string> path_storage;
  path_storage.reserve(maximum);
  for (std::size_t index{0}; index < maximum; index += 1) {
    path_storage.push_back("/p" + std::to_string(index));
  }

  std::vector<std::string_view> path_views;
  path_views.reserve(maximum);
  for (const auto &value : path_storage) {
    path_views.push_back(value);
  }

  std::vector<sourcemeta::one::Authentication::Policy> policies;
  policies.reserve(maximum);
  for (std::size_t index{0}; index < maximum; index += 1) {
    policies.push_back(
        {sourcemeta::one::Authentication::Type::Public,
         std::span<const std::string_view>{&path_views[index], 1}});
  }

  const auto path{test_path("maximum_policies.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/p0/foo", "").allowed);
  EXPECT_TRUE(authentication.admits("/p63/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("/missing", "").allowed);
}
