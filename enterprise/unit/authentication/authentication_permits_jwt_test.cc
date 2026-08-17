#include "authentication_helpers.h"

// Whether a caller reaches a location, as the token they present is varied.
// Every case builds a table, asks for a caller, and asks what it reaches

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

TEST(admission_by_a_jwt_policy_identifies_the_principal) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  const auto permitted{authentication.permits(
      AT("/secure/foo"), authentication.caller({.bearer = SIGNED_TOKEN}))};
  EXPECT_TRUE(permitted);
}

TEST(principal_identifies_the_admitting_policy_among_several) {
  setenv("ONE_TEST_KEY_PRINCIPAL_MIXED", "principal-mixed", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL_MIXED"}};
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
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};

  const auto apikey_permitted{authentication.permits(
      AT("/both/x"), authentication.caller({.bearer = "principal-mixed"}))};
  EXPECT_TRUE(apikey_permitted);

  const auto jwt_permitted{authentication.permits(
      AT("/both/x"), authentication.caller({.bearer = SIGNED_TOKEN}))};
  EXPECT_TRUE(jwt_permitted);
}

TEST(jwt_admits_a_valid_token_and_caches_the_key_set) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   calls)};
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = "not-a-token"})));
  EXPECT_FALSE(authentication.permits(AT("/secure/x"),
                                      authentication.caller({.bearer = ""})));
  // A second valid request reuses the cached key set rather than refetching
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  EXPECT_EQ(*calls, 1);
}

TEST(jwt_admits_a_token_whose_type_the_policy_requires) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .token_type = "at+jwt"}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
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
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .token_type = "JWT"}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_without_a_required_type_admits_any_type) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // A provider that does not stamp the header cannot be told apart this way,
  // so a policy that names no type keeps working against one
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_a_token_for_the_wrong_audience) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "different",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_a_token_from_the_wrong_issuer) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "different",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_a_disallowed_algorithm) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_when_the_signing_key_is_absent) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{UNRELATED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_when_the_key_set_cannot_be_fetched) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_resolves_the_key_set_through_discovery) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // No key set location is pinned, so it is discovered from the issuer
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://acme.test",
            .audience = "client",
            .algorithms = algorithms}}}};
  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://acme.test/.well-known/openid-configuration",
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
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
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
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .algorithms = algorithms}}}};
  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER(
          {{"acme/.well-known/openid-configuration",
            R"JSON({ "issuer": "acme", "jwks_uri": "https://acme.test/keys" })JSON"},
           {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
          calls)};
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  EXPECT_EQ(*calls, 0);
}

TEST(jwt_policies_sharing_an_issuer_use_their_own_key_set) {
  const std::array<std::string_view, 1> primary_paths{{"/primary"}};
  const std::array<std::string_view, 1> secondary_paths{{"/secondary"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = primary_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/primary",
                .algorithms = algorithms}},
       {.paths = secondary_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/secondary",
            .algorithms = algorithms}}}};
  const auto path{TEST_PATH("jwt_shared_issuer.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      STUB_FETCHER(
          {{"https://idp.test/primary", std::string{SIGNED_KEYS}},
           {"https://idp.test/secondary", std::string{UNRELATED_KEYS}}},
          nullptr)};
  // The primary path is populated first, which under a per-issuer cache would
  // have leaked its key set to the secondary path
  EXPECT_TRUE(authentication.permits(
      AT("/primary/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  EXPECT_FALSE(authentication.permits(
      AT("/secondary/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_claims_admit_only_a_token_carrying_a_named_value) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_ONE_GROUP}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "groups": [ "platform" ] })JSON")})));
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "groups": [ "support" ] })JSON")})));
  // A token the policy would otherwise admit, carrying no such claim at all
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = TOKEN_WITH("{}")})));
}

TEST(jwt_claims_admit_any_one_of_the_named_values) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_TWO_GROUPS}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "groups": [ "platform" ] })JSON")})));
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "groups": [ "oncall" ] })JSON")})));
  // Belonging to something else as well takes nothing away
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(
               R"JSON({ "groups": [ "support", "oncall" ] })JSON")})));
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "groups": [ "support" ] })JSON")})));
}

