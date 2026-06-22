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
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_TRUE(authentication.admits("/vendor/bar", "").allowed);
  EXPECT_FALSE(authentication.admits("/public", "").allowed);
}

TEST(Authentication, extensionless_policy_covers_every_representation) {
  const std::array<std::string_view, 1> paths{{"/secret/data"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_REPRESENTATION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, paths, keys}}};
  const auto path{test_path("representation_agnostic.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  setenv("ONE_TEST_KEY_REPRESENTATION", "representation-secret", 1);

  const sourcemeta::one::Authentication authentication{path};
  // The resource, every representation of it, and its subtree are all governed
  EXPECT_FALSE(authentication.admits("/secret/data", "").allowed);
  EXPECT_FALSE(authentication.admits("/secret/data.json", "").allowed);
  EXPECT_FALSE(authentication.admits("/secret/data.xml", "").allowed);
  EXPECT_FALSE(authentication.admits("/secret/data/nested", "").allowed);
  EXPECT_TRUE(
      authentication.admits("/secret/data", "representation-secret").allowed);
  EXPECT_TRUE(
      authentication.admits("/secret/data.json", "representation-secret")
          .allowed);
  EXPECT_TRUE(authentication.admits("/secret/data.xml", "representation-secret")
                  .allowed);
  EXPECT_TRUE(
      authentication.admits("/secret/data/nested", "representation-secret")
          .allowed);
  // A sibling resource sharing a textual prefix is outside the policy, so even
  // the key does not admit it (it is governed by no policy at all)
  EXPECT_FALSE(
      authentication.admits("/secret/database", "representation-secret")
          .allowed);
  EXPECT_FALSE(
      authentication.admits("/secret/data2.json", "representation-secret")
          .allowed);
}

TEST(Authentication,
     extension_specific_policy_covers_only_that_representation) {
  const std::array<std::string_view, 1> paths{{"/secret/data.json"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SPECIFIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, paths, keys}}};
  const auto path{test_path("representation_specific.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  setenv("ONE_TEST_KEY_SPECIFIC", "specific-secret", 1);

  const sourcemeta::one::Authentication authentication{path};
  // Only the named representation is gated
  EXPECT_FALSE(authentication.admits("/secret/data.json", "").allowed);
  EXPECT_TRUE(
      authentication.admits("/secret/data.json", "specific-secret").allowed);
  EXPECT_TRUE(
      authentication.admits("/secret/data.json/nested", "specific-secret")
          .allowed);
  // The bare resource and other representations are outside this policy, so
  // the key does not admit them either
  EXPECT_FALSE(
      authentication.admits("/secret/data", "specific-secret").allowed);
  EXPECT_FALSE(
      authentication.admits("/secret/data.xml", "specific-secret").allowed);
}

TEST(Authentication, representation_agnostic_and_specific_policies_compose) {
  const std::array<std::string_view, 1> resource{{"/docs/page"}};
  const std::array<std::string_view, 1> representation{{"/docs/page.pdf"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PDF"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, resource},
       {sourcemeta::one::Authentication::Type::ApiKey, representation, keys}}};
  const auto path{test_path("representation_compose.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  setenv("ONE_TEST_KEY_PDF", "pdf-secret", 1);

  const sourcemeta::one::Authentication authentication{path};
  // The public resource policy admits the resource and its representations,
  // including the pdf, since union semantics let public win over the apiKey
  EXPECT_TRUE(authentication.admits("/docs/page", "").allowed);
  EXPECT_TRUE(authentication.admits("/docs/page.html", "").allowed);
  EXPECT_TRUE(authentication.admits("/docs/page.pdf", "").allowed);
}

TEST(Authentication, base_path_is_stripped_before_matching) {
  const std::array<std::string_view, 1> public_paths{{"/public"}};
  const std::array<std::string_view, 1> apikey_paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_BASE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, public_paths},
       {sourcemeta::one::Authentication::Type::ApiKey, apikey_paths, keys}}};
  const auto path{test_path("base_path.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  setenv("ONE_TEST_KEY_BASE", "base-secret", 1);

  const sourcemeta::one::Authentication authentication{path};
  // With the base path stripped, the registry path under it is matched
  EXPECT_TRUE(authentication.admits("/registry/public/string", "", "/registry")
                  .allowed);
  EXPECT_FALSE(
      authentication.admits("/registry/private/secret", "", "/registry")
          .allowed);
  EXPECT_TRUE(
      authentication
          .admits("/registry/private/secret", "base-secret", "/registry")
          .allowed);

  // An empty base path strips nothing
  EXPECT_TRUE(authentication.admits("/public/string", "", "").allowed);
  EXPECT_FALSE(authentication.admits("/private/secret", "", "").allowed);

  // A base path that is not a whole-segment prefix is left in place, so the
  // path matches no policy and is denied
  EXPECT_FALSE(
      authentication.admits("/registryextra/public/string", "", "/registry")
          .allowed);

  // A request outside the base path is left in place and matches no policy
  EXPECT_FALSE(
      authentication.admits("/elsewhere/public/string", "", "/registry")
          .allowed);
}

TEST(Authentication, corrupted_section_offset_denies_everything) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::Public, paths}}};
  const auto path{test_path("corrupted_offset.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

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
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.admits("/internal/foo", "scoped-secret").allowed);
  EXPECT_FALSE(authentication.admits("/other", "scoped-secret").allowed);
}

