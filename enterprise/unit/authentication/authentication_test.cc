#include <sourcemeta/one/authentication.h>

#include <gtest/gtest.h>

#include <array>       // std::array
#include <cstddef>     // std::byte, std::size_t
#include <cstdlib>     // setenv
#include <filesystem>  // std::filesystem::path
#include <fstream>     // std::ofstream, std::fstream
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{AUTHENTICATION_TEST_DIRECTORY} / name;
}

TEST(Authentication, missing_artifact_denies_everything) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/acme/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("", "").allowed);
}

TEST(Authentication, malformed_artifact_denies_everything) {
  const auto path{test_path("malformed.bin")};
  std::ofstream stream{path, std::ios::binary};
  const std::array<char, 64> garbage{};
  stream.write(garbage.data(), garbage.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/acme/foo", "").allowed);
}

TEST(Authentication, structurally_corrupt_artifact_denies_everything) {
  const auto path{test_path("corrupt.bin")};
  std::ofstream stream{path, std::ios::binary};
  // A header whose magic and version are valid but whose node table is empty,
  // which is structurally impossible since the root node is always present
  std::array<char, 40> header{};
  header[0] = 'A';
  header[1] = 'U';
  header[2] = 'T';
  header[3] = 'H';
  header[4] = 2;
  stream.write(header.data(), header.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
}

TEST(Authentication, artifact_exceeding_the_policy_ceiling_denies_everything) {
  const auto path{test_path("too-many-policies.bin")};
  std::ofstream stream{path, std::ios::binary};
  // A header with valid magic and version but a policy count past the bitmask
  // width, which would shift out of range during matching
  std::array<char, 40> header{};
  header[0] = 'A';
  header[1] = 'U';
  header[2] = 'T';
  header[3] = 'H';
  header[4] = 2;
  header[8] =
      static_cast<char>(sourcemeta::one::Authentication::MAXIMUM_POLICIES + 1);
  header[12] = 1;
  stream.write(header.data(), header.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/acme/foo", "").allowed);
}

TEST(Authentication, public_root_admits_every_path) {
  const std::array<std::string_view, 1> paths{{"/"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("public_root.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("", "").allowed);
  EXPECT_TRUE(authentication.admits("acme", "").allowed);
  EXPECT_TRUE(authentication.admits("/acme/foo/bar", "").allowed);
}

TEST(Authentication, scoped_prefix_admits_only_its_subtree) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
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
  const std::array<std::string_view, 1> paths{{"/internal"}};
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
  const std::array<std::string_view, 1> alpha{{"/alpha"}};
  const std::array<std::string_view, 1> beta{{"/beta"}};
  const std::array<std::string_view, 1> gamma{{"/gamma"}};
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
  const std::array<std::string_view, 1> internal{{"/internal"}};
  const std::array<std::string_view, 1> secret{{"/internal/secret"}};
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
  const std::array<std::string_view, 2> paths{{"/internal", "/vendor"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("multiple_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_TRUE(authentication.admits("/vendor/bar", "").allowed);
  EXPECT_FALSE(authentication.admits("/public", "").allowed);
}

TEST(Authentication, corrupted_section_offset_denies_everything) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("corrupted_offset.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  // Overwrite the node section offset, the seventh 32-bit header field, with a
  // value that aliases the header. Without a canonical layout check this would
  // read policy masks out of unrelated bytes and admit unexpectedly
  std::fstream stream{path, std::ios::binary | std::ios::in | std::ios::out};
  stream.seekp(24);
  const std::array<char, 4> aliased{};
  stream.write(aliased.data(), aliased.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("/", "").allowed);
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

TEST(Authentication, apikey_admits_matching_credential) {
  setenv("ONE_TEST_KEY_MATCH", "secret-match", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MATCH"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, paths, keys}}};
  const auto path{test_path("apikey_match.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/internal/foo", "secret-match").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "wrong").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
}

TEST(Authentication, apikey_with_multiple_keys_admits_any) {
  setenv("ONE_TEST_KEY_MULTI_A", "key-a", 1);
  setenv("ONE_TEST_KEY_MULTI_B", "key-b", 1);
  const std::array<std::string_view, 2> keys{
      {"ONE_TEST_KEY_MULTI_A", "ONE_TEST_KEY_MULTI_B"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, paths, keys}}};
  const auto path{test_path("apikey_multi.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/internal/foo", "key-a").allowed);
  EXPECT_TRUE(authentication.admits("/internal/foo", "key-b").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "key-c").allowed);
}

TEST(Authentication, apikey_with_unset_variable_denies) {
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_UNSET"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, paths, keys}}};
  const auto path{test_path("apikey_unset.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.admits("/internal/foo", "anything").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
}

TEST(Authentication, apikey_denies_uncovered_path) {
  setenv("ONE_TEST_KEY_SCOPED", "scoped-secret", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SCOPED"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, paths, keys}}};
  const auto path{test_path("apikey_scoped.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/internal/foo", "scoped-secret").allowed);
  EXPECT_FALSE(authentication.admits("/other", "scoped-secret").allowed);
}

TEST(Authentication, public_overrides_apikey_on_shared_path) {
  setenv("ONE_TEST_KEY_SHARED", "shared-secret", 1);
  const std::array<std::string_view, 1> public_paths{{"/internal"}};
  const std::array<std::string_view, 1> apikey_paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHARED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, public_paths},
       {sourcemeta::one::Authentication::Type::ApiKey, apikey_paths, keys}}};
  const auto path{test_path("apikey_shared.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  const sourcemeta::one::Authentication authentication{path};
  // The public policy admits anonymously even without a credential
  EXPECT_TRUE(authentication.admits("/internal/foo", "").allowed);
}

TEST(Authentication, metadata_outside_its_region_denies_everything) {
  setenv("ONE_TEST_KEY_REGION", "region-secret", 1);
  const std::array<std::string_view, 1> public_paths{{"/public"}};
  const std::array<std::string_view, 1> apikey_paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_REGION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, public_paths},
       {sourcemeta::one::Authentication::Type::ApiKey, apikey_paths, keys}}};
  const auto path{test_path("metadata_region.bin")};
  sourcemeta::one::Authentication::save(policies, path);

  // Point the second policy's metadata offset, the first field of its entry at
  // byte 52, into the policy table instead of the trailing metadata region
  std::fstream stream{path, std::ios::binary | std::ios::in | std::ios::out};
  stream.seekp(52);
  const std::array<char, 4> aliased{};
  stream.write(aliased.data(), aliased.size());
  stream.close();

  // The whole artifact is rejected, so even the untouched public policy denies
  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.admits("/public/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "region-secret").allowed);
}