TEST(jwt_claims_require_every_rule_it_declares) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_GROUP_AND_DEPARTMENT}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(
               R"JSON({ "groups": [ "platform" ], "department": "engineering" })JSON")})));
  // Either rule alone leaves the other unsatisfied
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "groups": [ "platform" ] })JSON")})));
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer =
               TOKEN_WITH(R"JSON({ "department": "engineering" })JSON")})));
}

TEST(jwt_claims_read_a_scope_as_a_space_delimited_set) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_SCOPE}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "scope": "registry:read" })JSON")})));
  // The value is one of several granted, in any position
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(
               R"JSON({ "scope": "openid registry:read profile" })JSON")})));
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "scope": "openid" })JSON")})));
  // A granted scope that merely contains the required one as a prefix is a
  // different grant, and admitting it would hand over what nobody issued
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer =
               TOKEN_WITH(R"JSON({ "scope": "registry:readwrite" })JSON")})));
  // Scope values are case-sensitive by RFC 6749 Section 3.3
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "scope": "Registry:Read" })JSON")})));
}

TEST(jwt_claims_deny_an_ordinary_rule_that_names_no_value) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_GROUPS_NO_VALUES}}}};
  const auto path{TEST_PATH("jwt_claims_groups_empty.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // The same reading the scope rule gets, on the path that defers the
  // comparison rather than making it here
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "groups": [ "platform" ] })JSON")})));
}

TEST(jwt_claims_deny_a_scope_rule_that_names_no_value) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_SCOPE_NO_VALUES}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // An allow list naming nothing admits nobody, rather than widening to
  // every token that carries any scope at all
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "scope": "registry:read" })JSON")})));
}

TEST(jwt_claims_deny_a_scope_rule_this_cannot_read) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_SCOPE_UNREADABLE}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // A value that is not a scope token denies, since passing it over would
  // leave a rule that admits every token carrying any scope
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "scope": "registry:read" })JSON")})));
}

TEST(jwt_claims_scope_without_a_constraint_still_requires_a_scope) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_SCOPE_UNCONSTRAINED}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // Constraining no value asks only that a scope be carried, so any one does
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "scope": "anything" })JSON")})));
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = TOKEN_WITH("{}")})));
  // A scope that is not a space-delimited string grants nothing this can read
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "scope": [ "anything" ] })JSON")})));
}

TEST(jwt_claims_match_a_group_object_on_its_identifier_alone) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_ONE_GROUP}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // The shape RFC 9068 Section 2.2.3.1 gives the claim by way of RFC 7643
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(
               R"JSON({ "groups": [ { "value": "platform", "display": "Platform" } ] })JSON")})));
  // A display name is neither unique nor stable, so admitting on one would let
  // whoever can rename a group grant access
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(
               R"JSON({ "groups": [ { "value": "g-1", "display": "platform" } ] })JSON")})));
}

TEST(jwt_claims_never_match_a_value_that_is_not_a_string) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_VERIFIED}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "verified": true })JSON")})));
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "verified": 1 })JSON")})));
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "verified": "true" })JSON")})));
}

TEST(jwt_without_claims_admits_a_token_carrying_none) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = TOKEN_WITH("{}")})));
}

TEST(jwt_claims_that_do_not_parse_deny_everything) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms,
                .claims = CLAIMS_ONE_GROUP}},
       {.paths = open_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{TEST_PATH("jwt_claims_corrupt.bin")};
  SAVE(policies, path, path, ANYWHERE);

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
      sourcemeta::one::Authentication::Table{path},
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // Rules that cannot be read are not passed over, since doing so would drop
  // the restriction and admit everyone the policy was meant to narrow
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"),
      authentication.caller(
          {.bearer = TOKEN_WITH(R"JSON({ "groups": [ "platform" ] })JSON")})));
  // The whole artifact denies, rather than only the policy that carried them
  EXPECT_FALSE(authentication.permits(
      AT("/open/x"), authentication.caller({.bearer = TOKEN_WITH("{}")})));
}
