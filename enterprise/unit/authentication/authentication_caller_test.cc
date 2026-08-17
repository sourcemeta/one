#include "authentication_helpers.h"

// Which view a presented credential places its holder in. Every case builds a
// table, asks for a caller, and reads the view it was placed in

static constexpr std::string_view CLAIMS_ONCALL_GROUP{
    R"JSON({
      "groups": {
        "essential": true,
        "values": [ "oncall" ]
      }
    })JSON"};

// A table compiled in this process reads every section from the bytes it holds
// rather than from a mapping it does not have, so it answers a credential the
// same way one read back from a file does
TEST(a_compiled_table_reads_a_policy_without_a_mapping) {
  setenv("ONE_TEST_COMPILED_KEY", "compiled-secret", 1);
  const std::array<std::string_view, 1> paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_COMPILED_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "guard",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("compiled_only"), ANYWHERE)},
      STUB_FETCHER({}, nullptr)};

  EXPECT_TRUE(authentication.permits(AT("/open"), authentication.caller({})));
  EXPECT_FALSE(
      authentication.permits(AT("/private"), authentication.caller({})));
  EXPECT_TRUE(authentication.permits(
      AT("/private"), authentication.caller({.bearer = "compiled-secret"})));
  EXPECT_FALSE(authentication.permits(
      AT("/private"), authentication.caller({.bearer = "wrong-secret"})));
  EXPECT_EQ(authentication.caller({.bearer = "compiled-secret"}).view(),
            "guard");
}

TEST(a_caller_presenting_nothing_is_served_the_anonymous_view) {
  setenv("ONE_TEST_VIEW_ANONYMOUS_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_VIEW_ANONYMOUS_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(), "public");
  EXPECT_EQ(authentication.caller({.bearer = "retired-secret"}).view(),
            "public");
  EXPECT_EQ(authentication.caller({.bearer = "machine-secret"}).view(),
            "machine");
}

TEST(a_caller_presenting_nothing_belongs_to_no_policy) {
  setenv("ONE_TEST_CLASSIFY_ANONYMOUS_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{
      {"ONE_TEST_CLASSIFY_ANONYMOUS_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(),
            sourcemeta::one::VIEW_PUBLIC);
}

TEST(a_credential_opening_nothing_belongs_to_no_policy) {
  setenv("ONE_TEST_CLASSIFY_UNKNOWN_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_CLASSIFY_UNKNOWN_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = "retired-secret"}).view(),
            sourcemeta::one::VIEW_PUBLIC);
}

TEST(a_key_places_its_caller_in_the_policy_it_opens) {
  setenv("ONE_TEST_CLASSIFY_FIRST_KEY", "first-secret", 1);
  setenv("ONE_TEST_CLASSIFY_SECOND_KEY", "second-secret", 1);
  const std::array<std::string_view, 1> first_paths{{"/first"}};
  const std::array<std::string_view, 1> second_paths{{"/second"}};
  const std::array<std::string_view, 1> first_keys{
      {"ONE_TEST_CLASSIFY_FIRST_KEY"}};
  const std::array<std::string_view, 1> second_keys{
      {"ONE_TEST_CLASSIFY_SECOND_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = first_paths,
        .name = "first",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                first_keys}},
       {.paths = second_paths,
        .name = "second",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = second_keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = "first-secret"}).view(), "first");
  EXPECT_EQ(authentication.caller({.bearer = "second-secret"}).view(),
            "second");
}

