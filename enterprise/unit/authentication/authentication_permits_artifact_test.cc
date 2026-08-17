#include "authentication_helpers.h"

// Whether a caller reaches a location when the table could not be read.
// Every case builds a broken or missing artifact and asks the same thing,
// which is that nothing is reached rather than everything

TEST(missing_artifact_denies_everything) {
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          std::filesystem::path{"/no/such/authentication.bin"}},
      STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(
      authentication.permits(AT("/"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/acme/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(
      authentication.permits(AT(""), authentication.caller({.bearer = ""})));
}

TEST(malformed_artifact_denies_everything) {
  const auto path{TEST_PATH("malformed.bin")};
  std::ofstream stream{path, std::ios::binary};
  const std::array<char, 64> garbage{};
  stream.write(garbage.data(), garbage.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(
      authentication.permits(AT("/"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/acme/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(structurally_corrupt_artifact_denies_everything) {
  const auto path{TEST_PATH("corrupt.bin")};
  std::ofstream stream{path, std::ios::binary};
  // A valid header over an empty node table
  std::array<char, 40> header{};
  header[0] = 'A';
  header[1] = 'U';
  header[2] = 'T';
  header[3] = 'H';
  header[4] = 4;
  stream.write(header.data(), header.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(
      authentication.permits(AT("/"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(artifact_exceeding_the_policy_ceiling_denies_everything) {
  const auto path{TEST_PATH("too-many-policies.bin")};
  std::ofstream stream{path, std::ios::binary};
  // A valid header declaring a policy count past the supported maximum
  std::array<char, 40> header{};
  header[0] = 'A';
  header[1] = 'U';
  header[2] = 'T';
  header[3] = 'H';
  header[4] = 4;
  // One past what a 64 bit mask has room to name
  header[8] = static_cast<char>(65);
  header[12] = 1;
  stream.write(header.data(), header.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(
      authentication.permits(AT("/"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(AT("/acme/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(corrupted_section_offset_denies_everything) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_OFFSET"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{TEST_PATH("corrupted_offset.bin")};
  SAVE(policies, path, path, ANYWHERE);

  // Overwrite the node section offset with a value that aliases the header
  std::fstream stream{path, std::ios::binary | std::ios::in | std::ios::out};
  stream.seekp(24);
  const std::array<char, 4> aliased{};
  stream.write(aliased.data(), aliased.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  EXPECT_FALSE(authentication.permits(AT("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(
      authentication.permits(AT("/"), authentication.caller({.bearer = ""})));
}

TEST(an_artifact_whose_table_omits_the_anonymous_view_is_refused) {
  setenv("ONE_TEST_KEY_TABLE_ANONYMOUS", "table-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_TABLE_ANONYMOUS"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{TEST_PATH("table_anonymous.bin")};
  SAVE(policies, path, path, ANYWHERE);

  // The anonymous view is the entry naming no policy, so giving the entry that
  // holds it a policy is what a table naming nobody anonymous looks like. Where
  // the table sits is read from the header rather than recomputed here
  std::fstream stream{path, std::ios::binary | std::ios::in | std::ios::out};
  stream.seekg(static_cast<std::streamoff>(sizeof(std::uint32_t) * 8));
  std::uint32_t views_offset{0};
  std::array<char, sizeof(views_offset)> located{};
  stream.read(located.data(), located.size());
  std::memcpy(&views_offset, located.data(), sizeof(views_offset));

  stream.seekp(static_cast<std::streamoff>(views_offset));
  const std::array<char, 8> occupied{{1, 0, 0, 0, 0, 0, 0, 0}};
  stream.write(occupied.data(), occupied.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, STUB_FETCHER({}, nullptr)};
  // A refused artifact denies rather than serving anybody a tree it guessed AT
  EXPECT_FALSE(authentication.permits(
      AT("/machine/x"), authentication.caller({.bearer = "table-secret"})));
  EXPECT_FALSE(authentication.permits(AT("/anywhere"),
                                      authentication.caller({.bearer = ""})));
}

TEST(jwt_without_a_transport_denies_rather_than_crashes) {
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
  const sourcemeta::one::Authentication authentication{TABLE(policies), {}};
  EXPECT_FALSE(authentication.permits(
      AT("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}
