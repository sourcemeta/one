#include "authentication_helpers.h"

// Signing in through a GitHub deployment end to end. Every case drives a login
// and its callback against a deployment it controls, and reads the outcome

static constexpr std::string_view GITHUB_REDIRECT_URI{
    "https://registry.test/self/v1/auth/callback/github"};

// A deployment answers a request for a page it does not serve with nothing at
// all, so a case that wants a policy admitting nobody says so through its rules
static auto
GITHUB_POLICY(const TestGitHub &deployment,
              const std::span<const std::string_view> paths,
              const std::span<const std::string_view> users = {},
              const std::span<const std::string_view> organizations = {},
              const std::span<const std::string_view> teams = {},
              const std::span<const std::string_view> email_domains = {})
    -> sourcemeta::one::Authentication::Policy {
  return {.paths = paths,
          .name = "github",
          .credential = sourcemeta::one::Authentication::Policy::GitHub{
              .host = deployment.host,
              .client_id = "Iv1.0123456789abcdef",
              .client_secret_variable = "ONE_TEST_GITHUB_CLIENT_SECRET",
              .users = users,
              .organizations = organizations,
              .teams = teams,
              .email_domains = email_domains,
              .session_secrets = SESSION_SECRETS}};
}

// Start a login and complete it, which is the whole of what a case about
// admission drives. A login that does not get as far as a redirect is returned
// as it is, since that is the answer a case asking about one wants
static auto
SIGN_IN_GITHUB(const sourcemeta::one::Authentication &authentication)
    -> sourcemeta::one::Authentication::Outcome {
  auto started{authentication.login("github", INSTANCE_URL, GITHUB_REDIRECT_URI,
                                    false, "")};
  if (started.result !=
      sourcemeta::one::Authentication::Outcome::Result::Redirect) {
    return started;
  }

  const auto carried{"sourcemeta_one_transaction=" +
                     COOKIE_VALUE(started.cookies.front())};
  const std::array<std::string_view, 1> presented{{carried}};
  return authentication.callback(
      "github", INSTANCE_URL, GITHUB_REDIRECT_URI,
      {.state = QUERY_OF(started.location, "state"), .code = "a-code"},
      {.cookies = presented});
}

static auto INSTANCE(
    const TestGitHub &deployment,
    const std::span<const sourcemeta::one::Authentication::Policy> policies,
    const std::string &name) -> sourcemeta::one::Authentication {
  return sourcemeta::one::Authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH(name), ANYWHERE)},
      deployment.fetcher()};
}

// A deployment publishes nothing to discover and issues no identity token, so
// a login against one is composed rather than fetched, binds no nonce, and asks
// for exactly the scopes the policy's rules need
TEST(a_login_asks_for_the_least_the_rules_of_a_policy_need) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const TestGitHub deployment;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> organizations{{"acme"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, {}, organizations)}};
  const auto authentication{INSTANCE(deployment, policies, "one_github_scope")};

  const auto started{authentication.login("github", INSTANCE_URL,
                                          GITHUB_REDIRECT_URI, false, "")};
  EXPECT_EQ(started.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_TRUE(started.location.starts_with(
      "https://github.test/login/oauth/authorize?"));
  EXPECT_EQ(QUERY_OF(started.location, "client_id"), "Iv1.0123456789abcdef");
  EXPECT_EQ(QUERY_OF(started.location, "response_type"), "code");
  EXPECT_EQ(
      sourcemeta::core::URI::unescape(QUERY_OF(started.location, "scope")),
      "read:org");
  EXPECT_EQ(QUERY_OF(started.location, "code_challenge_method"), "S256");
  EXPECT_EQ(sourcemeta::core::URI{started.location}.query().value().at("nonce"),
            std::nullopt);
  // Starting a login reaches the deployment for nothing at all
  EXPECT_TRUE(deployment.asked->empty());
}

TEST(a_login_asks_for_nothing_where_a_policy_names_only_accounts) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const TestGitHub deployment;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> users{{"octocat"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, users)}};
  const auto authentication{
      INSTANCE(deployment, policies, "one_github_no_scope")};

  const auto started{authentication.login("github", INSTANCE_URL,
                                          GITHUB_REDIRECT_URI, false, "")};
  EXPECT_EQ(started.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(sourcemeta::core::URI{started.location}.query().value().at("scope"),
            std::nullopt);
}

