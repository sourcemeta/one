#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::sys_seconds, std::chrono::seconds
#include <cstddef>     // std::byte, std::size_t
#include <cstdlib>     // setenv
#include <filesystem>  // std::filesystem::path
#include <fstream>     // std::ofstream, std::fstream
#include <map>         // std::map
#include <memory>      // std::shared_ptr, std::make_shared
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{AUTHENTICATION_TEST_DIRECTORY} / name;
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

// A session expiry far enough in the future to outlive any test run
static constexpr std::chrono::sys_seconds SESSION_EXPIRY{
    std::chrono::seconds{2000000000}};

// An interactive policy names the environment variable holding the secret
// that signs its session and transaction cookies, so the tests set that
// variable and mint cookies under its value
static constexpr const char *SESSION_SECRET_VARIABLE{"ONE_TEST_SESSION_SECRET"};
static constexpr std::string_view SESSION_SECRET{"session-secret"};

static auto stub_fetcher(std::map<std::string, std::string> responses,
                         std::shared_ptr<int> calls)
    -> sourcemeta::core::JWKSProvider::Fetcher {
  return [responses = std::move(responses),
          calls = std::move(calls)](const std::string_view url)
             -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
    if (calls != nullptr) {
      *calls += 1;
    }

    const auto match{responses.find(std::string{url})};
    if (match == responses.cend()) {
      return std::nullopt;
    }

    return sourcemeta::core::JWKSProvider::FetchResult{.body = match->second,
                                                       .max_age = std::nullopt};
  };
}

TEST(missing_artifact_denies_everything) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"},
      stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/acme/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("", "").allowed);
}

TEST(malformed_artifact_denies_everything) {
  const auto path{test_path("malformed.bin")};
  std::ofstream stream{path, std::ios::binary};
  const std::array<char, 64> garbage{};
  stream.write(garbage.data(), garbage.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/acme/foo", "").allowed);
}

TEST(structurally_corrupt_artifact_denies_everything) {
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

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
}

TEST(artifact_exceeding_the_policy_ceiling_denies_everything) {
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

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/", "").allowed);
  EXPECT_FALSE(authentication.admits("/acme/foo", "").allowed);
}

TEST(corrupted_section_offset_denies_everything) {
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

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("/", "").allowed);
}

