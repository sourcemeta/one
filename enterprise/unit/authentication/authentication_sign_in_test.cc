#include "authentication_helpers.h"

// What signing in end to end answers with. Every case drives a login and its
// callback against a provider it controls, and reads the outcome

// Start a login and complete it, with the identity token carrying whatever a
// case says the provider asserted about the person. A login that does not get
// as far as a redirect is returned as it is, since that is the answer a case
// asking about one wants
static auto SIGN_IN(const sourcemeta::one::Authentication &authentication,
                    const TestProvider &provider, const std::string_view policy,
                    const std::string_view client_id,
                    const std::string_view asserted)
    -> sourcemeta::one::Authentication::Outcome {
  auto started{
      authentication.login(policy, INSTANCE_URL, REDIRECT_URI, false, "")};
  if (started.result !=
      sourcemeta::one::Authentication::Outcome::Result::Redirect) {
    return started;
  }

  auto payload{sourcemeta::core::parse_json(asserted)};
  payload.assign("iss", sourcemeta::core::JSON{std::string{provider.issuer}});
  payload.assign("aud", sourcemeta::core::JSON{std::string{client_id}});
  payload.assign("exp", sourcemeta::core::JSON{2000000000});
  payload.assign("iat", sourcemeta::core::JSON{1700000000});
  payload.assign("nonce",
                 sourcemeta::core::JSON{QUERY_OF(started.location, "nonce")});
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(CLAIMS_PRIVATE_KEY))};
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{"ES256"});
  *provider.identity =
      sourcemeta::core::jwt_sign(header, payload, key.value()).value();

  const auto carried{"sourcemeta_one_transaction=" +
                     COOKIE_VALUE(started.cookies.front())};
  const std::array<std::string_view, 1> presented{{carried}};
  return authentication.callback(
      policy, INSTANCE_URL, REDIRECT_URI,
      {.state = QUERY_OF(started.location, "state"), .code = "a-code"},
      {.cookies = presented});
}

TEST(a_policy_naming_no_rule_admits_whoever_signs_in) {
  setenv("ONE_TEST_ADMIT_OPEN", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_OPEN",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_admit_open"), ANYWHERE)},
      provider.fetcher()};

  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "somebody-else" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// A claim that never arrived is a question the provider may still answer at its
// UserInfo endpoint, which is where a scope's claims land by default under this
// flow. A claim that arrived and fell short is an answer already given, so
// asking anywhere else would only repeat it.
//
// Whether asking again could still change the answer is exactly what tells the
// two apart, so that is what these say: the same provider answers the missing
// claim at UserInfo, and only the one that had not been answered is admitted

TEST(a_claim_that_never_arrived_is_asked_for_rather_than_refused) {
  setenv("ONE_TEST_ADMIT_PARTIAL", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  provider.userinfo = R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_PARTIAL",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_admit_partial"), ANYWHERE)},
      provider.fetcher()};

  // It never arrived, so UserInfo is asked and answers
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);

  // It arrived and fell short, so nothing is asked and the answer stands, even
  // though the very same UserInfo answer would have satisfied the rule
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "support" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // And what the token carried on its own is enough where it satisfies the rule
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// The same claim, with a provider that answers nothing at its UserInfo
// endpoint, which is what shows the admission above came from asking

TEST(a_claim_that_never_arrived_and_is_nowhere_to_ask_refuses) {
  setenv("ONE_TEST_ADMIT_NO_USERINFO", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_NO_USERINFO",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_admit_no_userinfo"), ANYWHERE)},
      provider.fetcher()};

  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// Rules are cumulative, so one that refuses settles it whatever another would
// have allowed

TEST(a_rule_that_refuses_settles_it_whatever_another_wants) {
  setenv("ONE_TEST_ADMIT_BOTH", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  provider.userinfo =
      R"JSON({ "sub": "a1b2", "groups": [ "platform" ], "email": "jane@acme.test", "email_verified": true })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_BOTH",
            .claims = CLAIMS_ONE_GROUP,
            .email_domains = domains,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_admit_both"), ANYWHERE)},
      provider.fetcher()};

  // Both hold, which is the control
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ],
                       "email": "jane@acme.test", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);

  // The group falls short, and no address can make up for it
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "support" ],
                       "email": "jane@acme.test", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // The address is somewhere else, and no group can make up for that
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ],
                       "email": "jane@elsewhere.test", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
}

// A domain rule reads an address and the assertion that it was verified, which
// OpenID Connect Core Section 5.1 has speak for the address delivered with it
// and no other. A provider saying it will not vouch is an answer, so it settles
// the matter, while absence alone is what leaves the question open

