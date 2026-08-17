#include "authentication_helpers.h"

// Whether a schema at one location may reference a schema at another, which
// is whether everybody who reaches the first also reaches the second

TEST(reference_through_a_broken_artifact_is_rejected) {
  const sourcemeta::one::Authentication::Table gate{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_FALSE(gate.reference_permitted(AT("/open/one"), AT("/open/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/open/one"), AT("/secret/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/secret/one"), AT("/secret/two")));
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
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa,
                .token_type = "application/"}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
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
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa,
                .token_type = "application/one/two"}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa,
            .token_type = "one/two"}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
}

TEST(reference_to_a_public_schema_is_permitted) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_PUBLIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = secret_paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto gate{TABLE(policies)};
  EXPECT_TRUE(gate.reference_permitted(AT("/secret/one"), AT("/open/two")));
  EXPECT_TRUE(gate.reference_permitted(AT("/open/one"), AT("/open/two")));
}

TEST(public_schema_referencing_an_apikey_schema_is_rejected) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_LEAK"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = secret_paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/open/one"), AT("/secret/two")));
}

TEST(reference_within_the_same_policy_is_permitted) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_SAME"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto gate{TABLE(policies)};
  EXPECT_TRUE(
      gate.reference_permitted(AT("/internal/one"), AT("/internal/two")));
  EXPECT_TRUE(
      gate.reference_permitted(AT("/internal/one"), AT("/internal/one")));
}

TEST(reference_across_disjoint_policies_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_REF_ALPHA"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_REF_BETA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                alpha_keys}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = beta_keys}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
}

TEST(reference_from_a_narrower_to_a_wider_audience_is_permitted) {
  const std::array<std::string_view, 1> broad_paths{{"/p"}};
  const std::array<std::string_view, 1> nested_paths{{"/p/inner"}};
  const std::array<std::string_view, 1> broad_keys{{"ONE_TEST_REF_BROAD"}};
  const std::array<std::string_view, 1> nested_keys{{"ONE_TEST_REF_NESTED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = broad_paths,
        .name = "broad",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                broad_keys}},
       {.paths = nested_paths,
        .name = "nested",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = nested_keys}}}};
  const auto gate{TABLE(policies)};
  EXPECT_TRUE(gate.reference_permitted(AT("/p/one"), AT("/p/inner/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/p/inner/two"), AT("/p/one")));
}

TEST(reference_within_the_same_oidc_scope_is_permitted) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // The two policies differ in name and in the environment variable holding
  // the secret, neither of which affects who can authenticate, so the scopes
  // stay equal
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://login.test",
                .client_id = "registry",
                .client_secret_variable = "ONE_TEST_OIDC_REF_SAME",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://login.test",
            .client_id = "registry",
            .client_secret_variable = "ONE_TEST_OIDC_REF_SAME_OTHER",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto gate{TABLE(policies)};
  EXPECT_TRUE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
}

TEST(reference_across_distinct_oidc_clients_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://login.test",
                .client_id = "registry",
                .client_secret_variable = "ONE_TEST_OIDC_REF_ALPHA",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://login.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_BETA",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
}

TEST(reference_across_swapped_oidc_identities_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // One policy's issuer is the other's client identifier and vice versa, so
  // the scopes share both strings yet denote different provider clients
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://login.test",
                .client_id = "registry",
                .client_secret_variable = "ONE_TEST_OIDC_REF_SWAP_ALPHA",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "registry",
            .client_id = "https://login.test",
            .client_secret_variable = "ONE_TEST_OIDC_REF_SWAP_BETA",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
}

TEST(reference_mixing_identities_across_oidc_policies_is_rejected) {
  const std::array<std::string_view, 1> source_paths{{"/source"}};
  const std::array<std::string_view, 1> target_paths{{"/target"}};
  // The referrer pairs an issuer and a client identifier that the referent
  // only carries through two different policies, so no single referent scope
  // matches and the reference must not slip through their union
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = source_paths,
        .name = "source",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://alpha.test",
                .client_id = "dashboard",
                .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_SOURCE",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = target_paths,
        .name = "target-one",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://alpha.test",
                .client_id = "registry",
                .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_ONE",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = target_paths,
        .name = "target-two",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://beta.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_TWO",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/source/one"), AT("/target/two")));
}

