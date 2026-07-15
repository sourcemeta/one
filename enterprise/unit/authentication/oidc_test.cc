#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

// A key pair generated for these tests and used nowhere else. The provider
// publishes the public half as its key set and the tests sign with the
// private half
static constexpr std::string_view OIDC_TEST_PRIVATE_KEY{
    R"PEM(-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDGHTgp3tR0Xc/e
xyi2PWDdSgR+qaCaXt9wE3+yMbik0Edx0yPg7JTTpxfN1e+HHdNzYHWrIGBrxhBT
P2OnLI96X7KF1P3Cq6mqizCmB0CaRYQNiDNJdOIxqgPqDjNULOVr035WO6i7Obk3
mK4/PuhTSZeglyHRyzyldYDiwEDLCtOjiBnXIwMH/c2Lgmeq+ZW6v/t6LHgG0w5p
DsuaUgAmoaJ6DV5txmgty+0N0lVVE/kQQKjovxh8svo/W8vnFEv6touNTy5rh05y
EP0UzQBBwymY/U+UvXAgvgbNKjaXNrROruQq77ms4ZuXyVopGG4gwYi/t9VKjisr
iwsHm8KRAgMBAAECggEAHYz7NSejOg5KNNkxJenG2osxW3FS3uPa4T1PVVWcTJ0y
ovRTDDhNxLpOTyojldxpj3KqapGkQD8lbT1t0GBRsHe4ycSo61Iutx0W7xTy2nAX
D+uqdsEEAlNxTQa8OkiOGgTMefFhLDgie/v+tblc7xtxjgctCL2PeMCrrmbt4zsh
cNNwDTGETc0yNAme/eudrGQ68dDe9CnKlERB6yendn0WAjrc7HIauG3I2wHcBn6Q
gkougIUJqhAMHMsCskZTXPHkcBSGJtHfpxo5Iqyjbj6a6tRpoZZGU39Px09FT1Nu
BiX7wAFEQstgQroMBlMdiUQYTEET6yuI58v6wb0LTwKBgQDrlF3/BI7aGxo40xHP
vEnivGigJQe8iwpywH4EiJI5dcCFIBNU+2Ny0dqq9sJnTEaZXR33301bBWiEPlxf
R9nBOXaIkDbq+QQXiW485haU8corXkcqPgII5pDE2C1OdoVc/Z5YZT0rmfa944j1
lKoxECFT+ZVF8+iAna/wokOGlwKBgQDXSXmdUEQQbmoYVbdJeOdBINVP1bsityUt
DWFVVmYbCdn92iuAedYW2kXWczmYDNiOVMaD+a2Z0lPe9MciNvKjtHNX0zwA/+Mq
8QTllN1ONSRlM7KV15Q90SjdK2sQJ0XRQjdEedf/KjHmOb/efOkC2jDXIUJVnSIV
Qt6py5MNFwKBgDsp4M1qDKJUCirZP3DnfbrWzIPjqOS+GpqlI0DqN6b8nqQQYSqH
k/tDHuKu5DXjHxvnGd6lkxzX597GdpZrShHP56f2aARtE40Bs9DRjDxN2RM5MzA1
sHchyJvXGtHIzEeFXl4e1tT7bs9TtJLQikmWHnijhsyxq+OZNpV9eu3zAoGAfZMT
B+qwuPDem7ErxwRpx3hyVC30COzRF3VNh+xshGr7p3GhnD/028GXqN3vAzzC+EqE
vKhfVXD0kQEwQknQwCJagSjqQB8CD6MkWxG5AIxI3WsJSRPFGUWuU0unGHX/6G56
NEvp7KFdF9AplYpAk/RXNrTkr3GHVuV5YYsoDCMCgYBqe5pxNt8BzD572Cd0zhzD
fdaQojW0YOXdGJ54Il7GT6I8PCPK4rmuCSmJ+ZkmoFIJt2rGWMCWPxl8hvkNNTSf
9KQwPmkX1wDQn0tb9vohBK/ytgf2ZC+wF/45dtfSwpPKX36tHBj1IVPGX2sBHHOV
BV4U+8VZCBRnYP8w6+shQg==
-----END PRIVATE KEY-----
)PEM"};

static constexpr std::string_view OIDC_TEST_JWKS{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "xh04Kd7UdF3P3scotj1g3UoEfqmgml7fcBN_sjG4pNBHcdMj4OyU06cXzdXvhx3Tc2B1qyBga8YQUz9jpyyPel-yhdT9wqupqoswpgdAmkWEDYgzSXTiMaoD6g4zVCzla9N-Vjuouzm5N5iuPz7oU0mXoJch0cs8pXWA4sBAywrTo4gZ1yMDB_3Ni4JnqvmVur_7eix4BtMOaQ7LmlIAJqGieg1ebcZoLcvtDdJVVRP5EECo6L8YfLL6P1vL5xRL-raLjU8ua4dOchD9FM0AQcMpmP1PlL1wIL4GzSo2lza0Tq7kKu-5rOGbl8laKRhuIMGIv7fVSo4rK4sLB5vCkQ", "e": "AQAB" } ] })JSON"};

