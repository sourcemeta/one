#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::sys_seconds, std::chrono::seconds
#include <cstddef>     // std::byte, std::size_t
#include <cstdlib>     // setenv
#include <filesystem>  // std::filesystem::path
#include <fstream>     // std::ofstream, std::fstream, std::ifstream
#include <iterator>    // std::istreambuf_iterator
#include <map>         // std::map
#include <memory>      // std::shared_ptr, std::make_shared
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

// Every gate question is asked about a canonical location, so the tests name
// one the same way a request would
static auto at(const std::string_view input)
    -> sourcemeta::one::Authentication::Path {
  return sourcemeta::one::Authentication::Path::parse(input,
                                                      "http://localhost:8000")
      .value();
}

// These tests name locations directly rather than modelling what an instance
// serves, so every path a policy is scoped to is one to gate
static auto anywhere(const std::string_view) -> bool { return true; }

// One cookie field, which is what a browser conforming to RFC 6265 sends
static auto fields(const std::string_view value)
    -> std::array<std::string_view, 1> {
  return {{value}};
}

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
    R"JSON({
      "keys": [
        {
          "kty": "RSA",
          "n": "oHTpl-jfNfBuXmBp58sW8s_77UP6j2jA0mjjKjhDkxhp7Agk-xLNGgfPCS_bjdZ6YU6FGeab8uVjkSgo9_0OCJUaF4vzEGwXmNuGawANxnZtiYjWvbJlq-2mn_L7rsqGQcSkMmyM0g4aX7dF8wB6DVrXShJ78fcrNtpeoU72YGEdjehA8qVclDFwBdpCGynxxnWJePk72lQb6gkVMqKMc3jBF8GkWf8oP_sjss-fpOjSUMR1c8_0JlTYWO46KWOZa0EO2t8H1V3imMyzbhoxRd_qZHmo46gJkG-ZdebjX0vGQllaCwu0z4kLcXIfAZhqPEkdssDGhC_txwJuhaPDFQ",
          "e": "AQAB"
        }
      ]
    })JSON"};
static constexpr std::string_view UNRELATED_KEYS{
    R"JSON({
      "keys": [
        {
          "kty": "RSA",
          "n": "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ",
          "e": "AQAB"
        }
      ]
    })JSON"};

// A claim rule is matched against whatever a provider put in a token, so these
// tests mint their own rather than reuse a fixed vector. The curve keeps the
// key small enough to read
static constexpr std::string_view CLAIMS_PRIVATE_KEY{
    R"JSON({
      "kty": "EC",
      "crv": "P-256",
      "x": "sQbBlwx8VKtzct6SJjoYb4hmXMRIhBdC_rQtfrA7GdU",
      "y": "gE3c3l2Uux8jUbm0DVEnXwPQlsD7ln4CLWt6FGxNbhk",
      "d": "n0c-5YK2MYjEvSiF8OOaQOiheqm14U4iN6PdZAGLXOE"
    })JSON"};
static constexpr std::string_view CLAIMS_KEYS{
    R"JSON({
      "keys": [
        {
          "kty": "EC",
          "crv": "P-256",
          "x": "sQbBlwx8VKtzct6SJjoYb4hmXMRIhBdC_rQtfrA7GdU",
          "y": "gE3c3l2Uux8jUbm0DVEnXwPQlsD7ln4CLWt6FGxNbhk"
        }
      ]
    })JSON"};