TEST(an_address_the_provider_will_not_vouch_for_is_refused) {
  setenv("ONE_TEST_ADMIT_UNVOUCHED", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  provider.userinfo =
      R"JSON({ "sub": "a1b2", "email": "jane@acme.test", "email_verified": true })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_UNVOUCHED",
            .email_domains = domains,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_admit_unvouched"), ANYWHERE)},
      provider.fetcher()};

  // The provider declined to vouch, which is an answer, so asking again cannot
  // change it even though UserInfo would have vouched
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "email": "jane@acme.test",
                             "email_verified": false })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // An address that is not one settles it the same way
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "email": 42 })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // Absence alone leaves the question open, so it is asked and answered
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// The token wins wherever both answers speak, since it arrives signed and
// verified while a UserInfo response is protected only by the transport that
// carried it. So the second fills gaps rather than overruling a signature

TEST(a_second_answer_fills_gaps_without_overruling_the_token) {
  setenv("ONE_TEST_COMBINE_TOKEN", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  provider.userinfo = R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_COMBINE_TOKEN",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_combine_token"), ANYWHERE)},
      provider.fetcher()};

  // The token said something else about the very claim UserInfo would satisfy,
  // and the token is what stands
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "support" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // Where the token said nothing, the gap is filled
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// An address and the assertion that it was verified travel as a pair, from
// whichever answer carried the address. OpenID Connect Core Section 5.1 has
// `email_verified` speak for the `email` delivered alongside it and no other,
// so letting one answer's assertion vouch for the other answer's address would
// admit an address the provider never verified
// An address and the assertion that it was verified travel as a pair, from
// whichever answer carried the address. OpenID Connect Core Section 5.1 has
// `email_verified` speak for the `email` delivered alongside it and no other,
// so letting one answer's assertion vouch for the other answer's address would
// admit an address the provider never verified

