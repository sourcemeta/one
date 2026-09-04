#include "authentication_helpers.h"

// What compiling a set of policies refuses. Every case declares something a
// configuration may not, and reads the error it earns

TEST(save_rejects_a_nameless_policy) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_NAMELESS"}}}};
  const auto path{TEST_PATH("oidc_nameless.bin")};
  try {
    SAVE(policies, path, path, ANYWHERE);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationPolicyNameError &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy requires a name of its own");
  }
}

TEST(save_rejects_a_nameless_key_policy) {
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NAMELESS"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{TEST_PATH("key_nameless.bin")};
  try {
    SAVE(policies, path, path, ANYWHERE);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationPolicyNameError &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy requires a name of its own");
  }
}

TEST(save_rejects_two_policies_sharing_a_name) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_KEY_SAME_A"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_KEY_SAME_B"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "shared",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                alpha_keys}},
       {.paths = beta_paths,
        .name = "shared",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = beta_keys}}}};
  const auto path{TEST_PATH("name_shared.bin")};
  try {
    SAVE(policies, path, path, ANYWHERE);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationPolicyNameError &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy requires a name of its own");
  }
}

TEST(save_rejects_a_policy_taking_the_anonymous_name) {
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_RESERVED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "public",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{TEST_PATH("name_reserved.bin")};
  try {
    SAVE(policies, path, path, ANYWHERE);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationPolicyNameError &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy requires a name of its own");
  }
}

TEST(save_rejects_a_policy_named_as_a_combination_of_others) {
  setenv("ONE_TEST_KEY_COMBINATION", "combination-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{
      {"ONE_TEST_KEY_COMBINATION"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  // The two token policies name one issuer, so they combine into a view spelled
  // by joining them, which is the name the third policy also carries
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms}},
       {.paths = beta_paths,
        .name = "beta",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms}},
       {.paths = machine_paths,
        .name = "alpha+beta",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const auto path{TEST_PATH("name_combination.bin")};
  try {
    SAVE(policies, path, path, ANYWHERE);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationViewNameCollisionError &error) {
    EXPECT_STREQ(
        error.what(),
        "An authentication policy name collides with a combination of others");
  }
}

// A ceiling and a missing secret are refusals a caller earns the same way, and
// both used to answer with an error naming no policy at all
TEST(save_rejects_more_policies_than_a_set_can_name) {
  std::vector<std::vector<std::string_view>> paths;
  std::vector<std::vector<std::string_view>> keys;
  std::vector<std::string> names;
  // One past what a 64 bit mask has room to name
  constexpr std::size_t TOTAL{65};
  for (std::size_t index{0}; index < TOTAL; index += 1) {
    names.push_back("policy-" + std::to_string(index));
    paths.push_back({"/scope"});
    keys.push_back({"ONE_TEST_KEY_CEILING"});
  }

  std::vector<sourcemeta::one::Authentication::Policy> policies;
  for (std::size_t index{0}; index < TOTAL; index += 1) {
    policies.push_back(
        {.paths = paths[index],
         .name = names[index],
         .credential = sourcemeta::one::Authentication::Policy::ApiKey{
             .keys = keys[index]}});
  }

  const auto path{TEST_PATH("ceiling.bin")};
  try {
    SAVE(policies, path, path, ANYWHERE);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationTooManyPoliciesError &error) {
    EXPECT_STREQ(error.what(), "Too many authentication policies");
    EXPECT_EQ(error.count(), TOTAL);
  }
}

TEST(save_rejects_an_interactive_policy_without_a_session_secret) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_NO_SECRET"}}}};
  const auto path{TEST_PATH("no_session_secret.bin")};
  try {
    SAVE(policies, path, path, ANYWHERE);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationMissingSecretError &error) {
    EXPECT_STREQ(error.what(),
                 "An interactive authentication policy requires a session "
                 "secret");
    EXPECT_EQ(error.name(), "okta");
  }
}

TEST(views_refuse_a_group_whose_combinations_cannot_be_produced) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, COMBINABLE_CEILING + 1>
      policies{};
  std::vector<std::string> names;
  names.reserve(policies.size());
  for (std::size_t index{0}; index < policies.size(); index++) {
    names.push_back("p" + std::to_string(index));
  }

  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).name = names.at(index);
    policies.at(index).credential =
        sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"};
  }

  const auto path{
      TEST_PATH("views_refuse_a_group_whose_combinations_cannot_be_produced")};
  try {
    SAVE(policies, path, path, ANYWHERE);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationTooManyViewsError &error) {
    EXPECT_STREQ(error.what(),
                 "Too many authentication policies share an issuer");
    EXPECT_EQ(error.issuer(), "https://idp.example.com/realms/staff");
    EXPECT_EQ(error.count(), COMBINABLE_CEILING + 1);
  }
}
