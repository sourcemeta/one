#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>

#include <gtest/gtest.h>

#include <array>       // std::array
#include <cstddef>     // std::byte, std::size_t
#include <cstdlib>     // setenv
#include <filesystem>  // std::filesystem::path
#include <fstream>     // std::ofstream, std::fstream
#include <map>         // std::map
#include <memory>      // std::shared_ptr, std::make_shared
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{AUTHENTICATION_TEST_DIRECTORY} / name;
}

TEST(Authentication, missing_artifact_denies_everything) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
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

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/acme/foo", "").allowed);
}

TEST(Authentication, structurally_corrupt_artifact_denies_everything) {
  const auto path{test_path("corrupt.bin")};
  std::ofstream stream{path, std::ios::binary};
  // A valid header over an empty node table
  std::array<char, 40> header{};
  header[0] = 'A';
  header[1] = 'U';
  header[2] = 'T';
  header[3] = 'H';
  header[4] = 4;
  stream.write(header.data(), header.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
}

TEST(Authentication, artifact_exceeding_the_policy_ceiling_denies_everything) {
  const auto path{test_path("too-many-policies.bin")};
  std::ofstream stream{path, std::ios::binary};
  // A valid header declaring a policy count past the supported maximum
  std::array<char, 40> header{};
  header[0] = 'A';
  header[1] = 'U';
  header[2] = 'T';
  header[3] = 'H';
  header[4] = 4;
  header[8] =
      static_cast<char>(sourcemeta::one::Authentication::MAXIMUM_POLICIES + 1);
  header[12] = 1;
  stream.write(header.data(), header.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/acme/foo", "").allowed);
}

TEST(Authentication, corrupted_section_offset_denies_everything) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_OFFSET"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("corrupted_offset.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  // Overwrite the node section offset with a value that aliases the header
  std::fstream stream{path, std::ios::binary | std::ios::in | std::ios::out};
  stream.seekp(24);
  const std::array<char, 4> aliased{};
  stream.write(aliased.data(), aliased.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("/", "").allowed);
}

TEST(Authentication, zero_policies_admits_every_path) {
  const std::array<sourcemeta::one::Authentication::Policy, 0> policies{};
  const auto path{test_path("zero_policies.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("", "").allowed);
  EXPECT_TRUE(authentication.admits("/acme/foo/bar", "").allowed);
  EXPECT_EQ(authentication.governing("/"), (std::vector<std::size_t>{}));
  EXPECT_EQ(authentication.governing("/acme"), (std::vector<std::size_t>{}));
}

TEST(Authentication, uncovered_paths_are_public_around_a_gated_scope) {
  setenv("ONE_TEST_KEY_SCOPE", "scope-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SCOPE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("uncovered_public.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  // The covered subtree is gated
  EXPECT_FALSE(authentication.admits("/internal", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_TRUE(authentication.admits("/internal", "scope-secret").allowed);
  EXPECT_TRUE(authentication.admits("/internal/foo", "scope-secret").allowed);
  // Everything outside it is public
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("/vendor", "").allowed);
  EXPECT_TRUE(authentication.admits("/vendor/foo", "").allowed);
}

TEST(Authentication, scope_matches_whole_segments_only) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SEGMENT"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("segment_boundary.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  // The scope gates its own segment
  EXPECT_FALSE(authentication.admits("/internal", "").allowed);
  // A textual prefix that is not a whole segment is a different path, so it is
  // uncovered and public
  EXPECT_TRUE(authentication.admits("/internalish", "").allowed);
  EXPECT_TRUE(authentication.admits("/int", "").allowed);
  EXPECT_TRUE(authentication.admits("/internal-team", "").allowed);
}

TEST(Authentication, distinct_policies_each_gate_their_scope) {
  const std::array<std::string_view, 1> alpha{{"/alpha"}};
  const std::array<std::string_view, 1> beta{{"/beta"}};
  const std::array<std::string_view, 1> gamma{{"/gamma"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_KEY_DA"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_KEY_DB"}};
  const std::array<std::string_view, 1> gamma_keys{{"ONE_TEST_KEY_DG"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{alpha, alpha_keys}, {beta, beta_keys}, {gamma, gamma_keys}}};
  const auto path{test_path("distinct_policies.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.admits("/alpha/one", "").allowed);
  EXPECT_FALSE(authentication.admits("/beta/two", "").allowed);
  EXPECT_FALSE(authentication.admits("/gamma/three", "").allowed);
  // Between the scopes the registry is public
  EXPECT_TRUE(authentication.admits("/delta", "").allowed);
}

TEST(Authentication, nested_prefixes_gate_their_subtrees) {
  const std::array<std::string_view, 1> internal{{"/internal"}};
  const std::array<std::string_view, 1> secret{{"/internal/secret"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_NI"}};
  const std::array<std::string_view, 1> secret_keys{{"ONE_TEST_KEY_NS"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{internal, internal_keys}, {secret, secret_keys}}};
  const auto path{test_path("nested_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.admits("/internal", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/other", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/secret", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/secret/deep", "").allowed);
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("/public", "").allowed);
}

TEST(Authentication, nested_inner_key_widens_access) {
  setenv("ONE_TEST_KEY_WI", "wi-secret", 1);
  setenv("ONE_TEST_KEY_WO", "wo-secret", 1);
  const std::array<std::string_view, 1> outer{{"/internal"}};
  const std::array<std::string_view, 1> inner{{"/internal/secret"}};
  const std::array<std::string_view, 1> outer_keys{{"ONE_TEST_KEY_WO"}};
  const std::array<std::string_view, 1> inner_keys{{"ONE_TEST_KEY_WI"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{outer, outer_keys}, {inner, inner_keys}}};
  const auto path{test_path("nested_widen.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  // The inner path is covered by both, so either key admits it
  EXPECT_TRUE(authentication.admits("/internal/secret", "wo-secret").allowed);
  EXPECT_TRUE(authentication.admits("/internal/secret", "wi-secret").allowed);
  // The outer path is covered only by the outer policy
  EXPECT_TRUE(authentication.admits("/internal/other", "wo-secret").allowed);
  EXPECT_FALSE(authentication.admits("/internal/other", "wi-secret").allowed);
}

TEST(Authentication, single_policy_with_multiple_prefixes) {
  const std::array<std::string_view, 2> paths{{"/internal", "/vendor"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MP"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("multiple_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("/vendor/bar", "").allowed);
  EXPECT_TRUE(authentication.admits("/public", "").allowed);
}

TEST(Authentication, extensionless_policy_gates_every_representation) {
  setenv("ONE_TEST_KEY_REPRESENTATION", "representation-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_REPRESENTATION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("representation_agnostic.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
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
  // A sibling sharing a textual prefix is covered by no policy, so it is public
  EXPECT_TRUE(authentication.admits("/secret/database", "").allowed);
  EXPECT_TRUE(authentication.admits("/secret/data2.json", "").allowed);
}

TEST(Authentication, extension_specific_policy_gates_only_that_representation) {
  setenv("ONE_TEST_KEY_SPECIFIC", "specific-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data.json"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SPECIFIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("representation_specific.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  // Only the named representation is gated
  EXPECT_FALSE(authentication.admits("/secret/data.json", "").allowed);
  EXPECT_TRUE(
      authentication.admits("/secret/data.json", "specific-secret").allowed);
  EXPECT_TRUE(
      authentication.admits("/secret/data.json/nested", "specific-secret")
          .allowed);
  // The bare resource and other representations are uncovered, so public
  EXPECT_TRUE(authentication.admits("/secret/data", "").allowed);
  EXPECT_TRUE(authentication.admits("/secret/data.xml", "").allowed);
}

TEST(Authentication, extension_handling_is_confined_to_the_terminal_segment) {
  const std::array<std::string_view, 1> paths{{"/v1"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_V1"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("intermediate_dot.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  // The policy on /v1 gates its own subtree
  EXPECT_FALSE(authentication.admits("/v1", "").allowed);
  EXPECT_FALSE(authentication.admits("/v1/secret", "").allowed);
  // As a terminal segment, /v1.0 is a representation of /v1 under the
  // content-negotiation rule, the same way /person.json represents /person
  EXPECT_FALSE(authentication.admits("/v1.0", "").allowed);
  // But as an intermediate segment it is a distinct directory that does not
  // descend into the /v1 subtree, so its children are uncovered and public
  EXPECT_TRUE(authentication.admits("/v1.0/secret", "").allowed);
}

TEST(Authentication, base_path_is_stripped_before_matching) {
  setenv("ONE_TEST_KEY_BASE", "base-secret", 1);
  const std::array<std::string_view, 1> apikey_paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_BASE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{apikey_paths, keys}}};
  const auto path{test_path("base_path.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  // With the base path stripped, the registry path under it is matched. The
  // uncovered public path is admitted, the covered one is gated
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
  // path is covered by no policy and is public
  EXPECT_TRUE(
      authentication.admits("/registryextra/public/string", "", "/registry")
          .allowed);

  // A request outside the base path is left in place and covered by no policy
  EXPECT_TRUE(authentication.admits("/elsewhere/public/string", "", "/registry")
                  .allowed);
}

TEST(Authentication, apikey_admits_matching_credential) {
  setenv("ONE_TEST_KEY_MATCH", "secret-match", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MATCH"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("apikey_match.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
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
      {{paths, keys}}};
  const auto path{test_path("apikey_multi.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_TRUE(authentication.admits("/internal/foo", "key-a").allowed);
  EXPECT_TRUE(authentication.admits("/internal/foo", "key-b").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "key-c").allowed);
}

TEST(Authentication, apikey_with_unset_variable_denies) {
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_UNSET"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("apikey_unset.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.admits("/internal/foo", "anything").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
}

TEST(Authentication, sha256_policy_admits_the_matching_credential) {
  const std::string raw{"raw-secret-key"};
  setenv("ONE_TEST_KEY_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHA"}};
  const std::array<std::string_view, 1> paths{{"/secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys, sourcemeta::one::Authentication::Algorithm::Sha256}}};
  const auto path{test_path("sha256_match.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_TRUE(authentication.admits("/secret/foo", raw).allowed);
  EXPECT_FALSE(authentication.admits("/secret/foo", "wrong").allowed);
  EXPECT_FALSE(authentication.admits("/secret/foo", "").allowed);
  // Presenting the stored hash itself does not authenticate
  EXPECT_FALSE(
      authentication.admits("/secret/foo", sourcemeta::core::sha256(raw))
          .allowed);
}

TEST(Authentication, mixed_algorithms_admit_either_key_with_identity_first) {
  setenv("ONE_TEST_KEY_MIXA_ID", "plain-a", 1);
  const std::string raw{"hashed-a"};
  setenv("ONE_TEST_KEY_MIXA_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> paths{{"/mixed"}};
  const std::array<std::string_view, 1> identity_keys{{"ONE_TEST_KEY_MIXA_ID"}};
  const std::array<std::string_view, 1> sha256_keys{{"ONE_TEST_KEY_MIXA_SHA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{paths, identity_keys,
        sourcemeta::one::Authentication::Algorithm::Identity},
       {paths, sha256_keys,
        sourcemeta::one::Authentication::Algorithm::Sha256}}};
  const auto path{test_path("mixed_identity_first.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  // Either key type opens the path regardless of declaration order. The sha256
  // key must work even though the identity policy is checked first and fails
  EXPECT_TRUE(authentication.admits("/mixed/x", "plain-a").allowed);
  EXPECT_TRUE(authentication.admits("/mixed/x", raw).allowed);
  EXPECT_FALSE(authentication.admits("/mixed/x", "neither").allowed);
}

TEST(Authentication, mixed_algorithms_admit_either_key_with_sha256_first) {
  setenv("ONE_TEST_KEY_MIXB_ID", "plain-b", 1);
  const std::string raw{"hashed-b"};
  setenv("ONE_TEST_KEY_MIXB_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> paths{{"/mixed"}};
  const std::array<std::string_view, 1> identity_keys{{"ONE_TEST_KEY_MIXB_ID"}};
  const std::array<std::string_view, 1> sha256_keys{{"ONE_TEST_KEY_MIXB_SHA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{paths, sha256_keys, sourcemeta::one::Authentication::Algorithm::Sha256},
       {paths, identity_keys,
        sourcemeta::one::Authentication::Algorithm::Identity}}};
  const auto path{test_path("mixed_sha256_first.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  // The identity key must work even though the sha256 policy is checked first
  EXPECT_TRUE(authentication.admits("/mixed/x", raw).allowed);
  EXPECT_TRUE(authentication.admits("/mixed/x", "plain-b").allowed);
  EXPECT_FALSE(authentication.admits("/mixed/x", "neither").allowed);
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
        {std::span<const std::string_view>{&path_views[index], 1}, {}});
  }

  const auto path{test_path("maximum_policies.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  // The keyless policies gate their scope with no key that can open it
  EXPECT_FALSE(authentication.admits("/p0/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("/p63/foo", "").allowed);
  // An uncovered path is public
  EXPECT_TRUE(authentication.admits("/missing", "").allowed);
}

TEST(Authentication, governing_returns_policy_indices_in_declaration_order) {
  const std::array<std::string_view, 1> root_paths{{"/"}};
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> root_keys{{"ONE_TEST_KEY_GR"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_GI"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{root_paths, root_keys}, {internal_paths, internal_keys}}};
  const auto path{test_path("governing.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_EQ(authentication.governing("/"), (std::vector<std::size_t>{0}));
  EXPECT_EQ(authentication.governing("/vendor"), (std::vector<std::size_t>{0}));
  EXPECT_EQ(authentication.governing("/internal"),
            (std::vector<std::size_t>{0, 1}));
  EXPECT_EQ(authentication.governing("/internal/foo"),
            (std::vector<std::size_t>{0, 1}));
}

TEST(Authentication, governing_of_an_ungoverned_path_is_empty) {
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_GE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{internal_paths, keys}}};
  const auto path{test_path("governing_empty.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_EQ(authentication.governing("/vendor"), (std::vector<std::size_t>{}));
  EXPECT_EQ(authentication.governing("/internal"),
            (std::vector<std::size_t>{0}));
}

TEST(Authentication, reference_through_a_broken_artifact_is_rejected) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_FALSE(authentication.reference_permitted("/open/one", "/open/two"));
  EXPECT_FALSE(authentication.reference_permitted("/open/one", "/secret/two"));
  EXPECT_FALSE(
      authentication.reference_permitted("/secret/one", "/secret/two"));
}

TEST(Authentication, reference_to_a_public_schema_is_permitted) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_PUBLIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{secret_paths, keys}}};
  const auto path{test_path("ref_to_public.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_TRUE(authentication.reference_permitted("/secret/one", "/open/two"));
  EXPECT_TRUE(authentication.reference_permitted("/open/one", "/open/two"));
}

TEST(Authentication, public_schema_referencing_an_apikey_schema_is_rejected) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_LEAK"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{secret_paths, keys}}};
  const auto path{test_path("ref_public_to_apikey.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.reference_permitted("/open/one", "/secret/two"));
}

TEST(Authentication, reference_within_the_same_policy_is_permitted) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_SAME"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("ref_same_policy.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
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
      {{alpha_paths, alpha_keys}, {beta_paths, beta_keys}}};
  const auto path{test_path("ref_disjoint.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
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
      {{broad_paths, broad_keys}, {nested_paths, nested_keys}}};
  const auto path{test_path("ref_narrow_to_wide.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_TRUE(authentication.reference_permitted("/p/one", "/p/inner/two"));
  EXPECT_FALSE(authentication.reference_permitted("/p/inner/two", "/p/one"));
}

// A locally signed RS256 access token (typ "at+jwt") with issuer "acme",
// audience "client", and an expiry far in the future, alongside the key set
// that verifies it and an unrelated key set that does not. Reused from the core
// JOSE test vectors so no signing happens here
static constexpr std::string_view SIGNED_TOKEN{
    "eyJhbGciOiJSUzI1NiIsInR5cCI6ImF0K2p3dCJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "U3ZBo7MvSW0U099gJ_"
    "vIA5T8HJ2XnKSzYmqkx7SDxgxQfmxQyu3QZIeKT68AAH7wQjWRvNWQ7f3Es57UUNUQAMs-"
    "z5TWlVBKtYZf5ZcbYqc4KrQ-ApwpjoFGJxurnd1R_"
    "tz02WssnvrZNKnxNPuGoYIkJKNCl59yLFJwRLf3nK_Jcxs-"
    "1m2MvKsm647PuXqhYOKlZkHOvkIV0RV8cLJ56_gDVjj7TlKQgwbTdW_"
    "71QLwLWRFGftU2EAWuqayTSpPeUA6kB4sfn7JNsweqDs7uev30m6y8BE9uzwzHuuovaN1cZz0o"
    "TAGXcx64sfbPs6HEMp5_FoU0SccxArAbnHSjA"};
static constexpr std::string_view SIGNED_KEYS{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "oHTpl-jfNfBuXmBp58sW8s_77UP6j2jA0mjjKjhDkxhp7Agk-xLNGgfPCS_bjdZ6YU6FGeab8uVjkSgo9_0OCJUaF4vzEGwXmNuGawANxnZtiYjWvbJlq-2mn_L7rsqGQcSkMmyM0g4aX7dF8wB6DVrXShJ78fcrNtpeoU72YGEdjehA8qVclDFwBdpCGynxxnWJePk72lQb6gkVMqKMc3jBF8GkWf8oP_sjss-fpOjSUMR1c8_0JlTYWO46KWOZa0EO2t8H1V3imMyzbhoxRd_qZHmo46gJkG-ZdebjX0vGQllaCwu0z4kLcXIfAZhqPEkdssDGhC_txwJuhaPDFQ", "e": "AQAB" } ] })JSON"};
static constexpr std::string_view UNRELATED_KEYS{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ", "e": "AQAB" } ] })JSON"};

static auto stub_fetcher(std::map<std::string, std::string> responses,
                         std::shared_ptr<int> calls)
    -> sourcemeta::one::Authentication::JWKSFetcher {
  return
      [responses = std::move(responses),
       calls = std::move(calls)](const std::string_view url)
          -> std::optional<sourcemeta::one::Authentication::JWKSFetchResult> {
        if (calls != nullptr) {
          *calls += 1;
        }

        const auto match{responses.find(std::string{url})};
        if (match == responses.cend()) {
          return std::nullopt;
        }

        return sourcemeta::one::Authentication::JWKSFetchResult{
            .body = match->second, .max_age = std::nullopt};
      };
}

TEST(Authentication, jwt_admits_a_valid_token_and_caches_the_key_set) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_valid.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         calls)};
  EXPECT_TRUE(authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
  EXPECT_FALSE(authentication.admits("/secure/x", "not-a-token").allowed);
  EXPECT_FALSE(authentication.admits("/secure/x", "").allowed);
  // A second valid request reuses the cached key set rather than refetching
  EXPECT_TRUE(authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
  EXPECT_EQ(*calls, 1);
}

TEST(Authentication, jwt_denies_a_token_for_the_wrong_audience) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "different",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_audience.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  EXPECT_FALSE(authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
}

TEST(Authentication, jwt_denies_a_token_from_the_wrong_issuer) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "different",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_issuer.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  EXPECT_FALSE(authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
}

TEST(Authentication, jwt_denies_a_disallowed_algorithm) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_algorithm.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  EXPECT_FALSE(authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
}

TEST(Authentication, jwt_denies_when_the_signing_key_is_absent) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_unknown_key.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path,
      stub_fetcher({{"https://idp.test/jwks", std::string{UNRELATED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
}

TEST(Authentication, jwt_denies_when_the_key_set_cannot_be_fetched) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_fetch_fails.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
}

TEST(Authentication, an_apikey_credential_never_triggers_a_jwt_fetch) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_no_fetch.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         calls)};
  EXPECT_FALSE(authentication.admits("/secure/x", "static-api-key").allowed);
  EXPECT_FALSE(authentication.admits("/secure/x", "").allowed);
  EXPECT_EQ(*calls, 0);
}

TEST(Authentication, jwt_resolves_the_key_set_through_discovery) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // No key set location is pinned, so it is discovered from the issuer
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://acme.test",
        .audience = "client",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_discovery.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path,
      stub_fetcher({{"https://acme.test/.well-known/openid-configuration",
                     R"JSON({ "jwks_uri": "https://acme.test/keys" })JSON"},
                    {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
                   nullptr)};
  // The issuer claim is "acme", independent of the discovery host
  const std::array<sourcemeta::one::Authentication::Policy, 1> issuer_policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .algorithms = algorithms}}};
  const auto issuer_path{test_path("jwt_discovery_issuer.bin")};
  sourcemeta::one::Authentication::save(issuer_policies, issuer_path,
                                        issuer_path);
  const sourcemeta::one::Authentication issuer_authentication{
      issuer_path,
      stub_fetcher({{"acme/.well-known/openid-configuration",
                     R"JSON({ "jwks_uri": "https://acme.test/keys" })JSON"},
                    {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(issuer_authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
}

TEST(Authentication, mixed_apikey_and_jwt_policies_admit_either_credential) {
  setenv("ONE_TEST_KEY_BOTH", "static-secret", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_BOTH"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths, .keys = keys},
       {.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_mixed.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  // The static key opens the path
  EXPECT_TRUE(authentication.admits("/both/x", "static-secret").allowed);
  // The token opens the path
  EXPECT_TRUE(authentication.admits("/both/x", SIGNED_TOKEN).allowed);
  // Neither a wrong key nor a wrong token opens it
  EXPECT_FALSE(authentication.admits("/both/x", "wrong").allowed);
}

TEST(Authentication, reference_rules_treat_a_jwt_scope_conservatively) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_reference.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  // Reference checks read only the policy, so no key set transport is needed
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // A public schema may not reference one behind the token scope
  EXPECT_FALSE(authentication.reference_permitted("/open/one", "/secure/two"));
  // The token scope may reference a public schema, and itself
  EXPECT_TRUE(authentication.reference_permitted("/secure/one", "/open/two"));
  EXPECT_TRUE(authentication.reference_permitted("/secure/one", "/secure/two"));
}