TEST(a_login_asks_for_both_scopes_where_a_policy_names_both_kinds_of_rule) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const TestGitHub deployment;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> teams{{"acme/platform"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, {}, {}, teams, domains)}};
  const auto authentication{
      INSTANCE(deployment, policies, "one_github_both_scopes")};

  const auto started{authentication.login("github", INSTANCE_URL,
                                          GITHUB_REDIRECT_URI, false, "")};
  EXPECT_EQ(
      sourcemeta::core::URI::unescape(QUERY_OF(started.location, "scope")),
      "read:org user:email");
}

// A policy naming accounts is answered against the handle the deployment says
// the token was issued for, which costs no call beyond the one that asks
TEST(a_policy_naming_an_account_admits_only_that_account) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> users{{"octocat"}};

  const TestGitHub admitted;
  const std::array<sourcemeta::one::Authentication::Policy, 1> first{
      {GITHUB_POLICY(admitted, paths, users)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(admitted, first, "one_github_user_yes")).result,
      sourcemeta::one::Authentication::Outcome::Result::Redirect);

  TestGitHub refused;
  refused.user = R"JSON({ "login": "hubot", "id": 99, "email": null })JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> second{
      {GITHUB_POLICY(refused, paths, users)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(refused, second, "one_github_user_no")).result,
      sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
}

// A handle names an account rather than a phrase, so it is compared without
// regard to case on both sides
TEST(a_handle_is_compared_without_regard_to_case) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> users{{"OctoCat"}};
  TestGitHub deployment;
  deployment.user = R"JSON({ "login": "OCTOCAT", "id": 583231 })JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, users)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(deployment, policies, "one_github_user_case"))
          .result,
      sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

TEST(a_policy_naming_an_organisation_admits_a_member) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> organizations{{"acme"}};

  TestGitHub admitted;
  admitted.organizations = R"JSON([ { "login": "acme", "id": 1 } ])JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> first{
      {GITHUB_POLICY(admitted, paths, {}, organizations)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(admitted, first, "one_github_org_yes")).result,
      sourcemeta::one::Authentication::Outcome::Result::Redirect);

  TestGitHub refused;
  refused.organizations = R"JSON([ { "login": "contoso", "id": 2 } ])JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> second{
      {GITHUB_POLICY(refused, paths, {}, organizations)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(refused, second, "one_github_org_no")).result,
      sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
}

// A team is named by the organisation holding it alongside its slug, so a team
// of the same name in another organisation is a different team
TEST(a_policy_naming_a_team_admits_a_member_of_that_team_alone) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> teams{{"acme/platform"}};

  TestGitHub admitted;
  admitted.teams =
      R"JSON([ { "slug": "platform", "organization": { "login": "acme" } } ])JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> first{
      {GITHUB_POLICY(admitted, paths, {}, {}, teams)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(admitted, first, "one_github_team_yes")).result,
      sourcemeta::one::Authentication::Outcome::Result::Redirect);

  TestGitHub refused;
  refused.teams =
      R"JSON([ { "slug": "platform", "organization": { "login": "contoso" } } ])JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> second{
      {GITHUB_POLICY(refused, paths, {}, {}, teams)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(refused, second, "one_github_team_no")).result,
      sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
}

// The address on an account is the public one and is frequently unset, so a
// domain rule is answered against the primary address the account holds, and
// only where the deployment vouches for it
TEST(a_domain_rule_is_answered_against_a_verified_primary_address) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};

  TestGitHub admitted;
  admitted.emails =
      R"JSON([ { "email": "spare@other.test", "primary": false, "verified": true },
               { "email": "octocat@acme.test", "primary": true, "verified": true } ])JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> first{
      {GITHUB_POLICY(admitted, paths, {}, {}, {}, domains)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(admitted, first, "one_github_email_yes")).result,
      sourcemeta::one::Authentication::Outcome::Result::Redirect);

  TestGitHub elsewhere;
  elsewhere.emails =
      R"JSON([ { "email": "octocat@other.test", "primary": true, "verified": true } ])JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> second{
      {GITHUB_POLICY(elsewhere, paths, {}, {}, {}, domains)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(elsewhere, second, "one_github_email_no")).result,
      sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  TestGitHub unverified;
  unverified.emails =
      R"JSON([ { "email": "octocat@acme.test", "primary": true, "verified": false } ])JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> third{
      {GITHUB_POLICY(unverified, paths, {}, {}, {}, domains)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(unverified, third, "one_github_email_unverified"))
          .result,
      sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
}

