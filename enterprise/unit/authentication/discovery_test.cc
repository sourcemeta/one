#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/test.h>

TEST(discovery_url_appends_the_well_known_path) {
  EXPECT_EQ(sourcemeta::one::discovery_url("https://acme.test"),
            "https://acme.test/.well-known/openid-configuration");
}

TEST(discovery_url_strips_a_trailing_slash) {
  EXPECT_EQ(sourcemeta::one::discovery_url("https://acme.test/"),
            "https://acme.test/.well-known/openid-configuration");
}

TEST(discovery_url_of_an_empty_issuer) {
  EXPECT_EQ(sourcemeta::one::discovery_url(""),
            "/.well-known/openid-configuration");
}

TEST(discovery_parse_reads_every_endpoint) {
  const auto document{sourcemeta::one::discovery_parse(R"JSON({
    "jwks_uri": "https://acme.test/keys",
    "authorization_endpoint": "https://acme.test/authorize",
    "token_endpoint": "https://acme.test/token",
    "userinfo_endpoint": "https://acme.test/userinfo"
  })JSON")};
  EXPECT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().jwks_uri.has_value());
  EXPECT_EQ(document.value().jwks_uri.value(), "https://acme.test/keys");
  EXPECT_TRUE(document.value().authorization_endpoint.has_value());
  EXPECT_EQ(document.value().authorization_endpoint.value(),
            "https://acme.test/authorize");
  EXPECT_TRUE(document.value().token_endpoint.has_value());
  EXPECT_EQ(document.value().token_endpoint.value(), "https://acme.test/token");
  EXPECT_TRUE(document.value().userinfo_endpoint.has_value());
  EXPECT_EQ(document.value().userinfo_endpoint.value(),
            "https://acme.test/userinfo");
}

TEST(discovery_parse_reads_a_document_with_only_a_key_set_location) {
  const auto document{sourcemeta::one::discovery_parse(R"JSON({
    "jwks_uri": "https://acme.test/keys"
  })JSON")};
  EXPECT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().jwks_uri.has_value());
  EXPECT_EQ(document.value().jwks_uri.value(), "https://acme.test/keys");
  EXPECT_FALSE(document.value().authorization_endpoint.has_value());
  EXPECT_FALSE(document.value().token_endpoint.has_value());
  EXPECT_FALSE(document.value().userinfo_endpoint.has_value());
}

TEST(discovery_parse_reads_an_empty_document) {
  const auto document{sourcemeta::one::discovery_parse("{}")};
  EXPECT_TRUE(document.has_value());
  EXPECT_FALSE(document.value().jwks_uri.has_value());
  EXPECT_FALSE(document.value().authorization_endpoint.has_value());
  EXPECT_FALSE(document.value().token_endpoint.has_value());
  EXPECT_FALSE(document.value().userinfo_endpoint.has_value());
}

TEST(discovery_parse_ignores_a_non_string_endpoint) {
  const auto document{sourcemeta::one::discovery_parse(R"JSON({
    "jwks_uri": 1,
    "authorization_endpoint": [ "https://acme.test/authorize" ],
    "token_endpoint": null,
    "userinfo_endpoint": "https://acme.test/userinfo"
  })JSON")};
  EXPECT_TRUE(document.has_value());
  EXPECT_FALSE(document.value().jwks_uri.has_value());
  EXPECT_FALSE(document.value().authorization_endpoint.has_value());
  EXPECT_FALSE(document.value().token_endpoint.has_value());
  EXPECT_TRUE(document.value().userinfo_endpoint.has_value());
  EXPECT_EQ(document.value().userinfo_endpoint.value(),
            "https://acme.test/userinfo");
}

TEST(discovery_parse_denies_a_document_that_is_not_an_object) {
  EXPECT_FALSE(sourcemeta::one::discovery_parse("[]").has_value());
  EXPECT_FALSE(
      sourcemeta::one::discovery_parse("\"https://acme.test\"").has_value());
  EXPECT_FALSE(sourcemeta::one::discovery_parse("null").has_value());
}

TEST(discovery_parse_denies_a_document_that_is_not_json) {
  EXPECT_FALSE(sourcemeta::one::discovery_parse("").has_value());
  EXPECT_FALSE(sourcemeta::one::discovery_parse("<html></html>").has_value());
  EXPECT_FALSE(sourcemeta::one::discovery_parse("{").has_value());
}