TEST(zero_policies_admits_every_path) {
  const std::array<sourcemeta::one::Authentication::Policy, 0> policies{};
  const auto path{test_path("zero_policies.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("", "").allowed);
  EXPECT_TRUE(authentication.admits("/acme/foo/bar", "").allowed);
  EXPECT_EQ(authentication.governing("/"), (std::vector<std::size_t>{}));
  EXPECT_EQ(authentication.governing("/acme"), (std::vector<std::size_t>{}));
}

TEST(uncovered_paths_are_public_around_a_gated_scope) {
  setenv("ONE_TEST_KEY_SCOPE", "scope-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SCOPE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("uncovered_public.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
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

TEST(scope_matches_whole_segments_only) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SEGMENT"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("segment_boundary.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The scope gates its own segment
  EXPECT_FALSE(authentication.admits("/internal", "").allowed);
  // A textual prefix that is not a whole segment is a different path, so it is
  // uncovered and public
  EXPECT_TRUE(authentication.admits("/internalish", "").allowed);
  EXPECT_TRUE(authentication.admits("/int", "").allowed);
  EXPECT_TRUE(authentication.admits("/internal-team", "").allowed);
}

TEST(distinct_policies_each_gate_their_scope) {
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

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/alpha/one", "").allowed);
  EXPECT_FALSE(authentication.admits("/beta/two", "").allowed);
  EXPECT_FALSE(authentication.admits("/gamma/three", "").allowed);
  // Between the scopes the registry is public
  EXPECT_TRUE(authentication.admits("/delta", "").allowed);
}

TEST(nested_prefixes_gate_their_subtrees) {
  const std::array<std::string_view, 1> internal{{"/internal"}};
  const std::array<std::string_view, 1> secret{{"/internal/secret"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_NI"}};
  const std::array<std::string_view, 1> secret_keys{{"ONE_TEST_KEY_NS"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{internal, internal_keys}, {secret, secret_keys}}};
  const auto path{test_path("nested_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/internal", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/other", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/secret", "").allowed);
  EXPECT_FALSE(authentication.admits("/internal/secret/deep", "").allowed);
  EXPECT_TRUE(authentication.admits("/", "").allowed);
  EXPECT_TRUE(authentication.admits("/public", "").allowed);
}

TEST(nested_inner_key_widens_access) {
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

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The inner path is covered by both, so either key admits it
  EXPECT_TRUE(authentication.admits("/internal/secret", "wo-secret").allowed);
  EXPECT_TRUE(authentication.admits("/internal/secret", "wi-secret").allowed);
  // The outer path is covered only by the outer policy
  EXPECT_TRUE(authentication.admits("/internal/other", "wo-secret").allowed);
  EXPECT_FALSE(authentication.admits("/internal/other", "wi-secret").allowed);
}

TEST(single_policy_with_multiple_prefixes) {
  const std::array<std::string_view, 2> paths{{"/internal", "/vendor"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MP"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("multiple_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("/vendor/bar", "").allowed);
  EXPECT_TRUE(authentication.admits("/public", "").allowed);
}

TEST(extensionless_policy_gates_every_representation) {
  setenv("ONE_TEST_KEY_REPRESENTATION", "representation-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_REPRESENTATION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("representation_agnostic.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
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

TEST(extension_specific_policy_gates_only_that_representation) {
  setenv("ONE_TEST_KEY_SPECIFIC", "specific-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data.json"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SPECIFIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("representation_specific.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
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

TEST(extension_handling_is_confined_to_the_terminal_segment) {
  const std::array<std::string_view, 1> paths{{"/v1"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_V1"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("intermediate_dot.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
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

TEST(base_path_is_stripped_before_matching) {
  setenv("ONE_TEST_KEY_BASE", "base-secret", 1);
  const std::array<std::string_view, 1> apikey_paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_BASE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{apikey_paths, keys}}};
  const auto path{test_path("base_path.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // With the base path stripped, the registry path under it is matched. The
  // uncovered public path is admitted, the covered one is gated
  EXPECT_TRUE(
      authentication.admits("/registry/public/string", "", {}, "/registry")
          .allowed);
  EXPECT_FALSE(
      authentication.admits("/registry/private/secret", "", {}, "/registry")
          .allowed);
  EXPECT_TRUE(
      authentication
          .admits("/registry/private/secret", "base-secret", {}, "/registry")
          .allowed);

  // An empty base path strips nothing
  EXPECT_TRUE(authentication.admits("/public/string", "", "").allowed);
  EXPECT_FALSE(authentication.admits("/private/secret", "", "").allowed);

  // A base path that is not a whole-segment prefix is left in place, so the
  // path is covered by no policy and is public
  EXPECT_TRUE(
      authentication.admits("/registryextra/public/string", "", {}, "/registry")
          .allowed);

  // A request outside the base path is left in place and covered by no policy
  EXPECT_TRUE(
      authentication.admits("/elsewhere/public/string", "", {}, "/registry")
          .allowed);
}

TEST(apikey_admits_matching_credential) {
  setenv("ONE_TEST_KEY_MATCH", "secret-match", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MATCH"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("apikey_match.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.admits("/internal/foo", "secret-match").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "wrong").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
}

TEST(apikey_with_multiple_keys_admits_any) {
  setenv("ONE_TEST_KEY_MULTI_A", "key-a", 1);
  setenv("ONE_TEST_KEY_MULTI_B", "key-b", 1);
  const std::array<std::string_view, 2> keys{
      {"ONE_TEST_KEY_MULTI_A", "ONE_TEST_KEY_MULTI_B"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("apikey_multi.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.admits("/internal/foo", "key-a").allowed);
  EXPECT_TRUE(authentication.admits("/internal/foo", "key-b").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "key-c").allowed);
}

TEST(apikey_with_unset_variable_denies) {
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_UNSET"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("apikey_unset.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits("/internal/foo", "anything").allowed);
  EXPECT_FALSE(authentication.admits("/internal/foo", "").allowed);
}

TEST(sha256_policy_admits_the_matching_credential) {
  const std::string raw{"raw-secret-key"};
  setenv("ONE_TEST_KEY_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHA"}};
  const std::array<std::string_view, 1> paths{{"/secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys, sourcemeta::one::Authentication::Algorithm::Sha256}}};
  const auto path{test_path("sha256_match.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.admits("/secret/foo", raw).allowed);
  EXPECT_FALSE(authentication.admits("/secret/foo", "wrong").allowed);
  EXPECT_FALSE(authentication.admits("/secret/foo", "").allowed);
  // Presenting the stored hash itself does not authenticate
  EXPECT_FALSE(
      authentication.admits("/secret/foo", sourcemeta::core::sha256(raw))
          .allowed);
}

TEST(mixed_algorithms_admit_either_key_with_identity_first) {
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

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // Either key type opens the path regardless of declaration order. The sha256
  // key must work even though the identity policy is checked first and fails
  EXPECT_TRUE(authentication.admits("/mixed/x", "plain-a").allowed);
  EXPECT_TRUE(authentication.admits("/mixed/x", raw).allowed);
  EXPECT_FALSE(authentication.admits("/mixed/x", "neither").allowed);
}

TEST(mixed_algorithms_admit_either_key_with_sha256_first) {
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

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The identity key must work even though the sha256 policy is checked first
  EXPECT_TRUE(authentication.admits("/mixed/x", raw).allowed);
  EXPECT_TRUE(authentication.admits("/mixed/x", "plain-b").allowed);
  EXPECT_FALSE(authentication.admits("/mixed/x", "neither").allowed);
}

TEST(supports_the_maximum_number_of_policies) {
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

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The keyless policies gate their scope with no key that can open it
  EXPECT_FALSE(authentication.admits("/p0/foo", "").allowed);
  EXPECT_FALSE(authentication.admits("/p63/foo", "").allowed);
  // An uncovered path is public
  EXPECT_TRUE(authentication.admits("/missing", "").allowed);
}

TEST(governing_returns_policy_indices_in_declaration_order) {
  const std::array<std::string_view, 1> root_paths{{"/"}};
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> root_keys{{"ONE_TEST_KEY_GR"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_GI"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{root_paths, root_keys}, {internal_paths, internal_keys}}};
  const auto path{test_path("governing.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.governing("/"), (std::vector<std::size_t>{0}));
  EXPECT_EQ(authentication.governing("/vendor"), (std::vector<std::size_t>{0}));
  EXPECT_EQ(authentication.governing("/internal"),
            (std::vector<std::size_t>{0, 1}));
  EXPECT_EQ(authentication.governing("/internal/foo"),
            (std::vector<std::size_t>{0, 1}));
}

TEST(governing_of_an_ungoverned_path_is_empty) {
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_GE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{internal_paths, keys}}};
  const auto path{test_path("governing_empty.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.governing("/vendor"), (std::vector<std::size_t>{}));
  EXPECT_EQ(authentication.governing("/internal"),
            (std::vector<std::size_t>{0}));
}

TEST(reference_through_a_broken_artifact_is_rejected) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"},
      stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.reference_permitted("/open/one", "/open/two"));
  EXPECT_FALSE(authentication.reference_permitted("/open/one", "/secret/two"));
  EXPECT_FALSE(
      authentication.reference_permitted("/secret/one", "/secret/two"));
}

TEST(reference_to_a_public_schema_is_permitted) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_PUBLIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{secret_paths, keys}}};
  const auto path{test_path("ref_to_public.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.reference_permitted("/secret/one", "/open/two"));
  EXPECT_TRUE(authentication.reference_permitted("/open/one", "/open/two"));
}

TEST(public_schema_referencing_an_apikey_schema_is_rejected) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_LEAK"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{secret_paths, keys}}};
  const auto path{test_path("ref_public_to_apikey.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.reference_permitted("/open/one", "/secret/two"));
}

TEST(reference_within_the_same_policy_is_permitted) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_SAME"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("ref_same_policy.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.reference_permitted("/internal/one", "/internal/two"));
  EXPECT_TRUE(
      authentication.reference_permitted("/internal/one", "/internal/one"));
}

TEST(reference_across_disjoint_policies_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_REF_ALPHA"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_REF_BETA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{alpha_paths, alpha_keys}, {beta_paths, beta_keys}}};
  const auto path{test_path("ref_disjoint.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.reference_permitted("/alpha/one", "/beta/two"));
  EXPECT_FALSE(authentication.reference_permitted("/beta/two", "/alpha/one"));
}

TEST(reference_from_a_narrower_to_a_wider_audience_is_permitted) {
  const std::array<std::string_view, 1> broad_paths{{"/p"}};
  const std::array<std::string_view, 1> nested_paths{{"/p/inner"}};
  const std::array<std::string_view, 1> broad_keys{{"ONE_TEST_REF_BROAD"}};
  const std::array<std::string_view, 1> nested_keys{{"ONE_TEST_REF_NESTED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{broad_paths, broad_keys}, {nested_paths, nested_keys}}};
  const auto path{test_path("ref_narrow_to_wide.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.reference_permitted("/p/one", "/p/inner/two"));
  EXPECT_FALSE(authentication.reference_permitted("/p/inner/two", "/p/one"));
}

TEST(jwt_admits_a_valid_token_and_caches_the_key_set) {
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

TEST(jwt_denies_a_token_for_the_wrong_audience) {
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

TEST(jwt_denies_a_token_from_the_wrong_issuer) {
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

TEST(jwt_denies_a_disallowed_algorithm) {
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

TEST(jwt_denies_when_the_signing_key_is_absent) {
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

TEST(jwt_denies_when_the_key_set_cannot_be_fetched) {
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

TEST(an_apikey_credential_never_triggers_a_jwt_fetch) {
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

TEST(jwt_resolves_the_key_set_through_discovery) {
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
      stub_fetcher(
          {{"https://acme.test/.well-known/openid-configuration",
            R"JSON({ "issuer": "https://acme.test", "jwks_uri": "https://acme.test/keys" })JSON"},
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
      stub_fetcher(
          {{"acme/.well-known/openid-configuration",
            R"JSON({ "issuer": "acme", "jwks_uri": "https://acme.test/keys" })JSON"},
           {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
          nullptr)};
  EXPECT_TRUE(issuer_authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
}

TEST(mixed_apikey_and_jwt_policies_admit_either_credential) {
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

TEST(oidc_policy_admits_no_presented_credential) {
  setenv("ONE_TEST_OIDC_DENY", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_DENY",
        .name = "okta",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"}}};
  const auto path{test_path("oidc_deny.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  // The provider is reachable and would verify the token, yet no presented
  // credential opens the path, not even one the equivalent token policy
  // would accept, and the provider is never contacted
  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      path,
      stub_fetcher({{"acme/.well-known/openid-configuration",
                     R"JSON({ "jwks_uri": "https://acme.test/keys" })JSON"},
                    {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
                   calls)};

  const auto empty_verdict{authentication.admits("/portal/x", "")};
  EXPECT_FALSE(empty_verdict.allowed);
  EXPECT_FALSE(empty_verdict.principal.has_value());

  const auto secret_verdict{authentication.admits("/portal/x", "confidential")};
  EXPECT_FALSE(secret_verdict.allowed);
  EXPECT_FALSE(secret_verdict.principal.has_value());

  const auto token_verdict{authentication.admits("/portal/x", SIGNED_TOKEN)};
  EXPECT_FALSE(token_verdict.allowed);
  EXPECT_FALSE(token_verdict.principal.has_value());

  EXPECT_EQ(*calls, 0);
}

TEST(union_of_an_apikey_and_an_oidc_policy_admits_only_the_key) {
  setenv("ONE_TEST_KEY_OIDC_UNION", "union-secret", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_OIDC_UNION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths, .keys = keys},
       {.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_KEY_OIDC_UNION",
        .name = "okta",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"}}};
  const auto path{test_path("oidc_union.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto key_verdict{authentication.admits("/both/x", "union-secret")};
  EXPECT_TRUE(key_verdict.allowed);
  EXPECT_TRUE(key_verdict.principal.has_value());
  EXPECT_EQ(key_verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::ApiKey);
  EXPECT_EQ(key_verdict.principal.value().policy, std::size_t{0});

  const auto token_verdict{authentication.admits("/both/x", SIGNED_TOKEN)};
  EXPECT_FALSE(token_verdict.allowed);
  EXPECT_FALSE(token_verdict.principal.has_value());
}

TEST(oidc_policy_admits_its_session_cookie) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SESSION",
        .name = "okta",
        .session_secret_variable = SESSION_SECRET_VARIABLE}}};
  const auto path{test_path("oidc_session.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{path,
                                                       stub_fetcher({}, calls)};

  const auto sealed{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta", "subject": "jane@acme.test" })JSON",
      SESSION_SECRET, SESSION_EXPIRY)};
  const std::string cookies{"theme=dark; sourcemeta_one_session_okta=" +
                            sealed};

  const auto verdict{authentication.admits("/portal/x", "", cookies)};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.principal.has_value());
  EXPECT_EQ(verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::OIDC);
  EXPECT_EQ(verdict.principal.value().policy, std::size_t{0});
  EXPECT_EQ(*calls, 0);

  const auto anonymous_verdict{authentication.admits("/portal/x", "")};
  EXPECT_FALSE(anonymous_verdict.allowed);
  EXPECT_FALSE(anonymous_verdict.principal.has_value());
}

TEST(session_cookie_is_bound_to_the_policy_it_was_minted_for) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_BIND_A",
        .name = "okta",
        .session_secret_variable = SESSION_SECRET_VARIABLE},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_BIND_B",
        .name = "google",
        .session_secret_variable = SESSION_SECRET_VARIABLE}}};
  const auto path{test_path("oidc_session_bound.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto sealed{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta" })JSON", SESSION_SECRET, SESSION_EXPIRY)};

  // The session opens the path its policy governs
  const std::string okta_cookies{"sourcemeta_one_session_okta=" + sealed};
  EXPECT_TRUE(authentication.admits("/alpha/x", "", okta_cookies).allowed);

  // The same session does not open a path governed by another policy
  EXPECT_FALSE(authentication.admits("/beta/x", "", okta_cookies).allowed);

  // Nor does re-presenting the value under the other policy's cookie name
  const std::string renamed_cookies{"sourcemeta_one_session_google=" + sealed};
  EXPECT_FALSE(authentication.admits("/beta/x", "", renamed_cookies).allowed);
}

TEST(expired_session_cookie_is_denied) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_EXPIRED",
        .name = "okta",
        .session_secret_variable = SESSION_SECRET_VARIABLE}}};
  const auto path{test_path("oidc_session_expired.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const std::chrono::sys_seconds past{std::chrono::seconds{1000}};
  const auto sealed{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta" })JSON", SESSION_SECRET, past)};
  const std::string cookies{"sourcemeta_one_session_okta=" + sealed};
  EXPECT_FALSE(authentication.admits("/portal/x", "", cookies).allowed);
}

TEST(forged_session_cookie_is_denied) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_FORGED",
        .name = "okta",
        .session_secret_variable = SESSION_SECRET_VARIABLE}}};
  const auto path{test_path("oidc_session_forged.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  // A value sealed under a secret this policy does not hold
  const auto foreign{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta" })JSON", "other-secret", SESSION_EXPIRY)};
  const std::string foreign_cookies{"sourcemeta_one_session_okta=" + foreign};
  EXPECT_FALSE(authentication.admits("/portal/x", "", foreign_cookies).allowed);

  // A value whose signature no longer matches its contents
  auto tampered{sourcemeta::one::session_seal(R"JSON({ "policy": "okta" })JSON",
                                              SESSION_SECRET, SESSION_EXPIRY)};
  tampered.back() = tampered.back() == 'A' ? 'B' : 'A';
  const std::string tampered_cookies{"sourcemeta_one_session_okta=" + tampered};
  EXPECT_FALSE(
      authentication.admits("/portal/x", "", tampered_cookies).allowed);

  // A value that is not a sealed session at all
  EXPECT_FALSE(
      authentication
          .admits("/portal/x", "", "sourcemeta_one_session_okta=garbage")
          .allowed);
}

TEST(session_payload_must_declare_its_policy) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_PAYLOAD",
        .name = "okta",
        .session_secret_variable = SESSION_SECRET_VARIABLE}}};
  const auto path{test_path("oidc_session_payload.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const std::array<std::string_view, 4> payloads{
      {"not json", "[ 1, 2 ]", R"JSON({ "subject": "jane" })JSON",
       R"JSON({ "policy": "google" })JSON"}};
  for (const auto payload : payloads) {
    const auto sealed{
        sourcemeta::one::session_seal(payload, SESSION_SECRET, SESSION_EXPIRY)};
    const std::string cookies{"sourcemeta_one_session_okta=" + sealed};
    EXPECT_FALSE(authentication.admits("/portal/x", "", cookies).allowed);
  }
}

TEST(session_cookie_without_a_configured_secret_is_denied) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  // The session secret variable is deliberately never set in the environment
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_NO_SECRETS",
        .name = "okta",
        .session_secret_variable = "ONE_TEST_OIDC_UNSET_SECRET"}}};
  const auto path{test_path("oidc_session_no_secrets.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto sealed{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta" })JSON", SESSION_SECRET, SESSION_EXPIRY)};
  const std::string cookies{"sourcemeta_one_session_okta=" + sealed};
  EXPECT_FALSE(authentication.admits("/portal/x", "", cookies).allowed);
}

TEST(session_admitted_under_a_rotated_secret) {
  // The secret variable holds the newest secret first, then the one it
  // replaces, so a cookie signed under the old secret still verifies
  setenv("ONE_TEST_OIDC_ROTATED_SECRET", "new-secret\nold-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_ROTATED",
        .name = "okta",
        .session_secret_variable = "ONE_TEST_OIDC_ROTATED_SECRET"}}};
  const auto path{test_path("oidc_session_rotated.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  // A cookie signed under the older secret is still admitted
  const auto old_sealed{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta" })JSON", "old-secret", SESSION_EXPIRY)};
  EXPECT_TRUE(
      authentication
          .admits("/portal/x", "", "sourcemeta_one_session_okta=" + old_sealed)
          .allowed);

  // A fresh login mints under the newest secret alone, so the minted value
  // verifies under a new-only secret set and not under an old-only one
  const auto minted{authentication.seal(
      "okta", R"JSON({ "policy": "okta" })JSON", SESSION_EXPIRY)};
  EXPECT_TRUE(minted.has_value());
  const std::chrono::sys_seconds now{std::chrono::seconds{1000000}};
  const std::array<std::string_view, 1> new_only{{"new-secret"}};
  EXPECT_TRUE(
      sourcemeta::one::session_open(minted.value(), new_only, now).has_value());
  const std::array<std::string_view, 1> old_only{{"old-secret"}};
  EXPECT_FALSE(
      sourcemeta::one::session_open(minted.value(), old_only, now).has_value());

  // A secret no longer in the set is rejected
  const auto retired{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta" })JSON", "retired-secret", SESSION_EXPIRY)};
  EXPECT_FALSE(
      authentication
          .admits("/portal/x", "", "sourcemeta_one_session_okta=" + retired)
          .allowed);
}

TEST(session_with_a_blank_configured_secret_is_denied) {
  setenv("ONE_TEST_OIDC_BLANK_SECRET", "", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_BLANK",
        .name = "okta",
        .session_secret_variable = "ONE_TEST_OIDC_BLANK_SECRET"}}};
  const auto path{test_path("oidc_session_blank.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  // A blank secret would let anyone forge sessions, so it never verifies one
  const auto forged{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta" })JSON", "", SESSION_EXPIRY)};
  const std::string cookies{"sourcemeta_one_session_okta=" + forged};
  EXPECT_FALSE(authentication.admits("/portal/x", "", cookies).allowed);
}

TEST(save_rejects_a_nameless_interactive_policy) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_NAMELESS"}}};
  const auto path{test_path("oidc_nameless.bin")};
  try {
    sourcemeta::one::Authentication::save(policies, path, path);
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(),
                 "Interactive authentication policies require a name");
  }
}

TEST(union_of_an_apikey_and_an_oidc_policy_admits_key_or_session) {
  setenv("ONE_TEST_KEY_SESSION_UNION", "union-key", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SESSION_UNION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths, .keys = keys},
       {.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SESSION_UNION",
        .name = "okta",
        .session_secret_variable = SESSION_SECRET_VARIABLE}}};
  const auto path{test_path("oidc_session_union.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto key_verdict{authentication.admits("/both/x", "union-key")};
  EXPECT_TRUE(key_verdict.allowed);
  EXPECT_TRUE(key_verdict.principal.has_value());
  EXPECT_EQ(key_verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::ApiKey);
  EXPECT_EQ(key_verdict.principal.value().policy, std::size_t{0});

  const auto sealed{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta" })JSON", SESSION_SECRET, SESSION_EXPIRY)};
  const std::string cookies{"sourcemeta_one_session_okta=" + sealed};
  const auto session_verdict{authentication.admits("/both/x", "", cookies)};
  EXPECT_TRUE(session_verdict.allowed);
  EXPECT_TRUE(session_verdict.principal.has_value());
  EXPECT_EQ(session_verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::OIDC);
  EXPECT_EQ(session_verdict.principal.value().policy, std::size_t{1});

  EXPECT_FALSE(authentication.admits("/both/x", "").allowed);
}

TEST(session_cookie_does_not_open_an_apikey_path) {
  setenv("ONE_TEST_KEY_NO_SESSION", "key-only", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NO_SESSION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("oidc_session_apikey.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto sealed{sourcemeta::one::session_seal(
      R"JSON({ "policy": "okta" })JSON", SESSION_SECRET, SESSION_EXPIRY)};
  const std::string cookies{"sourcemeta_one_session_okta=" + sealed};
  EXPECT_FALSE(authentication.admits("/internal/x", "", cookies).allowed);
}

TEST(interactive_returns_the_policy_by_name) {
  setenv("ONE_TEST_KEY_INTERACTIVE", "key-value", 1);
  const std::array<std::string_view, 1> key_paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_INTERACTIVE"}};
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{key_paths, keys},
       {.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_LOOKUP_A",
        .name = "okta",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://accounts.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_LOOKUP_B",
        .name = "google",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"}}};
  const auto path{test_path("oidc_lookup.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto okta{authentication.interactive("okta")};
  EXPECT_TRUE(okta.has_value());
  EXPECT_EQ(okta.value().issuer, "https://login.test");
  EXPECT_EQ(okta.value().client_id, "registry");
  EXPECT_EQ(okta.value().client_secret_variable, "ONE_TEST_OIDC_LOOKUP_A");
  EXPECT_EQ(okta.value().default_path, "/alpha");

  const auto google{authentication.interactive("google")};
  EXPECT_TRUE(google.has_value());
  EXPECT_EQ(google.value().issuer, "https://accounts.test");
  EXPECT_EQ(google.value().client_id, "dashboard");
  EXPECT_EQ(google.value().client_secret_variable, "ONE_TEST_OIDC_LOOKUP_B");
  EXPECT_EQ(google.value().default_path, "/beta");

  EXPECT_FALSE(authentication.interactive("github").has_value());
  EXPECT_FALSE(authentication.interactive("").has_value());
}

TEST(interactive_default_path_is_the_first_path_declared) {
  // Declared out of alphabetical order, so that the first declared path wins
  // rather than the first sorted one
  const std::array<std::string_view, 2> paths{{"/zeta", "/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_MULTI",
        .name = "okta",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"}}};
  const auto path{test_path("oidc_default_path.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto okta{authentication.interactive("okta")};
  EXPECT_TRUE(okta.has_value());
  EXPECT_EQ(okta.value().default_path, "/zeta");
}

TEST(interactive_through_a_broken_artifact_is_empty) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"},
      stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.interactive("okta").has_value());
}

TEST(seal_and_open_round_trip_under_the_policy_secret) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SEAL_A",
        .name = "okta",
        .session_secret_variable = SESSION_SECRET_VARIABLE},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SEAL_B",
        .name = "google",
        .session_secret_variable = "ONE_TEST_OIDC_SEAL_OTHER"}}};
  const auto path{test_path("oidc_seal.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  // The other policy holds its own, distinct secret
  setenv("ONE_TEST_OIDC_SEAL_OTHER", "another-secret", 1);
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{authentication.seal("okta", "the-payload", SESSION_EXPIRY)};
  EXPECT_TRUE(sealed.has_value());
  const auto payload{authentication.open("okta", sealed.value())};
  EXPECT_TRUE(payload.has_value());
  EXPECT_EQ(payload.value(), "the-payload");

  // A value opened under a different policy, holding a different secret, does
  // not verify
  EXPECT_FALSE(authentication.open("google", sealed.value()).has_value());

  // An unknown policy seals and opens nothing
  EXPECT_FALSE(
      authentication.seal("github", "the-payload", SESSION_EXPIRY).has_value());
  EXPECT_FALSE(authentication.open("github", sealed.value()).has_value());
}

TEST(seal_without_a_configured_secret_produces_nothing) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  // The session secret variable is deliberately never set in the environment
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SEAL_NONE",
        .name = "okta",
        .session_secret_variable = "ONE_TEST_OIDC_SEAL_NONE_SECRET"}}};
  const auto path{test_path("oidc_seal_none.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.seal("okta", "the-payload", SESSION_EXPIRY).has_value());
  EXPECT_FALSE(authentication.open("okta", "anything").has_value());
}

TEST(open_rejects_an_expired_value) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SEAL_EXPIRED",
        .name = "okta",
        .session_secret_variable = SESSION_SECRET_VARIABLE}}};
  const auto path{test_path("oidc_seal_expired.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const std::chrono::sys_seconds past{std::chrono::seconds{1000}};
  const auto sealed{authentication.seal("okta", "the-payload", past)};
  EXPECT_TRUE(sealed.has_value());
  EXPECT_FALSE(authentication.open("okta", sealed.value()).has_value());
}

TEST(reference_within_the_same_oidc_scope_is_permitted) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // The two policies differ in name and in the environment variable holding
  // the secret, neither of which affects who can authenticate, so the scopes
  // stay equal
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_REF_SAME",
        .name = "alpha",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_REF_SAME_OTHER",
        .name = "beta",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"}}};
  const auto path{test_path("oidc_ref_same.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.reference_permitted("/alpha/one", "/beta/two"));
  EXPECT_TRUE(authentication.reference_permitted("/beta/two", "/alpha/one"));
}

TEST(reference_across_distinct_oidc_clients_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_REF_ALPHA",
        .name = "alpha",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_BETA",
        .name = "beta",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"}}};
  const auto path{test_path("oidc_ref_distinct.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.reference_permitted("/alpha/one", "/beta/two"));
  EXPECT_FALSE(authentication.reference_permitted("/beta/two", "/alpha/one"));
}

TEST(reference_across_swapped_oidc_identities_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // One policy's issuer is the other's client identifier and vice versa, so
  // the scopes share both strings yet denote different provider clients
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_REF_SWAP_ALPHA",
        .name = "alpha",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "registry",
        .client_id = "https://login.test",
        .client_secret_variable = "ONE_TEST_OIDC_REF_SWAP_BETA",
        .name = "beta",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"}}};
  const auto path{test_path("oidc_ref_swapped.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.reference_permitted("/alpha/one", "/beta/two"));
  EXPECT_FALSE(authentication.reference_permitted("/beta/two", "/alpha/one"));
}

TEST(reference_mixing_identities_across_oidc_policies_is_rejected) {
  const std::array<std::string_view, 1> source_paths{{"/source"}};
  const std::array<std::string_view, 1> target_paths{{"/target"}};
  // The referrer pairs an issuer and a client identifier that the referent
  // only carries through two different policies, so no single referent scope
  // matches and the reference must not slip through their union
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = source_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://alpha.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_SOURCE",
        .name = "source",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"},
       {.paths = target_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://alpha.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_ONE",
        .name = "target-one",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"},
       {.paths = target_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://beta.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_TWO",
        .name = "target-two",
        .session_secret_variable = "ONE_TEST_OIDC_SESSION_UNUSED"}}};
  const auto path{test_path("oidc_ref_mixed.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted("/source/one", "/target/two"));
}

TEST(admission_by_an_apikey_policy_identifies_the_principal) {
  setenv("ONE_TEST_KEY_PRINCIPAL", "principal-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("principal_apikey.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto verdict{
      authentication.admits("/internal/foo", "principal-secret")};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.principal.has_value());
  EXPECT_EQ(verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::ApiKey);
  EXPECT_EQ(verdict.principal.value().policy, std::size_t{0});
}

TEST(admission_by_a_jwt_policy_identifies_the_principal) {
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
  const auto path{test_path("principal_jwt.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  const auto verdict{authentication.admits("/secure/foo", SIGNED_TOKEN)};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.principal.has_value());
  EXPECT_EQ(verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::JWT);
  EXPECT_EQ(verdict.principal.value().policy, std::size_t{0});
}

TEST(principal_identifies_the_admitting_policy_among_several) {
  setenv("ONE_TEST_KEY_PRINCIPAL_MIXED", "principal-mixed", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL_MIXED"}};
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
  const auto path{test_path("principal_mixed.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};

  const auto apikey_verdict{
      authentication.admits("/both/x", "principal-mixed")};
  EXPECT_TRUE(apikey_verdict.allowed);
  EXPECT_TRUE(apikey_verdict.principal.has_value());
  EXPECT_EQ(apikey_verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::ApiKey);
  EXPECT_EQ(apikey_verdict.principal.value().policy, std::size_t{0});

  const auto jwt_verdict{authentication.admits("/both/x", SIGNED_TOKEN)};
  EXPECT_TRUE(jwt_verdict.allowed);
  EXPECT_TRUE(jwt_verdict.principal.has_value());
  EXPECT_EQ(jwt_verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::JWT);
  EXPECT_EQ(jwt_verdict.principal.value().policy, std::size_t{1});
}

TEST(anonymous_and_denied_verdicts_carry_no_principal) {
  setenv("ONE_TEST_KEY_PRINCIPAL_NONE", "principal-none", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL_NONE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("principal_none.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  // An uncovered path admits an anonymous caller
  const auto anonymous_verdict{authentication.admits("/open/foo", "")};
  EXPECT_TRUE(anonymous_verdict.allowed);
  EXPECT_FALSE(anonymous_verdict.principal.has_value());

  // A denial identifies nobody
  const auto denied_verdict{authentication.admits("/internal/foo", "wrong")};
  EXPECT_FALSE(denied_verdict.allowed);
  EXPECT_FALSE(denied_verdict.principal.has_value());

  // A broken artifact denies with no principal either
  const sourcemeta::one::Authentication missing{
      std::filesystem::path{"/no/such/authentication.bin"},
      stub_fetcher({}, nullptr)};
  const auto missing_verdict{missing.admits("/internal/foo", "principal-none")};
  EXPECT_FALSE(missing_verdict.allowed);
  EXPECT_FALSE(missing_verdict.principal.has_value());
}

TEST(reference_rules_treat_a_jwt_scope_conservatively) {
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

TEST(jwt_without_a_transport_denies_rather_than_crashes) {
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
  const auto path{test_path("jwt_no_transport.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(authentication.admits("/secure/x", SIGNED_TOKEN).allowed);
}

TEST(jwt_policies_sharing_an_issuer_use_their_own_key_set) {
  const std::array<std::string_view, 1> primary_paths{{"/primary"}};
  const std::array<std::string_view, 1> secondary_paths{{"/secondary"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = primary_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/primary",
        .algorithms = algorithms},
       {.paths = secondary_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/secondary",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_shared_issuer.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher(
                {{"https://idp.test/primary", std::string{SIGNED_KEYS}},
                 {"https://idp.test/secondary", std::string{UNRELATED_KEYS}}},
                nullptr)};
  // The primary path is populated first, which under a per-issuer cache would
  // have leaked its key set to the secondary path
  EXPECT_TRUE(authentication.admits("/primary/x", SIGNED_TOKEN).allowed);
  EXPECT_FALSE(authentication.admits("/secondary/x", SIGNED_TOKEN).allowed);
}

TEST(reference_between_jwt_scopes_distinguishes_algorithms) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> gamma_paths{{"/gamma"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> ecdsa{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = ecdsa},
       {.paths = gamma_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa}}};
  const auto path{test_path("jwt_reference_algorithms.bin")};
  sourcemeta::one::Authentication::save(policies, path, path);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // Same issuer, audience, and key set but a different algorithm is a different
  // scope, so no token could satisfy the reference
  EXPECT_FALSE(authentication.reference_permitted("/alpha/one", "/beta/two"));
  // An identical policy is the same scope
  EXPECT_TRUE(authentication.reference_permitted("/alpha/one", "/gamma/two"));
}