TEST(an_address_arrives_with_its_own_assertion_or_not_at_all) {
  setenv("ONE_TEST_COMBINE_PAIR", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://provider.test",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_COMBINE_PAIR",
            .email_domains = domains,
            .session_secrets = SESSION_SECRETS}}}};

  // The token vouches for an address it never carried, and the second answer
  // supplies one without an assertion. Neither half may vouch for the other
  TestProvider orphaned;
  orphaned.userinfo = R"JSON({ "sub": "a1b2", "email": "jane@acme.test" })JSON";
  const sourcemeta::one::Authentication unvouched{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("combine_pair"), ANYWHERE)},
      orphaned.fetcher()};
  EXPECT_EQ(SIGN_IN(unvouched, orphaned, "okta", "client",
                    R"JSON({ "sub": "a1b2", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // The control differs in one thing: the pair arrives whole from the answer
  // that carried the address
  TestProvider whole;
  whole.userinfo = R"JSON({ "sub": "a1b2", "email": "jane@acme.test",
                            "email_verified": true })JSON";
  const sourcemeta::one::Authentication vouched{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("combine_whole"), ANYWHERE)},
      whole.fetcher()};
  EXPECT_EQ(SIGN_IN(vouched, whole, "okta", "client",
                    R"JSON({ "sub": "a1b2", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

TEST(admitting_reads_two_answers_only_once_they_are_combined) {
  setenv("ONE_TEST_ADMIT_SPLIT", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  provider.userinfo =
      R"JSON({ "sub": "a1b2", "email": "jane@acme.test", "email_verified": true })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_SPLIT",
            .claims = CLAIMS_ONE_GROUP,
            .email_domains = domains,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_admit_split"), ANYWHERE)},
      provider.fetcher()};

  // The token carries the group and UserInfo carries the address, so only the
  // two together satisfy both rules
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);

  // The same second answer cannot supply the group, so what the token carried
  // is what decided that half
  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "support" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
}

// A rule compared against a claim carrying objects is compared on the `value`
// sub-attribute alone, which RFC 9068 gives group, role and entitlement claims
// by way of RFC 7643. So a rule naming what a person sees rather than what
// identifies them matches nothing, and a refusal cannot show that: the token it
// concerns is sealed inside a cookie where an operator cannot look. It is said
// in the log instead, which is the only place it can be said

TEST(a_claim_answered_with_objects_is_named_where_an_operator_looks) {
  setenv("ONE_TEST_SHAPE_OBJECTS", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "shapes",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SHAPE_OBJECTS",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("shape_objects"), ANYWHERE)},
      provider.fetcher()};

  const auto objects{SIGN_IN(authentication, provider, "shapes", "client",
                             R"JSON({ "sub": "a1b2",
               "groups": [ { "value": "g-1", "display": "platform" } ] })JSON")};
  EXPECT_EQ(objects.result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
  EXPECT_TRUE(REPORTED(objects, "groups"));
  // Nothing about it reaches the person
  EXPECT_TRUE(objects.cookies.empty());
}

// The same rule, answered in the shape it names, says nothing at all. This
// names a policy of its own because what is said is said once per claim and
// policy however often somebody signs in

TEST(a_claim_answered_in_the_shape_a_rule_names_says_nothing) {
  setenv("ONE_TEST_SHAPE_STRINGS", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "strings",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SHAPE_STRINGS",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("shape_strings"), ANYWHERE)},
      provider.fetcher()};

  // It matched, so there is nothing to explain
  const auto matched{SIGN_IN(authentication, provider, "strings", "client",
                             R"JSON({ "sub": "a1b2",
                                      "groups": [ "platform" ] })JSON")};
  EXPECT_EQ(matched.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_FALSE(REPORTED(matched, "groups"));

  // It fell short in the shape the rule names, which is an ordinary refusal
  // rather than a mistake worth naming
  const auto fell_short{SIGN_IN(authentication, provider, "strings", "client",
                                R"JSON({ "sub": "a1b2",
                                         "groups": [ "support" ] })JSON")};
  EXPECT_EQ(fell_short.result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
  EXPECT_FALSE(REPORTED(fell_short, "objects"));
}

// A rule on `scope` is never named that way. That claim is read as one
// space-delimited string rather than compared member by member, so one arriving
// as anything else is refused outright, and calling it an identifier mismatch
// would describe a mistake nobody made

TEST(a_scope_arriving_as_objects_is_refused_without_being_named) {
  setenv("ONE_TEST_SHAPE_SCOPE", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "scopes",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SHAPE_SCOPE",
            .claims = CLAIMS_SCOPE,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("shape_scope"), ANYWHERE)},
      provider.fetcher()};

  const auto objects{SIGN_IN(
      authentication, provider, "scopes", "client",
      R"JSON({ "sub": "a1b2", "scope": [ { "value": "registry:read" } ] })JSON")};
  EXPECT_EQ(objects.result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
  EXPECT_FALSE(REPORTED(objects, "scope"));
}

TEST(a_callback_under_a_name_this_instance_does_not_serve_is_refused) {
  setenv("ONE_TEST_ADMIT_UNKNOWN", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_UNKNOWN",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_admit_unknown"), ANYWHERE)},
      provider.fetcher()};

  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(SIGN_IN(authentication, provider, "nowhere", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Missing);
}

TEST(a_provider_naming_no_authentication_method_gets_the_header) {
  setenv("ONE_TEST_OIDC_AUTH_SILENT", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;

  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_AUTH_SILENT",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_oidc_auth_silent"), ANYWHERE)},
      provider.fetcher()};

  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_TRUE(*provider.secret_in_header);
}

TEST(a_provider_naming_the_header_gets_the_header) {
  setenv("ONE_TEST_OIDC_AUTH_BASIC", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  provider.advertises = R"JSON({
    "token_endpoint_auth_methods_supported": [ "client_secret_basic" ]
  })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_AUTH_BASIC",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_oidc_auth_basic"), ANYWHERE)},
      provider.fetcher()};

  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_TRUE(*provider.secret_in_header);
}

// A provider that does not take the header leaves the body as the only way to
// authenticate, so the preference gives way rather than the login failing. A
// body is what logging and proxies keep, which is why it is the second choice

TEST(a_provider_refusing_the_header_gets_the_body_instead) {
  setenv("ONE_TEST_OIDC_AUTH_POST", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  TestProvider provider;
  provider.advertises = R"JSON({
    "token_endpoint_auth_methods_supported": [ "client_secret_post" ]
  })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_AUTH_POST",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("one_test_oidc_auth_post"), ANYWHERE)},
      provider.fetcher()};

  EXPECT_EQ(SIGN_IN(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_FALSE(*provider.secret_in_header);
}

TEST(a_login_asking_for_nowhere_returns_to_what_the_policy_governs) {
  setenv("ONE_TEST_DEFAULT_PATH", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const TestProvider provider;
  const std::array<std::string_view, 2> paths{{"/portal", "/second"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_DEFAULT_PATH",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("default_path"), ANYWHERE)},
      provider.fetcher()};

  const auto completed{SIGN_IN(authentication, provider, "okta", "client",
                               R"JSON({ "sub": "a1b2" })JSON")};
  EXPECT_EQ(completed.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(completed.location, "/portal");
}

// An instance that could not read its artifact offers no login at all, which
// is the same answer it gives to every other question
