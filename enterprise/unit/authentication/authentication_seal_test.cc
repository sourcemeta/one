#include "authentication_helpers.h"

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

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

static auto field_end(const std::string_view value, const Field field)
    -> std::size_t {
  const auto end{value.find('.', field_start(value, field))};
  return end == std::string_view::npos ? value.size() : end;
}

// Shares nothing with the set a value is minted under, so a reader holding it
// holds no secret that value was sealed with
static const std::array<std::string_view, 1> SEAL_SECRETS_FOREIGN{
    {"ONE_TEST_SEAL_FOREIGN"}};

static const std::array<std::string_view, 2> SEAL_SECRETS_ROTATED{
    {"ONE_TEST_SEAL_ROTATED", "ONE_TEST_SEAL_SECRET"}};
static const std::array<std::string_view, 1> SEAL_SECRETS{
    {"ONE_TEST_SEAL_SECRET"}};
// Change one character of one field, which is the smallest disturbance that
// should cost a value its signature.
//
// The two instants are read as numbers, and the interval they name is bounded
// against the clock, before any signature is verified. So a disturbance that
// leaves one of them unreadable, or moves one of them by the decades that its
// leading digit is worth, is refused for naming an interval nothing could have
// minted, and the signature covering that field goes untested while the case
// still passes. Their last digit is the one that leaves the value as readable
// and as temporally sound as it was, so that the signature is all that is left
// to refuse it
static auto disturb(const std::string_view value, const Field field)
    -> std::string {
  const auto instant{field == Field::Issued || field == Field::Expiry};
  std::string result{value};
  auto &character{result[instant ? field_end(value, field) - 1
                                 : field_start(value, field)]};
  if (character >= '0' && character <= '9') {
    character = (character == '9' ? '8' : static_cast<char>(character + 1));
  } else {
    character = (character == 'a' ? 'b' : 'a');
  }

  return result;
}

