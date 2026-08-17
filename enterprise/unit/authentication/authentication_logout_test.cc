#include "authentication_helpers.h"

// What ending a session answers with. Every case builds a table, asks it to
// sign somebody out, and reads the outcome

// Signing out asks the provider to end its own session, carrying the identity
// token as proof of whose it is asking about. Reaching that at all means the
// session opened and named the policy that minted it
TEST(signing_out_asks_the_provider_that_established_the_session) {
  setenv("ONE_TEST_LOGOUT_A", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  provider.advertises =
      R"JSON({ "end_session_endpoint": "https://provider.test/logout" })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_LOGOUT_A",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("logout_known"), ANYWHERE)},
      provider.fetcher()};

  const auto established{SESSION_FOR("okta", SESSION_SECRETS, "jane")};
  const auto carried{"sourcemeta_one_session=" + established};
  const auto outcome{
      authentication.logout({.cookies = FIELDS(carried)}, INSTANCE_URL, "/")};
  EXPECT_TRUE(outcome.location.starts_with("https://provider.test/logout"));
  EXPECT_TRUE(outcome.location.find("id_token_hint=") != std::string::npos);
  // Both cookies expire whatever happened
  EXPECT_EQ(outcome.cookies.size(), 3);
}

// A session naming a policy this instance does not serve opens nothing, so
// there is nobody to ask and the browser is simply forgotten here
TEST(signing_out_with_a_session_naming_no_policy_here_asks_nobody) {
  setenv("ONE_TEST_LOGOUT_B", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  provider.advertises =
      R"JSON({ "end_session_endpoint": "https://provider.test/logout" })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_LOGOUT_B",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("logout_unknown"), ANYWHERE)},
      provider.fetcher()};

  // A session another instance minted under a name this one never declared
  const auto elsewhere{SESSION_FOR("nowhere", SESSION_SECRETS, "jane")};
  const auto carried{"sourcemeta_one_session=" + elsewhere};
  const auto outcome{
      authentication.logout({.cookies = FIELDS(carried)}, INSTANCE_URL, "/")};
  EXPECT_EQ(outcome.location, "/");
  // The instance still forgets, which is the secure outcome either way
  EXPECT_EQ(outcome.cookies.size(), 3);
}
