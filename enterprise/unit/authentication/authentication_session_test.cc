#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem::path
#include <map>         // std::map
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

// The sealed values this instance mints are its own workings rather than its
// interface, so these reach them the only way anybody can: by starting a login
// and handing what it produced back to the callback that reads it. That is also
// the only way they are ever reached in service of a request.
//
// A value that opens carries the callback past the one gate that reads it, and
// a value that does not is refused there. So the two answers below are what
// every case here distinguishes, and the provider is deliberately given no
// token endpoint, which stops the callback one step after that gate and keeps
// every test off a network

static const std::string_view INSTANCE{"https://registry.test"};
static const std::string_view REDIRECT{
    "https://registry.test/self/v1/auth/callback/okta"};

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{AUTHENTICATION_TEST_DIRECTORY} / name;
}

static auto anywhere(const std::string_view) -> bool { return true; }

// A request carries cookie fields rather than bare values, so a test that
// wants a value read has to present it the way a browser would
static auto field(const std::string_view value) -> std::string {
  std::string result{"sourcemeta_one_transaction="};
  result += value;
  return result;
}

// A provider complete enough to start a login against, which answers nothing
// at its token endpoint. A callback that got past the seal stops there, and
// one that did not is refused before it is ever consulted
static auto provider() -> sourcemeta::one::Authentication::Fetcher {
  return
      [](sourcemeta::one::Authentication::ProviderRequest &&request)
          -> std::optional<sourcemeta::one::Authentication::ProviderResponse> {
        if (request.url !=
            "https://acme.test/.well-known/openid-configuration") {
          return std::nullopt;
        }

        return sourcemeta::one::Authentication::ProviderResponse{
            .status = 200,
            .body = R"JSON({
          "issuer": "https://acme.test",
          "authorization_endpoint": "https://acme.test/authorize",
          "token_endpoint": "https://acme.test/token",
          "jwks_uri": "https://acme.test/keys",
          "response_types_supported": [ "code" ],
          "subject_types_supported": [ "public" ],
          "id_token_signing_alg_values_supported": [ "RS256" ]
        })JSON",
            .max_age = std::nullopt};
      };
}

static const std::array<std::string_view, 1> SESSION_SECRETS{
    {"ONE_TEST_SEAL_SECRET"}};
static const std::array<std::string_view, 2> ROTATED_SECRETS{
    {"ONE_TEST_SEAL_ROTATED", "ONE_TEST_SEAL_SECRET"}};

static auto instance(const std::string &name,
                     const std::span<const std::string_view> secrets)
    -> sourcemeta::one::Authentication {
  setenv("ONE_TEST_SEAL_SECRET", "session-secret", 1);
  setenv("ONE_TEST_SEAL_ROTATED", "rotated-secret", 1);
  setenv("ONE_TEST_SEAL_CLIENT", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_SEAL_CLIENT",
            .session_secrets = secrets}}}};
  return sourcemeta::one::Authentication{
      sourcemeta::one::Authentication::compile(policies, test_path(name),
                                               anywhere),
      provider()};
}

// What a login handed the browser: the sealed value it must bring back, and the
// state the provider is expected to echo beside it
struct Started {
  std::string sealed;
  std::string state;
};

static auto value_of(const std::string_view cookie) -> std::string {
  const auto equals{cookie.find('=')};
  const auto end{cookie.find(';', equals)};
  return std::string{cookie.substr(equals + 1, end - equals - 1)};
}

static auto query_of(const std::string_view url, const std::string_view name)
    -> std::string {
  std::string needle{name};
  needle += "=";
  const auto start{url.find(needle) + needle.size()};
  const auto end{url.find('&', start)};
  return std::string{url.substr(start, end - start)};
}