TEST(session_round_trips_what_a_login_sealed) {
  const auto authentication{instance("seal_roundtrip", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
}

TEST(session_value_is_cookie_safe) {
  const auto authentication{instance("seal_cookie_safe", SEAL_SECRETS)};
  const auto started{start(authentication)};
  for (const auto character : started.sealed) {
    EXPECT_TRUE(character > 0x20 && character < 0x7f && character != '"' &&
                character != ',' && character != ';' && character != '\\');
  }
}

TEST(session_value_has_the_shape_the_tests_below_assume) {
  const auto authentication{instance("seal_shape", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_EQ(std::ranges::count(started.sealed, '.'), 4);
  EXPECT_EQ(started.sealed.front(), '1');
}

TEST(session_denies_a_tampered_version) {
  const auto authentication{instance("seal_version", SEAL_SECRETS)};
  const auto started{start(authentication)};
  // The control is the value it was made from, which opens
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(
      opens(authentication, started, disturb(started.sealed, Field::Version)));
}

TEST(session_denies_a_tampered_issuance) {
  const auto authentication{instance("seal_issuance", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(
      opens(authentication, started, disturb(started.sealed, Field::Issued)));
}

TEST(session_denies_a_tampered_expiry) {
  const auto authentication{instance("seal_expiry", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(
      opens(authentication, started, disturb(started.sealed, Field::Expiry)));
}

TEST(session_denies_a_tampered_payload) {
  const auto authentication{instance("seal_payload", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(
      opens(authentication, started, disturb(started.sealed, Field::Payload)));
}

TEST(session_denies_a_tampered_signature) {
  const auto authentication{instance("seal_signature", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started,
                     disturb(started.sealed, Field::Signature)));
}

TEST(session_denies_a_truncated_signature) {
  const auto authentication{instance("seal_truncated", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started,
                     started.sealed.substr(0, started.sealed.size() - 1)));
}

TEST(session_denies_a_lengthened_signature) {
  const auto authentication{instance("seal_lengthened", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started, started.sealed + "a"));
}

TEST(session_denies_a_transplanted_signature) {
  const auto authentication{instance("seal_transplant", SEAL_SECRETS)};
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
  const auto authentication{instance("seal_malformed", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started, ""));
  EXPECT_FALSE(opens(authentication, started, "1"));
  EXPECT_FALSE(opens(authentication, started, "1.2.3.4"));
  EXPECT_FALSE(opens(authentication, started, "not-a-sealed-value"));
  EXPECT_FALSE(opens(authentication, started, "1....."));
}

TEST(session_denies_a_signature_that_is_not_base64url) {
  const auto authentication{instance("seal_base64url", SEAL_SECRETS)};
  const auto started{start(authentication)};
  std::string altered{
      started.sealed.substr(0, field_start(started.sealed, Field::Signature))};
  altered += "!!!!";
  EXPECT_TRUE(opens(authentication, started, started.sealed));
  EXPECT_FALSE(opens(authentication, started, altered));
}

TEST(session_denies_a_wrong_secret) {
  const auto minting{instance("seal_wrong_minting", SEAL_SECRETS)};
  const auto started{start(minting)};

  // A reader holding no secret the value was sealed under, which is the one
  // thing that differs. Its secret set shares nothing with the minting one, so
  // that accepting the value could only ever mean the signature went unchecked
  const auto reading{instance("seal_wrong_reading", SEAL_SECRETS_FOREIGN)};

  // Both are read against the state the reader's own login sealed, so the
  // control and the case differ in the secret and in nothing else
  const auto theirs{start(reading)};
  EXPECT_TRUE(opens(reading, theirs, theirs.sealed));
  EXPECT_FALSE(opens(reading, theirs, started.sealed));

  // And the same value opens where the secret is held, which is what shows the
  // refusal above came from the secret rather than from anything else about it
  EXPECT_TRUE(opens(minting, started, started.sealed));
}

TEST(session_admits_a_value_sealed_under_an_older_secret) {
  const auto minting{instance("seal_old", SEAL_SECRETS)};
  const auto started{start(minting)};

  // The newest secret leads and the one that sealed this follows, which is what
  // lets a secret be replaced without ending what it signed
  const auto rotated{instance("seal_new", SEAL_SECRETS_ROTATED)};
  EXPECT_TRUE(opens(rotated, started, started.sealed));
}

TEST(session_denies_a_value_sealed_under_another_policy_name) {
  const auto authentication{instance("seal_policy", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));

  // The same value offered under a name it was not sealed for, which is what a
  // key derived per policy has to refuse
  EXPECT_FALSE(
      opens(authentication, started, started.sealed, {.policy = "unknown"}));
}

TEST(session_denies_a_state_the_provider_did_not_echo) {
  const auto authentication{instance("seal_state", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_TRUE(opens(authentication, started, started.sealed));

  // The value opens, and is still refused, since what it sealed and what came
  // back do not agree. That is what stops a callback assembled elsewhere
  EXPECT_FALSE(opens(authentication, started, started.sealed,
                     {.state = "not-the-state-it-sealed"}));
}

TEST(session_denies_a_callback_carrying_no_transaction) {
  const auto authentication{instance("seal_absent", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_FALSE(
      opens(authentication, started, started.sealed, {.carried = false}));
}

TEST(session_reads_every_value_a_request_carried) {
  const auto authentication{instance("seal_several", SEAL_SECRETS)};
  const auto started{start(authentication)};

  // A parent domain and the host itself can each set one, and neither the
  // header nor the order says which is which, so every value is tried rather
  // than the first. Letting whoever set the other one decide would turn the
  // cookie from a defence into the way past it
  EXPECT_TRUE(opens(authentication, started, started.sealed,
                    {.shadow = "somebody-elses-value"}));
}

// A login tells the provider where to come back to, and the transaction it
// seals carries that. A callback naming somewhere else is completing a
// different login, so it is refused before a code is redeemed rather than left
// for the provider to catch on the exchange
TEST(session_denies_a_callback_naming_another_return_address) {
  const auto authentication{instance("seal_redirect", SEAL_SECRETS)};
  const auto started{start(authentication)};
  EXPECT_FALSE(opens(authentication, started, started.sealed,
                     {.redirect = "https://registry.test/somewhere/else"}));

  // The control differs in that alone
  EXPECT_TRUE(opens(authentication, started, started.sealed));
}
