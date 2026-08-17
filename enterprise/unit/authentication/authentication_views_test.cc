#include "authentication_helpers.h"

// Which views a set of policies segments a registry into, read back as the
// names a build would fan its output across

// The names a table serves, which is what the naming rule below decides. What
// each name stands for is read from what its view reaches rather than from the
// set behind it, since that set is the artifact's own business
static auto VIEW_NAMES(const sourcemeta::one::Authentication::Table &table)
    -> std::vector<std::string_view> {
  std::vector<std::string_view> result;
  for (const auto &view : table.views()) {
    result.push_back(view.name());
  }

  return result;
}

TEST(an_instance_that_read_no_artifact_shows_nothing) {
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          std::filesystem::path{"/no/such/authentication.bin"}},
      STUB_FETCHER({}, nullptr)};
  // It admits nobody, so it must not answer that everything is public either,
  // which is what knowing nothing about who governs what would otherwise mean
  EXPECT_FALSE(authentication.permits(AT("/anywhere"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.table().views().empty());
  EXPECT_FALSE(authentication.table().visible(
      AT("/anywhere"), authentication.table().view("public")));
  EXPECT_FALSE(authentication.table().visible(
      AT("/"), authentication.table().view("public")));
}

TEST(a_corrupt_artifact_shows_nothing) {
  const auto path{TEST_PATH("visible_corrupt.bin")};
  std::ofstream stream{path, std::ios::binary};
  const std::array<char, 4> garbage{{'N', 'O', 'P', 'E'}};
  stream.write(garbage.data(), garbage.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/anywhere"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.table().views().empty());
  EXPECT_FALSE(authentication.table().visible(
      AT("/anywhere"), authentication.table().view("public")));
}

TEST(views_of_nothing_are_the_public_one_alone) {
  const auto gate{
      TABLE(std::span<const sourcemeta::one::Authentication::Policy>{})};
  EXPECT_EQ(VIEW_NAMES(gate), (std::vector<std::string_view>{"public"}));
}

TEST(views_name_a_static_key_policy_on_its_own) {
  const std::array<std::string_view, 1> paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "vault",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};

  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate),
            (std::vector<std::string_view>{"public", "vault"}));
}

TEST(views_name_an_interactive_policy_on_its_own) {
  const std::array<std::string_view, 1> paths{{"/console"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "desk",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://idp.example.com/realms/staff",
            .session_secrets = SESSION_SECRETS}}}};

  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate),
            (std::vector<std::string_view>{"public", "desk"}));
}

TEST(views_name_a_token_policy_on_its_own) {
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate),
            (std::vector<std::string_view>{"public", "machine"}));
}

TEST(views_combine_token_policies_that_name_one_issuer) {
  const std::array<std::string_view, 1> legal_paths{{"/legal"}};
  const std::array<std::string_view, 1> tech_paths{{"/tech"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = legal_paths,
        .name = "legal",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = tech_paths,
        .name = "tech",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  // A claim is a list and a rule is met by any of its values, so one token can
  // satisfy both
  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate), (std::vector<std::string_view>{
                                  "public", "legal", "legal+tech", "tech"}));
}

TEST(views_never_combine_token_policies_across_issuers) {
  const std::array<std::string_view, 1> staff_paths{{"/internal"}};
  const std::array<std::string_view, 1> partner_paths{{"/partners"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = staff_paths,
        .name = "staff",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = partner_paths,
        .name = "partner",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.partner.com/realms/main"}}}};

  // A token carries one issuer and is verified against it before any rule is
  // read, so nobody can ever hold both
  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate),
            (std::vector<std::string_view>{"public", "partner", "staff"}));
}

TEST(views_never_combine_token_policies_testing_one_claim_across_issuers) {
  const std::array<std::string_view, 1> legal_paths{{"/legal"}};
  const std::array<std::string_view, 1> partner_paths{{"/partners"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = legal_paths,
        .name = "legal",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff",
                .claims = R"({"department":{"values":["legal"]}})"}},
       {.paths = partner_paths,
        .name = "partner-legal",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.partner.com/realms/main",
            .claims = R"({"department":{"values":["legal"]}})"}}}};

  // The issuer is decisive and is read first, so testing the same claim for the
  // same value means nothing across them
  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate), (std::vector<std::string_view>{"public", "legal",
                                                             "partner-legal"}));
}

TEST(views_never_combine_interactive_policies_under_one_issuer) {
  const std::array<std::string_view, 1> first_paths{{"/one"}};
  const std::array<std::string_view, 1> second_paths{{"/two"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = first_paths,
        .name = "first",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://idp.example.com/realms/staff",
                .session_secrets = SESSION_SECRETS}},
       {.paths = second_paths,
        .name = "second",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://idp.example.com/realms/staff",
            .session_secrets = SESSION_SECRETS}}}};

  // A browser holds one session naming one policy, so sharing an issuer buys an
  // interactive caller nothing
  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate),
            (std::vector<std::string_view>{"public", "first", "second"}));
}

