#include <sourcemeta/core/test.h>
#include <sourcemeta/one/authentication.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem::path
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

// Every gate question is asked about a canonical location, so the tests name
// one the same way a request would
static auto at(const std::string_view input)
    -> sourcemeta::one::Authentication::Path {
  return sourcemeta::one::Authentication::Path::parse(input,
                                                      "http://localhost:8000")
      .value();
}

// These tests name locations directly rather than modelling what an instance
// serves, so every path a policy is scoped to is one to gate
static auto anywhere(const std::string_view) -> bool { return true; }

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{AUTHENTICATION_TEST_DIRECTORY} / name;
}

TEST(admits_every_path_without_a_credential) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_TRUE(authentication.admits(at("/"), {.bearer = ""}).allowed);
  EXPECT_TRUE(authentication.admits(at(""), {.bearer = ""}).allowed);
  EXPECT_TRUE(
      authentication.admits(at("/acme/foo/bar"), {.bearer = ""}).allowed);
}

TEST(admits_every_path_with_any_credential) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_TRUE(
      authentication.admits(at("/internal"), {.bearer = "anything"}).allowed);
  EXPECT_TRUE(authentication.admits(at("/internal/foo"), {.bearer = "another"})
                  .allowed);
}

TEST(save_emits_an_empty_artifact_that_admits_everything) {
  const auto path{test_path("community_public_root.bin")};
  sourcemeta::one::Authentication::save({}, path, path, anywhere);

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(std::filesystem::file_size(path), 0);

  const sourcemeta::one::Authentication authentication{path, {}};
  EXPECT_TRUE(authentication.admits(at("/"), {.bearer = ""}).allowed);
  EXPECT_TRUE(
      authentication.admits(at("/internal/foo"), {.bearer = ""}).allowed);
}

TEST(save_creates_the_directory_it_writes_into) {
  const auto path{test_path("nested") / "deeper" / "authentication.bin"};
  std::filesystem::remove_all(test_path("nested"));
  sourcemeta::one::Authentication::save({}, path, path, anywhere);

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(std::filesystem::file_size(path), 0);
}

TEST(save_rejects_any_policy) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{paths, {}}}};
  const auto path{test_path("community_policy.bin")};
  try {
    sourcemeta::one::Authentication::save(policies, path, path, anywhere);
    FAIL();
  } catch (const sourcemeta::one::EnterpriseOnlyFeatureError &error) {
    EXPECT_STREQ(error.what(),
                 "Authentication is only available on the enterprise edition");
  }
}

TEST(permits_every_reference) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_TRUE(authentication.reference_permitted(at("/one"), at("/two")));
  EXPECT_TRUE(authentication.reference_permitted(at("/public/one"),
                                                 at("/private/two")));
  EXPECT_TRUE(
      authentication.reference_permitted(at("/internal/a"), at("/internal/a")));
}

TEST(records_the_anonymous_view_alone) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_EQ(authentication.view_count(), std::size_t{1});
  EXPECT_EQ(authentication.view_at(0).name, "public");
  EXPECT_EQ(authentication.view_at(0).policies,
            sourcemeta::one::Authentication::PolicySet{0});
}

TEST(shows_every_path_in_its_only_view) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_TRUE(authentication.visible(at("/"), 0));
  EXPECT_TRUE(authentication.visible(at(""), 0));
  EXPECT_TRUE(authentication.visible(at("/internal/foo"), 0));
}

TEST(shows_nothing_under_an_index_naming_no_view) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_EQ(authentication.view_count(), std::size_t{1});
  EXPECT_FALSE(authentication.visible(at("/"), 1));
  EXPECT_FALSE(authentication.visible(at("/internal/foo"), 1));
}

TEST(serves_every_caller_the_anonymous_view) {
  const std::array<std::string_view, 1> cookies{
      {"sourcemeta_one_session=whatever"}};
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_EQ(authentication.view({.bearer = ""}), "public");
  EXPECT_EQ(authentication.view({.bearer = "anything"}), "public");
  EXPECT_EQ(authentication.view({.bearer = "", .cookies = cookies}), "public");
}

TEST(classifies_a_caller_presenting_nothing_as_anonymous) {
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_EQ(authentication.classify({.bearer = ""}),
            sourcemeta::one::Authentication::PolicySet{0});
}

TEST(classifies_a_caller_presenting_a_credential_as_anonymous) {
  const std::array<std::string_view, 1> cookies{
      {"sourcemeta_one_session=whatever"}};
  const sourcemeta::one::Authentication authentication{
      std::filesystem::path{"/no/such/authentication.bin"}, {}};
  EXPECT_EQ(authentication.classify({.bearer = "anything"}),
            sourcemeta::one::Authentication::PolicySet{0});
  EXPECT_EQ(authentication.classify({.bearer = "", .cookies = cookies}),
            sourcemeta::one::Authentication::PolicySet{0});
  EXPECT_EQ(authentication.classify({.bearer = "another", .cookies = cookies}),
            sourcemeta::one::Authentication::PolicySet{0});
}

TEST(views_of_nothing_are_the_public_one_alone) {
  const auto views{sourcemeta::one::Authentication::views({})};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}}}));
}

TEST(views_of_a_static_key_policy_are_the_public_one_alone) {
  const std::array<std::string_view, 1> paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .keys = keys,
        .type = sourcemeta::one::Authentication::Type::ApiKey,
        .name = "vault"}}};

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}}}));
}

TEST(views_of_several_policies_are_the_public_one_alone) {
  const std::array<std::string_view, 1> first_paths{{"/legal"}};
  const std::array<std::string_view, 1> second_paths{{"/tech"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = first_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "legal"},
       {.paths = second_paths,
        .type = sourcemeta::one::Authentication::Type::JWT,
        .issuer = "https://idp.example.com/realms/staff",
        .name = "tech"}}};

  const auto views{sourcemeta::one::Authentication::views(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}}}));
}