// Claim rules reach a policy already compiled into individual claim requests
// (OpenID Connect Core 1.0 Section 5.5.1), with their names and their values
// sorted, since the serialised bytes are what decide whether two policies
// count as one audience
static constexpr std::string_view CLAIMS_ONE_GROUP{
    R"JSON({
      "groups": {
        "essential": true,
        "values": [ "platform" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_ONCALL_GROUP{
    R"JSON({
      "groups": {
        "essential": true,
        "values": [ "oncall" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_TWO_GROUPS{
    R"JSON({
      "groups": {
        "essential": true,
        "values": [ "oncall", "platform" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_GROUP_AND_DEPARTMENT{
    R"JSON({
      "department": {
        "essential": true,
        "values": [ "engineering" ]
      },
      "groups": {
        "essential": true,
        "values": [ "platform" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_SCOPE{
    R"JSON({
      "scope": {
        "essential": true,
        "values": [ "registry:read" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_VERIFIED{
    R"JSON({
      "verified": {
        "essential": true,
        "values": [ "true" ]
      }
    })JSON"};

// Rules the indexer never emits, since the configuration format refuses them,
// which a corrupt artifact could still carry into the gate
static constexpr std::string_view CLAIMS_SCOPE_NO_VALUES{
    R"JSON({
      "scope": {
        "essential": true,
        "values": []
      }
    })JSON"};
static constexpr std::string_view CLAIMS_SCOPE_UNREADABLE{
    R"JSON({
      "scope": {
        "essential": true,
        "values": [ 42 ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_SCOPE_UNCONSTRAINED{
    R"JSON({
      "scope": {
        "essential": true
      }
    })JSON"};
static constexpr std::string_view CLAIMS_GROUPS_NO_VALUES{
    R"JSON({
      "groups": {
        "essential": true,
        "values": []
      }
    })JSON"};

// A token from the issuer and audience the claim tests configure, carrying
// whatever additional claims a case is about
static auto token_with(const std::string_view claims) -> std::string {
  auto payload{sourcemeta::core::parse_json(claims)};
  payload.assign("iss", sourcemeta::core::JSON{"acme"});
  payload.assign("aud", sourcemeta::core::JSON{"client"});
  payload.assign("exp", sourcemeta::core::JSON{2000000000});
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(CLAIMS_PRIVATE_KEY))};
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{"ES256"});
  return sourcemeta::core::jwt_sign(header, payload, key.value()).value();
}

// A sealed value carries the instant it was minted, and is only honoured for
// a bounded interval after it, so these are read from the clock rather than
// named as constants
static auto minted_now() -> std::chrono::sys_seconds {
  return std::chrono::time_point_cast<std::chrono::seconds>(
      std::chrono::system_clock::now());
}

// An expiry far enough ahead to outlive any test run, and near enough to the
// instant of minting for the value to be one this system would produce
static auto session_expiry() -> std::chrono::sys_seconds {
  return minted_now() + std::chrono::hours{1};
}

// An interactive policy names the environment variable holding the secret
// that signs its session and transaction cookies, so the tests set that
// variable and mint cookies under its value
static constexpr const char *SESSION_SECRET_VARIABLE{"ONE_TEST_SESSION_SECRET"};
static constexpr std::string_view SESSION_SECRET{"session-secret"};

// An interactive policy names one variable per secret it accepts, newest
// first, so each set of policies points at the variables it needs
static constexpr std::array<std::string_view, 1> SESSION_SECRETS{
    {SESSION_SECRET_VARIABLE}};
static constexpr std::array<std::string_view, 1> SESSION_SECRETS_UNUSED{
    {"ONE_TEST_OIDC_SESSION_UNUSED"}};
static constexpr std::array<std::string_view, 1> SESSION_SECRETS_BLANK{
    {"ONE_TEST_OIDC_BLANK_SECRET"}};
static constexpr std::array<std::string_view, 1> SESSION_SECRETS_OPEN{
    {"ONE_TEST_OIDC_OPEN_SECRET"}};
static constexpr std::array<std::string_view, 2> SESSION_SECRETS_ROTATED{
    {"ONE_TEST_OIDC_ROTATED_SECRET", "ONE_TEST_OIDC_ROTATED_SECRET_OLD"}};
static constexpr std::array<std::string_view, 1> SESSION_SECRETS_SEAL_NONE{
    {"ONE_TEST_OIDC_SEAL_NONE_SECRET"}};
static constexpr std::array<std::string_view, 1> SESSION_SECRETS_SEAL_OTHER{
    {"ONE_TEST_OIDC_SEAL_OTHER"}};
static constexpr std::array<std::string_view, 1> SESSION_SECRETS_UNSET{
    {"ONE_TEST_OIDC_UNSET_SECRET"}};

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
  EXPECT_FALSE(authentication.admits(at("/"), {.bearer = ""}).allowed);
  EXPECT_FALSE(authentication.admits(at("/acme/foo"), {.bearer = ""}).allowed);
  EXPECT_FALSE(authentication.admits(at(""), {.bearer = ""}).allowed);
}

TEST(malformed_artifact_denies_everything) {
  const auto path{test_path("malformed.bin")};
  std::ofstream stream{path, std::ios::binary};
  const std::array<char, 64> garbage{};
  stream.write(garbage.data(), garbage.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits(at("/"), {.bearer = ""}).allowed);
  EXPECT_FALSE(authentication.admits(at("/acme/foo"), {.bearer = ""}).allowed);
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
  EXPECT_FALSE(authentication.admits(at("/"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = ""}).allowed);
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
  EXPECT_FALSE(authentication.admits(at("/"), {.bearer = ""}).allowed);
  EXPECT_FALSE(authentication.admits(at("/acme/foo"), {.bearer = ""}).allowed);
}

TEST(corrupted_section_offset_denies_everything) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_OFFSET"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("corrupted_offset.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  // Overwrite the node section offset with a value that aliases the header
  std::fstream stream{path, std::ios::binary | std::ios::in | std::ios::out};
  stream.seekp(24);
  const std::array<char, 4> aliased{};
  stream.write(aliased.data(), aliased.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = ""}).allowed);
  EXPECT_FALSE(authentication.admits(at("/"), {.bearer = ""}).allowed);
}

TEST(zero_policies_admits_every_path) {
  const std::array<sourcemeta::one::Authentication::Policy, 0> policies{};
  const auto path{test_path("zero_policies.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.admits(at("/"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication.admits(at(""), {.bearer = ""}).allowed);
  EXPECT_TRUE(
      authentication.admits(at("/acme/foo/bar"), {.bearer = ""}).allowed);
  EXPECT_EQ(authentication.governing(at("/")), (std::vector<std::size_t>{}));
  EXPECT_EQ(authentication.governing(at("/acme")),
            (std::vector<std::size_t>{}));
}

TEST(uncovered_paths_are_public_around_a_gated_scope) {
  setenv("ONE_TEST_KEY_SCOPE", "scope-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SCOPE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("uncovered_public.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The covered subtree is gated
  EXPECT_FALSE(authentication.admits(at("/internal"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication.admits(at("/internal"), {.bearer = "scope-secret"})
                  .allowed);
  EXPECT_TRUE(
      authentication.admits(at("/internal/foo"), {.bearer = "scope-secret"})
          .allowed);
  // Everything outside it is public
  EXPECT_TRUE(authentication.admits(at("/"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication.admits(at("/vendor"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication.admits(at("/vendor/foo"), {.bearer = ""}).allowed);
}

TEST(scope_matches_whole_segments_only) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SEGMENT"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("segment_boundary.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The scope gates its own segment
  EXPECT_FALSE(authentication.admits(at("/internal"), {.bearer = ""}).allowed);
  // A textual prefix that is not a whole segment is a different path, so it is
  // uncovered and public
  EXPECT_TRUE(
      authentication.admits(at("/internalish"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication.admits(at("/int"), {.bearer = ""}).allowed);
  EXPECT_TRUE(
      authentication.admits(at("/internal-team"), {.bearer = ""}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits(at("/alpha/one"), {.bearer = ""}).allowed);
  EXPECT_FALSE(authentication.admits(at("/beta/two"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/gamma/three"), {.bearer = ""}).allowed);
  // Between the scopes the registry is public
  EXPECT_TRUE(authentication.admits(at("/delta"), {.bearer = ""}).allowed);
}

TEST(nested_prefixes_gate_their_subtrees) {
  const std::array<std::string_view, 1> internal{{"/internal"}};
  const std::array<std::string_view, 1> secret{{"/internal/secret"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_NI"}};
  const std::array<std::string_view, 1> secret_keys{{"ONE_TEST_KEY_NS"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{internal, internal_keys}, {secret, secret_keys}}};
  const auto path{test_path("nested_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.admits(at("/internal"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/other"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/secret"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/secret/deep"), {.bearer = ""})
          .allowed);
  EXPECT_TRUE(authentication.admits(at("/"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication.admits(at("/public"), {.bearer = ""}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The inner path is covered by both, so either key admits it
  EXPECT_TRUE(
      authentication.admits(at("/internal/secret"), {.bearer = "wo-secret"})
          .allowed);
  EXPECT_TRUE(
      authentication.admits(at("/internal/secret"), {.bearer = "wi-secret"})
          .allowed);
  // The outer path is covered only by the outer policy
  EXPECT_TRUE(
      authentication.admits(at("/internal/other"), {.bearer = "wo-secret"})
          .allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/other"), {.bearer = "wi-secret"})
          .allowed);
}

TEST(single_policy_with_multiple_prefixes) {
  const std::array<std::string_view, 2> paths{{"/internal", "/vendor"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MP"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("multiple_prefixes.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/vendor/bar"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication.admits(at("/public"), {.bearer = ""}).allowed);
}

TEST(extensionless_policy_gates_every_representation) {
  setenv("ONE_TEST_KEY_REPRESENTATION", "representation-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_REPRESENTATION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("representation_agnostic.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The resource, every representation of it, and its subtree are all governed
  EXPECT_FALSE(
      authentication.admits(at("/secret/data"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/secret/data.json"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/secret/data.xml"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/secret/data/nested"), {.bearer = ""}).allowed);
  EXPECT_TRUE(
      authentication
          .admits(at("/secret/data"), {.bearer = "representation-secret"})
          .allowed);
  EXPECT_TRUE(
      authentication
          .admits(at("/secret/data.json"), {.bearer = "representation-secret"})
          .allowed);
  EXPECT_TRUE(
      authentication
          .admits(at("/secret/data.xml"), {.bearer = "representation-secret"})
          .allowed);
  EXPECT_TRUE(authentication
                  .admits(at("/secret/data/nested"),
                          {.bearer = "representation-secret"})
                  .allowed);
  // A sibling sharing a textual prefix is covered by no policy, so it is public
  EXPECT_TRUE(
      authentication.admits(at("/secret/database"), {.bearer = ""}).allowed);
  EXPECT_TRUE(
      authentication.admits(at("/secret/data2.json"), {.bearer = ""}).allowed);
}

TEST(extension_specific_policy_gates_only_that_representation) {
  setenv("ONE_TEST_KEY_SPECIFIC", "specific-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data.json"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SPECIFIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("representation_specific.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // Only the named representation is gated
  EXPECT_FALSE(
      authentication.admits(at("/secret/data.json"), {.bearer = ""}).allowed);
  EXPECT_TRUE(
      authentication
          .admits(at("/secret/data.json"), {.bearer = "specific-secret"})
          .allowed);
  EXPECT_TRUE(
      authentication
          .admits(at("/secret/data.json/nested"), {.bearer = "specific-secret"})
          .allowed);
  // The bare resource and other representations are uncovered, so public
  EXPECT_TRUE(
      authentication.admits(at("/secret/data"), {.bearer = ""}).allowed);
  EXPECT_TRUE(
      authentication.admits(at("/secret/data.xml"), {.bearer = ""}).allowed);
}

TEST(extension_handling_is_confined_to_the_terminal_segment) {
  const std::array<std::string_view, 1> paths{{"/v1"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_V1"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("intermediate_dot.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The policy on /v1 gates its own subtree
  EXPECT_FALSE(authentication.admits(at("/v1"), {.bearer = ""}).allowed);
  EXPECT_FALSE(authentication.admits(at("/v1/secret"), {.bearer = ""}).allowed);
  // As a terminal segment, /v1.0 is a representation of /v1 under the
  // content-negotiation rule, the same way /person.json represents /person
  EXPECT_FALSE(authentication.admits(at("/v1.0"), {.bearer = ""}).allowed);
  // But as an intermediate segment it is a distinct directory that does not
  // descend into the /v1 subtree, so its children are uncovered and public
  EXPECT_TRUE(
      authentication.admits(at("/v1.0/secret"), {.bearer = ""}).allowed);
}

TEST(an_explicit_route_is_gated_on_the_target_as_it_arrived) {
  setenv("ONE_TEST_KEY_ROUTE", "route-secret", 1);
  const std::array<std::string_view, 1> apikey_paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_ROUTE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{apikey_paths, keys}}};
  const auto path{test_path("explicit_route.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication
          .admits_route("/private/secret", {.bearer = "", .cookies = {}})
          .allowed);
  EXPECT_TRUE(authentication
                  .admits_route("/private/secret",
                                {.bearer = "route-secret", .cookies = {}})
                  .allowed);

  // A target covered by no policy is admitted, including one whose spelling
  // only resembles a governed prefix
  EXPECT_TRUE(authentication
                  .admits_route("/public/string", {.bearer = "", .cookies = {}})
                  .allowed);
  EXPECT_TRUE(
      authentication
          .admits_route("/privateextra/secret", {.bearer = "", .cookies = {}})
          .allowed);
}

TEST(apikey_admits_matching_credential) {
  setenv("ONE_TEST_KEY_MATCH", "secret-match", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MATCH"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("apikey_match.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.admits(at("/internal/foo"), {.bearer = "secret-match"})
          .allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = "wrong"}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = ""}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.admits(at("/internal/foo"), {.bearer = "key-a"}).allowed);
  EXPECT_TRUE(
      authentication.admits(at("/internal/foo"), {.bearer = "key-b"}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = "key-c"}).allowed);
}

TEST(apikey_with_unset_variable_denies) {
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_UNSET"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("apikey_unset.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = "anything"})
          .allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = ""}).allowed);
}

TEST(apikey_with_an_empty_variable_denies) {
  setenv("ONE_TEST_KEY_EMPTY", "", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_EMPTY"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("apikey_empty.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // A variable an operator meant to hold a key but left blank gates the path
  // exactly as an unset one does, rather than opening it to everyone
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = "anything"})
          .allowed);
}

TEST(apikey_ignores_an_empty_variable_beside_a_real_one) {
  setenv("ONE_TEST_KEY_PAIR_BLANK", "", 1);
  setenv("ONE_TEST_KEY_PAIR_REAL", "pair-secret", 1);
  const std::array<std::string_view, 2> keys{
      {"ONE_TEST_KEY_PAIR_BLANK", "ONE_TEST_KEY_PAIR_REAL"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("apikey_pair.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The blank one neither admits anybody nor keeps the key beside it from
  // working
  EXPECT_TRUE(
      authentication.admits(at("/internal/foo"), {.bearer = "pair-secret"})
          .allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/internal/foo"), {.bearer = "wrong"}).allowed);
}

TEST(sha256_policy_with_an_empty_variable_denies) {
  setenv("ONE_TEST_KEY_SHA_EMPTY", "", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHA_EMPTY"}};
  const std::array<std::string_view, 1> paths{{"/secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .keys = keys,
        .algorithm = sourcemeta::one::Authentication::Algorithm::Sha256}}};
  const auto path{test_path("sha256_empty.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/secret/foo"), {.bearer = ""}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/secret/foo"), {.bearer = "anything"}).allowed);
  // Nor does the digest of nothing, which is what an empty credential hashes to
  EXPECT_FALSE(
      authentication
          .admits(at("/secret/foo"), {.bearer = sourcemeta::core::sha256("")})
          .allowed);
}

TEST(sha256_policy_admits_the_matching_credential) {
  const std::string raw{"raw-secret-key"};
  setenv("ONE_TEST_KEY_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHA"}};
  const std::array<std::string_view, 1> paths{{"/secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .keys = keys,
        .algorithm = sourcemeta::one::Authentication::Algorithm::Sha256}}};
  const auto path{test_path("sha256_match.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.admits(at("/secret/foo"), {.bearer = raw}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/secret/foo"), {.bearer = "wrong"}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/secret/foo"), {.bearer = ""}).allowed);
  // Presenting the stored hash itself does not authenticate
  EXPECT_FALSE(
      authentication
          .admits(at("/secret/foo"), {.bearer = sourcemeta::core::sha256(raw)})
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // Either key type opens the path regardless of declaration order. The sha256
  // key must work even though the identity policy is checked first and fails
  EXPECT_TRUE(
      authentication.admits(at("/mixed/x"), {.bearer = "plain-a"}).allowed);
  EXPECT_TRUE(authentication.admits(at("/mixed/x"), {.bearer = raw}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/mixed/x"), {.bearer = "neither"}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The identity key must work even though the sha256 policy is checked first
  EXPECT_TRUE(authentication.admits(at("/mixed/x"), {.bearer = raw}).allowed);
  EXPECT_TRUE(
      authentication.admits(at("/mixed/x"), {.bearer = "plain-b"}).allowed);
  EXPECT_FALSE(
      authentication.admits(at("/mixed/x"), {.bearer = "neither"}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The keyless policies gate their scope with no key that can open it
  EXPECT_FALSE(authentication.admits(at("/p0/foo"), {.bearer = ""}).allowed);
  EXPECT_FALSE(authentication.admits(at("/p63/foo"), {.bearer = ""}).allowed);
  // An uncovered path is public
  EXPECT_TRUE(authentication.admits(at("/missing"), {.bearer = ""}).allowed);
}

TEST(governing_returns_policy_indices_in_declaration_order) {
  const std::array<std::string_view, 1> root_paths{{"/"}};
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> root_keys{{"ONE_TEST_KEY_GR"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_GI"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{root_paths, root_keys}, {internal_paths, internal_keys}}};
  const auto path{test_path("governing.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.governing(at("/")), (std::vector<std::size_t>{0}));
  EXPECT_EQ(authentication.governing(at("/vendor")),
            (std::vector<std::size_t>{0}));
  EXPECT_EQ(authentication.governing(at("/internal")),
            (std::vector<std::size_t>{0, 1}));
  EXPECT_EQ(authentication.governing(at("/internal/foo")),
            (std::vector<std::size_t>{0, 1}));
}

TEST(governing_of_an_ungoverned_path_is_empty) {
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_GE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{internal_paths, keys}}};
  const auto path{test_path("governing_empty.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.governing(at("/vendor")),
            (std::vector<std::size_t>{}));
  EXPECT_EQ(authentication.governing(at("/internal")),
            (std::vector<std::size_t>{0}));
}

TEST(reference_through_a_broken_artifact_is_rejected) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"},
      stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/open/one"), at("/open/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/open/one"), at("/secret/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/secret/one"), at("/secret/two")));
}

TEST(reference_to_a_public_schema_is_permitted) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_PUBLIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{secret_paths, keys}}};
  const auto path{test_path("ref_to_public.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.reference_permitted(at("/secret/one"), at("/open/two")));
  EXPECT_TRUE(
      authentication.reference_permitted(at("/open/one"), at("/open/two")));
}

TEST(public_schema_referencing_an_apikey_schema_is_rejected) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_LEAK"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{secret_paths, keys}}};
  const auto path{test_path("ref_public_to_apikey.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/open/one"), at("/secret/two")));
}

TEST(reference_within_the_same_policy_is_permitted) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_SAME"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("ref_same_policy.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.reference_permitted(at("/internal/one"),
                                                 at("/internal/two")));
  EXPECT_TRUE(authentication.reference_permitted(at("/internal/one"),
                                                 at("/internal/one")));
}

TEST(reference_across_disjoint_policies_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_REF_ALPHA"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_REF_BETA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{alpha_paths, alpha_keys}, {beta_paths, beta_keys}}};
  const auto path{test_path("ref_disjoint.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_from_a_narrower_to_a_wider_audience_is_permitted) {
  const std::array<std::string_view, 1> broad_paths{{"/p"}};
  const std::array<std::string_view, 1> nested_paths{{"/p/inner"}};
  const std::array<std::string_view, 1> broad_keys{{"ONE_TEST_REF_BROAD"}};
  const std::array<std::string_view, 1> nested_keys{{"ONE_TEST_REF_NESTED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{broad_paths, broad_keys}, {nested_paths, nested_keys}}};
  const auto path{test_path("ref_narrow_to_wide.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.reference_permitted(at("/p/one"), at("/p/inner/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/p/inner/two"), at("/p/one")));
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         calls)};
  EXPECT_TRUE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
  EXPECT_FALSE(authentication.admits(at("/secure/x"), {.bearer = "not-a-token"})
                   .allowed);
  EXPECT_FALSE(authentication.admits(at("/secure/x"), {.bearer = ""}).allowed);
  // A second valid request reuses the cached key set rather than refetching
  EXPECT_TRUE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
  EXPECT_EQ(*calls, 1);
}

TEST(jwt_admits_a_token_whose_type_the_policy_requires) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .token_type = "at+jwt"}}};
  const auto path{test_path("jwt_type_match.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  EXPECT_TRUE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
}

TEST(jwt_denies_a_token_whose_type_is_not_the_required_one) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // An identity token is signed by the same provider under the same key, and
  // carries the client identifier as its audience, so where a policy names
  // that audience the type is the only thing telling the two apart
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .token_type = "JWT"}}};
  const auto path{test_path("jwt_type_mismatch.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
}

TEST(jwt_without_a_required_type_admits_any_type) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // A provider that does not stamp the header cannot be told apart this way,
  // so a policy that names no type keeps working against one
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_type_absent.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  EXPECT_TRUE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path,
      stub_fetcher({{"https://idp.test/jwks", std::string{UNRELATED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         calls)};
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = "static-api-key"})
          .allowed);
  EXPECT_FALSE(authentication.admits(at("/secure/x"), {.bearer = ""}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://acme.test/.well-known/openid-configuration",
                           R"JSON({
              "issuer": "https://acme.test",
              "jwks_uri": "https://acme.test/keys",
              "response_types_supported": [ "code" ],
              "subject_types_supported": [ "public" ],
              "id_token_signing_alg_values_supported": [ "RS256" ]
            })JSON"},
                          {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
                         calls)};
  // Both the provider metadata and the key set it names are retrieved, and
  // the token then fails only on its issuer claim, which names a different
  // issuer than the policy trusts
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
  EXPECT_EQ(*calls, 2);
}

TEST(jwt_without_a_discoverable_issuer_fails_closed) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // The issuer claim is an opaque string rather than an https URL, so with no
  // pinned key set location there is nowhere trustworthy to discover one
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_discovery_issuer.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);
  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      path,
      stub_fetcher(
          {{"acme/.well-known/openid-configuration",
            R"JSON({ "issuer": "acme", "jwks_uri": "https://acme.test/keys" })JSON"},
           {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
          calls)};
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
  EXPECT_EQ(*calls, 0);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  // The static key opens the path
  EXPECT_TRUE(authentication.admits(at("/both/x"), {.bearer = "static-secret"})
                  .allowed);
  // The token opens the path
  EXPECT_TRUE(
      authentication.admits(at("/both/x"), {.bearer = SIGNED_TOKEN}).allowed);
  // Neither a wrong key nor a wrong token opens it
  EXPECT_FALSE(
      authentication.admits(at("/both/x"), {.bearer = "wrong"}).allowed);
}

TEST(oidc_identity_without_rules_admits_whoever_signs_in) {
  setenv("ONE_TEST_OIDC_ADMIT_OPEN", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_ADMIT_OPEN",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_admit_open.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto anybody{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2"
  })JSON")};

  EXPECT_EQ(authentication.admits_identity("okta", anybody),
            sourcemeta::one::Authentication::Admission::Admitted);
}

TEST(oidc_identity_missing_a_claim_is_incomplete_rather_than_refused) {
  setenv("ONE_TEST_OIDC_ADMIT_PARTIAL", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .claims = CLAIMS_ONE_GROUP,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_ADMIT_PARTIAL",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_admit_partial.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  using Admission = sourcemeta::one::Authentication::Admission;
  const auto no_group{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2"
  })JSON")};
  const auto another_group{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "groups": [ "support" ]
  })JSON")};
  const auto the_group{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "groups": [ "platform" ]
  })JSON")};

  // A claim that never arrived is a question the provider may still answer at
  // its UserInfo endpoint, which is where a scope's claims land by default
  EXPECT_EQ(authentication.admits_identity("okta", no_group),
            Admission::Incomplete);
  // A claim that arrived and fell short is an answer already given, so asking
  // anywhere else would only repeat it
  EXPECT_EQ(authentication.admits_identity("okta", another_group),
            Admission::Refused);
  EXPECT_EQ(authentication.admits_identity("okta", the_group),
            Admission::Admitted);
}

TEST(oidc_identity_refused_by_one_rule_is_refused_whatever_another_wants) {
  setenv("ONE_TEST_OIDC_ADMIT_BOTH", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .claims = CLAIMS_ONE_GROUP,
        .email_domains = domains,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_ADMIT_BOTH",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_admit_both.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  using Admission = sourcemeta::one::Authentication::Admission;
  const auto foreign_address{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@other.test",
    "email_verified": true
  })JSON")};
  const auto unvouched_address{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test",
    "email_verified": false,
    "groups": [ "platform" ]
  })JSON")};
  const auto neither_half{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2"
  })JSON")};
  const auto both_halves{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test",
    "email_verified": true,
    "groups": [ "platform" ]
  })JSON")};

  // The address settles it, so the absent group changes nothing
  EXPECT_EQ(authentication.admits_identity("okta", foreign_address),
            Admission::Refused);
  // An address the provider will not vouch for is an answer, not a gap
  EXPECT_EQ(authentication.admits_identity("okta", unvouched_address),
            Admission::Refused);
  // Neither half having arrived leaves both worth asking about
  EXPECT_EQ(authentication.admits_identity("okta", neither_half),
            Admission::Incomplete);
  EXPECT_EQ(authentication.admits_identity("okta", both_halves),
            Admission::Admitted);
}

TEST(oidc_identity_refuses_an_unvouched_address_whose_companion_is_absent) {
  setenv("ONE_TEST_OIDC_ADMIT_UNVOUCHED", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .email_domains = domains,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_ADMIT_UNVOUCHED",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_admit_unvouched.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  using Admission = sourcemeta::one::Authentication::Admission;
  const auto declined_to_vouch{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email_verified": false
  })JSON")};
  const auto not_an_address{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": 42
  })JSON")};
  const auto nothing_at_all{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2"
  })JSON")};

  // The provider saying it will not vouch is an answer, so the address that
  // never arrived cannot turn it into a question worth asking again
  EXPECT_EQ(authentication.admits_identity("okta", declined_to_vouch),
            Admission::Refused);
  // An address that is not one settles it the same way
  EXPECT_EQ(authentication.admits_identity("okta", not_an_address),
            Admission::Refused);
  // Absence alone is what leaves the question open
  EXPECT_EQ(authentication.admits_identity("okta", nothing_at_all),
            Admission::Incomplete);
}

TEST(combining_two_answers_fills_gaps_without_overruling_the_token) {
  const auto token{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "groups": [ "platform" ],
    "department": "engineering"
  })JSON")};
  const auto extra{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "department": "sales",
    "cost_centre": "R&D"
  })JSON")};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "groups": [ "platform" ],
    "department": "engineering",
    "cost_centre": "R&D"
  })JSON")};

  // What only the second answer carried is added, and what the signed one
  // already said stands
  EXPECT_EQ(sourcemeta::one::Authentication::combine_claims(token, extra),
            expected);
}

TEST(combining_two_answers_keeps_an_address_with_its_own_assertion) {
  // A token vouching for an address it never carried says nothing about the
  // one a second answer supplies
  const auto orphan{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email_verified": true
  })JSON")};
  const auto supplied{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test"
  })JSON")};
  const auto without_the_assertion{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test"
  })JSON")};

  EXPECT_EQ(sourcemeta::one::Authentication::combine_claims(orphan, supplied),
            without_the_assertion);

  // The pair arrives whole from the answer that carried the address
  const auto pair{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test",
    "email_verified": true
  })JSON")};
  const auto with_the_pair{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test",
    "email_verified": true
  })JSON")};

  EXPECT_EQ(sourcemeta::one::Authentication::combine_claims(orphan, pair),
            with_the_pair);

  // A token carrying the address keeps its own assertion, so a second answer
  // cannot vouch for it either
  const auto unverified{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test"
  })JSON")};
  const auto vouching{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "someone@acme.test",
    "email_verified": true
  })JSON")};
  const auto keeping_its_own{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test"
  })JSON")};

  EXPECT_EQ(
      sourcemeta::one::Authentication::combine_claims(unverified, vouching),
      keeping_its_own);

  // An assertion arriving on its own from the second answer is dropped for
  // the same reason as one left behind by the first
  const auto nothing_to_vouch_for{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email_verified": true
  })JSON")};
  const auto neither_half{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2"
  })JSON")};

  EXPECT_EQ(sourcemeta::one::Authentication::combine_claims(
                neither_half, nothing_to_vouch_for),
            neither_half);

  // An address without an assertion still arrives, since it claims nothing
  const auto address_alone{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test"
  })JSON")};
  const auto carried_over{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test"
  })JSON")};

  EXPECT_EQ(sourcemeta::one::Authentication::combine_claims(neither_half,
                                                            address_alone),
            carried_over);
}

TEST(admitting_reads_two_answers_only_once_they_are_combined) {
  setenv("ONE_TEST_OIDC_ADMIT_SPLIT", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .claims = CLAIMS_ONE_GROUP,
        .email_domains = domains,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_ADMIT_SPLIT",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_admit_split.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  using Admission = sourcemeta::one::Authentication::Admission;
  const auto token{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "groups": [ "platform" ]
  })JSON")};
  const auto extra{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "email": "jane@acme.test",
    "email_verified": true
  })JSON")};

  // Either answer alone leaves a rule unanswered
  EXPECT_EQ(authentication.admits_identity("okta", token),
            Admission::Incomplete);
  EXPECT_EQ(authentication.admits_identity("okta", extra),
            Admission::Incomplete);
  // Only what combining them produces admits, so a combine dropping either
  // side is caught here rather than passing on a payload the test built
  EXPECT_EQ(authentication.admits_identity(
                "okta",
                sourcemeta::one::Authentication::combine_claims(token, extra)),
            Admission::Admitted);
}

TEST(oidc_identity_names_a_claim_the_provider_answers_with_objects) {
  setenv("ONE_TEST_OIDC_SHAPE", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .claims = CLAIMS_ONE_GROUP,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_SHAPE",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_shape.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto objects{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "groups": [ { "value": "g-1", "display": "platform" } ]
  })JSON")};
  const auto strings{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "groups": [ "platform" ]
  })JSON")};
  const auto absent{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2"
  })JSON")};

  // The rule compares an identifier, so a rule naming what a person sees
  // matches nothing, and that is what an operator has no other way to learn
  EXPECT_EQ(authentication.object_shaped_claims("okta", objects),
            (std::vector<std::string_view>{"groups"}));
  // Nothing to say where a claim arrived in the shape a rule names
  EXPECT_EQ(authentication.object_shaped_claims("okta", strings),
            (std::vector<std::string_view>{}));
  // Nor where it never arrived, which is a different problem with its own word
  EXPECT_EQ(authentication.object_shaped_claims("okta", absent),
            (std::vector<std::string_view>{}));
  // A claim no rule names is nobody's business here
  const auto elsewhere{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "groups": [ "platform" ],
    "roles": [ { "value": "r-1" } ]
  })JSON")};
  EXPECT_EQ(authentication.object_shaped_claims("okta", elsewhere),
            (std::vector<std::string_view>{}));
  // And a policy that names no rule has nothing to report about
  EXPECT_EQ(authentication.object_shaped_claims("nowhere", objects),
            (std::vector<std::string_view>{}));
}