// The verifier and challenge pair from RFC 7636 appendix B, pinning the
// challenge derivation against the specification
static constexpr std::string_view PKCE_VERIFIER{
    "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk"};
static constexpr std::string_view PKCE_CHALLENGE{
    "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM"};

static constexpr std::array<sourcemeta::core::JWSAlgorithm, 1>
    OIDC_TEST_ALGORITHMS{{sourcemeta::core::JWSAlgorithm::RS256}};

static auto test_provider() -> sourcemeta::core::JWKSProvider {
  return sourcemeta::core::JWKSProvider{
      "https://login.test/jwks",
      [](const std::string_view url)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        if (url != "https://login.test/jwks") {
          return std::nullopt;
        }

        return sourcemeta::core::JWKSProvider::FetchResult{
            .body = std::string{OIDC_TEST_JWKS}, .max_age = std::nullopt};
      },
      {}};
}

static auto sign_id_token(const std::string_view payload) -> std::string {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(OIDC_TEST_PRIVATE_KEY)};
  EXPECT_TRUE(key.has_value());
  const auto token{sourcemeta::core::jwt_sign(
      sourcemeta::core::parse_json(R"JSON({ "alg": "RS256" })JSON"),
      sourcemeta::core::parse_json(payload), key.value())};
  EXPECT_TRUE(token.has_value());
  return token.value();
}

TEST(oidc_transaction_mints_unique_url_safe_secrets) {
  const auto first{sourcemeta::one::oidc_transaction()};
  const auto second{sourcemeta::one::oidc_transaction()};

  for (const auto &value : {first.state, first.nonce, first.code_verifier,
                            second.state, second.nonce, second.code_verifier}) {
    EXPECT_EQ(value.size(), 43);
    for (const auto character : value) {
      const auto safe{(character >= 'a' && character <= 'z') ||
                      (character >= 'A' && character <= 'Z') ||
                      (character >= '0' && character <= '9') ||
                      character == '-' || character == '_'};
      EXPECT_TRUE(safe);
    }
  }

  EXPECT_TRUE(first.state != second.state);
  EXPECT_TRUE(first.nonce != second.nonce);
  EXPECT_TRUE(first.code_verifier != second.code_verifier);
  EXPECT_TRUE(first.state != first.nonce);
  EXPECT_TRUE(first.state != first.code_verifier);
}

TEST(oidc_authorization_url_composes_the_provider_redirect) {
  const sourcemeta::one::OIDCTransaction transaction{
      .state = "the state",
      .nonce = "the-nonce",
      .code_verifier = std::string{PKCE_VERIFIER}};
  const auto url{sourcemeta::one::oidc_authorization_url(
      "https://login.test/authorize",
      {.client_id = "registry",
       .client_secret = "hunter2",
       .redirect_uri = "https://schemas.example.com/self/v1/auth/callback/"
                       "okta"},
      "openid email", transaction)};
  EXPECT_EQ(url, "https://login.test/authorize?response_type=code&"
                 "client_id=registry&"
                 "redirect_uri=https%3A%2F%2Fschemas.example.com%2Fself%2Fv1%2F"
                 "auth%2Fcallback%2Fokta&"
                 "scope=openid%20email&"
                 "state=the%20state&"
                 "nonce=the-nonce&"
                 "code_challenge=" +
                     std::string{PKCE_CHALLENGE} +
                     "&code_challenge_method=S256");
}

TEST(oidc_authorization_url_extends_an_existing_query) {
  const sourcemeta::one::OIDCTransaction transaction{
      .state = "abc", .nonce = "def", .code_verifier = "ghi"};
  const auto url{sourcemeta::one::oidc_authorization_url(
      "https://login.test/authorize?tenant=acme",
      {.client_id = "registry",
       .client_secret = "hunter2",
       .redirect_uri = "https://schemas.example.com/callback"},
      "openid", transaction)};
  EXPECT_TRUE(url.starts_with(
      "https://login.test/authorize?tenant=acme&response_type=code&"));
}

TEST(oidc_token_request_composes_the_exchange_body) {
  const auto body{sourcemeta::one::oidc_token_request(
      {.client_id = "registry",
       .client_secret = "hunter 2&more",
       .redirect_uri = "https://schemas.example.com/self/v1/auth/callback/"
                       "okta"},
      "the/code", PKCE_VERIFIER)};
  EXPECT_EQ(body,
            "grant_type=authorization_code&"
            "code=the%2Fcode&"
            "redirect_uri=https%3A%2F%2Fschemas.example.com%2Fself%2Fv1%2F"
            "auth%2Fcallback%2Fokta&"
            "client_id=registry&"
            "client_secret=hunter%202%26more&"
            "code_verifier=dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk");
}

TEST(oidc_parse_token_response_reads_the_identity_token) {
  const auto id_token{sourcemeta::one::oidc_parse_token_response(
      R"JSON({ "access_token": "xyz", "token_type": "Bearer", "id_token": "aaa.bbb.ccc" })JSON")};
  EXPECT_TRUE(id_token.has_value());
  EXPECT_EQ(id_token.value(), "aaa.bbb.ccc");
}

