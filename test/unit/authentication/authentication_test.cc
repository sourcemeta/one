#include <sourcemeta/one/authentication.h>

#include <gtest/gtest.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
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

TEST(Authentication, malformed_artifact_is_rejected) {
  const auto path{test_path("malformed.bin")};
  std::ofstream stream{path, std::ios::binary};
  const std::array<char, 64> garbage{};
  stream.write(garbage.data(), garbage.size());
  stream.close();

  EXPECT_THROW(const sourcemeta::one::Authentication authentication{path},
               std::runtime_error);
}

TEST(Authentication, public_root_covers_every_path) {
  const std::array<std::string_view, 1> paths{"/"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("public_root.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_EQ(authentication.match("/"), 0b1);
  EXPECT_EQ(authentication.match(""), 0b1);
  EXPECT_EQ(authentication.match("acme"), 0b1);
  EXPECT_EQ(authentication.match("/acme/foo/bar"), 0b1);
}

TEST(Authentication, public_root_admits_everyone) {
  const std::array<std::string_view, 1> paths{"/"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("public_root_admits.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  const auto verdict{
      authentication.admits(authentication.match("/acme/foo"), "")};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.key_name.empty());
}

TEST(Authentication, scoped_prefix_covers_only_its_subtree) {
  const std::array<std::string_view, 1> paths{"/internal"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("scoped_prefix.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_EQ(authentication.match("/internal"), 0b1);
  EXPECT_EQ(authentication.match("/internal/foo"), 0b1);
  EXPECT_EQ(authentication.match("/internal/foo/bar"), 0b1);
  EXPECT_EQ(authentication.match("/"), 0);
  EXPECT_EQ(authentication.match("/vendor"), 0);
}

TEST(Authentication, scoped_prefix_matches_whole_segments_only) {
  const std::array<std::string_view, 1> paths{"/internal"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("segment_boundary.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_EQ(authentication.match("/internalish"), 0);
  EXPECT_EQ(authentication.match("/int"), 0);
  EXPECT_EQ(authentication.match("/internal-team"), 0);
}

TEST(Authentication, uncovered_path_is_denied_when_configured) {
  const std::array<std::string_view, 1> paths{"/internal"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("uncovered_denied.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  const auto covered{
      authentication.admits(authentication.match("/internal/foo"), "")};
  EXPECT_TRUE(covered.allowed);
  const auto uncovered{
      authentication.admits(authentication.match("/vendor"), "")};
  EXPECT_FALSE(uncovered.allowed);
}

TEST(Authentication, distinct_policies_receive_distinct_identifiers) {
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
  EXPECT_EQ(authentication.match("/alpha/one"), 0b001);
  EXPECT_EQ(authentication.match("/beta/two"), 0b010);
  EXPECT_EQ(authentication.match("/gamma/three"), 0b100);
  EXPECT_EQ(authentication.match("/delta"), 0);
}

TEST(Authentication, nested_prefixes_accumulate_along_the_path) {
  const std::array<std::string_view, 1> root{"/"};
  const std::array<std::string_view, 1> internal{"/internal"};
  const std::array<std::string_view, 1> secret{"/internal/secret"};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{sourcemeta::one::Authentication::Type::Public, root},
       {sourcemeta::one::Authentication::Type::Public, internal},
       {sourcemeta::one::Authentication::Type::Public, secret}}};
  const auto path{test_path("nested_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_EQ(authentication.match("/"), 0b001);
  EXPECT_EQ(authentication.match("/public"), 0b001);
  EXPECT_EQ(authentication.match("/internal"), 0b011);
  EXPECT_EQ(authentication.match("/internal/other"), 0b011);
  EXPECT_EQ(authentication.match("/internal/secret"), 0b111);
  EXPECT_EQ(authentication.match("/internal/secret/deep"), 0b111);
}

TEST(Authentication, single_policy_with_multiple_prefixes) {
  const std::array<std::string_view, 2> paths{"/internal", "/vendor"};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("multiple_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_EQ(authentication.match("/internal/foo"), 0b1);
  EXPECT_EQ(authentication.match("/vendor/bar"), 0b1);
  EXPECT_EQ(authentication.match("/public"), 0);
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
  EXPECT_EQ(authentication.match("/p0/foo"), static_cast<std::uint64_t>(1));
  EXPECT_EQ(authentication.match("/p63/foo"), static_cast<std::uint64_t>(1)
                                                  << 63);
  EXPECT_EQ(authentication.match("/missing"), 0);
}