TEST(oidc_identity_says_nothing_about_the_shape_of_a_scope) {
  setenv("ONE_TEST_OIDC_SHAPE_SCOPE", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .claims = CLAIMS_SCOPE,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_SHAPE_SCOPE",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_shape_scope.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto objects{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2",
    "scope": [ { "value": "registry:read" } ]
  })JSON")};

  // A scope is read whole rather than member by member, so one arriving as
  // anything else is refused outright, and naming an identifier here would
  // point an operator at a mistake they did not make
  EXPECT_EQ(authentication.object_shaped_claims("okta", objects),
            (std::vector<std::string_view>{}));
}

TEST(oidc_identity_under_an_unknown_policy_is_refused) {
  setenv("ONE_TEST_OIDC_ADMIT_UNKNOWN", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_ADMIT_UNKNOWN",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_admit_unknown.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // A name no interactive policy answers to could never have minted a session
  const auto somebody{sourcemeta::core::parse_json(R"JSON({
    "sub": "a1b2"
  })JSON")};

  EXPECT_EQ(authentication.admits_identity("nowhere", somebody),
            sourcemeta::one::Authentication::Admission::Refused);
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
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_deny.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

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

  const auto empty_verdict{
      authentication.admits(at("/portal/x"), {.bearer = ""})};
  EXPECT_FALSE(empty_verdict.allowed);
  EXPECT_FALSE(empty_verdict.principal.has_value());

  const auto secret_verdict{
      authentication.admits(at("/portal/x"), {.bearer = "confidential"})};
  EXPECT_FALSE(secret_verdict.allowed);
  EXPECT_FALSE(secret_verdict.principal.has_value());

  const auto token_verdict{
      authentication.admits(at("/portal/x"), {.bearer = SIGNED_TOKEN})};
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
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_union.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto key_verdict{
      authentication.admits(at("/both/x"), {.bearer = "union-secret"})};
  EXPECT_TRUE(key_verdict.allowed);
  EXPECT_TRUE(key_verdict.principal.has_value());
  EXPECT_EQ(key_verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::ApiKey);
  EXPECT_EQ(key_verdict.principal.value().policy, std::size_t{0});

  const auto token_verdict{
      authentication.admits(at("/both/x"), {.bearer = SIGNED_TOKEN})};
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
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_session.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{path,
                                                       stub_fetcher({}, calls)};

  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta", "subject": "jane@acme.test" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  const std::string cookies{"theme=dark; sourcemeta_one_session=" + sealed};

  const auto verdict{authentication.admits(
      at("/portal/x"), {.bearer = "", .cookies = fields(cookies)})};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.principal.has_value());
  EXPECT_EQ(verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::OIDC);
  EXPECT_EQ(verdict.principal.value().policy, std::size_t{0});
  EXPECT_EQ(*calls, 0);

  const auto anonymous_verdict{
      authentication.admits(at("/portal/x"), {.bearer = ""})};
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
        .session_secrets = SESSION_SECRETS},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_BIND_B",
        .name = "google",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_session_bound.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};

  // The session opens the path its policy governs
  const std::string okta_cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_TRUE(authentication
                  .admits(at("/alpha/x"),
                          {.bearer = "", .cookies = fields(okta_cookies)})
                  .allowed);

  // And not a path governed by another policy. Both policies here read the
  // same session secret, so the value verifies under either and the payload is
  // the only thing that tells them apart. There is no cookie name left to
  // separate them, which makes this the control rather than a second opinion
  EXPECT_FALSE(authentication
                   .admits(at("/beta/x"),
                           {.bearer = "", .cookies = fields(okta_cookies)})
                   .allowed);
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
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_session_expired.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto past{minted_now() - std::chrono::hours{2}};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET, past,
      past + std::chrono::hours{1})};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_FALSE(
      authentication
          .admits(at("/portal/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
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
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_session_forged.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  // A value sealed under a secret this policy does not hold
  const auto foreign{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, "other-secret",
      minted_now(), session_expiry())};
  const std::string foreign_cookies{"sourcemeta_one_session=" + foreign};
  EXPECT_FALSE(authentication
                   .admits(at("/portal/x"),
                           {.bearer = "", .cookies = fields(foreign_cookies)})
                   .allowed);

  // A value whose signature no longer matches its contents
  auto tampered{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  tampered.back() = tampered.back() == 'A' ? 'B' : 'A';
  const std::string tampered_cookies{"sourcemeta_one_session=" + tampered};
  EXPECT_FALSE(authentication
                   .admits(at("/portal/x"),
                           {.bearer = "", .cookies = fields(tampered_cookies)})
                   .allowed);

  // A value that is not a sealed session at all
  EXPECT_FALSE(
      authentication
          .admits(at("/portal/x"),
                  {.bearer = "",
                   .cookies = fields("sourcemeta_one_session=garbage")})
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
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_session_payload.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const std::array<std::string_view, 4> payloads{
      {"not json", "[ 1, 2 ]", R"JSON({ "subject": "jane" })JSON",
       R"JSON({ "policy": "google" })JSON"}};
  for (const auto payload : payloads) {
    const auto sealed{sourcemeta::one::Authentication::seal_value(
        payload, sourcemeta::one::Authentication::Purpose::Session,
        SESSION_SECRET, minted_now(), session_expiry())};
    const std::string cookies{"sourcemeta_one_session=" + sealed};
    EXPECT_FALSE(
        authentication
            .admits(at("/portal/x"), {.bearer = "", .cookies = fields(cookies)})
            .allowed);
  }
}

TEST(session_is_admitted_when_a_shadowing_cookie_precedes_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SHADOW_A",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_shadow_before.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};

  // A parent domain can set a cookie the host also sets, and the header says
  // nothing about which is which, so the genuine one is honoured wherever it
  // appears
  const std::string cookies{"sourcemeta_one_session=not-a-session; "
                            "sourcemeta_one_session=" +
                            sealed};
  EXPECT_TRUE(
      authentication
          .admits(at("/alpha/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
}

TEST(session_is_admitted_when_a_shadowing_cookie_follows_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SHADOW_B",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_shadow_after.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};

  // Taking the last match would deny here, which is the shape that lets a
  // neighbouring host lock somebody out of an instance it does not control
  const std::string cookies{"sourcemeta_one_session=" + sealed +
                            "; sourcemeta_one_session=not-a-session"};
  EXPECT_TRUE(
      authentication
          .admits(at("/alpha/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
}

TEST(session_is_admitted_when_it_arrives_in_a_later_cookie_field) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_FIELD_LATER",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_field_later.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};

  // A request may carry its cookies across several fields rather than one, so
  // reading only the first would deny a session that did arrive
  const std::string second{"sourcemeta_one_session=" + sealed};
  const std::array<std::string_view, 2> carried{
      {"sourcemeta_one_session=not-a-session", second}};
  EXPECT_TRUE(
      authentication.admits(at("/alpha/x"), {.bearer = "", .cookies = carried})
          .allowed);
}

TEST(session_is_admitted_when_it_arrives_in_an_earlier_cookie_field) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_FIELD_EARLIER",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_field_earlier.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};

  // And neither field is the one that decides, so a later one carrying nothing
  // does not undo an earlier one that does
  const std::string first{"sourcemeta_one_session=" + sealed};
  const std::array<std::string_view, 2> carried{
      {first, "sourcemeta_one_session=not-a-session"}};
  EXPECT_TRUE(
      authentication.admits(at("/alpha/x"), {.bearer = "", .cookies = carried})
          .allowed);
}

TEST(a_session_for_another_policy_does_not_end_the_search) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // Both policies read the same session secret, so a value minted for one
  // opens under the other and is only told apart by the payload
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SEARCH_A",
        .name = "okta",
        .session_secrets = SESSION_SECRETS},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SEARCH_B",
        .name = "google",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_search.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto other{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "google" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  const auto mine{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};

  // The first value opens but was minted elsewhere, so stopping there would
  // deny a caller who did present a session for this policy
  const std::string cookies{"sourcemeta_one_session=" + other +
                            "; sourcemeta_one_session=" + mine};
  EXPECT_TRUE(
      authentication
          .admits(at("/alpha/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);

  // And a value minted elsewhere still opens nothing on its own
  const std::string alone{"sourcemeta_one_session=" + other};
  EXPECT_FALSE(
      authentication
          .admits(at("/alpha/x"), {.bearer = "", .cookies = fields(alone)})
          .allowed);
}

TEST(a_shadowing_cookie_alone_never_admits) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SHADOW_C",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_shadow_only.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // Trying every value admits a caller if any one opens, and never because
  // several did not
  const std::string cookies{"sourcemeta_one_session=not-a-session; "
                            "sourcemeta_one_session=nor-is-this"};
  EXPECT_FALSE(
      authentication
          .admits(at("/alpha/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
}

TEST(a_session_never_admits_under_a_policy_sharing_its_secret) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // Deliberately the same secret for both, which the configuration permits.
  // The value therefore verifies under either policy and only the payload
  // distinguishes them
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SHARED_A",
        .name = "okta",
        .session_secrets = SESSION_SECRETS},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SHARED_B",
        .name = "google",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_shared_secret.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  EXPECT_TRUE(
      authentication
          .admits(at("/alpha/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
  EXPECT_FALSE(
      authentication
          .admits(at("/beta/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
}

TEST(a_session_naming_no_policy_never_admits) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_NAMELESS_PAYLOAD",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_nameless_payload.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  // Correctly sealed, and says nothing about which policy it is for
  const auto empty{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "subject": "somebody" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  EXPECT_FALSE(
      authentication
          .admits(at("/alpha/x"),
                  {.bearer = "",
                   .cookies = fields("sourcemeta_one_session=" + empty)})
          .allowed);

  // Names a policy that does not exist
  const auto unknown{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "nowhere" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  EXPECT_FALSE(
      authentication
          .admits(at("/alpha/x"),
                  {.bearer = "",
                   .cookies = fields("sourcemeta_one_session=" + unknown)})
          .allowed);

  // Names a policy but not as a string
  const auto typed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": 42 })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  EXPECT_FALSE(
      authentication
          .admits(at("/alpha/x"),
                  {.bearer = "",
                   .cookies = fields("sourcemeta_one_session=" + typed)})
          .allowed);
}

TEST(open_session_recovers_the_payload_of_whichever_policy_minted_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_OPEN_A",
        .name = "okta",
        .session_secrets = SESSION_SECRETS},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_OPEN_B",
        .name = "google",
        .session_secrets = SESSION_SECRETS_OPEN}}};
  setenv("ONE_TEST_OIDC_OPEN_SECRET", "another-secret", 1);
  const auto path{test_path("oidc_open_session.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta", "subject": "jane" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};

  const auto payload{authentication.open_session(sealed)};
  EXPECT_TRUE(payload.has_value());
  EXPECT_EQ(payload.value(),
            R"JSON({ "policy": "okta", "subject": "jane" })JSON");
}

TEST(open_session_refuses_a_value_whose_payload_names_another_policy) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // Sharing the secret again, so the value verifies under both and only the
  // payload decides. A caller must not be able to learn a policy the value was
  // never minted for
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_OPENX_A",
        .name = "okta",
        .session_secrets = SESSION_SECRETS},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_OPENX_B",
        .name = "google",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_open_session_cross.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "nowhere" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  EXPECT_FALSE(authentication.open_session(sealed).has_value());
}

TEST(open_session_refuses_a_transaction) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_OPEN_PURPOSE",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_open_session_purpose.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto transaction{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Transaction, SESSION_SECRET,
      minted_now(), session_expiry())};
  EXPECT_FALSE(authentication.open_session(transaction).has_value());
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
        .session_secrets = SESSION_SECRETS_UNSET}}};
  const auto path{test_path("oidc_session_no_secrets.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_FALSE(
      authentication
          .admits(at("/portal/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
}

TEST(session_admitted_under_a_rotated_secret) {
  // The policy names the newest secret first, then the one it replaces, so a
  // cookie signed under the old secret still verifies
  setenv("ONE_TEST_OIDC_ROTATED_SECRET", "new-secret", 1);
  setenv("ONE_TEST_OIDC_ROTATED_SECRET_OLD", "old-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_ROTATED",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_ROTATED}}};
  const auto path{test_path("oidc_session_rotated.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  // A cookie signed under the older secret is still admitted
  const auto old_sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, "old-secret",
      minted_now(), session_expiry())};
  EXPECT_TRUE(
      authentication
          .admits(at("/portal/x"),
                  {.bearer = "",
                   .cookies = fields("sourcemeta_one_session=" + old_sealed)})
          .allowed);

  // A fresh login mints under the newest secret alone, so the minted value
  // verifies under a new-only secret set and not under an old-only one
  const auto minted{authentication.seal(
      "okta", sourcemeta::one::Authentication::Purpose::Session,
      R"JSON({ "policy": "okta" })JSON", session_expiry())};
  EXPECT_TRUE(minted.has_value());
  // The value was minted from the clock, so it is read against the same one
  const auto now{minted_now()};
  const std::array<std::string_view, 1> new_only{{"new-secret"}};
  EXPECT_TRUE(sourcemeta::one::Authentication::open_value(
                  minted.value(),
                  sourcemeta::one::Authentication::Purpose::Session, new_only,
                  now)
                  .has_value());
  const std::array<std::string_view, 1> old_only{{"old-secret"}};
  EXPECT_FALSE(sourcemeta::one::Authentication::open_value(
                   minted.value(),
                   sourcemeta::one::Authentication::Purpose::Session, old_only,
                   now)
                   .has_value());

  // A secret no longer in the set is rejected
  const auto retired{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, "retired-secret",
      minted_now(), session_expiry())};
  EXPECT_FALSE(
      authentication
          .admits(at("/portal/x"),
                  {.bearer = "",
                   .cookies = fields("sourcemeta_one_session=" + retired)})
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
        .session_secrets = SESSION_SECRETS_BLANK}}};
  const auto path{test_path("oidc_session_blank.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  // A blank secret would let anyone forge sessions, so it never verifies one
  const auto forged{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, "", minted_now(),
      session_expiry())};
  const std::string cookies{"sourcemeta_one_session=" + forged};
  EXPECT_FALSE(
      authentication
          .admits(at("/portal/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
}

TEST(save_creates_the_directory_it_writes_into) {
  setenv("ONE_TEST_KEY_NESTED", "nested-secret", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NESTED"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("nested") / "deeper" / "authentication.bin"};
  std::filesystem::remove_all(test_path("nested"));
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  EXPECT_TRUE(std::filesystem::exists(path));
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.admits(at("/internal/foo"), {.bearer = "nested-secret"})
          .allowed);
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
    sourcemeta::one::Authentication::save(policies, path, path, anywhere);
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
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_session_union.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto key_verdict{
      authentication.admits(at("/both/x"), {.bearer = "union-key"})};
  EXPECT_TRUE(key_verdict.allowed);
  EXPECT_TRUE(key_verdict.principal.has_value());
  EXPECT_EQ(key_verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::ApiKey);
  EXPECT_EQ(key_verdict.principal.value().policy, std::size_t{0});

  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  const auto session_verdict{authentication.admits(
      at("/both/x"), {.bearer = "", .cookies = fields(cookies)})};
  EXPECT_TRUE(session_verdict.allowed);
  EXPECT_TRUE(session_verdict.principal.has_value());
  EXPECT_EQ(session_verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::OIDC);
  EXPECT_EQ(session_verdict.principal.value().policy, std::size_t{1});

  EXPECT_FALSE(authentication.admits(at("/both/x"), {.bearer = ""}).allowed);
}

TEST(session_cookie_does_not_open_an_apikey_path) {
  setenv("ONE_TEST_KEY_NO_SESSION", "key-only", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NO_SESSION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("oidc_session_apikey.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_FALSE(
      authentication
          .admits(at("/internal/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
}

TEST(interactive_returns_the_policy_by_name) {
  setenv("ONE_TEST_KEY_INTERACTIVE", "key-value", 1);
  setenv("ONE_TEST_OIDC_LOOKUP_A", "lookup-a-secret", 1);
  setenv("ONE_TEST_OIDC_LOOKUP_B", "lookup-b-secret", 1);
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
        .session_secrets = SESSION_SECRETS_UNUSED},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://accounts.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_LOOKUP_B",
        .name = "google",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_lookup.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  const auto okta{authentication.interactive("okta")};
  EXPECT_TRUE(okta.has_value());
  EXPECT_EQ(okta.value().issuer, "https://login.test");
  EXPECT_EQ(okta.value().client_id, "registry");
  EXPECT_EQ(okta.value().default_path, "/alpha");
  EXPECT_EQ(authentication.client_secret("okta").value(), "lookup-a-secret");

  const auto google{authentication.interactive("google")};
  EXPECT_TRUE(google.has_value());
  EXPECT_EQ(google.value().issuer, "https://accounts.test");
  EXPECT_EQ(google.value().client_id, "dashboard");
  EXPECT_EQ(google.value().default_path, "/beta");
  EXPECT_EQ(authentication.client_secret("google").value(), "lookup-b-secret");

  EXPECT_FALSE(authentication.interactive("github").has_value());
  EXPECT_FALSE(authentication.interactive("").has_value());
  EXPECT_FALSE(authentication.client_secret("github").has_value());
  EXPECT_FALSE(authentication.client_secret("").has_value());
}

TEST(provider_endpoints_are_retrieved_once_and_reused) {
  const auto calls{std::make_shared<int>(0)};
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_CACHE",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_endpoints_cached.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const std::map<std::string, std::string> responses{
      {"https://login.test/.well-known/openid-configuration",
       R"JSON({
         "issuer": "https://login.test",
         "authorization_endpoint": "https://login.test/authorize",
         "token_endpoint": "https://login.test/token",
         "jwks_uri": "https://login.test/jwks",
         "end_session_endpoint": "https://login.test/logout",
         "response_types_supported": [ "code" ],
         "subject_types_supported": [ "public" ],
         "id_token_signing_alg_values_supported": [ "RS256" ]
       })JSON"}};
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher(responses, calls)};

  const auto first{authentication.endpoints("okta")};
  EXPECT_TRUE(first.has_value());
  EXPECT_EQ(first.value().authorization, "https://login.test/authorize");
  EXPECT_EQ(first.value().token, "https://login.test/token");
  EXPECT_EQ(first.value().jwks_uri, "https://login.test/jwks");
  EXPECT_EQ(first.value().end_session, "https://login.test/logout");
  EXPECT_EQ(*calls, 1);

  // Asking again does not ask the provider again. A login is an
  // unauthenticated endpoint, so fetching per request would let anybody drive
  // outbound traffic one for one
  const auto second{authentication.endpoints("okta")};
  EXPECT_TRUE(second.has_value());
  EXPECT_EQ(second.value().token, "https://login.test/token");
  EXPECT_EQ(*calls, 1);
}

TEST(a_provider_naming_no_authentication_method_takes_the_header) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_AUTH_ABSENT",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_auth_absent.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const std::map<std::string, std::string> responses{
      {"https://login.test/.well-known/openid-configuration",
       R"JSON({
         "issuer": "https://login.test",
         "authorization_endpoint": "https://login.test/authorize",
         "token_endpoint": "https://login.test/token",
         "jwks_uri": "https://login.test/jwks",

         "response_types_supported": [ "code" ],
         "subject_types_supported": [ "public" ],
         "id_token_signing_alg_values_supported": [ "RS256" ]
       })JSON"}};
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher(responses, nullptr)};

  const auto endpoints{authentication.endpoints("okta")};
  EXPECT_TRUE(endpoints.has_value());
  // RFC 8414 Section 2 makes an absent list mean `client_secret_basic`, so
  // saying nothing is an answer rather than the absence of one
  EXPECT_TRUE(endpoints.value().token_endpoint_basic_auth);
}

TEST(a_provider_naming_the_header_takes_the_header) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_AUTH_BASIC",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_auth_basic.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const std::map<std::string, std::string> responses{
      {"https://login.test/.well-known/openid-configuration",
       R"JSON({
         "issuer": "https://login.test",
         "authorization_endpoint": "https://login.test/authorize",
         "token_endpoint": "https://login.test/token",
         "jwks_uri": "https://login.test/jwks",
         "token_endpoint_auth_methods_supported": [ "client_secret_basic", "client_secret_post" ],
         "response_types_supported": [ "code" ],
         "subject_types_supported": [ "public" ],
         "id_token_signing_alg_values_supported": [ "RS256" ]
       })JSON"}};
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher(responses, nullptr)};

  const auto endpoints{authentication.endpoints("okta")};
  EXPECT_TRUE(endpoints.has_value());
  // Offering both, the header is the one RFC 6749 Section 2.3.1 asks for
  EXPECT_TRUE(endpoints.value().token_endpoint_basic_auth);
}

TEST(a_provider_refusing_the_header_gets_the_body_instead) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_AUTH_POST",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_auth_post.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const std::map<std::string, std::string> responses{
      {"https://login.test/.well-known/openid-configuration",
       R"JSON({
         "issuer": "https://login.test",
         "authorization_endpoint": "https://login.test/authorize",
         "token_endpoint": "https://login.test/token",
         "jwks_uri": "https://login.test/jwks",
         "token_endpoint_auth_methods_supported": [ "client_secret_post" ],
         "response_types_supported": [ "code" ],
         "subject_types_supported": [ "public" ],
         "id_token_signing_alg_values_supported": [ "RS256" ]
       })JSON"}};
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher(responses, nullptr)};

  const auto endpoints{authentication.endpoints("okta")};
  EXPECT_TRUE(endpoints.has_value());
  // A provider that does not take the header leaves the body as the only way
  // to authenticate, so the preference gives way rather than the login failing
  EXPECT_FALSE(endpoints.value().token_endpoint_basic_auth);
}