TEST(oidc_parse_token_response_denies_malformed_responses) {
  EXPECT_FALSE(sourcemeta::one::oidc_parse_token_response("").has_value());
  EXPECT_FALSE(
      sourcemeta::one::oidc_parse_token_response("not json").has_value());
  EXPECT_FALSE(
      sourcemeta::one::oidc_parse_token_response("[ 1, 2 ]").has_value());
  EXPECT_FALSE(sourcemeta::one::oidc_parse_token_response(
                   R"JSON({ "access_token": "xyz" })JSON")
                   .has_value());
  EXPECT_FALSE(sourcemeta::one::oidc_parse_token_response(
                   R"JSON({ "id_token": 42 })JSON")
                   .has_value());
}

TEST(oidc_validate_accepts_a_valid_identity_token) {
  auto provider{test_provider()};
  const auto token{sign_id_token(R"JSON({
    "iss": "https://login.test",
    "aud": "registry",
    "exp": 2000000000,
    "nonce": "the-nonce",
    "sub": "jane@acme.test"
  })JSON")};

  const auto identity{sourcemeta::one::oidc_validate(
      provider, token, OIDC_TEST_ALGORITHMS, "https://login.test",
      {.client_id = "registry"}, "the-nonce")};
  EXPECT_TRUE(identity.has_value());
  EXPECT_EQ(identity.value().subject, "jane@acme.test");
}

TEST(oidc_validate_binds_the_token_to_the_transaction) {
  auto provider{test_provider()};

  // The right nonce admits, any other nonce or its absence denies
  const auto token{sign_id_token(R"JSON({
    "iss": "https://login.test",
    "aud": "registry",
    "exp": 2000000000,
    "nonce": "the-nonce",
    "sub": "jane@acme.test"
  })JSON")};
  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, token, OIDC_TEST_ALGORITHMS, "https://login.test",
                   {.client_id = "registry"}, "another-nonce")
                   .has_value());
  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, token, OIDC_TEST_ALGORITHMS, "https://login.test",
                   {.client_id = "registry"}, "")
                   .has_value());

  const auto nonceless{sign_id_token(R"JSON({
    "iss": "https://login.test",
    "aud": "registry",
    "exp": 2000000000,
    "sub": "jane@acme.test"
  })JSON")};
  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, nonceless, OIDC_TEST_ALGORITHMS,
                   "https://login.test", {.client_id = "registry"}, "the-nonce")
                   .has_value());
}

TEST(oidc_validate_denies_a_token_for_another_issuer_or_client) {
  auto provider{test_provider()};
  const auto token{sign_id_token(R"JSON({
    "iss": "https://login.test",
    "aud": "registry",
    "exp": 2000000000,
    "nonce": "the-nonce",
    "sub": "jane@acme.test"
  })JSON")};

  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, token, OIDC_TEST_ALGORITHMS, "https://other.test",
                   {.client_id = "registry"}, "the-nonce")
                   .has_value());
  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, token, OIDC_TEST_ALGORITHMS, "https://login.test",
                   {.client_id = "dashboard"}, "the-nonce")
                   .has_value());
}

TEST(oidc_validate_denies_expired_and_forged_tokens) {
  auto provider{test_provider()};

  const auto expired{sign_id_token(R"JSON({
    "iss": "https://login.test",
    "aud": "registry",
    "exp": 1000,
    "nonce": "the-nonce",
    "sub": "jane@acme.test"
  })JSON")};
  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, expired, OIDC_TEST_ALGORITHMS,
                   "https://login.test", {.client_id = "registry"}, "the-nonce")
                   .has_value());

  auto tampered{sign_id_token(R"JSON({
    "iss": "https://login.test",
    "aud": "registry",
    "exp": 2000000000,
    "nonce": "the-nonce",
    "sub": "jane@acme.test"
  })JSON")};
  tampered.back() = tampered.back() == 'A' ? 'B' : 'A';
  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, tampered, OIDC_TEST_ALGORITHMS,
                   "https://login.test", {.client_id = "registry"}, "the-nonce")
                   .has_value());

  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, "not a token", OIDC_TEST_ALGORITHMS,
                   "https://login.test", {.client_id = "registry"}, "the-nonce")
                   .has_value());
}

TEST(oidc_validate_requires_a_subject) {
  auto provider{test_provider()};
  const auto token{sign_id_token(R"JSON({
    "iss": "https://login.test",
    "aud": "registry",
    "exp": 2000000000,
    "nonce": "the-nonce"
  })JSON")};
  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, token, OIDC_TEST_ALGORITHMS, "https://login.test",
                   {.client_id = "registry"}, "the-nonce")
                   .has_value());

  const auto blank{sign_id_token(R"JSON({
    "iss": "https://login.test",
    "aud": "registry",
    "exp": 2000000000,
    "nonce": "the-nonce",
    "sub": ""
  })JSON")};
  EXPECT_FALSE(sourcemeta::one::oidc_validate(
                   provider, blank, OIDC_TEST_ALGORITHMS, "https://login.test",
                   {.client_id = "registry"}, "the-nonce")
                   .has_value());
}