TEST(reference_between_oidc_scopes_distinguishes_claims) {
  setenv("ONE_TEST_OIDC_REF_CLAIMS", "confidential", 1);
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<std::string_view, 1> gated_paths{{"/gated"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = open_paths,
        .name = "open",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://acme.test",
                .client_id = "dashboard",
                .client_secret_variable = "ONE_TEST_OIDC_REF_CLAIMS",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = gated_paths,
        .name = "gated",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_CLAIMS",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto gate{TABLE(policies)};
  // The same provider client admitting a narrower set of people is a different
  // audience, so neither direction reaches the other
  EXPECT_FALSE(gate.reference_permitted(AT("/open/one"), AT("/gated/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/gated/two"), AT("/open/one")));
}

TEST(reference_between_oidc_scopes_distinguishes_email_domains) {
  setenv("ONE_TEST_OIDC_REF_DOMAINS", "confidential", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_domains{{"acme.test"}};
  const std::array<std::string_view, 1> beta_domains{{"other.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://acme.test",
                .client_id = "dashboard",
                .client_secret_variable = "ONE_TEST_OIDC_REF_DOMAINS",
                .email_domains = alpha_domains,
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_DOMAINS",
            .email_domains = beta_domains,
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
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
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://acme.test",
                .client_id = "dashboard",
                .client_secret_variable = "ONE_TEST_OIDC_REF_SPELLING",
                .claims = CLAIMS_TWO_GROUPS,
                .email_domains = alpha_domains,
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_SPELLING",
            .claims = CLAIMS_TWO_GROUPS,
            .email_domains = beta_domains,
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto gate{TABLE(policies)};
  // A domain names a host, so its case says nothing about who is admitted,
  // and neither does the order the rules were written in
  EXPECT_TRUE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
}

TEST(reference_rules_treat_a_jwt_scope_conservatively) {
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
  // Reference checks read only the policy, so no key set transport is needed
  const auto gate{TABLE(policies)};
  // A public schema may not reference one behind the token scope
  EXPECT_FALSE(gate.reference_permitted(AT("/open/one"), AT("/secure/two")));
  // The token scope may reference a public schema, and itself
  EXPECT_TRUE(gate.reference_permitted(AT("/secure/one"), AT("/open/two")));
  EXPECT_TRUE(gate.reference_permitted(AT("/secure/one"), AT("/secure/two")));
}

TEST(reference_between_jwt_scopes_distinguishes_claims) {
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<std::string_view, 1> gated_paths{{"/gated"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = open_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms}},
       {.paths = gated_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_ONE_GROUP}}}};
  const auto gate{TABLE(policies)};
  // Two policies alike but for their rules admit different callers, so the
  // looser one may not reach what the stricter one guards
  EXPECT_FALSE(gate.reference_permitted(AT("/open/one"), AT("/gated/two")));
  // The reverse is refused too, exactly as a differing token type is. A scope
  // is one indivisible identity rather than a set compared piecewise, so the
  // cost is a build that has to say so, against disclosing a referent to
  // somebody the referrer never admitted
  EXPECT_FALSE(gate.reference_permitted(AT("/gated/two"), AT("/open/one")));
}

TEST(reference_between_jwt_scopes_ignores_the_order_rules_were_written_in) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms,
                .claims = CLAIMS_TWO_GROUPS}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_TWO_GROUPS}}}};
  const auto gate{TABLE(policies)};
  // The rules arrive canonical, so two policies admitting the same callers
  // carry identical bytes and count as one audience in either direction
  EXPECT_TRUE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
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
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = ecdsa}},
       {.paths = gamma_paths,
        .name = "policy-2",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto gate{TABLE(policies)};
  // Same issuer, audience, and key set but a different algorithm is a different
  // scope, so no token could satisfy the reference
  EXPECT_FALSE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  // An identical policy is the same scope
  EXPECT_TRUE(gate.reference_permitted(AT("/alpha/one"), AT("/gamma/two")));
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
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = one_order}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = other_order}}}};
  const auto gate{TABLE(policies)};
  EXPECT_TRUE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
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
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa,
                .token_type = "at+jwt"}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa,
            .token_type = "Application/AT+JWT"}}}};
  const auto gate{TABLE(policies)};
  EXPECT_TRUE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
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
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = repeated}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = once}}}};
  const auto gate{TABLE(policies)};
  EXPECT_TRUE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
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
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://login.test",
                .audience = "registry",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "registry",
            .audience = "https://login.test",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
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
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://alpha.test",
                .audience = "dashboard",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa}},
       {.paths = target_paths,
        .name = "policy-1",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://alpha.test",
                .audience = "registry",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa}},
       {.paths = target_paths,
        .name = "policy-2",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://beta.test",
            .audience = "dashboard",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto gate{TABLE(policies)};
  EXPECT_FALSE(gate.reference_permitted(AT("/source/one"), AT("/target/two")));
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
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "https://idp.test/jwks",
                .jwks_uri = "registry",
                .algorithms = rsa}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "registry",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto path{TEST_PATH("jwt_reference_swapped_keys.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(AT("/alpha/one"), AT("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(AT("/beta/two"), AT("/alpha/one")));
}

// A configured policy path that only differs cosmetically still has to gate the
// location it names. A spelling the matcher could not traverse would leave the
// target public while the configuration reads as though it were gated