TEST(provider_endpoints_of_an_unreachable_provider_are_absent) {
  const auto calls{std::make_shared<int>(0)};
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_UNREACHABLE",
        .name = "okta",
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_endpoints_unreachable.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{path,
                                                       stub_fetcher({}, calls)};
  EXPECT_FALSE(authentication.endpoints("okta").has_value());
  EXPECT_FALSE(authentication.endpoints("github").has_value());
}

TEST(client_secret_of_an_unset_variable_is_absent) {
  unsetenv("ONE_TEST_OIDC_SECRET_UNSET");
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_SECRET_UNSET",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_secret_unset.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.interactive("okta").has_value());
  EXPECT_FALSE(authentication.client_secret("okta").has_value());
}

TEST(client_secret_of_an_empty_variable_is_absent) {
  setenv("ONE_TEST_OIDC_SECRET_EMPTY", "", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_SECRET_EMPTY",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_secret_empty.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // A policy an operator meant to configure but left blank cannot authenticate
  // to its provider, and says so rather than attempting the exchange with
  // nothing
  EXPECT_TRUE(authentication.interactive("okta").has_value());
  EXPECT_FALSE(authentication.client_secret("okta").has_value());
}

TEST(client_secret_of_a_policy_naming_no_variable_is_absent) {
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .name = "okta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_secret_nameless.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // A policy that names no variable at all has nowhere to read a secret from,
  // which is not the same as naming one that happens to be unset
  EXPECT_TRUE(authentication.interactive("okta").has_value());
  EXPECT_FALSE(authentication.client_secret("okta").has_value());
}

TEST(client_secret_of_a_non_interactive_policy_is_absent) {
  setenv("ONE_TEST_KEY_NOT_INTERACTIVE", "key-value", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NOT_INTERACTIVE"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths, .keys = keys, .name = "internal"}}};
  const auto path{test_path("oidc_secret_apikey.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.client_secret("internal").has_value());
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
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_default_path.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

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
        .session_secrets = SESSION_SECRETS},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_OIDC_SEAL_B",
        .name = "google",
        .session_secrets = SESSION_SECRETS_SEAL_OTHER}}};
  const auto path{test_path("oidc_seal.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  // The other policy holds its own, distinct secret
  setenv("ONE_TEST_OIDC_SEAL_OTHER", "another-secret", 1);
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{authentication.seal(
      "okta", sourcemeta::one::Authentication::Purpose::Session, "the-payload",
      session_expiry())};
  EXPECT_TRUE(sealed.has_value());
  const auto payload{authentication.open(
      "okta", sourcemeta::one::Authentication::Purpose::Session,
      sealed.value())};
  EXPECT_TRUE(payload.has_value());
  EXPECT_EQ(payload.value(), "the-payload");

  // A value opened under a different policy, holding a different secret, does
  // not verify
  EXPECT_FALSE(authentication
                   .open("google",
                         sourcemeta::one::Authentication::Purpose::Session,
                         sealed.value())
                   .has_value());

  // An unknown policy seals and opens nothing
  EXPECT_FALSE(authentication
                   .seal("github",
                         sourcemeta::one::Authentication::Purpose::Session,
                         "the-payload", session_expiry())
                   .has_value());
  EXPECT_FALSE(authentication
                   .open("github",
                         sourcemeta::one::Authentication::Purpose::Session,
                         sealed.value())
                   .has_value());
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
        .session_secrets = SESSION_SECRETS_SEAL_NONE}}};
  const auto path{test_path("oidc_seal_none.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication
                   .seal("okta",
                         sourcemeta::one::Authentication::Purpose::Session,
                         "the-payload", session_expiry())
                   .has_value());
  EXPECT_FALSE(authentication
                   .open("okta",
                         sourcemeta::one::Authentication::Purpose::Session,
                         "anything")
                   .has_value());
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
        .session_secrets = SESSION_SECRETS}}};
  const auto path{test_path("oidc_seal_expired.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const std::chrono::sys_seconds past{std::chrono::seconds{1000}};
  const auto sealed{authentication.seal(
      "okta", sourcemeta::one::Authentication::Purpose::Session, "the-payload",
      past)};
  EXPECT_TRUE(sealed.has_value());
  EXPECT_FALSE(authentication
                   .open("okta",
                         sourcemeta::one::Authentication::Purpose::Session,
                         sealed.value())
                   .has_value());
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
        .session_secrets = SESSION_SECRETS_UNUSED},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_REF_SAME_OTHER",
        .name = "beta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_ref_same.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
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
        .session_secrets = SESSION_SECRETS_UNUSED},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://login.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_BETA",
        .name = "beta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_ref_distinct.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
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
        .session_secrets = SESSION_SECRETS_UNUSED},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "registry",
        .client_id = "https://login.test",
        .client_secret_variable = "ONE_TEST_OIDC_REF_SWAP_BETA",
        .name = "beta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_ref_swapped.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
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
        .session_secrets = SESSION_SECRETS_UNUSED},
       {.paths = target_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://alpha.test",
        .client_id = "registry",
        .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_ONE",
        .name = "target-one",
        .session_secrets = SESSION_SECRETS_UNUSED},
       {.paths = target_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://beta.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_TWO",
        .name = "target-two",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_ref_mixed.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/source/one"), at("/target/two")));
}