TEST(a_key_is_placed_without_reference_to_any_path) {
  setenv("ONE_TEST_CLASSIFY_DEEP_KEY", "deep-secret", 1);
  const std::array<std::string_view, 1> paths{{"/deep/inside/somewhere"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_CLASSIFY_DEEP_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "deep",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  // Admitted under the policy where it governs, so the gate has read the key
  const auto governed{
      authentication.permits(AT("/deep/inside/somewhere/x"),
                             authentication.caller({.bearer = "deep-secret"}))};
  EXPECT_TRUE(governed);

  // Admitted anonymously where no policy governs, which is a different answer
  // reached without reading the key at all
  const auto ungoverned{authentication.permits(
      AT("/elsewhere"), authentication.caller({.bearer = "deep-secret"}))};
  EXPECT_TRUE(ungoverned);

  // The same answer for a caller carrying nothing, so being admitted there says
  // nothing about who is asking
  const auto anonymous{authentication.permits(
      AT("/elsewhere"), authentication.caller({.bearer = ""}))};
  EXPECT_TRUE(anonymous);

  // The placement answers once for the caller, naming the policy the key opens
  // wherever they happen to be asking from
  EXPECT_EQ(authentication.caller({.bearer = "deep-secret"}).view(), "deep");
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(),
            sourcemeta::one::VIEW_PUBLIC);
}

TEST(a_key_opening_two_policies_reaches_only_the_first_declared) {
  setenv("ONE_TEST_CLASSIFY_SHARED_EARLY", "shared-secret", 1);
  setenv("ONE_TEST_CLASSIFY_SHARED_LATE", "shared-secret", 1);
  const std::array<std::string_view, 1> early_paths{{"/early"}};
  const std::array<std::string_view, 1> late_paths{{"/late"}};
  const std::array<std::string_view, 1> early_keys{
      {"ONE_TEST_CLASSIFY_SHARED_EARLY"}};
  const std::array<std::string_view, 1> late_keys{
      {"ONE_TEST_CLASSIFY_SHARED_LATE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = early_paths,
        .name = "early",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                early_keys}},
       {.paths = late_paths,
        .name = "late",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = late_keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  // Two variables holding one value is the form the configuration cannot see,
  // and it is the only shape where being admitted and being shown could ever
  // have parted. A caller is read as the first policy their key opens, and what
  // they reach is what that placement holds, so the second policy's area is not
  // theirs. It never was in any useful sense: the answer served there would
  // have been read from a view that does not hold it
  const auto early{authentication.permits(
      AT("/early/x"), authentication.caller({.bearer = "shared-secret"}))};
  EXPECT_TRUE(early);
  const auto late{authentication.permits(
      AT("/late/x"), authentication.caller({.bearer = "shared-secret"}))};
  EXPECT_FALSE(late);

  // What is reached and where it is read from are one answer, which is what
  // naming the placement here pins
  EXPECT_EQ(authentication.caller({.bearer = "shared-secret"}).view(), "early");
}

TEST(a_token_belongs_to_every_policy_of_its_issuer_that_it_satisfies) {
  const std::array<std::string_view, 1> platform_paths{{"/platform"}};
  const std::array<std::string_view, 1> oncall_paths{{"/oncall"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = platform_paths,
        .name = "platform",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms,
                .claims = CLAIMS_ONE_GROUP}},
       {.paths = oncall_paths,
        .name = "oncall",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_ONCALL_GROUP}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies),
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_EQ(authentication
                .caller({.bearer = TOKEN_WITH(
                             R"JSON({ "groups": [ "platform" ] })JSON")})
                .view(),
            "platform");
  EXPECT_EQ(authentication
                .caller({.bearer = TOKEN_WITH(
                             R"JSON({ "groups": [ "oncall" ] })JSON")})
                .view(),
            "oncall");
  // One token carrying both reaches both areas, so a placement naming either
  // alone would hide one of them. The combination is spelled from the policies
  // sorted rather than in the order they were declared, so it has one name
  // wherever it is reached from
  EXPECT_EQ(
      authentication
          .caller({.bearer = TOKEN_WITH(
                       R"JSON({ "groups": [ "oncall", "platform" ] })JSON")})
          .view(),
      "oncall+platform");
  EXPECT_EQ(authentication
                .caller({.bearer = TOKEN_WITH(
                             R"JSON({ "groups": [ "support" ] })JSON")})
                .view(),
            sourcemeta::one::VIEW_PUBLIC);
}

TEST(a_session_places_its_caller_in_the_policy_that_established_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_CLASSIFY_SESSION_SECRET", "confidential", 1);
  setenv("ONE_TEST_CLASSIFY_SESSION_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> portal_paths{{"/portal"}};
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{
      {"ONE_TEST_CLASSIFY_SESSION_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = portal_paths,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_CLASSIFY_SESSION_SECRET",
                .session_secrets = SESSION_SECRETS}},
       {.paths = machine_paths,
        .name = "machine",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  const auto sealed{SESSION_FOR("okta", SESSION_SECRETS, "jane@acme.test")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  EXPECT_EQ(
      authentication.caller({.bearer = "", .cookies = FIELDS(cookies)}).view(),
      "okta");
  EXPECT_EQ(authentication.caller({.bearer = "machine-secret"}).view(),
            "machine");
  // A request carrying a key is read as that key, so the session it also
  // carried places nobody, exactly as it admits nobody
  EXPECT_EQ(
      authentication
          .caller({.bearer = "machine-secret", .cookies = FIELDS(cookies)})
          .view(),
      "machine");
  EXPECT_EQ(
      authentication
          .caller({.bearer = "retired-secret", .cookies = FIELDS(cookies)})
          .view(),
      sourcemeta::one::VIEW_PUBLIC);
}

TEST(save_writes_the_largest_table_a_configuration_can_declare) {
  constexpr std::size_t groups{4};
  constexpr auto per_group{COMBINABLE_CEILING};
  constexpr auto total{groups * per_group};
  std::vector<std::string> path_storage;
  std::vector<std::string> name_storage;
  std::vector<std::string> issuer_storage;
  std::vector<std::string> claims_storage;
  path_storage.reserve(total);
  name_storage.reserve(total);
  issuer_storage.reserve(total);
  claims_storage.reserve(total);
  for (std::size_t index{0}; index < total; index += 1) {
    path_storage.push_back("/p" + std::to_string(index));
    name_storage.push_back("p" + std::to_string(index));
    // The first group answers to the tokens these tests mint, so that a caller
    // reaches into the table rather than only past it. The rest name issuers of
    // their own, which is what keeps the groups apart and the table at its
    // largest
    issuer_storage.push_back(index < per_group
                                 ? "acme"
                                 : "https://idp.test/" +
                                       std::to_string(index / per_group));
    claims_storage.push_back(
        R"JSON({ "groups": { "essential": true, "values": [ "g)JSON" +
        std::to_string(index) + R"JSON(" ] } })JSON");
  }

  std::vector<std::string_view> path_views;
  path_views.reserve(total);
  for (const auto &value : path_storage) {
    path_views.push_back(value);
  }

  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  std::vector<sourcemeta::one::Authentication::Policy> policies;
  policies.reserve(total);
  for (std::size_t index{0}; index < total; index += 1) {
    policies.push_back(
        {.paths = std::span<const std::string_view>{&path_views[index], 1},
         .name = name_storage[index],
         .credential = sourcemeta::one::Authentication::Policy::Token{
             .issuer = issuer_storage[index],
             .audience = "client",
             .jwks_uri = "https://idp.test/jwks",
             .algorithms = algorithms,
             .claims = claims_storage[index]}});
  }

  // Each group contributes every combination over it, which is the most views a
  // configuration can ask for at the maximum number of policies
  const auto path{TEST_PATH(
      "save_writes_the_largest_table_a_configuration_can_declare.bin")};
  SAVE(policies, path, path, ANYWHERE);
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      STUB_FETCHER({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(), "public");
  // A name read back out of a table this size, rather than the entry that sits
  // first in it whatever was written after
  EXPECT_EQ(
      authentication
          .caller({.bearer = TOKEN_WITH(R"JSON({ "groups": [ "g0" ] })JSON")})
          .view(),
      "p0");
  EXPECT_EQ(
      authentication
          .caller({.bearer = TOKEN_WITH(R"JSON({ "groups": [ "g15" ] })JSON")})
          .view(),
      "p15");
  // And a combination, spelled from its members sorted rather than from the
  // order the token happened to carry them in
  EXPECT_EQ(authentication
                .caller({.bearer = TOKEN_WITH(
                             R"JSON({ "groups": [ "g1", "g0" ] })JSON")})
                .view(),
            "p0+p1");
  EXPECT_EQ(authentication
                .caller({.bearer = TOKEN_WITH(
                             R"JSON({ "groups": [ "g2", "g15" ] })JSON")})
                .view(),
            "p15+p2");
  // A token no policy answers to is placed nowhere, which is the anonymous
  // view rather than a name the table happens to carry
  EXPECT_EQ(
      authentication
          .caller({.bearer = TOKEN_WITH(R"JSON({ "groups": [ "gx" ] })JSON")})
          .view(),
      "public");
}

TEST(save_accepts_a_separator_that_spells_no_other_combination) {
  setenv("ONE_TEST_KEY_LONE_SEPARATOR", "separator-secret", 1);
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{
      {"ONE_TEST_KEY_LONE_SEPARATOR"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = machine_paths,
        .name = "alpha+beta",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const auto path{TEST_PATH(
      "save_accepts_a_separator_that_spells_no_other_combination.bin")};
  SAVE(policies, path, path, ANYWHERE);
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = "separator-secret"}).view(),
            "alpha+beta");
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(), "public");
}