// Rules of different kinds are cumulative, exactly as the claim rules of an
// interactive policy are, so satisfying one of two is satisfying neither
TEST(rules_of_different_kinds_are_cumulative) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> users{{"octocat"}};
  const std::array<std::string_view, 1> organizations{{"acme"}};

  TestGitHub deployment;
  deployment.organizations = R"JSON([ { "login": "contoso", "id": 2 } ])JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, users, organizations)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(deployment, policies, "one_github_cumulative"))
          .result,
      sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
}

// The token endpoint answers a failure with a 200 carrying an `error` member,
// so a callback that read the status alone would take a refused code for a
// grant and fail further along for a reason nobody could place
TEST(a_token_endpoint_naming_an_error_in_a_success_is_no_grant) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> users{{"octocat"}};
  TestGitHub deployment;
  deployment.token =
      R"JSON({ "error": "bad_verification_code", "error_description": "The code passed is incorrect or expired." })JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, users)}};
  const auto outcome{
      SIGN_IN_GITHUB(INSTANCE(deployment, policies, "one_github_token_error"))};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Incomplete);
  EXPECT_TRUE(REPORTED(outcome, "bad_verification_code"));
  // The account was never asked for, since there was nothing to ask with
  EXPECT_EQ(deployment.asked->size(), 1);
}

// The token endpoint answers in a form encoding unless a request asks for JSON,
// and the API refuses a request that names no user agent at all, so both are
// asked for on every call this makes
TEST(every_call_carries_what_a_deployment_requires_of_one) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> organizations{{"acme"}};
  TestGitHub deployment;
  deployment.organizations = R"JSON([ { "login": "acme", "id": 1 } ])JSON";
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, {}, organizations)}};
  EXPECT_EQ(SIGN_IN_GITHUB(INSTANCE(deployment, policies, "one_github_headers"))
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_TRUE(*deployment.asked_for_json);
  EXPECT_TRUE(*deployment.named_agent);
  EXPECT_EQ(deployment.asked->size(), 3);
  EXPECT_EQ(deployment.asked->at(0),
            "https://github.test/login/oauth/access_token");
  EXPECT_EQ(deployment.asked->at(1), "https://github.test/api/v3/user");
  EXPECT_EQ(deployment.asked->at(2),
            "https://github.test/api/v3/user/orgs?per_page=100&page=1");
}

// The public deployment answers its API under a host of its own, while every
// other answers below the origin it is served at
TEST(the_public_deployment_answers_its_api_under_its_own_host) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> users{{"octocat"}};
  TestGitHub deployment;
  deployment.host = "https://github.com";
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, users)}};
  EXPECT_EQ(
      SIGN_IN_GITHUB(INSTANCE(deployment, policies, "one_github_public_host"))
          .result,
      sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(deployment.asked->size(), 2);
  EXPECT_EQ(deployment.asked->at(0),
            "https://github.com/login/oauth/access_token");
  EXPECT_EQ(deployment.asked->at(1), "https://api.github.com/user");
}

// A listing is read inside a request handler, so one that never comes back
// shorter than the page asked for is refused rather than followed for as long
// as it keeps pointing somewhere
TEST(a_listing_that_never_shortens_is_refused_rather_than_followed) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> organizations{{"acme"}};

  // A page as long as the one that was asked for is one more page to read, and
  // this deployment answers every page with one
  std::string endless{"["};
  for (std::size_t index{0}; index < 100; index += 1) {
    if (index > 0) {
      endless += ",";
    }

    endless += R"JSON({ "login": "contoso", "id": 2 })JSON";
  }

  endless += "]";

  TestGitHub deployment;
  deployment.organizations = endless;
  const auto fetcher{[endless](sourcemeta::one::Authentication::ProviderRequest
                                   &&request)
                         -> std::optional<sourcemeta::one::Authentication::
                                              ProviderResponse> {
    if (request.url == "https://github.test/login/oauth/access_token") {
      return sourcemeta::one::Authentication::ProviderResponse{
          .status = 200,
          .body =
              R"JSON({ "access_token": "an-access-token", "token_type": "bearer" })JSON"};
    }

    if (request.url == "https://github.test/api/v3/user") {
      return sourcemeta::one::Authentication::ProviderResponse{
          .status = 200,
          .body = R"JSON({ "login": "octocat", "id": 583231 })JSON"};
    }

    return sourcemeta::one::Authentication::ProviderResponse{.status = 200,
                                                             .body = endless};
  }};

  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, {}, organizations)}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_github_endless"), ANYWHERE)},
      fetcher};

  const auto outcome{SIGN_IN_GITHUB(authentication)};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
  EXPECT_TRUE(REPORTED(outcome, "did not end within the pages this reads"));
}

