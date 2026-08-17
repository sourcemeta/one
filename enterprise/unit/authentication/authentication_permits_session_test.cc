#include "authentication_helpers.h"

// Whether a caller reaches a location, as the session cookie they carry is
// varied. Every case builds a table, asks for a caller, and asks what it
// reaches

static constexpr std::array<std::string_view, 1> SESSION_SECRETS_UNSET{
    {"ONE_TEST_OIDC_UNSET_SECRET"}};

static constexpr std::array<std::string_view, 1> SESSION_SECRETS_BLANK{
    {"ONE_TEST_OIDC_BLANK_SECRET"}};
static constexpr std::array<std::string_view, 2> SESSION_SECRETS_ROTATED{
    {"ONE_TEST_OIDC_ROTATED_SECRET", "ONE_TEST_OIDC_ROTATED_SECRET_OLD"}};

TEST(oidc_policy_admits_no_presented_credential) {
  setenv("ONE_TEST_OIDC_DENY", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_DENY",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{TEST_PATH("oidc_deny.bin")};
  SAVE(policies, path, path, ANYWHERE);

  // The provider is reachable and would verify the token, yet no presented
  // credential opens the path, not even one the equivalent token policy
  // would accept, and the provider is never contacted
  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      STUB_FETCHER({{"acme/.well-known/openid-configuration",
                     R"JSON({ "jwks_uri": "https://acme.test/keys" })JSON"},
                    {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
                   calls)};

  const auto empty_permitted{authentication.permits(
      AT("/portal/x"), authentication.caller({.bearer = ""}))};
  EXPECT_FALSE(empty_permitted);

  const auto secret_permitted{authentication.permits(
      AT("/portal/x"), authentication.caller({.bearer = "confidential"}))};
  EXPECT_FALSE(secret_permitted);

  const auto token_permitted{authentication.permits(
      AT("/portal/x"), authentication.caller({.bearer = SIGNED_TOKEN}))};
  EXPECT_FALSE(token_permitted);

  EXPECT_EQ(*calls, 0);
}

TEST(union_of_an_apikey_and_an_oidc_policy_admits_only_the_key) {
  setenv("ONE_TEST_KEY_OIDC_UNION", "union-secret", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_OIDC_UNION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_KEY_OIDC_UNION",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};

  const auto key_permitted{authentication.permits(
      AT("/both/x"), authentication.caller({.bearer = "union-secret"}))};
  EXPECT_TRUE(key_permitted);

  const auto token_permitted{authentication.permits(
      AT("/both/x"), authentication.caller({.bearer = SIGNED_TOKEN}))};
  EXPECT_FALSE(token_permitted);
}

TEST(oidc_policy_admits_its_session_cookie) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SESSION",
            .session_secrets = SESSION_SECRETS}}}};
  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{TABLE(policies),
                                                       STUB_FETCHER({}, calls)};

  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "jane@acme.test")};
  const std::string cookies{"theme=dark; sourcemeta_one_session=" + sealed};

  const auto permitted{authentication.permits(
      AT("/portal/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)}))};
  EXPECT_TRUE(permitted);
  EXPECT_EQ(*calls, 0);

  const auto anonymous_permitted{authentication.permits(
      AT("/portal/x"), authentication.caller({.bearer = ""}))};
  EXPECT_FALSE(anonymous_permitted);
}

TEST(session_cookie_is_bound_to_the_policy_it_was_minted_for) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_OIDC_BIND_A",
                .session_secrets = SESSION_SECRETS}},
       {.paths = beta_paths,
        .name = "google",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_BIND_B",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{TEST_PATH("oidc_session_bound.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};

  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "")};

  // The session opens the path its policy governs
  const std::string okta_cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_TRUE(authentication.permits(
      AT("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(okta_cookies)})));

  // And not a path governed by another policy. Both policies here read the
  // same session secret, so the value verifies under either and the payload is
  // the only thing that tells them apart. There is no cookie name left to
  // separate them, which makes this the control rather than a second opinion
  EXPECT_FALSE(authentication.permits(
      AT("/beta/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(okta_cookies)})));
}

TEST(forged_session_cookie_is_denied) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_FOREIGN_SECRET", "other-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_FORGED",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("oidc_session_forged"), ANYWHERE)},
      STUB_FETCHER({}, nullptr)};

  // The control, which is a session this policy did mint
  const auto genuine{SESSION_FOR("okta", SESSION_SECRETS, "")};
  EXPECT_TRUE(authentication.permits(
      AT("/portal/x"),
      authentication.caller(
          {.cookies = FIELDS("sourcemeta_one_session=" + genuine)})));

  // A session minted under a secret this policy does not hold, which differs
  // from the one above in that alone
  const std::array<std::string_view, 1> foreign_secrets{
      {"ONE_TEST_FOREIGN_SECRET"}};
  const auto foreign{SESSION_FOR("okta", foreign_secrets, "")};
  EXPECT_FALSE(authentication.permits(
      AT("/portal/x"),
      authentication.caller(
          {.cookies = FIELDS("sourcemeta_one_session=" + foreign)})));

  // A value whose signature no longer matches its contents
  auto tampered{genuine};
  tampered.back() = tampered.back() == 'A' ? 'B' : 'A';
  EXPECT_FALSE(authentication.permits(
      AT("/portal/x"),
      authentication.caller(
          {.cookies = FIELDS("sourcemeta_one_session=" + tampered)})));

  // A value that is not a sealed session at all
  EXPECT_FALSE(authentication.permits(
      AT("/portal/x"),
      authentication.caller(
          {.cookies = FIELDS("sourcemeta_one_session=garbage")})));
}

TEST(session_is_admitted_when_a_shadowing_cookie_precedes_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SHADOW_A",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "")};

  // A parent domain can set a cookie the host also sets, and the header says
  // nothing about which is which, so the genuine one is honoured wherever it
  // appears
  const std::string cookies{"sourcemeta_one_session=not-a-session; "
                            "sourcemeta_one_session=" +
                            sealed};
  EXPECT_TRUE(authentication.permits(
      AT("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));
}

TEST(session_is_admitted_when_a_shadowing_cookie_follows_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SHADOW_B",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "")};

  // Taking the last match would deny here, which is the shape that lets a
  // neighbouring host lock somebody out of an instance it does not control
  const std::string cookies{"sourcemeta_one_session=" + sealed +
                            "; sourcemeta_one_session=not-a-session"};
  EXPECT_TRUE(authentication.permits(
      AT("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));
}

TEST(session_is_admitted_when_it_arrives_in_a_later_cookie_field) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_FIELD_LATER",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "")};

  // A request may carry its cookies across several fields rather than one, so
  // reading only the first would deny a session that did arrive
  const std::string second{"sourcemeta_one_session=" + sealed};
  const std::array<std::string_view, 2> carried{
      {"sourcemeta_one_session=not-a-session", second}};
  EXPECT_TRUE(authentication.permits(
      AT("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = carried})));
}

TEST(session_is_admitted_when_it_arrives_in_an_earlier_cookie_field) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_FIELD_EARLIER",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "")};

  // And neither field is the one that decides, so a later one carrying nothing
  // does not undo an earlier one that does
  const std::string first{"sourcemeta_one_session=" + sealed};
  const std::array<std::string_view, 2> carried{
      {first, "sourcemeta_one_session=not-a-session"}};
  EXPECT_TRUE(authentication.permits(
      AT("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = carried})));
}

TEST(a_session_for_another_policy_does_not_end_the_search) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // Both policies read the same session secret, so a value minted for one
  // opens under the other and is only told apart by the payload
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_OIDC_SEARCH_A",
                .session_secrets = SESSION_SECRETS}},
       {.paths = beta_paths,
        .name = "google",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SEARCH_B",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto other{SESSION_FOR("google", SESSION_SECRETS, "")};
  const auto mine{SESSION_FOR("okta", SESSION_SECRETS, "")};

  // The first value opens but was minted elsewhere, so stopping there would
  // deny a caller who did present a session for this policy
  const std::string cookies{"sourcemeta_one_session=" + other +
                            "; sourcemeta_one_session=" + mine};
  EXPECT_TRUE(authentication.permits(
      AT("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));

  // And a value minted elsewhere still opens nothing on its own
  const std::string alone{"sourcemeta_one_session=" + other};
  EXPECT_FALSE(authentication.permits(
      AT("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(alone)})));
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
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_OIDC_SHARED_A",
                .session_secrets = SESSION_SECRETS}},
       {.paths = beta_paths,
        .name = "google",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SHARED_B",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  EXPECT_TRUE(authentication.permits(
      AT("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));
  EXPECT_FALSE(authentication.permits(
      AT("/beta/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));
}

// Signing out asks the provider to end its own session, carrying the identity
// token as proof of whose it is asking about. Reaching that at all means the
// session opened and named the policy that minted it

TEST(a_transaction_never_admits_as_a_session) {
  setenv("ONE_TEST_PURPOSE_CLIENT", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_PURPOSE_CLIENT",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("oidc_open_session_purpose"), ANYWHERE)},
      STUB_FETCHER({{"https://acme.test/.well-known/openid-configuration",
                     R"JSON({
              "issuer": "https://acme.test",
              "authorization_endpoint": "https://acme.test/authorize",
              "token_endpoint": "https://acme.test/token",
              "jwks_uri": "https://acme.test/keys",
              "response_types_supported": [ "code" ],
              "subject_types_supported": [ "public" ],
              "id_token_signing_alg_values_supported": [ "RS256", "ES256" ]
            })JSON"}},
                   nullptr)};

  const auto started{authentication.login("okta", "https://registry.test",
                                          "https://registry.test/callback",
                                          false, "")};
  EXPECT_EQ(started.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  const auto transaction{COOKIE_VALUE(started.cookies.front())};

  // The control is a session, which does admit
  const auto session{SESSION_FOR("okta", SESSION_SECRETS, "")};
  EXPECT_TRUE(authentication.permits(
      AT("/portal/x"),
      authentication.caller(
          {.cookies = FIELDS("sourcemeta_one_session=" + session)})));

  EXPECT_FALSE(authentication.permits(
      AT("/portal/x"),
      authentication.caller(
          {.cookies = FIELDS("sourcemeta_one_session=" + transaction)})));
}

TEST(session_cookie_without_a_configured_secret_is_denied) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  // The session secret variable is deliberately never set in the environment
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_NO_SECRETS",
            .session_secrets = SESSION_SECRETS_UNSET}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};

  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_FALSE(authentication.permits(
      AT("/portal/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));
}

TEST(session_admitted_under_a_rotated_secret) {
  // The policy names the newest secret first, then the one it replaces, so a
  // session established under the old secret still verifies
  setenv("ONE_TEST_OIDC_ROTATED_SECRET", "new-secret", 1);
  setenv("ONE_TEST_OIDC_ROTATED_SECRET_OLD", "old-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_ROTATED",
            .session_secrets = SESSION_SECRETS_ROTATED}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("oidc_session_rotated"), ANYWHERE)},
      STUB_FETCHER({}, nullptr)};

  // A session established when the old secret was the only one is still
  // admitted, which is what lets a secret be replaced without ending it
  const std::array<std::string_view, 1> old_only{
      {"ONE_TEST_OIDC_ROTATED_SECRET_OLD"}};
  const auto established{SESSION_FOR("okta", old_only, "")};
  EXPECT_TRUE(authentication.permits(
      AT("/portal/x"),
      authentication.caller(
          {.cookies = FIELDS("sourcemeta_one_session=" + established)})));

  // A secret no longer in the set is refused, which differs from the one above
  // in exactly that
  setenv("ONE_TEST_OIDC_RETIRED_SECRET", "retired-secret", 1);
  const std::array<std::string_view, 1> retired_only{
      {"ONE_TEST_OIDC_RETIRED_SECRET"}};
  const auto retired{SESSION_FOR("okta", retired_only, "")};
  EXPECT_FALSE(authentication.permits(
      AT("/portal/x"),
      authentication.caller(
          {.cookies = FIELDS("sourcemeta_one_session=" + retired)})));
}

TEST(session_with_a_blank_configured_secret_is_denied) {
  setenv("ONE_TEST_OIDC_BLANK_SECRET", "", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_BLANK",
            .session_secrets = SESSION_SECRETS_BLANK}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("oidc_session_blank"), ANYWHERE)},
      STUB_FETCHER({}, nullptr)};

  // A blank secret would let anybody forge a session, so nothing verifies one,
  // not even a session this system established under a real secret
  const auto established{SESSION_FOR("okta", SESSION_SECRETS, "")};
  EXPECT_FALSE(authentication.permits(
      AT("/portal/x"),
      authentication.caller(
          {.cookies = FIELDS("sourcemeta_one_session=" + established)})));
}

TEST(union_of_an_apikey_and_an_oidc_policy_admits_key_or_session) {
  setenv("ONE_TEST_KEY_SESSION_UNION", "union-key", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SESSION_UNION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SESSION_UNION",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};

  const auto key_permitted{authentication.permits(
      AT("/both/x"), authentication.caller({.bearer = "union-key"}))};
  EXPECT_TRUE(key_permitted);

  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  const auto session_permitted{authentication.permits(
      AT("/both/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)}))};
  EXPECT_TRUE(session_permitted);

  EXPECT_FALSE(authentication.permits(AT("/both/x"),
                                      authentication.caller({.bearer = ""})));
}

TEST(session_cookie_does_not_open_an_apikey_path) {
  setenv("ONE_TEST_KEY_NO_SESSION", "key-only", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NO_SESSION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};

  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_FALSE(authentication.permits(
      AT("/internal/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));
}

// A login is offered under the name a policy was declared with, and nowhere
// else. A name this instance does not serve is answered as missing, which is
// the same answer a typo gets

TEST(a_session_is_bound_to_the_policy_whose_secret_sealed_it) {
  setenv("ONE_TEST_BIND_A", "confidential", 1);
  setenv("ONE_TEST_BIND_B", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_BIND_OTHER_SECRET", "another-secret", 1);
  TestProvider provider;
  const std::array<std::string_view, 1> alpha{{"/alpha"}};
  const std::array<std::string_view, 1> beta{{"/beta"}};
  const std::array<std::string_view, 1> other{{"ONE_TEST_BIND_OTHER_SECRET"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = provider.issuer,
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_BIND_A",
                .session_secrets = SESSION_SECRETS}},
       {.paths = beta,
        .name = "google",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_BIND_B",
            .session_secrets = other}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("bind_policies"), ANYWHERE)},
      provider.fetcher()};

  const auto established{SESSION_FOR("okta", SESSION_SECRETS, "jane")};
  const auto carried{"sourcemeta_one_session=" + established};

  // It opens what the policy that minted it governs
  EXPECT_TRUE(authentication.permits(
      AT("/alpha/x"), authentication.caller({.cookies = FIELDS(carried)})));
  // And nothing the other governs, which holds a secret of its own
  EXPECT_FALSE(authentication.permits(
      AT("/beta/x"), authentication.caller({.cookies = FIELDS(carried)})));
}

// Without a secret there is nothing to seal a login with, so it does not start

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
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_PRECEDENCE_SECRET",
                .session_secrets = SESSION_SECRETS}},
       {.paths = machine_paths,
        .name = "machine",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "jane@acme.test")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  // Each alone opens what it governs, which is what makes the pair below a
  // choice between two live credentials rather than one working answer
  EXPECT_TRUE(authentication.permits(
      AT("/portal/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));
  EXPECT_TRUE(authentication.permits(
      AT("/machine/x"), authentication.caller({.bearer = "machine-secret"})));

  // Presented together, the request is read as the key it carried, so the
  // portal the session would have opened is refused
  EXPECT_FALSE(authentication.permits(
      AT("/portal/x"), authentication.caller({.bearer = "machine-secret",
                                              .cookies = FIELDS(cookies)})));
  const auto permitted{authentication.permits(
      AT("/machine/x"), authentication.caller({.bearer = "machine-secret",
                                               .cookies = FIELDS(cookies)}))};
  EXPECT_TRUE(permitted);
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
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_FALLBACK_SECRET",
                .session_secrets = SESSION_SECRETS}},
       {.paths = machine_paths,
        .name = "machine",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "jane@acme.test")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  // A key that opens nothing is still a key that was presented, so the session
  // is set aside and nothing admits. The cost of the rule, and the reason it is
  // worth stating rather than leaving to be discovered
  EXPECT_FALSE(authentication.permits(
      AT("/portal/x"), authentication.caller({.bearer = "retired-secret",
                                              .cookies = FIELDS(cookies)})));

  // The same session presented on its own still opens it, so what changed is
  // what the request carried rather than whether the session is any good
  EXPECT_TRUE(authentication.permits(
      AT("/portal/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));
}

TEST(a_session_never_opens_as_a_transaction) {
  setenv("ONE_TEST_PURPOSE_BACK", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_PURPOSE_BACK",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("purpose_back"), ANYWHERE)},
      provider.fetcher()};

  // A session this instance established, obtained the way anybody obtains one
  const auto established{SESSION_FOR("okta", SESSION_SECRETS, "jane")};

  // Presented where a callback looks for the transaction it is completing. It
  // cannot open there, so the callback is refused before the provider is
  // consulted at all
  const auto carried{"sourcemeta_one_transaction=" + established};
  const std::array<std::string_view, 1> presented{{carried}};
  const auto outcome{authentication.callback(
      "okta", "https://registry.test", "https://registry.test/callback",
      {.state = "any-state", .code = "a-code"}, {.cookies = presented})};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Invalid);

  // And the control: the same value read as what it is does open, so the
  // refusal above came from the purpose rather than from the value
  const auto as_a_session{"sourcemeta_one_session=" + established};
  EXPECT_TRUE(authentication.permits(
      AT("/portal/x"),
      authentication.caller({.cookies = FIELDS(as_a_session)})));
}

// A ceiling and a missing secret are refusals a caller earns the same way, and
// both used to answer with an error naming no policy at all