TEST(reference_between_oidc_scopes_distinguishes_claims) {
  setenv("ONE_TEST_OIDC_REF_CLAIMS", "confidential", 1);
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<std::string_view, 1> gated_paths{{"/gated"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = open_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_CLAIMS",
        .name = "open",
        .session_secrets = SESSION_SECRETS_UNUSED},
       {.paths = gated_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .claims = CLAIMS_ONE_GROUP,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_CLAIMS",
        .name = "gated",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_ref_claims.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The same provider client admitting a narrower set of people is a different
  // audience, so neither direction reaches the other
  EXPECT_FALSE(
      authentication.reference_permitted(at("/open/one"), at("/gated/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/gated/two"), at("/open/one")));
}

TEST(reference_between_oidc_scopes_distinguishes_email_domains) {
  setenv("ONE_TEST_OIDC_REF_DOMAINS", "confidential", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_domains{{"acme.test"}};
  const std::array<std::string_view, 1> beta_domains{{"other.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .email_domains = alpha_domains,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_DOMAINS",
        .name = "alpha",
        .session_secrets = SESSION_SECRETS_UNUSED},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .email_domains = beta_domains,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_DOMAINS",
        .name = "beta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_ref_domains.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
}

TEST(reference_between_oidc_scopes_ignores_how_rules_were_written) {
  setenv("ONE_TEST_OIDC_REF_SPELLING", "confidential", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // The same two domains, written in a different order and a different case
  const std::array<std::string_view, 2> alpha_domains{
      {"acme.test", "Other.Test"}};
  const std::array<std::string_view, 2> beta_domains{
      {"OTHER.test", "ACME.TEST"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .claims = CLAIMS_TWO_GROUPS,
        .email_domains = alpha_domains,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_SPELLING",
        .name = "alpha",
        .session_secrets = SESSION_SECRETS_UNUSED},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://acme.test",
        .claims = CLAIMS_TWO_GROUPS,
        .email_domains = beta_domains,
        .client_id = "dashboard",
        .client_secret_variable = "ONE_TEST_OIDC_REF_SPELLING",
        .name = "beta",
        .session_secrets = SESSION_SECRETS_UNUSED}}};
  const auto path{test_path("oidc_ref_spelling.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // A domain names a host, so its case says nothing about who is admitted,
  // and neither does the order the rules were written in
  EXPECT_TRUE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(admission_by_an_apikey_policy_identifies_the_principal) {
  setenv("ONE_TEST_KEY_PRINCIPAL", "principal-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("principal_apikey.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto verdict{authentication.admits(at("/internal/foo"),
                                           {.bearer = "principal-secret"})};
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};
  const auto verdict{
      authentication.admits(at("/secure/foo"), {.bearer = SIGNED_TOKEN})};
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                         nullptr)};

  const auto apikey_verdict{
      authentication.admits(at("/both/x"), {.bearer = "principal-mixed"})};
  EXPECT_TRUE(apikey_verdict.allowed);
  EXPECT_TRUE(apikey_verdict.principal.has_value());
  EXPECT_EQ(apikey_verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::ApiKey);
  EXPECT_EQ(apikey_verdict.principal.value().policy, std::size_t{0});

  const auto jwt_verdict{
      authentication.admits(at("/both/x"), {.bearer = SIGNED_TOKEN})};
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};

  // An uncovered path admits an anonymous caller
  const auto anonymous_verdict{
      authentication.admits(at("/open/foo"), {.bearer = ""})};
  EXPECT_TRUE(anonymous_verdict.allowed);
  EXPECT_FALSE(anonymous_verdict.principal.has_value());

  // A denial identifies nobody
  const auto denied_verdict{
      authentication.admits(at("/internal/foo"), {.bearer = "wrong"})};
  EXPECT_FALSE(denied_verdict.allowed);
  EXPECT_FALSE(denied_verdict.principal.has_value());

  // A broken artifact denies with no principal either
  const sourcemeta::one::Authentication missing{
      std::filesystem::path{"/no/such/authentication.bin"},
      stub_fetcher({}, nullptr)};
  const auto missing_verdict{
      missing.admits(at("/internal/foo"), {.bearer = "principal-none"})};
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  // Reference checks read only the policy, so no key set transport is needed
  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // A public schema may not reference one behind the token scope
  EXPECT_FALSE(
      authentication.reference_permitted(at("/open/one"), at("/secure/two")));
  // The token scope may reference a public schema, and itself
  EXPECT_TRUE(
      authentication.reference_permitted(at("/secure/one"), at("/open/two")));
  EXPECT_TRUE(
      authentication.reference_permitted(at("/secure/one"), at("/secure/two")));
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = SIGNED_TOKEN}).allowed);
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher(
                {{"https://idp.test/primary", std::string{SIGNED_KEYS}},
                 {"https://idp.test/secondary", std::string{UNRELATED_KEYS}}},
                nullptr)};
  // The primary path is populated first, which under a per-issuer cache would
  // have leaked its key set to the secondary path
  EXPECT_TRUE(authentication.admits(at("/primary/x"), {.bearer = SIGNED_TOKEN})
                  .allowed);
  EXPECT_FALSE(
      authentication.admits(at("/secondary/x"), {.bearer = SIGNED_TOKEN})
          .allowed);
}