// A deployment cannot be asked whether a sign-in still stands without showing
// the person its own pages, so a browser signed in through one is left no
// marker that would send it back there on its own
TEST(signing_in_leaves_no_marker_for_a_silent_renewal) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const TestGitHub deployment;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> users{{"octocat"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, users)}};
  const auto authentication{
      INSTANCE(deployment, policies, "one_github_no_renewal")};

  const auto outcome{SIGN_IN_GITHUB(authentication)};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  // The session and the spent transaction, and nothing else
  EXPECT_EQ(outcome.cookies.size(), 2);
  EXPECT_TRUE(outcome.cookies.at(0).starts_with("sourcemeta_one_session="));
  EXPECT_TRUE(outcome.cookies.at(1).starts_with("sourcemeta_one_transaction="));

  const auto carried{"sourcemeta_one_session=" +
                     COOKIE_VALUE(outcome.cookies.front())};
  const std::array<std::string_view, 1> presented{{carried}};
  EXPECT_EQ(authentication.renewal(AT("/portal"), {.cookies = presented}),
            std::nullopt);
}

// A silent attempt is a navigation rather than something hidden, so one against
// a deployment that cannot answer it would land somebody on its sign-in page in
// the middle of browsing here. It is not made at all
TEST(a_silent_attempt_against_a_deployment_is_an_ordinary_login) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const TestGitHub deployment;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> users{{"octocat"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, users)}};
  const auto authentication{
      INSTANCE(deployment, policies, "one_github_silent")};

  const auto started{authentication.login("github", INSTANCE_URL,
                                          GITHUB_REDIRECT_URI, true, "")};
  EXPECT_EQ(started.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(
      sourcemeta::core::URI{started.location}.query().value().at("prompt"),
      std::nullopt);
}

// A login that cannot be completed only strands the person at the deployment,
// so the secret the exchange will need is required before the browser is sent
// anywhere
TEST(a_login_without_a_client_secret_is_refused_before_the_redirect) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  unsetenv("ONE_TEST_GITHUB_ABSENT");
  const TestGitHub deployment;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> users{{"octocat"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "github",
        .credential = sourcemeta::one::Authentication::Policy::GitHub{
            .host = deployment.host,
            .client_id = "Iv1.0123456789abcdef",
            .client_secret_variable = "ONE_TEST_GITHUB_ABSENT",
            .users = users,
            .session_secrets = SESSION_SECRETS}}}};
  const auto authentication{
      INSTANCE(deployment, policies, "one_github_no_secret")};

  const auto started{authentication.login("github", INSTANCE_URL,
                                          GITHUB_REDIRECT_URI, false, "")};
  EXPECT_EQ(started.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(REPORTED(started, "No client secret is set for the policy"));
  EXPECT_TRUE(deployment.asked->empty());
}

// The session a login ends in reaches every path the policy governs, and a
// caller holding none reaches none of them
TEST(a_session_a_deployment_established_reaches_what_the_policy_governs) {
  setenv("ONE_TEST_GITHUB_CLIENT_SECRET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const TestGitHub deployment;
  const std::array<std::string_view, 2> paths{{"/portal", "/archive"}};
  const std::array<std::string_view, 1> users{{"octocat"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {GITHUB_POLICY(deployment, paths, users)}};
  const auto authentication{
      INSTANCE(deployment, policies, "one_github_session")};

  const auto outcome{SIGN_IN_GITHUB(authentication)};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(outcome.location, "/portal");

  const auto carried{"sourcemeta_one_session=" +
                     COOKIE_VALUE(outcome.cookies.front())};
  const std::array<std::string_view, 1> presented{{carried}};
  const auto caller{authentication.caller({.cookies = presented})};
  EXPECT_EQ(caller.view(), "github");
  EXPECT_TRUE(authentication.permits(AT("/portal"), caller));
  EXPECT_TRUE(authentication.permits(AT("/archive"), caller));

  const auto stranger{authentication.caller({})};
  EXPECT_EQ(stranger.view(), "public");
  EXPECT_FALSE(authentication.permits(AT("/portal"), stranger));
  EXPECT_FALSE(authentication.permits(AT("/archive"), stranger));
}