TEST(views_never_combine_static_key_policies) {
  const std::array<std::string_view, 1> first_paths{{"/one"}};
  const std::array<std::string_view, 1> second_paths{{"/two"}};
  const std::array<std::string_view, 1> first_keys{{"one-secret"}};
  const std::array<std::string_view, 1> second_keys{{"two-secret"}};
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

  // A caller presents one key, so it satisfies one of these whatever it holds
  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate),
            (std::vector<std::string_view>{"public", "first", "second"}));
}

TEST(views_spell_a_combination_the_same_however_it_was_declared) {
  const std::array<std::string_view, 1> tech_paths{{"/tech"}};
  const std::array<std::string_view, 1> legal_paths{{"/legal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = tech_paths,
        .name = "tech",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = legal_paths,
        .name = "legal",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate), (std::vector<std::string_view>{
                                  "public", "legal", "legal+tech", "tech"}));
}

TEST(views_of_three_token_policies_under_one_issuer_are_every_combination) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = paths,
        .name = "a",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = paths,
        .name = "b",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = paths,
        .name = "c",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate),
            (std::vector<std::string_view>{"public", "a", "a+b", "a+b+c", "a+c",
                                           "b", "b+c", "c"}));
}

TEST(views_mix_a_combining_group_with_policies_that_stand_alone) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = paths,
        .name = "vault",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "legal",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = paths,
        .name = "tech",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate),
            (std::vector<std::string_view>{"public", "legal", "legal+tech",
                                           "tech", "vault"}));
}

TEST(views_hold_the_public_one_even_when_every_path_is_governed) {
  const std::array<std::string_view, 1> paths{{"/"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "everything",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};

  const auto gate{TABLE(policies)};
  EXPECT_EQ(VIEW_NAMES(gate),
            (std::vector<std::string_view>{"public", "everything"}));
}

TEST(views_of_six_token_policies_under_one_issuer_are_every_combination) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, 6> policies{};
  const std::array<std::string_view, 6> names{{"a", "b", "c", "d", "e", "f"}};
  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).name = names.at(index);
    policies.at(index).credential =
        sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"};
  }

  // Two to the sixth, being every non-empty combination plus the anonymous one
  const auto gate{TABLE(policies)};
  EXPECT_EQ(gate.views().size(), 64);
}

TEST(views_of_two_issuer_groups_are_a_sum_rather_than_a_product) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, 8> policies{};
  const std::array<std::string_view, 8> names{
      {"a", "b", "c", "d", "e", "f", "g", "h"}};
  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).name = names.at(index);
    policies.at(index).credential =
        sourcemeta::one::Authentication::Policy::Token{
            .issuer = index < 4 ? "https://idp.example.com/realms/staff"
                                : "https://idp.partner.com/realms/main"};
  }

  // Each group contributes its own combinations and nothing crosses between
  // them, so the total adds rather than multiplies
  const auto gate{TABLE(policies)};
  EXPECT_EQ(gate.views().size(), 1 + 15 + 15);
}

TEST(views_of_the_largest_combinable_group_are_every_combination) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, COMBINABLE_CEILING>
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

  const auto gate{TABLE(policies)};
  EXPECT_EQ(gate.views().size(), (std::size_t{1} << COMBINABLE_CEILING));
  EXPECT_EQ(gate.views().at(0).name(), "public");
}

TEST(views_of_many_policies_across_issuers_are_never_refused) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, 40> policies{};
  std::vector<std::string> names;
  names.reserve(policies.size());
  for (std::size_t index{0}; index < policies.size(); index++) {
    names.push_back("p" + std::to_string(index));
  }

  // Well past the ceiling in total, and no group approaches it, so nothing is
  // refused. What a build makes of forty views is not decided here
  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).name = names.at(index);
    policies.at(index).credential =
        sourcemeta::one::Authentication::Policy::Token{.issuer =
                                                           names.at(index)};
  }

  const auto gate{TABLE(policies)};
  EXPECT_EQ(gate.views().size(), 41);
}

TEST(a_name_no_view_holds_is_served_as_anonymous) {
  setenv("ONE_TEST_KEY_VIEW_INDEX", "index-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_VIEW_INDEX"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto gate{TABLE(policies)};
  EXPECT_EQ(gate.views().size(), std::size_t{2});
  // A location nobody governs is shown under either view this declares
  EXPECT_TRUE(gate.visible(AT("/open/x"), gate.view("public")));
  EXPECT_TRUE(gate.visible(AT("/open/x"), gate.view("machine")));

  // A build stamps a view's name on the actions it fans out, so a policy
  // withdrawn since leaves a name this no longer holds. It is served what a
  // caller holding nothing is served, rather than what the name used to reach
  const auto withdrawn{gate.view("retired")};
  EXPECT_EQ(withdrawn.name(), "public");
  EXPECT_TRUE(gate.visible(AT("/open/x"), withdrawn));
  EXPECT_FALSE(gate.visible(AT("/machine/x"), withdrawn));
}