TEST(jwt_claims_admit_only_a_token_carrying_a_named_value) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_ONE_GROUP}}};
  const auto path{test_path("jwt_claims_value.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  EXPECT_TRUE(authentication
                  .admits(at("/secure/x"),
                          {.bearer = token_with(
                               R"JSON({ "groups": [ "platform" ] })JSON")})
                  .allowed);
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "groups": [ "support" ] })JSON")})
                   .allowed);
  // A token the policy would otherwise admit, carrying no such claim at all
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = token_with("{}")})
          .allowed);
}

TEST(jwt_claims_admit_any_one_of_the_named_values) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_TWO_GROUPS}}};
  const auto path{test_path("jwt_claims_alternatives.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  EXPECT_TRUE(authentication
                  .admits(at("/secure/x"),
                          {.bearer = token_with(
                               R"JSON({ "groups": [ "platform" ] })JSON")})
                  .allowed);
  EXPECT_TRUE(authentication
                  .admits(at("/secure/x"),
                          {.bearer = token_with(
                               R"JSON({ "groups": [ "oncall" ] })JSON")})
                  .allowed);
  // Belonging to something else as well takes nothing away
  EXPECT_TRUE(
      authentication
          .admits(at("/secure/x"),
                  {.bearer = token_with(
                       R"JSON({ "groups": [ "support", "oncall" ] })JSON")})
          .allowed);
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "groups": [ "support" ] })JSON")})
                   .allowed);
}

TEST(jwt_claims_require_every_rule_it_declares) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_GROUP_AND_DEPARTMENT}}};
  const auto path{test_path("jwt_claims_cumulative.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  EXPECT_TRUE(
      authentication
          .admits(
              at("/secure/x"),
              {.bearer = token_with(
                   R"JSON({ "groups": [ "platform" ], "department": "engineering" })JSON")})
          .allowed);
  // Either rule alone leaves the other unsatisfied
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "groups": [ "platform" ] })JSON")})
                   .allowed);
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "department": "engineering" })JSON")})
                   .allowed);
}

TEST(jwt_claims_read_a_scope_as_a_space_delimited_set) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_SCOPE}}};
  const auto path{test_path("jwt_claims_scope.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  EXPECT_TRUE(authentication
                  .admits(at("/secure/x"),
                          {.bearer = token_with(
                               R"JSON({ "scope": "registry:read" })JSON")})
                  .allowed);
  // The value is one of several granted, in any position
  EXPECT_TRUE(
      authentication
          .admits(
              at("/secure/x"),
              {.bearer = token_with(
                   R"JSON({ "scope": "openid registry:read profile" })JSON")})
          .allowed);
  EXPECT_FALSE(
      authentication
          .admits(at("/secure/x"),
                  {.bearer = token_with(R"JSON({ "scope": "openid" })JSON")})
          .allowed);
  // A granted scope that merely contains the required one as a prefix is a
  // different grant, and admitting it would hand over what nobody issued
  EXPECT_FALSE(
      authentication
          .admits(at("/secure/x"),
                  {.bearer = token_with(
                       R"JSON({ "scope": "registry:readwrite" })JSON")})
          .allowed);
  // Scope values are case-sensitive by RFC 6749 Section 3.3
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "scope": "Registry:Read" })JSON")})
                   .allowed);
}

TEST(jwt_claims_deny_an_ordinary_rule_that_names_no_value) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_GROUPS_NO_VALUES}}};
  const auto path{test_path("jwt_claims_groups_empty.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  // The same reading the scope rule gets, on the path that defers the
  // comparison rather than making it here
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "groups": [ "platform" ] })JSON")})
                   .allowed);
}

TEST(jwt_claims_deny_a_scope_rule_that_names_no_value) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_SCOPE_NO_VALUES}}};
  const auto path{test_path("jwt_claims_scope_empty.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  // An allow list naming nothing admits nobody, rather than widening to
  // every token that carries any scope at all
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "scope": "registry:read" })JSON")})
                   .allowed);
}

TEST(jwt_claims_deny_a_scope_rule_this_cannot_read) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_SCOPE_UNREADABLE}}};
  const auto path{test_path("jwt_claims_scope_unreadable.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  // A value that is not a scope token denies, since passing it over would
  // leave a rule that admits every token carrying any scope
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "scope": "registry:read" })JSON")})
                   .allowed);
}

TEST(jwt_claims_scope_without_a_constraint_still_requires_a_scope) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_SCOPE_UNCONSTRAINED}}};
  const auto path{test_path("jwt_claims_scope_open.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  // Constraining no value asks only that a scope be carried, so any one does
  EXPECT_TRUE(
      authentication
          .admits(at("/secure/x"),
                  {.bearer = token_with(R"JSON({ "scope": "anything" })JSON")})
          .allowed);
  EXPECT_FALSE(
      authentication.admits(at("/secure/x"), {.bearer = token_with("{}")})
          .allowed);
  // A scope that is not a space-delimited string grants nothing this can read
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "scope": [ "anything" ] })JSON")})
                   .allowed);
}

TEST(jwt_claims_match_a_group_object_on_its_identifier_alone) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_ONE_GROUP}}};
  const auto path{test_path("jwt_claims_group_object.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  // The shape RFC 9068 Section 2.2.3.1 gives the claim by way of RFC 7643
  EXPECT_TRUE(
      authentication
          .admits(
              at("/secure/x"),
              {.bearer = token_with(
                   R"JSON({ "groups": [ { "value": "platform", "display": "Platform" } ] })JSON")})
          .allowed);
  // A display name is neither unique nor stable, so admitting on one would let
  // whoever can rename a group grant access
  EXPECT_FALSE(
      authentication
          .admits(
              at("/secure/x"),
              {.bearer = token_with(
                   R"JSON({ "groups": [ { "value": "g-1", "display": "platform" } ] })JSON")})
          .allowed);
}

TEST(jwt_claims_never_match_a_value_that_is_not_a_string) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_VERIFIED}}};
  const auto path{test_path("jwt_claims_non_string.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  EXPECT_FALSE(
      authentication
          .admits(at("/secure/x"),
                  {.bearer = token_with(R"JSON({ "verified": true })JSON")})
          .allowed);
  EXPECT_FALSE(
      authentication
          .admits(at("/secure/x"),
                  {.bearer = token_with(R"JSON({ "verified": 1 })JSON")})
          .allowed);
  EXPECT_TRUE(
      authentication
          .admits(at("/secure/x"),
                  {.bearer = token_with(R"JSON({ "verified": "true" })JSON")})
          .allowed);
}