static auto start(const sourcemeta::one::Authentication &authentication)
    -> Started {
  const auto outcome{
      authentication.login("okta", INSTANCE, REDIRECT, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(outcome.cookies.size(), 1);
  return {.sealed = value_of(outcome.cookies.front()),
          .state = query_of(outcome.location, "state")};
}

// Whether the callback read the value as the transaction it names. Anything it
// cannot open is refused before the provider is consulted at all, and anything
// it opens gets one step further, to a provider that named no token endpoint
static auto opens(const sourcemeta::one::Authentication &authentication,
                  const Started &started, const std::string_view sealed)
    -> bool {
  const auto carried{field(sealed)};
  const std::array<std::string_view, 1> presented{{carried}};
  const auto outcome{authentication.callback(
      "okta", INSTANCE, REDIRECT,
      {.state = started.state, .code = "an-authorization-code"},
      {.cookies = presented})};
  return outcome.result !=
         sourcemeta::one::Authentication::Outcome::Result::Invalid;
}

// A sealed value is version.issued.expiry.payload.signature. Every field is
// covered by the signature, so a test that disturbs the wrong one still passes
// and quietly stops testing what it names. These say which field they mean
enum class Field : std::size_t {
  Version = 0,
  Issued = 1,
  Expiry = 2,
  Payload = 3,
  Signature = 4
};

static auto field_start(const std::string_view value, const Field field)
    -> std::size_t {
  std::size_t position{0};
  for (std::size_t index{0}; index < static_cast<std::size_t>(field);
       index += 1) {
    position = value.find('.', position) + 1;
  }

  return position;
}

// Change one character of one field, which is the smallest disturbance that
// should cost a value its signature
static auto disturb(const std::string_view value, const Field field)
    -> std::string {
  std::string result{value};
  const auto position{field_start(value, field)};
  result[position] = (result[position] == 'a' ? 'b' : 'a');
  return result;
}

TEST(session_round_trips_what_a_login_sealed) {
  const auto authentication{instance("seal_roundtrip", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
}

TEST(session_value_is_cookie_safe) {
  const auto authentication{instance("seal_cookie_safe", SESSION_SECRETS)};
  const auto started{start(authentication)};
  for (const auto character : started.sealed) {
    EXPECT_TRUE(character > 0x20 && character < 0x7f && character != '"' &&
                character != ',' && character != ';' && character != '\\');
  }
}

TEST(session_value_has_the_shape_the_tests_below_assume) {
  const auto authentication{instance("seal_shape", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_EQ(std::ranges::count(started.sealed, '.'), 4);
  EXPECT_EQ(started.sealed.front(), '1');
}

TEST(session_denies_a_tampered_version) {
  const auto authentication{instance("seal_version", SESSION_SECRETS)};
  const auto started{start(authentication)};
  // The control is the value it was made from, which opens
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(
      opens(authentication, started, disturb(started.sealed, Field::Version)));
}

TEST(session_denies_a_tampered_issuance) {
  const auto authentication{instance("seal_issuance", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(
      opens(authentication, started, disturb(started.sealed, Field::Issued)));
}

TEST(session_denies_a_tampered_expiry) {
  const auto authentication{instance("seal_expiry", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(
      opens(authentication, started, disturb(started.sealed, Field::Expiry)));
}

TEST(session_denies_a_tampered_payload) {
  const auto authentication{instance("seal_payload", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(
      opens(authentication, started, disturb(started.sealed, Field::Payload)));
}

TEST(session_denies_a_tampered_signature) {
  const auto authentication{instance("seal_signature", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started,
                     disturb(started.sealed, Field::Signature)));
}

TEST(session_denies_a_truncated_signature) {
  const auto authentication{instance("seal_truncated", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started,
                     started.sealed.substr(0, started.sealed.size() - 1)));
}

TEST(session_denies_a_lengthened_signature) {
  const auto authentication{instance("seal_lengthened", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started, started.sealed + "a"));
}

TEST(session_denies_a_transplanted_signature) {
  const auto authentication{instance("seal_transplant", SESSION_SECRETS)};
  const auto first{start(authentication)};
  const auto second{start(authentication)};

  // Everything but the signature from one value, and the signature from
  // another, which is what a signature covering every field has to refuse
  std::string spliced{
      first.sealed.substr(0, field_start(first.sealed, Field::Signature))};
  spliced += second.sealed.substr(field_start(second.sealed, Field::Signature));
  EXPECT_TRUE(opens(authentication, first, first.sealed));
  EXPECT_FALSE(opens(authentication, first, spliced));
}

TEST(session_denies_malformed_values) {
  const auto authentication{instance("seal_malformed", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started, ""));
  EXPECT_FALSE(opens(authentication, started, "1"));
  EXPECT_FALSE(opens(authentication, started, "1.2.3.4"));
  EXPECT_FALSE(opens(authentication, started, "not-a-sealed-value"));
  EXPECT_FALSE(opens(authentication, started, "1....."));
}

TEST(session_denies_a_signature_that_is_not_base64url) {
  const auto authentication{instance("seal_base64url", SESSION_SECRETS)};
  const auto started{start(authentication)};
  std::string altered{
      started.sealed.substr(0, field_start(started.sealed, Field::Signature))};
  altered += "!!!!";
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started, altered));
}

TEST(session_denies_a_wrong_secret) {
  const auto minting{instance("seal_wrong_minting", SESSION_SECRETS)};
  const auto started{start(minting)};

  // The same policy under a different secret, so the value differs from one it
  // would accept in exactly that
  const auto reading{instance("seal_wrong_reading", ROTATED_SECRETS)};
  const auto theirs{start(reading)};
  EXPECT_TRUE(opens(reading, theirs, theirs.sealed));
  EXPECT_FALSE(opens(reading, theirs, started.sealed));
}

TEST(session_admits_a_value_sealed_under_an_older_secret) {
  const auto minting{instance("seal_old", SESSION_SECRETS)};
  const auto started{start(minting)};

  // The newest secret leads and the one that sealed this follows, which is what
  // lets a secret be replaced without ending what it signed
  const auto rotated{instance("seal_new", ROTATED_SECRETS)};
  EXPECT_TRUE(opens(rotated, started, started.sealed));
}

TEST(session_denies_a_value_sealed_for_another_purpose) {
  const auto authentication{instance("seal_purpose", SESSION_SECRETS)};
  const auto started{start(authentication)};

  // A session is sealed for a different purpose than a transaction, and only a
  // transaction opens here. Anybody may obtain the value a login hands out
  // without holding any credential, so reading one as the other is what would
  // turn starting a login into being signed in
  const auto carried{field(started.sealed)};
  const std::array<std::string_view, 1> presented{{carried}};
  const auto session{
      authentication.logout({.cookies = presented}, INSTANCE, "/")};
  EXPECT_EQ(session.cookies.size(), 3);
  EXPECT_TRUE(opens(authentication, started, started.sealed));
}

TEST(session_denies_a_value_sealed_under_another_policy_name) {
  const auto authentication{instance("seal_policy", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));

  // The same value offered under a name it was not sealed for, which is what a
  // key derived per policy has to refuse
  const auto carried{field(started.sealed)};
  const std::array<std::string_view, 1> presented{{carried}};
  const auto outcome{authentication.callback(
      "unknown", INSTANCE, REDIRECT,
      {.state = started.state, .code = "an-authorization-code"},
      {.cookies = presented})};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Invalid);
}

TEST(session_denies_a_state_the_provider_did_not_echo) {
  const auto authentication{instance("seal_state", SESSION_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));

  // The value opens, and is still refused, since what it sealed and what came
  // back do not agree. That is what stops a callback assembled elsewhere
  const auto carried{field(started.sealed)};
  const std::array<std::string_view, 1> presented{{carried}};
  const auto outcome{authentication.callback(
      "okta", INSTANCE, REDIRECT,
      {.state = "not-the-state-it-sealed", .code = "an-authorization-code"},
      {.cookies = presented})};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Invalid);
}

TEST(session_denies_a_callback_carrying_no_transaction) {
  const auto authentication{instance("seal_absent", SESSION_SECRETS)};
  const auto started{start(authentication)};
  const auto outcome{authentication.callback(
      "okta", INSTANCE, REDIRECT,
      {.state = started.state, .code = "an-authorization-code"}, {})};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Invalid);
}

TEST(session_reads_every_value_a_request_carried) {
  const auto authentication{instance("seal_several", SESSION_SECRETS)};
  const auto started{start(authentication)};

  // A parent domain and the host itself can each set one, and neither the
  // header nor the order says which is which, so every value is tried rather
  // than the first. Letting whoever set the other one decide would turn the
  // cookie from a defence into the way past it
  const auto carried{field(started.sealed)};
  const std::array<std::string_view, 2> both{
      {"sourcemeta_one_transaction=somebody-elses-value", carried}};
  const auto outcome{authentication.callback(
      "okta", INSTANCE, REDIRECT,
      {.state = started.state, .code = "an-authorization-code"},
      {.cookies = both})};
  EXPECT_NE(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Invalid);
}
