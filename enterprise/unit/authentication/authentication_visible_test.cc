#include "authentication_helpers.h"

// Whether a given view shows a given location, which is the rule a build
// filters by

TEST(the_recorded_table_names_every_view_it_serves) {
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
                .algorithms = algorithms}},
       {.paths = oncall_paths,
        .name = "oncall",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto gate{TABLE(policies)};
  // The anonymous view first, then the rest by name, which is the order a build
  // fans its actions out in
  const auto table{gate.views()};
  EXPECT_EQ(table.size(), std::size_t{4});
  EXPECT_EQ(table.at(0).name(), "public");
  EXPECT_EQ(table.at(1).name(), "oncall");
  EXPECT_EQ(table.at(2).name(), "oncall+platform");
  EXPECT_EQ(table.at(3).name(), "platform");

  // And which policies each of those names stands for is read from what the
  // view reaches rather than from the set behind it
  EXPECT_FALSE(gate.visible(AT("/oncall/x"), table.at(0)));
  EXPECT_TRUE(gate.visible(AT("/oncall/x"), table.at(1)));
  EXPECT_TRUE(gate.visible(AT("/oncall/x"), table.at(2)));
  EXPECT_FALSE(gate.visible(AT("/oncall/x"), table.at(3)));
  EXPECT_FALSE(gate.visible(AT("/platform/x"), table.at(0)));
  EXPECT_FALSE(gate.visible(AT("/platform/x"), table.at(1)));
  EXPECT_TRUE(gate.visible(AT("/platform/x"), table.at(2)));
  EXPECT_TRUE(gate.visible(AT("/platform/x"), table.at(3)));
}

TEST(a_view_shows_what_it_governs_and_whatever_nobody_governs) {
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
                .algorithms = algorithms}},
       {.paths = oncall_paths,
        .name = "oncall",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto gate{TABLE(policies)};
  const auto anonymous{gate.view("public")};
  const auto oncall{gate.view("oncall")};
  const auto both{gate.view("oncall+platform")};
  const auto platform{gate.view("platform")};

  // What nobody governs is shown to everybody, the anonymous view included
  EXPECT_TRUE(gate.visible(AT("/open/x"), anonymous));
  EXPECT_TRUE(gate.visible(AT("/open/x"), oncall));
  EXPECT_TRUE(gate.visible(AT("/open/x"), both));
  EXPECT_TRUE(gate.visible(AT("/open/x"), platform));

  // And a governed location only to a view satisfying something governing it
  EXPECT_FALSE(gate.visible(AT("/platform/x"), anonymous));
  EXPECT_FALSE(gate.visible(AT("/platform/x"), oncall));
  EXPECT_TRUE(gate.visible(AT("/platform/x"), both));
  EXPECT_TRUE(gate.visible(AT("/platform/x"), platform));
  EXPECT_FALSE(gate.visible(AT("/oncall/x"), anonymous));
  EXPECT_TRUE(gate.visible(AT("/oncall/x"), oncall));
  EXPECT_TRUE(gate.visible(AT("/oncall/x"), both));
  EXPECT_FALSE(gate.visible(AT("/oncall/x"), platform));
}