TEST(jwt_without_claims_admits_a_token_carrying_none) {
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
  const auto path{test_path("jwt_claims_absent.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  EXPECT_TRUE(
      authentication.admits(at("/secure/x"), {.bearer = token_with("{}")})
          .allowed);
}

TEST(jwt_claims_that_do_not_parse_deny_everything) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_ONE_GROUP},
       {.paths = open_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms}}};
  const auto path{test_path("jwt_claims_corrupt.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  // Overwrite the first byte of the serialised rules, leaving their length
  // intact so that they are read but no longer parse
  std::string buffer;
  {
    std::ifstream input{path, std::ios::binary};
    buffer.assign(std::istreambuf_iterator<char>{input},
                  std::istreambuf_iterator<char>{});
  }
  const auto opening{buffer.find(R"("groups")")};
  if (opening == std::string::npos || opening == 0) {
    throw std::runtime_error{"Could not locate the serialized claim rules"};
  }

  buffer[opening - 1] = '?';
  {
    std::ofstream output{path, std::ios::binary | std::ios::trunc};
    output.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  }

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  // Rules that cannot be read are not passed over, since doing so would drop
  // the restriction and admit everyone the policy was meant to narrow
  EXPECT_FALSE(authentication
                   .admits(at("/secure/x"),
                           {.bearer = token_with(
                                R"JSON({ "groups": [ "platform" ] })JSON")})
                   .allowed);
  // The whole artifact denies, rather than only the policy that carried them
  EXPECT_FALSE(
      authentication.admits(at("/open/x"), {.bearer = token_with("{}")})
          .allowed);
}

TEST(reference_between_jwt_scopes_distinguishes_claims) {
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<std::string_view, 1> gated_paths{{"/gated"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = open_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms},
       {.paths = gated_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_ONE_GROUP}}};
  const auto path{test_path("jwt_claims_reference.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  // Two policies alike but for their rules admit different callers, so the
  // looser one may not reach what the stricter one guards
  EXPECT_FALSE(
      authentication.reference_permitted(at("/open/one"), at("/gated/two")));
  // The reverse is refused too, exactly as a differing token type is. A scope
  // is one indivisible identity rather than a set compared piecewise, so the
  // cost is a build that has to say so, against disclosing a referent to
  // somebody the referrer never admitted
  EXPECT_FALSE(
      authentication.reference_permitted(at("/gated/two"), at("/open/one")));
}

TEST(reference_between_jwt_scopes_ignores_the_order_rules_were_written_in) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_TWO_GROUPS},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_TWO_GROUPS}}};
  const auto path{test_path("jwt_claims_reference_order.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  // The rules arrive canonical, so two policies admitting the same callers
  // carry identical bytes and count as one audience in either direction
  EXPECT_TRUE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
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
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // Same issuer, audience, and key set but a different algorithm is a different
  // scope, so no token could satisfy the reference
  EXPECT_FALSE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  // An identical policy is the same scope
  EXPECT_TRUE(
      authentication.reference_permitted(at("/alpha/one"), at("/gamma/two")));
}

TEST(reference_between_jwt_scopes_ignores_algorithm_order) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 2> one_order{
      {sourcemeta::core::JWSAlgorithm::ES256,
       sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::core::JWSAlgorithm, 2> other_order{
      {sourcemeta::core::JWSAlgorithm::RS256,
       sourcemeta::core::JWSAlgorithm::ES256}};
  // The allow-list decides admission by membership, so these two admit exactly
  // the same tokens and must be the same scope
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = one_order},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = other_order}}};
  const auto path{test_path("jwt_reference_algorithm_order.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_between_jwt_scopes_ignores_token_type_spelling) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // A media type is matched case-insensitively and with the `application/`
  // prefix optional, so these two admit exactly the same tokens
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa,
        .token_type = "at+jwt"},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa,
        .token_type = "Application/AT+JWT"}}};
  const auto path{test_path("jwt_reference_token_type_spelling.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_between_jwt_scopes_ignores_repeated_algorithms) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 3> repeated{
      {sourcemeta::core::JWSAlgorithm::RS256,
       sourcemeta::core::JWSAlgorithm::ES256,
       sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::core::JWSAlgorithm, 2> once{
      {sourcemeta::core::JWSAlgorithm::RS256,
       sourcemeta::core::JWSAlgorithm::ES256}};
  // Naming an algorithm twice admits nothing a single mention does not
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = repeated},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = once}}};
  const auto path{test_path("jwt_reference_repeated_algorithms.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(a_token_type_that_is_the_bare_media_prefix_still_names_a_type) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // Reducing the prefix on its own would leave nothing, and a policy naming no
  // type accepts every type, so a policy that names one must never become one
  // that does not
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa,
        .token_type = "application/"},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa}}};
  const auto path{test_path("jwt_token_type_bare_prefix.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(a_token_type_carrying_a_further_separator_keeps_its_prefix) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // What follows the prefix is only a subtype when it carries no separator of
  // its own, so these two name different types and are read as such
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa,
        .token_type = "application/one/two"},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa,
        .token_type = "one/two"}}};
  const auto path{test_path("jwt_token_type_nested_separator.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_across_swapped_jwt_identities_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // One policy's issuer is the other's audience and vice versa, so the scopes
  // share both strings yet no token satisfies them both
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://login.test",
        .audience = "registry",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "registry",
        .audience = "https://login.test",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa}}};
  const auto path{test_path("jwt_reference_swapped.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_mixing_identities_across_jwt_policies_is_rejected) {
  const std::array<std::string_view, 1> source_paths{{"/source"}};
  const std::array<std::string_view, 1> target_paths{{"/target"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // The referrer pairs an issuer and an audience that the referent only
  // carries through two different policies, so no single referent scope
  // matches and the reference must not slip through their union
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = source_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://alpha.test",
        .audience = "dashboard",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa},
       {.paths = target_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://alpha.test",
        .audience = "registry",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa},
       {.paths = target_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://beta.test",
        .audience = "dashboard",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa}}};
  const auto path{test_path("jwt_reference_mixed.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/source/one"), at("/target/two")));
}

TEST(reference_across_swapped_jwt_key_set_locations_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // The key set location decides which keys sign an admitted token, so
  // trading it with the audience denotes a different scope as surely as
  // trading the issuer does
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "https://idp.test/jwks",
        .jwks_uri = "registry",
        .algorithms = rsa},
       {.paths = beta_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "registry",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = rsa}}};
  const auto path{test_path("jwt_reference_swapped_keys.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(
      authentication.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

// A configured policy path that only differs cosmetically still has to gate the
// location it names. A spelling the matcher could not traverse would leave the
// target public while the configuration reads as though it were gated

TEST(a_policy_path_declared_canonically_gates_its_location) {
  setenv("ONE_TEST_KEY_CANONICAL", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_CANONICAL"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("policy_declared_canonically.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/private/secret"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication
                  .admits(at("/private/secret"), {.bearer = "spelling-secret"})
                  .allowed);
  // A location the policy does not name stays public
  EXPECT_TRUE(
      authentication.admits(at("/public/string"), {.bearer = ""}).allowed);
}

TEST(a_policy_path_carrying_a_dot_segment_gates_its_location) {
  setenv("ONE_TEST_KEY_DOT", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"/./private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_DOT"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("policy_carrying_a_dot_segment.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/private/secret"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication
                  .admits(at("/private/secret"), {.bearer = "spelling-secret"})
                  .allowed);
  // A location the policy does not name stays public
  EXPECT_TRUE(
      authentication.admits(at("/public/string"), {.bearer = ""}).allowed);
}

TEST(a_policy_path_that_climbs_back_into_itself_gates_its_location) {
  setenv("ONE_TEST_KEY_CLIMB", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"/private/../private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_CLIMB"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("policy_that_climbs_back_into_itself.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/private/secret"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication
                  .admits(at("/private/secret"), {.bearer = "spelling-secret"})
                  .allowed);
  // A location the policy does not name stays public
  EXPECT_TRUE(
      authentication.admits(at("/public/string"), {.bearer = ""}).allowed);
}

TEST(a_policy_path_carrying_a_repeated_separator_gates_its_location) {
  setenv("ONE_TEST_KEY_SEPARATOR", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"//private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SEPARATOR"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, keys}}};
  const auto path{test_path("policy_carrying_a_repeated_separator.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.admits(at("/private/secret"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication
                  .admits(at("/private/secret"), {.bearer = "spelling-secret"})
                  .allowed);
  // A location the policy does not name stays public
  EXPECT_TRUE(
      authentication.admits(at("/public/string"), {.bearer = ""}).allowed);
}

TEST(views_of_nothing_are_the_public_one_alone) {
  const auto views{sourcemeta::one::Authentication::views({})};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}}}));
}

TEST(views_name_a_static_key_policy_on_its_own) {
  const std::array<std::string_view, 1> paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .keys = keys,
        .type = sourcemeta::one::Authentication::Type::ApiKey,
        .name = "vault"}}};

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "vault", .policies = {0}}}));
}

TEST(views_name_an_interactive_policy_on_its_own) {
  const std::array<std::string_view, 1> paths{{"/console"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "desk"}}};

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "desk", .policies = {0}}}));
}

TEST(views_name_a_token_policy_on_its_own) {
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "machine"}}};

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "machine", .policies = {0}}}));
}

TEST(views_combine_token_policies_that_name_one_issuer) {
  const std::array<std::string_view, 1> legal_paths{{"/legal"}};
  const std::array<std::string_view, 1> tech_paths{{"/tech"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = legal_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "legal"},
       {.paths = tech_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "tech"}}};

  // A claim is a list and a rule is met by any of its values, so one token can
  // satisfy both
  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "legal", .policies = {0}},
                       {.name = "legal+tech", .policies = {0, 1}},
                       {.name = "tech", .policies = {1}}}));
}

TEST(views_never_combine_token_policies_across_issuers) {
  const std::array<std::string_view, 1> staff_paths{{"/internal"}};
  const std::array<std::string_view, 1> partner_paths{{"/partners"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = staff_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "staff"},
       {.paths = partner_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.partner.com/realms/main",
        .name = "partner"}}};

  // A token carries one issuer and is verified against it before any rule is
  // read, so nobody can ever hold both
  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "partner", .policies = {1}},
                       {.name = "staff", .policies = {0}}}));
}

TEST(views_never_combine_token_policies_testing_one_claim_across_issuers) {
  const std::array<std::string_view, 1> legal_paths{{"/legal"}};
  const std::array<std::string_view, 1> partner_paths{{"/partners"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = legal_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .claims = R"({"department":{"values":["legal"]}})",
        .name = "legal"},
       {.paths = partner_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.partner.com/realms/main",
        .claims = R"({"department":{"values":["legal"]}})",
        .name = "partner-legal"}}};

  // The issuer is decisive and is read first, so testing the same claim for the
  // same value means nothing across them
  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "legal", .policies = {0}},
                       {.name = "partner-legal", .policies = {1}}}));
}

TEST(views_never_combine_interactive_policies_under_one_issuer) {
  const std::array<std::string_view, 1> first_paths{{"/one"}};
  const std::array<std::string_view, 1> second_paths{{"/two"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = first_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "first"},
       {.paths = second_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "second"}}};

  // A browser holds one session naming one policy, so sharing an issuer buys an
  // interactive caller nothing
  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "first", .policies = {0}},
                       {.name = "second", .policies = {1}}}));
}

TEST(views_never_combine_static_key_policies) {
  const std::array<std::string_view, 1> first_paths{{"/one"}};
  const std::array<std::string_view, 1> second_paths{{"/two"}};
  const std::array<std::string_view, 1> first_keys{{"one-secret"}};
  const std::array<std::string_view, 1> second_keys{{"two-secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = first_paths,
        .keys = first_keys,
        .type = sourcemeta::one::Authentication::Type::ApiKey,
        .name = "first"},
       {.paths = second_paths,
        .keys = second_keys,
        .type = sourcemeta::one::Authentication::Type::ApiKey,
        .name = "second"}}};

  // A caller presents one key, so it satisfies one of these whatever it holds
  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "first", .policies = {0}},
                       {.name = "second", .policies = {1}}}));
}

TEST(views_spell_a_combination_the_same_however_it_was_declared) {
  const std::array<std::string_view, 1> tech_paths{{"/tech"}};
  const std::array<std::string_view, 1> legal_paths{{"/legal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = tech_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "tech"},
       {.paths = legal_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "legal"}}};

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "legal", .policies = {1}},
                       {.name = "legal+tech", .policies = {0, 1}},
                       {.name = "tech", .policies = {0}}}));
}

TEST(views_of_three_token_policies_under_one_issuer_are_every_combination) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "a"},
       {.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "b"},
       {.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "c"}}};

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "a", .policies = {0}},
                       {.name = "a+b", .policies = {0, 1}},
                       {.name = "a+b+c", .policies = {0, 1, 2}},
                       {.name = "a+c", .policies = {0, 2}},
                       {.name = "b", .policies = {1}},
                       {.name = "b+c", .policies = {1, 2}},
                       {.name = "c", .policies = {2}}}));
}

TEST(views_mix_a_combining_group_with_policies_that_stand_alone) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = paths,
        .keys = keys,
        .type = sourcemeta::one::Authentication::Type::ApiKey,
        .name = "vault"},
       {.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "legal"},
       {.paths = paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "tech"}}};

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "legal", .policies = {1}},
                       {.name = "legal+tech", .policies = {1, 2}},
                       {.name = "tech", .policies = {2}},
                       {.name = "vault", .policies = {0}}}));
}

TEST(views_hold_the_public_one_even_when_every_path_is_governed) {
  const std::array<std::string_view, 1> paths{{"/"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .keys = keys,
        .type = sourcemeta::one::Authentication::Type::ApiKey,
        .name = "everything"}}};

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "everything", .policies = {0}}}));
}

TEST(views_of_six_token_policies_under_one_issuer_are_every_combination) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, 6> policies{};
  const std::array<std::string_view, 6> names{{"a", "b", "c", "d", "e", "f"}};
  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).type = sourcemeta::one::Authentication::Type::JWT;
    policies.at(index).issuer = "https://idp.example.com/realms/staff";
    policies.at(index).name = names.at(index);
  }

  // Two to the sixth, being every non-empty combination plus the anonymous one
  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views.size(), 64);
}

TEST(views_of_two_issuer_groups_are_a_sum_rather_than_a_product) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, 8> policies{};
  const std::array<std::string_view, 8> names{
      {"a", "b", "c", "d", "e", "f", "g", "h"}};
  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).type = sourcemeta::one::Authentication::Type::JWT;
    policies.at(index).issuer = index < 4
                                    ? "https://idp.example.com/realms/staff"
                                    : "https://idp.partner.com/realms/main";
    policies.at(index).name = names.at(index);
  }

  // Each group contributes its own combinations and nothing crosses between
  // them, so the total adds rather than multiplies
  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views.size(), 1 + 15 + 15);
}

