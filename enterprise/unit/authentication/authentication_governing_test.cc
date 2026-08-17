#include "authentication_helpers.h"

// Which policies govern a location, answered by name and in declaration order

TEST(zero_policies_admits_every_path) {
  const std::array<sourcemeta::one::Authentication::Policy, 0> policies{};
  const sourcemeta::one::Authentication authentication{
      TABLE(policies), STUB_FETCHER({}, nullptr)};
  EXPECT_TRUE(
      authentication.permits(AT("/"), authentication.caller({.bearer = ""})));
  EXPECT_TRUE(
      authentication.permits(AT(""), authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(AT("/acme/foo/bar"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_EQ(authentication.table().governing(AT("/")),
            (std::vector<std::string_view>{}));
  EXPECT_EQ(authentication.table().governing(AT("/acme")),
            (std::vector<std::string_view>{}));
}

TEST(governing_names_the_policies_in_declaration_order) {
  const std::array<std::string_view, 1> root_paths{{"/"}};
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> root_keys{{"ONE_TEST_KEY_GR"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_GI"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = root_paths,
        .name = "root",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = root_keys}},
       {.paths = internal_paths,
        .name = "internal",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = internal_keys}}}};
  const auto gate{TABLE(policies)};
  EXPECT_EQ(gate.governing(AT("/")), (std::vector<std::string_view>{"root"}));
  EXPECT_EQ(gate.governing(AT("/vendor")),
            (std::vector<std::string_view>{"root"}));
  EXPECT_EQ(gate.governing(AT("/internal")),
            (std::vector<std::string_view>{"root", "internal"}));
  EXPECT_EQ(gate.governing(AT("/internal/foo")),
            (std::vector<std::string_view>{"root", "internal"}));
}

TEST(governing_of_an_ungoverned_path_is_empty) {
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_GE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = internal_paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto gate{TABLE(policies)};
  EXPECT_EQ(gate.governing(AT("/vendor")), (std::vector<std::string_view>{}));
  EXPECT_EQ(gate.governing(AT("/internal")),
            (std::vector<std::string_view>{"policy"}));
}