TEST(Authentication, public_carveout_overrides_apikey) {
  setenv("ONE_TEST_KEY_SHARED", "shared-secret", 1);
  const std::array<std::string_view, 1> apikey_paths{{"/internal"}};
  const std::array<std::string_view, 1> public_paths{{"/internal/open"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHARED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, apikey_paths, keys},
       {sourcemeta::one::Authentication::Type::Public, public_paths}}};
  const auto path{test_path("apikey_carveout.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  // The public carve-out admits anonymously on its subtree
  EXPECT_TRUE(authentication.admits("/internal/open/foo", "").allowed);
  // The rest of the apiKey scope still requires the credential
  EXPECT_FALSE(authentication.admits("/internal/secret", "").allowed);
  EXPECT_TRUE(
      authentication.admits("/internal/secret", "shared-secret").allowed);
}

TEST(Authentication, save_rejects_apikey_fully_shadowed_by_public) {
  const std::array<std::string_view, 1> public_paths{{"/"}};
  const std::array<std::string_view, 1> apikey_paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_DEAD"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, public_paths},
       {sourcemeta::one::Authentication::Type::ApiKey, apikey_paths, keys}}};
  const auto path{test_path("apikey_shadowed.bin")};
  EXPECT_THROW(sourcemeta::one::Authentication::save(policies, path, path),
               sourcemeta::one::AuthenticationShadowedError);
}

TEST(Authentication,
     save_rejects_apikey_shadowed_by_public_with_trailing_slash) {
  const std::array<std::string_view, 1> public_paths{{"/internal/"}};
  const std::array<std::string_view, 1> apikey_paths{{"/internal/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_DEAD_SLASH"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, public_paths},
       {sourcemeta::one::Authentication::Type::ApiKey, apikey_paths, keys}}};
  const auto path{test_path("apikey_shadowed_slash.bin")};
  EXPECT_THROW(sourcemeta::one::Authentication::save(policies, path, path),
               sourcemeta::one::AuthenticationShadowedError);
}