TEST(views_of_the_largest_combinable_group_are_every_combination) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy,
             sourcemeta::one::Authentication::MAXIMUM_COMBINABLE_POLICIES>
      policies{};
  std::vector<std::string> names;
  names.reserve(policies.size());
  for (std::size_t index{0}; index < policies.size(); index++) {
    names.push_back("p" + std::to_string(index));
  }

  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).type = sourcemeta::one::Authentication::Type::JWT;
    policies.at(index).issuer = "https://idp.example.com/realms/staff";
    policies.at(index).name = names.at(index);
  }

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views.size(),
            (std::size_t{1}
             << sourcemeta::one::Authentication::MAXIMUM_COMBINABLE_POLICIES));
  EXPECT_EQ(views.at(0).name, "public");
}

TEST(views_refuse_a_group_whose_combinations_cannot_be_produced) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy,
             sourcemeta::one::Authentication::MAXIMUM_COMBINABLE_POLICIES + 1>
      policies{};
  std::vector<std::string> names;
  names.reserve(policies.size());
  for (std::size_t index{0}; index < policies.size(); index++) {
    names.push_back("p" + std::to_string(index));
  }

  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).type = sourcemeta::one::Authentication::Type::JWT;
    policies.at(index).issuer = "https://idp.example.com/realms/staff";
    policies.at(index).name = names.at(index);
  }

  try {
    const auto views{sourcemeta::one::Authentication::views(policies)};
    EXPECT_EQ(views.size(), 0);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationTooManyViewsError &error) {
    EXPECT_STREQ(error.what(),
                 "Too many authentication policies share an issuer");
    EXPECT_EQ(error.issuer(), "https://idp.example.com/realms/staff");
    EXPECT_EQ(error.count(),
              sourcemeta::one::Authentication::MAXIMUM_COMBINABLE_POLICIES + 1);
  }
}

TEST(views_of_many_policies_across_issuers_are_never_refused) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, 40> policies{};
  std::vector<std::string> names;
  names.reserve(policies.size());
  for (std::size_t index{0}; index < policies.size(); index++) {
    names.push_back("p" + std::to_string(index));
  }

  // Well past the ceiling in total, and no group approaches it, so nothing is
  // refused. What a build makes of forty views is not decided here
  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).type = sourcemeta::one::Authentication::Type::JWT;
    policies.at(index).issuer = names.at(index);
    policies.at(index).name = names.at(index);
  }

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views.size(), 41);
}

TEST(a_presented_key_decides_over_a_session) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_PRECEDENCE_SECRET", "confidential", 1);
  setenv("ONE_TEST_PRECEDENCE_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> portal_paths{{"/portal"}};
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{
      {"ONE_TEST_PRECEDENCE_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = portal_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_PRECEDENCE_SECRET",
        .name = "okta",
        .session_secrets = SESSION_SECRETS},
       {.paths = machine_paths, .keys = machine_keys, .name = "machine"}}};
  const auto path{test_path("precedence.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta", "subject": "jane@acme.test" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  // Each alone opens what it governs, which is what makes the pair below a
  // choice between two live credentials rather than one working answer
  EXPECT_TRUE(
      authentication
          .admits(at("/portal/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
  EXPECT_TRUE(
      authentication.admits(at("/machine/x"), {.bearer = "machine-secret"})
          .allowed);

  // Presented together, the request is read as the key it carried, so the
  // portal the session would have opened is refused
  EXPECT_FALSE(authentication
                   .admits(at("/portal/x"), {.bearer = "machine-secret",
                                             .cookies = fields(cookies)})
                   .allowed);
  const auto verdict{
      authentication.admits(at("/machine/x"), {.bearer = "machine-secret",
                                               .cookies = fields(cookies)})};
  EXPECT_TRUE(verdict.allowed);
  EXPECT_TRUE(verdict.principal.has_value());
  EXPECT_EQ(verdict.principal.value().type,
            sourcemeta::one::Authentication::Type::ApiKey);
  EXPECT_EQ(verdict.principal.value().policy, std::size_t{1});
}

TEST(a_presented_key_that_opens_nothing_sets_a_session_aside) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_FALLBACK_SECRET", "confidential", 1);
  setenv("ONE_TEST_FALLBACK_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> portal_paths{{"/portal"}};
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{{"ONE_TEST_FALLBACK_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = portal_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_FALLBACK_SECRET",
        .name = "okta",
        .session_secrets = SESSION_SECRETS},
       {.paths = machine_paths, .keys = machine_keys, .name = "machine"}}};
  const auto path{test_path("precedence_stale.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta", "subject": "jane@acme.test" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  // A key that opens nothing is still a key that was presented, so the session
  // is set aside and nothing admits. The cost of the rule, and the reason it is
  // worth stating rather than leaving to be discovered
  EXPECT_FALSE(authentication
                   .admits(at("/portal/x"), {.bearer = "retired-secret",
                                             .cookies = fields(cookies)})
                   .allowed);

  // The same session presented on its own still opens it, so what changed is
  // what the request carried rather than whether the session is any good
  EXPECT_TRUE(
      authentication
          .admits(at("/portal/x"), {.bearer = "", .cookies = fields(cookies)})
          .allowed);
}

TEST(a_caller_presenting_nothing_belongs_to_no_policy) {
  setenv("ONE_TEST_CLASSIFY_ANONYMOUS_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{
      {"ONE_TEST_CLASSIFY_ANONYMOUS_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths, .keys = keys, .name = "machine"}}};
  const auto path{test_path("classify_anonymous.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.classify({.bearer = ""}),
            sourcemeta::one::Authentication::PolicySet{0});
}

TEST(a_credential_opening_nothing_belongs_to_no_policy) {
  setenv("ONE_TEST_CLASSIFY_UNKNOWN_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_CLASSIFY_UNKNOWN_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths, .keys = keys, .name = "machine"}}};
  const auto path{test_path("classify_unknown.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.classify({.bearer = "retired-secret"}),
            sourcemeta::one::Authentication::PolicySet{0});
}

TEST(a_key_places_its_caller_in_the_policy_it_opens) {
  setenv("ONE_TEST_CLASSIFY_FIRST_KEY", "first-secret", 1);
  setenv("ONE_TEST_CLASSIFY_SECOND_KEY", "second-secret", 1);
  const std::array<std::string_view, 1> first_paths{{"/first"}};
  const std::array<std::string_view, 1> second_paths{{"/second"}};
  const std::array<std::string_view, 1> first_keys{
      {"ONE_TEST_CLASSIFY_FIRST_KEY"}};
  const std::array<std::string_view, 1> second_keys{
      {"ONE_TEST_CLASSIFY_SECOND_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = first_paths, .keys = first_keys, .name = "first"},
       {.paths = second_paths, .keys = second_keys, .name = "second"}}};
  const auto path{test_path("classify_key.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.classify({.bearer = "first-secret"}),
            sourcemeta::one::Authentication::PolicySet{0b01});
  EXPECT_EQ(authentication.classify({.bearer = "second-secret"}),
            sourcemeta::one::Authentication::PolicySet{0b10});
}

TEST(a_key_is_placed_without_reference_to_any_path) {
  setenv("ONE_TEST_CLASSIFY_DEEP_KEY", "deep-secret", 1);
  const std::array<std::string_view, 1> paths{{"/deep/inside/somewhere"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_CLASSIFY_DEEP_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths, .keys = keys, .name = "deep"}}};
  const auto path{test_path("classify_deep.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // The gate answers differently for two locations, and the placement answers
  // once for the caller, which is what makes it describe the whole registry
  EXPECT_TRUE(
      authentication
          .admits(at("/deep/inside/somewhere/x"), {.bearer = "deep-secret"})
          .allowed);
  EXPECT_TRUE(authentication.admits(at("/elsewhere"), {.bearer = "deep-secret"})
                  .allowed);
  EXPECT_EQ(authentication.classify({.bearer = "deep-secret"}),
            sourcemeta::one::Authentication::PolicySet{0b1});
}

TEST(a_key_opening_two_policies_is_read_as_the_first_declared) {
  setenv("ONE_TEST_CLASSIFY_SHARED_EARLY", "shared-secret", 1);
  setenv("ONE_TEST_CLASSIFY_SHARED_LATE", "shared-secret", 1);
  const std::array<std::string_view, 1> early_paths{{"/early"}};
  const std::array<std::string_view, 1> late_paths{{"/late"}};
  const std::array<std::string_view, 1> early_keys{
      {"ONE_TEST_CLASSIFY_SHARED_EARLY"}};
  const std::array<std::string_view, 1> late_keys{
      {"ONE_TEST_CLASSIFY_SHARED_LATE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = early_paths, .keys = early_keys, .name = "early"},
       {.paths = late_paths, .keys = late_keys, .name = "late"}}};
  const auto path{test_path("classify_shared.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  // Two variables holding one value is the form the configuration cannot see,
  // so the gate opens both and the placement still names one
  EXPECT_TRUE(authentication.admits(at("/early/x"), {.bearer = "shared-secret"})
                  .allowed);
  EXPECT_TRUE(authentication.admits(at("/late/x"), {.bearer = "shared-secret"})
                  .allowed);
  EXPECT_EQ(authentication.classify({.bearer = "shared-secret"}),
            sourcemeta::one::Authentication::PolicySet{0b01});
}

TEST(a_token_belongs_to_every_policy_of_its_issuer_that_it_satisfies) {
  const std::array<std::string_view, 1> platform_paths{{"/platform"}};
  const std::array<std::string_view, 1> oncall_paths{{"/oncall"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = platform_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_ONE_GROUP,
        .name = "platform"},
       {.paths = oncall_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "acme",
        .audience = "client",
        .jwks_uri = "https://idp.test/jwks",
        .algorithms = algorithms,
        .claims = CLAIMS_ONCALL_GROUP,
        .name = "oncall"}}};
  const auto path{test_path("classify_token_groups.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                         nullptr)};
  EXPECT_EQ(
      authentication.classify(
          {.bearer = token_with(R"JSON({ "groups": [ "platform" ] })JSON")}),
      sourcemeta::one::Authentication::PolicySet{0b01});
  EXPECT_EQ(
      authentication.classify(
          {.bearer = token_with(R"JSON({ "groups": [ "oncall" ] })JSON")}),
      sourcemeta::one::Authentication::PolicySet{0b10});
  // One token carrying both reaches both areas, so a placement naming either
  // alone would hide one of them
  EXPECT_EQ(authentication.classify(
                {.bearer = token_with(
                     R"JSON({ "groups": [ "oncall", "platform" ] })JSON")}),
            sourcemeta::one::Authentication::PolicySet{0b11});
  EXPECT_EQ(
      authentication.classify(
          {.bearer = token_with(R"JSON({ "groups": [ "support" ] })JSON")}),
      sourcemeta::one::Authentication::PolicySet{0});
}

TEST(a_session_places_its_caller_in_the_policy_that_established_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_CLASSIFY_SESSION_SECRET", "confidential", 1);
  setenv("ONE_TEST_CLASSIFY_SESSION_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> portal_paths{{"/portal"}};
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{
      {"ONE_TEST_CLASSIFY_SESSION_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = portal_paths,
        .type = sourcemeta::one::Authentication::Type::OIDC,
        .issuer = "acme",
        .client_id = "client",
        .client_secret_variable = "ONE_TEST_CLASSIFY_SESSION_SECRET",
        .name = "okta",
        .session_secrets = SESSION_SECRETS},
       {.paths = machine_paths, .keys = machine_keys, .name = "machine"}}};
  const auto path{test_path("classify_session.bin")};
  sourcemeta::one::Authentication::save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      path, stub_fetcher({}, nullptr)};
  const auto sealed{sourcemeta::one::Authentication::seal_value(
      R"JSON({ "policy": "okta", "subject": "jane@acme.test" })JSON",
      sourcemeta::one::Authentication::Purpose::Session, SESSION_SECRET,
      minted_now(), session_expiry())};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  EXPECT_EQ(authentication.classify({.bearer = "", .cookies = fields(cookies)}),
            sourcemeta::one::Authentication::PolicySet{0b01});
  EXPECT_EQ(authentication.classify({.bearer = "machine-secret"}),
            sourcemeta::one::Authentication::PolicySet{0b10});
  // A request carrying a key is read as that key, so the session it also
  // carried places nobody, exactly as it admits nobody
  EXPECT_EQ(authentication.classify(
                {.bearer = "machine-secret", .cookies = fields(cookies)}),
            sourcemeta::one::Authentication::PolicySet{0b10});
  EXPECT_EQ(authentication.classify(
                {.bearer = "retired-secret", .cookies = fields(cookies)}),
            sourcemeta::one::Authentication::PolicySet{0});
}
