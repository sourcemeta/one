#include "authentication_helpers.h"

// What starting a login answers with. Every case builds a table, asks it to
// start a login, and reads the outcome

TEST(a_login_through_a_broken_artifact_is_missing) {
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          std::filesystem::path{"/no/such/authentication.bin"}},
      {}};
  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Missing);
}

// A session is sealed under the secret of the policy that established it, so a
// value one policy minted is nothing to another, even where both are declared
// here. That is what stops a caller choosing which policy a value is read as

TEST(a_login_starts_only_under_a_declared_name) {
  setenv("ONE_TEST_NAMED", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_NAMED",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("named"), ANYWHERE)},
      provider.fetcher()};

  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(
      authentication.login("elsewhere", INSTANCE_URL, REDIRECT_URI, false, "")
          .result,
      sourcemeta::one::Authentication::Outcome::Result::Missing);
  EXPECT_EQ(
      authentication.login("", INSTANCE_URL, REDIRECT_URI, false, "").result,
      sourcemeta::one::Authentication::Outcome::Result::Missing);
}

// What a provider says about itself is retrieved once and kept for as long as
// it said, so a second login costs nothing. Anybody may start one, so asking
// again each time would let a stranger drive traffic at the provider

TEST(what_a_provider_said_is_retrieved_once_and_reused) {
  setenv("ONE_TEST_OIDC_CACHE", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;

  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_CACHE",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_oidc_cache"), ANYWHERE)},
      provider.fetcher()};

  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(*provider.discoveries, 1);

  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(*provider.discoveries, 1);
}

// RFC 6749 Section 2.3.1 has every server accept the client secret in an
// authorization header and discourages carrying it in the request body, so the
// header is used wherever the provider takes it. A provider that lists nothing
// is taken to accept it, which is what the specification assigns to silence

TEST(a_login_against_an_unreachable_provider_cannot_start) {
  setenv("ONE_TEST_OIDC_UNREACHABLE", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;

  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_UNREACHABLE",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_oidc_unreachable"), ANYWHERE)},
      provider.fetcher()};
  TestProvider silent{provider};
  silent.reachable = false;
  const sourcemeta::one::Authentication unreachable{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("unreachable"), ANYWHERE)},
      silent.fetcher()};

  const auto outcome{
      unreachable.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(REPORTED(outcome, "authorization endpoint"));

  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

TEST(a_login_without_a_client_secret_cannot_start) {
  unsetenv("ONE_TEST_SECRET_UNSET");
  const TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SECRET_UNSET",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("secret_unset"), ANYWHERE)},
      provider.fetcher()};

  const auto outcome{
      authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(REPORTED(outcome, "No client secret is set"));
  // Nothing about the refusal reaches the person
  EXPECT_TRUE(outcome.location.empty());
  EXPECT_TRUE(outcome.cookies.empty());
}

// A variable set to nothing is not a secret, so it is the same answer as one
// that was never set, which is what the control beside it shows

TEST(a_login_with_a_blank_client_secret_cannot_start) {
  setenv("ONE_TEST_SECRET_BLANK", "", 1);
  setenv("ONE_TEST_SECRET_SET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SECRET_BLANK",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("secret_blank"), ANYWHERE)},
      provider.fetcher()};
  const auto outcome{
      authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(REPORTED(outcome, "No client secret is set"));

  const std::array<sourcemeta::one::Authentication::Policy, 1> configured{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SECRET_SET",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication working{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              configured, TEST_PATH("secret_set"), ANYWHERE)},
      provider.fetcher()};
  EXPECT_EQ(working.login("okta", INSTANCE_URL, REDIRECT_URI, false, "").result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// A policy naming no variable at all names no secret either

TEST(a_login_naming_no_secret_variable_cannot_start) {
  const TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("secret_unnamed"), ANYWHERE)},
      provider.fetcher()};

  const auto outcome{
      authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(REPORTED(outcome, "No client secret is set"));
}

TEST(a_machine_policy_starts_no_login) {
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_MACHINE_KEY"}};
  setenv("ONE_TEST_MACHINE_KEY", "machine-secret", 1);
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("machine_login"), ANYWHERE)},
      TestProvider{}.fetcher()};

  EXPECT_EQ(
      authentication.login("machine", INSTANCE_URL, REDIRECT_URI, false, "")
          .result,
      sourcemeta::one::Authentication::Outcome::Result::Missing);
}

TEST(a_login_without_a_session_secret_cannot_start) {
  setenv("ONE_TEST_SEAL_NONE", "confidential", 1);
  unsetenv("ONE_TEST_SEAL_NONE_SECRET");
  TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> absent{{"ONE_TEST_SEAL_NONE_SECRET"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SEAL_NONE",
            .session_secrets = absent}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("seal_none"), ANYWHERE)},
      provider.fetcher()};

  const auto outcome{
      authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(REPORTED(outcome, "No session secret is set"));
  EXPECT_TRUE(outcome.cookies.empty());
}