TEST(Authentication, reference_to_a_public_schema_is_permitted) {
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_PUBLIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, open_paths},
       {sourcemeta::one::Authentication::Type::ApiKey, secret_paths, keys}}};
  const auto path{test_path("ref_to_public.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.reference_permitted("/secret/one", "/open/two"));
  EXPECT_TRUE(authentication.reference_permitted("/open/one", "/open/two"));
}

TEST(Authentication, public_schema_referencing_an_apikey_schema_is_rejected) {
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_LEAK"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, open_paths},
       {sourcemeta::one::Authentication::Type::ApiKey, secret_paths, keys}}};
  const auto path{test_path("ref_public_to_apikey.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.reference_permitted("/open/one", "/secret/two"));
}

TEST(Authentication, reference_within_the_same_policy_is_permitted) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_SAME"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, paths, keys}}};
  const auto path{test_path("ref_same_policy.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(
      authentication.reference_permitted("/internal/one", "/internal/two"));
  EXPECT_TRUE(
      authentication.reference_permitted("/internal/one", "/internal/one"));
}

TEST(Authentication, reference_across_disjoint_policies_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_REF_ALPHA"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_REF_BETA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, alpha_paths, alpha_keys},
       {sourcemeta::one::Authentication::Type::ApiKey, beta_paths, beta_keys}}};
  const auto path{test_path("ref_disjoint.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.reference_permitted("/alpha/one", "/beta/two"));
  EXPECT_FALSE(authentication.reference_permitted("/beta/two", "/alpha/one"));
}

TEST(Authentication,
     reference_from_a_narrower_to_a_wider_audience_is_permitted) {
  const std::array<std::string_view, 1> broad_paths{{"/p"}};
  const std::array<std::string_view, 1> nested_paths{{"/p/inner"}};
  const std::array<std::string_view, 1> broad_keys{{"ONE_TEST_REF_BROAD"}};
  const std::array<std::string_view, 1> nested_keys{{"ONE_TEST_REF_NESTED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, broad_paths, broad_keys},
       {sourcemeta::one::Authentication::Type::ApiKey, nested_paths,
        nested_keys}}};
  const auto path{test_path("ref_narrow_to_wide.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.reference_permitted("/p/one", "/p/inner/two"));
  EXPECT_FALSE(authentication.reference_permitted("/p/inner/two", "/p/one"));
}

TEST(Authentication, sharing_one_policy_is_insufficient) {
  const std::array<std::string_view, 1> broad_paths{{"/p"}};
  const std::array<std::string_view, 1> alpha_paths{{"/p/alpha"}};
  const std::array<std::string_view, 1> gamma_paths{{"/p/gamma"}};
  const std::array<std::string_view, 1> broad_keys{{"ONE_TEST_SHARE_BROAD"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_SHARE_ALPHA"}};
  const std::array<std::string_view, 1> gamma_keys{{"ONE_TEST_SHARE_GAMMA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, broad_paths, broad_keys},
       {sourcemeta::one::Authentication::Type::ApiKey, alpha_paths, alpha_keys},
       {sourcemeta::one::Authentication::Type::ApiKey, gamma_paths,
        gamma_keys}}};
  const auto path{test_path("ref_shared_policy.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(
      authentication.reference_permitted("/p/alpha/one", "/p/gamma/two"));
  EXPECT_FALSE(
      authentication.reference_permitted("/p/gamma/two", "/p/alpha/one"));
}

TEST(Authentication, reference_from_an_ungoverned_schema_is_permitted) {
  const std::array<std::string_view, 1> paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_UNGOVERNED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, paths, keys}}};
  const auto path{test_path("ref_ungoverned_from.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(
      authentication.reference_permitted("/nowhere/one", "/secret/two"));
}

TEST(Authentication, reference_to_an_ungoverned_schema_is_rejected) {
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_TO_DENY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, open_paths},
       {sourcemeta::one::Authentication::Type::ApiKey, secret_paths, keys}}};
  const auto path{test_path("ref_to_ungoverned.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.reference_permitted("/open/one", "/nowhere/two"));
  EXPECT_FALSE(
      authentication.reference_permitted("/secret/one", "/nowhere/two"));
}

TEST(Authentication, public_carveout_under_an_apikey_prefix) {
  const std::array<std::string_view, 1> private_paths{{"/private"}};
  const std::array<std::string_view, 1> open_paths{{"/private/open"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_CARVEOUT"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, private_paths, keys},
       {sourcemeta::one::Authentication::Type::Public, open_paths}}};
  const auto path{test_path("ref_carveout.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.reference_permitted("/private/secret",
                                                 "/private/open/shared"));
  EXPECT_FALSE(authentication.reference_permitted("/private/open/shared",
                                                  "/private/secret"));
}

TEST(Authentication, reference_honours_shared_environment_variable_names) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> shared_keys{{"ONE_TEST_REF_SHARED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::ApiKey, alpha_paths,
        shared_keys},
       {sourcemeta::one::Authentication::Type::ApiKey, beta_paths,
        shared_keys}}};
  const auto path{test_path("ref_shared_variable.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_TRUE(authentication.reference_permitted("/alpha/one", "/beta/two"));
  EXPECT_TRUE(authentication.reference_permitted("/beta/two", "/alpha/one"));
}

TEST(Authentication, reference_distinguishes_sibling_prefix_policies) {
  const std::array<std::string_view, 1> public_paths{{"/public"}};
  const std::array<std::string_view, 1> apikey_paths{{"/pub"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_SIBLING_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{sourcemeta::one::Authentication::Type::Public, public_paths},
       {sourcemeta::one::Authentication::Type::ApiKey, apikey_paths, keys}}};
  const auto path{test_path("ref_sibling_prefix.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path};
  EXPECT_FALSE(authentication.reference_permitted("/public/a", "/pub/b"));
  EXPECT_TRUE(authentication.reference_permitted("/pub/b", "/public/a"));
}

TEST(Authentication, reference_permitted_on_a_missing_artifact_is_permitted) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_TRUE(authentication.reference_permitted("/a/one", "/b/two"));
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
  sourcemeta::one::Authentication::save(policies, path, path);

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
