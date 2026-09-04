#include "authentication_helpers.h"

// Whether a caller reaches a location, as the shape of a policy scope is
// varied and the credential is held still

TEST(uncovered_paths_are_public_around_a_gated_scope) {
  setenv("ONE_TEST_KEY_SCOPE", "scope-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SCOPE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  // The covered subtree is gated
  EXPECT_FALSE(authentication.permits(AT("/internal"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      AT("/internal"), authentication.caller({.bearer = "scope-secret"})));
  EXPECT_TRUE(authentication.permits(
      AT("/internal/foo"), authentication.caller({.bearer = "scope-secret"})));
  // Everything outside it is public
  EXPECT_TRUE(
      authentication.permits(AT("/"), authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(AT("/vendor"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(AT("/vendor/foo"),
                                     authentication.caller({.bearer = ""})));
}

TEST(scope_matches_whole_segments_only) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SEGMENT"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{TEST_PATH("segment_boundary.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  // The scope gates its own segment
  EXPECT_FALSE(authentication.permits(AT("/internal"),
                                      authentication.caller({.bearer = ""})));
  // A textual prefix that is not a whole segment is a different path, so it is
  // uncovered and public
  EXPECT_TRUE(authentication.permits(AT("/internalish"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(AT("/int"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(AT("/internal-team"),
                                     authentication.caller({.bearer = ""})));
}

TEST(distinct_policies_each_gate_their_scope) {
  const std::array<std::string_view, 1> alpha{{"/alpha"}};
  const std::array<std::string_view, 1> beta{{"/beta"}};
  const std::array<std::string_view, 1> gamma{{"/gamma"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_KEY_DA"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_KEY_DB"}};
  const std::array<std::string_view, 1> gamma_keys{{"ONE_TEST_KEY_DG"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = alpha,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                alpha_keys}},
       {.paths = beta,
        .name = "beta",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = beta_keys}},
       {.paths = gamma,
        .name = "gamma",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = gamma_keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/alpha/one"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/beta/two"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/gamma/three"),
                                      authentication.caller({.bearer = ""})));
  // Between the scopes the registry is public
  EXPECT_TRUE(authentication.permits(AT("/delta"),
                                     authentication.caller({.bearer = ""})));
}

TEST(nested_prefixes_gate_their_subtrees) {
  const std::array<std::string_view, 1> internal{{"/internal"}};
  const std::array<std::string_view, 1> secret{{"/internal/secret"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_NI"}};
  const std::array<std::string_view, 1> secret_keys{{"ONE_TEST_KEY_NS"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = internal,
        .name = "internal",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                internal_keys}},
       {.paths = secret,
        .name = "secret",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = secret_keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/internal"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/internal/other"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/internal/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/internal/secret/deep"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(
      authentication.permits(AT("/"), authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(AT("/public"),
                                     authentication.caller({.bearer = ""})));
}

TEST(nested_inner_key_widens_access) {
  setenv("ONE_TEST_KEY_WI", "wi-secret", 1);
  setenv("ONE_TEST_KEY_WO", "wo-secret", 1);
  const std::array<std::string_view, 1> outer{{"/internal"}};
  const std::array<std::string_view, 1> inner{{"/internal/secret"}};
  const std::array<std::string_view, 1> outer_keys{{"ONE_TEST_KEY_WO"}};
  const std::array<std::string_view, 1> inner_keys{{"ONE_TEST_KEY_WI"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = outer,
        .name = "outer",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                outer_keys}},
       {.paths = inner,
        .name = "inner",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = inner_keys}}}};
  const auto path{TEST_PATH("nested_widen.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  // The inner path is covered by both, so either key admits it
  EXPECT_TRUE(authentication.permits(
      AT("/internal/secret"), authentication.caller({.bearer = "wo-secret"})));
  EXPECT_TRUE(authentication.permits(
      AT("/internal/secret"), authentication.caller({.bearer = "wi-secret"})));
  // The outer path is covered only by the outer policy
  EXPECT_TRUE(authentication.permits(
      AT("/internal/other"), authentication.caller({.bearer = "wo-secret"})));
  EXPECT_FALSE(authentication.permits(
      AT("/internal/other"), authentication.caller({.bearer = "wi-secret"})));
}

TEST(single_policy_with_multiple_prefixes) {
  const std::array<std::string_view, 2> paths{{"/internal", "/vendor"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MP"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/vendor/bar"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(AT("/public"),
                                     authentication.caller({.bearer = ""})));
}

TEST(extensionless_policy_gates_every_representation) {
  setenv("ONE_TEST_KEY_REPRESENTATION", "representation-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_REPRESENTATION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  // The resource, every representation of it, and its subtree are all governed
  EXPECT_FALSE(authentication.permits(AT("/secret/data"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/secret/data.json"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/secret/data.xml"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/secret/data/nested"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      AT("/secret/data"),
      authentication.caller({.bearer = "representation-secret"})));
  EXPECT_TRUE(authentication.permits(
      AT("/secret/data.json"),
      authentication.caller({.bearer = "representation-secret"})));
  EXPECT_TRUE(authentication.permits(
      AT("/secret/data.xml"),
      authentication.caller({.bearer = "representation-secret"})));
  EXPECT_TRUE(authentication.permits(
      AT("/secret/data/nested"),
      authentication.caller({.bearer = "representation-secret"})));
  // A sibling sharing a textual prefix is covered by no policy, so it is public
  EXPECT_TRUE(authentication.permits(AT("/secret/database"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(AT("/secret/data2.json"),
                                     authentication.caller({.bearer = ""})));
}

TEST(extension_specific_policy_gates_only_that_representation) {
  setenv("ONE_TEST_KEY_SPECIFIC", "specific-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data.json"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SPECIFIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  // Only the named representation is gated
  EXPECT_FALSE(authentication.permits(AT("/secret/data.json"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      AT("/secret/data.json"),
      authentication.caller({.bearer = "specific-secret"})));
  EXPECT_TRUE(authentication.permits(
      AT("/secret/data.json/nested"),
      authentication.caller({.bearer = "specific-secret"})));
  // The bare resource and other representations are uncovered, so public
  EXPECT_TRUE(authentication.permits(AT("/secret/data"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(AT("/secret/data.xml"),
                                     authentication.caller({.bearer = ""})));
}

TEST(extension_handling_is_confined_to_the_terminal_segment) {
  const std::array<std::string_view, 1> paths{{"/v1"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_V1"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  // The policy on /v1 gates its own subtree
  EXPECT_FALSE(
      authentication.permits(AT("/v1"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/v1/secret"),
                                      authentication.caller({.bearer = ""})));
  // As a terminal segment, /v1.0 is a representation of /v1 under the
  // content-negotiation rule, the same way /person.json represents /person
  EXPECT_FALSE(authentication.permits(AT("/v1.0"),
                                      authentication.caller({.bearer = ""})));
  // But as an intermediate segment it is a distinct directory that does not
  // descend into the /v1 subtree, so its children are uncovered and public
  EXPECT_TRUE(authentication.permits(AT("/v1.0/secret"),
                                     authentication.caller({.bearer = ""})));
}

TEST(an_explicit_route_is_gated_on_the_target_as_it_arrived) {
  setenv("ONE_TEST_KEY_ROUTE", "route-secret", 1);
  const std::array<std::string_view, 1> apikey_paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_ROUTE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = apikey_paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(
      sourcemeta::one::Authentication::RouteTarget{"/private/secret"},
      authentication.caller({.bearer = "", .cookies = {}})));
  EXPECT_TRUE(authentication.permits(
      sourcemeta::one::Authentication::RouteTarget{"/private/secret"},
      authentication.caller({.bearer = "route-secret", .cookies = {}})));

  // A target covered by no policy is admitted, including one whose spelling
  // only resembles a governed prefix
  EXPECT_TRUE(authentication.permits(
      sourcemeta::one::Authentication::RouteTarget{"/public/string"},
      authentication.caller({.bearer = "", .cookies = {}})));
  EXPECT_TRUE(authentication.permits(
      sourcemeta::one::Authentication::RouteTarget{"/privateextra/secret"},
      authentication.caller({.bearer = "", .cookies = {}})));
}

TEST(supports_the_maximum_number_of_policies) {
  // One bit per policy in a 64 bit mask, which is what an artifact has room
  // to name
  constexpr std::size_t MAXIMUM{64};
  std::vector<std::string> path_storage;
  path_storage.reserve(MAXIMUM);
  for (std::size_t index{0}; index < MAXIMUM; index += 1) {
    path_storage.push_back("/p" + std::to_string(index));
  }

  std::vector<std::string_view> path_views;
  path_views.reserve(MAXIMUM);
  for (const auto &value : path_storage) {
    path_views.push_back(value);
  }

  std::vector<std::string> name_storage;
  name_storage.reserve(MAXIMUM);
  for (std::size_t index{0}; index < MAXIMUM; index += 1) {
    name_storage.push_back("p" + std::to_string(index));
  }

  std::vector<sourcemeta::one::Authentication::Policy> policies;
  policies.reserve(MAXIMUM);
  for (std::size_t index{0}; index < MAXIMUM; index += 1) {
    policies.push_back(
        {.paths = std::span<const std::string_view>{&path_views[index], 1},
         .name = name_storage[index],
         .credential = sourcemeta::one::Authentication::Policy::ApiKey{}});
  }

  const auto path{TEST_PATH("maximum_policies.bin")};
  SAVE(policies, path, path, ANYWHERE);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  // The keyless policies gate their scope with no key that can open it
  EXPECT_FALSE(authentication.permits(AT("/p0/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/p63/foo"),
                                      authentication.caller({.bearer = ""})));
  // An uncovered path is public
  EXPECT_TRUE(authentication.permits(AT("/missing"),
                                     authentication.caller({.bearer = ""})));
}

TEST(an_apikey_credential_never_triggers_a_jwt_fetch) {
  setenv("ONE_TEST_KEY_NO_FETCH", "static-api-key", 1);
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NO_FETCH"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // A key policy and a token policy over one location, so that a credential
  // opening the first has a key set behind the second it could have asked for
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "keys",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "tokens",
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

  // The key opens the location, and nothing was fetched to decide that
  EXPECT_TRUE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = "static-api-key"})));
  EXPECT_EQ(*calls, 0);

  // A credential shaped like a token does reach the key set, which is what
  // makes the count above mean the key was decided without one
  static_cast<void>(
      authentication.caller({.bearer = std::string{SIGNED_TOKEN}}));
  EXPECT_EQ(*calls, 1);
}

TEST(a_shadowing_cookie_alone_never_admits) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SHADOW_C",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  // Trying every value admits a caller if any one opens, and never because
  // several did not
  const std::string cookies{"sourcemeta_one_session=not-a-session; "
                            "sourcemeta_one_session=nor-is-this"};
  EXPECT_FALSE(authentication.permits(
      AT("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)})));
}

TEST(a_policy_path_declared_canonically_gates_its_location) {
  setenv("ONE_TEST_KEY_CANONICAL", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_CANONICAL"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/private/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      AT("/private/secret"),
      authentication.caller({.bearer = "spelling-secret"})));
  // A location the policy does not name stays public
  EXPECT_TRUE(authentication.permits(AT("/public/string"),
                                     authentication.caller({.bearer = ""})));
}

TEST(a_policy_path_carrying_a_dot_segment_gates_its_location) {
  setenv("ONE_TEST_KEY_DOT", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"/./private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_DOT"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/private/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      AT("/private/secret"),
      authentication.caller({.bearer = "spelling-secret"})));
  // A location the policy does not name stays public
  EXPECT_TRUE(authentication.permits(AT("/public/string"),
                                     authentication.caller({.bearer = ""})));
}

TEST(a_policy_path_that_climbs_back_into_itself_gates_its_location) {
  setenv("ONE_TEST_KEY_CLIMB", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"/private/../private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_CLIMB"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/private/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      AT("/private/secret"),
      authentication.caller({.bearer = "spelling-secret"})));
  // A location the policy does not name stays public
  EXPECT_TRUE(authentication.permits(AT("/public/string"),
                                     authentication.caller({.bearer = ""})));
}

TEST(a_policy_path_carrying_a_repeated_separator_gates_its_location) {
  setenv("ONE_TEST_KEY_SEPARATOR", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"//private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SEPARATOR"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/private/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      AT("/private/secret"),
      authentication.caller({.bearer = "spelling-secret"})));
  // A location the policy does not name stays public
  EXPECT_TRUE(authentication.permits(AT("/public/string"),
                                     authentication.caller({.bearer = ""})));
}

// A route nobody governs is reached by anybody, so an audience requirement on
// it narrows nothing. Refusing a caller there for presenting a token issued
// elsewhere would refuse them for holding a credential they did not need, at a
// route they could have reached by presenting nothing at all
TEST(an_ungoverned_route_ignores_the_audience_it_requires) {
  setenv("ONE_TEST_UNGOVERNED_ROUTE_KEY", "elsewhere-secret", 1);
  const std::array<std::string_view, 1> paths{{"/elsewhere"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_UNGOVERNED_ROUTE_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "elsewhere",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("ungoverned_route"), ANYWHERE)},
      STUB_FETCHER({}, nullptr)};

  const sourcemeta::one::Authentication::RouteTarget target{"/self/v1/mcp"};

  // Nobody governs it, so it opens whatever the caller brought
  EXPECT_TRUE(authentication.permits(target, authentication.caller({}),
                                     "https://example.com/self/v1/mcp"));
  EXPECT_TRUE(authentication.permits(
      target, authentication.caller({.bearer = "elsewhere-secret"}),
      "https://example.com/self/v1/mcp"));

  // And the same route without a requirement answers identically, which is
  // what shows the requirement is what did nothing rather than the route
  EXPECT_TRUE(authentication.permits(target, authentication.caller({})));

  // A governed path still answers to who is asking
  EXPECT_FALSE(
      authentication.permits(AT("/elsewhere/x"), authentication.caller({})));
  EXPECT_TRUE(authentication.permits(
      AT("/elsewhere/x"),
      authentication.caller({.bearer = "elsewhere-secret"})));
}
