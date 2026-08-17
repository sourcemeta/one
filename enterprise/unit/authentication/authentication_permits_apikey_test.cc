#include "authentication_helpers.h"

// Whether a caller reaches a location, as the key they present is varied.
// Every case builds a table, asks for a caller, and asks what it reaches

TEST(apikey_admits_matching_credential) {
  setenv("ONE_TEST_KEY_MATCH", "secret-match", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MATCH"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "secret-match"})));
  EXPECT_FALSE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "wrong"})));
  EXPECT_FALSE(authentication.permits(AT("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(apikey_with_multiple_keys_admits_any) {
  setenv("ONE_TEST_KEY_MULTI_A", "key-a", 1);
  setenv("ONE_TEST_KEY_MULTI_B", "key-b", 1);
  const std::array<std::string_view, 2> keys{
      {"ONE_TEST_KEY_MULTI_A", "ONE_TEST_KEY_MULTI_B"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "key-a"})));
  EXPECT_TRUE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "key-b"})));
  EXPECT_FALSE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "key-c"})));
}

TEST(apikey_with_unset_variable_denies) {
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_UNSET"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "anything"})));
  EXPECT_FALSE(authentication.permits(AT("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(apikey_with_an_empty_variable_denies) {
  setenv("ONE_TEST_KEY_EMPTY", "", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_EMPTY"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{TEST_PATH("apikey_empty.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  // A variable an operator meant to hold a key but left blank gates the path
  // exactly as an unset one does, rather than opening it to everyone
  EXPECT_FALSE(authentication.permits(AT("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "anything"})));
}

TEST(apikey_ignores_an_empty_variable_beside_a_real_one) {
  setenv("ONE_TEST_KEY_PAIR_BLANK", "", 1);
  setenv("ONE_TEST_KEY_PAIR_REAL", "pair-secret", 1);
  const std::array<std::string_view, 2> keys{
      {"ONE_TEST_KEY_PAIR_BLANK", "ONE_TEST_KEY_PAIR_REAL"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  // The blank one neither admits anybody nor keeps the key beside it from
  // working
  EXPECT_TRUE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "pair-secret"})));
  EXPECT_FALSE(authentication.permits(AT("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "wrong"})));
}

TEST(sha256_policy_with_an_empty_variable_denies) {
  setenv("ONE_TEST_KEY_SHA_EMPTY", "", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHA_EMPTY"}};
  const std::array<std::string_view, 1> paths{{"/secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = keys,
            .algorithm = sourcemeta::one::Authentication::Algorithm::Sha256}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/secret/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(
      AT("/secret/foo"), authentication.caller({.bearer = "anything"})));
  // Nor does the digest of nothing, which is what an empty credential hashes to
  EXPECT_FALSE(authentication.permits(
      AT("/secret/foo"),
      authentication.caller({.bearer = sourcemeta::core::sha256("")})));
}

TEST(sha256_policy_admits_the_matching_credential) {
  const std::string raw{"raw-secret-key"};
  setenv("ONE_TEST_KEY_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHA"}};
  const std::array<std::string_view, 1> paths{{"/secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = keys,
            .algorithm = sourcemeta::one::Authentication::Algorithm::Sha256}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_TRUE(authentication.permits(AT("/secret/foo"),
                                     authentication.caller({.bearer = raw})));
  EXPECT_FALSE(authentication.permits(
      AT("/secret/foo"), authentication.caller({.bearer = "wrong"})));
  EXPECT_FALSE(authentication.permits(AT("/secret/foo"),
                                      authentication.caller({.bearer = ""})));
  // Presenting the stored hash itself does not authenticate
  EXPECT_FALSE(authentication.permits(
      AT("/secret/foo"),
      authentication.caller({.bearer = sourcemeta::core::sha256(raw)})));
}

TEST(mixed_algorithms_admit_either_key_with_identity_first) {
  setenv("ONE_TEST_KEY_MIXA_ID", "plain-a", 1);
  const std::string raw{"hashed-a"};
  setenv("ONE_TEST_KEY_MIXA_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> paths{{"/mixed"}};
  const std::array<std::string_view, 1> identity_keys{{"ONE_TEST_KEY_MIXA_ID"}};
  const std::array<std::string_view, 1> sha256_keys{{"ONE_TEST_KEY_MIXA_SHA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "identity",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{
                .keys = identity_keys,
                .algorithm =
                    sourcemeta::one::Authentication::Algorithm::Identity}},
       {.paths = paths,
        .name = "hashed",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = sha256_keys,
            .algorithm = sourcemeta::one::Authentication::Algorithm::Sha256}}}};
  const auto path{TEST_PATH("mixed_identity_first.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  // Either key type opens the path regardless of declaration order. The sha256
  // key must work even though the identity policy is checked first and fails
  EXPECT_TRUE(authentication.permits(
      AT("/mixed/x"), authentication.caller({.bearer = "plain-a"})));
  EXPECT_TRUE(authentication.permits(AT("/mixed/x"),
                                     authentication.caller({.bearer = raw})));
  EXPECT_FALSE(authentication.permits(
      AT("/mixed/x"), authentication.caller({.bearer = "neither"})));
}

TEST(mixed_algorithms_admit_either_key_with_sha256_first) {
  setenv("ONE_TEST_KEY_MIXB_ID", "plain-b", 1);
  const std::string raw{"hashed-b"};
  setenv("ONE_TEST_KEY_MIXB_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> paths{{"/mixed"}};
  const std::array<std::string_view, 1> identity_keys{{"ONE_TEST_KEY_MIXB_ID"}};
  const std::array<std::string_view, 1> sha256_keys{{"ONE_TEST_KEY_MIXB_SHA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "hashed",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{
                .keys = sha256_keys,
                .algorithm =
                    sourcemeta::one::Authentication::Algorithm::Sha256}},
       {.paths = paths,
        .name = "identity",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = identity_keys,
            .algorithm =
                sourcemeta::one::Authentication::Algorithm::Identity}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  // The identity key must work even though the sha256 policy is checked first
  EXPECT_TRUE(authentication.permits(AT("/mixed/x"),
                                     authentication.caller({.bearer = raw})));
  EXPECT_TRUE(authentication.permits(
      AT("/mixed/x"), authentication.caller({.bearer = "plain-b"})));
  EXPECT_FALSE(authentication.permits(
      AT("/mixed/x"), authentication.caller({.bearer = "neither"})));
}

TEST(mixed_apikey_and_jwt_policies_admit_either_credential) {
  setenv("ONE_TEST_KEY_BOTH", "static-secret", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_BOTH"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{TEST_PATH("jwt_mixed.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  // The static key opens the path
  EXPECT_TRUE(authentication.permits(
      AT("/both/x"), authentication.caller({.bearer = "static-secret"})));
  // The token opens the path
  EXPECT_TRUE(authentication.permits(
      AT("/both/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  // Neither a wrong key nor a wrong token opens it
  EXPECT_FALSE(authentication.permits(
      AT("/both/x"), authentication.caller({.bearer = "wrong"})));
}

// A policy naming no rule admits whoever its provider vouched for, so signing
// in is the whole of it

TEST(admission_by_an_apikey_policy_identifies_the_principal) {
  setenv("ONE_TEST_KEY_PRINCIPAL", "principal-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto permitted{authentication.permits(
      AT("/internal/foo"),
      authentication.caller({.bearer = "principal-secret"}))};
  EXPECT_TRUE(permitted);
}

TEST(anonymous_and_denied_verdicts_carry_no_principal) {
  setenv("ONE_TEST_KEY_PRINCIPAL_NONE", "principal-none", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL_NONE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{TEST_PATH("principal_none.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};

  // An uncovered path admits an anonymous caller
  const auto anonymous_permitted{authentication.permits(
      AT("/open/foo"), authentication.caller({.bearer = ""}))};
  EXPECT_TRUE(anonymous_permitted);

  // A denial identifies nobody
  const auto denied_permitted{authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "wrong"}))};
  EXPECT_FALSE(denied_permitted);

  // A broken artifact denies with no principal either
  const sourcemeta::one::Authentication missing{
      sourcemeta::one::Authentication::Table{
          std::filesystem::path{"/no/such/authentication.bin"}},
      STUB_FETCHER({}, nullptr)};
  const auto missing_permitted{missing.permits(
      AT("/internal/foo"), missing.caller({.bearer = "principal-none"}))};
  EXPECT_FALSE(missing_permitted);
}

TEST(save_creates_the_directory_it_writes_into) {
  setenv("ONE_TEST_KEY_NESTED", "nested-secret", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NESTED"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{TEST_PATH("nested") / "deeper" / "authentication.bin"};
  std::filesystem::remove_all(TEST_PATH("nested"));
  SAVE(policies, path, path, ANYWHERE);

  EXPECT_TRUE(std::filesystem::exists(path));
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "nested-secret"})));
}
