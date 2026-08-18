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

// A table that could not be read knows nothing about who governs what. Saying
// that nobody governs a location would be read as the location being open to
// everybody, which is the one answer a table in that state must not give
TEST(governing_through_an_unreadable_table_answers_nothing) {
  const sourcemeta::one::Authentication::Table gate{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_FALSE(gate.governing(AT("/anywhere")).has_value());
  EXPECT_FALSE(gate.governing(AT("/")).has_value());
}

// And a table that was read answers a set, which is what makes the absence
// above mean the table rather than the location
TEST(governing_through_a_readable_table_answers_a_set) {
  setenv("ONE_TEST_KEY_GOVERNS_READABLE", "held", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_GOVERNS_READABLE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto gate{TABLE(policies)};
  EXPECT_TRUE(gate.governing(AT("/vendor")).has_value());
  EXPECT_TRUE(gate.governing(AT("/vendor")).value().empty());
  EXPECT_EQ(gate.governing(AT("/internal")).value(),
            (std::vector<std::string_view>{"policy"}));
}
