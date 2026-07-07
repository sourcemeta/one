#include <sourcemeta/one/authentication_session.h>

#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::sys_seconds, std::chrono::seconds
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::chrono::sys_seconds NOW{std::chrono::seconds{1000000}};
static constexpr std::chrono::sys_seconds LATER{std::chrono::seconds{1000060}};

static const std::array<std::string_view, 1> SECRETS{{"session-secret"}};

TEST(session_round_trips_a_payload) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  const auto payload{sourcemeta::one::session_open(value, SECRETS, NOW)};
  EXPECT_TRUE(payload.has_value());
  EXPECT_EQ(payload.value(), "the-payload");
}

TEST(session_round_trips_an_empty_payload) {
  const auto value{sourcemeta::one::session_seal("", "session-secret", LATER)};
  const auto payload{sourcemeta::one::session_open(value, SECRETS, NOW)};
  EXPECT_TRUE(payload.has_value());
  EXPECT_EQ(payload.value(), "");
}

TEST(session_round_trips_a_payload_containing_separators) {
  const auto value{sourcemeta::one::session_seal(
      "left.right.{\"claims\":[1,2]}\n", "session-secret", LATER)};
  const auto payload{sourcemeta::one::session_open(value, SECRETS, NOW)};
  EXPECT_TRUE(payload.has_value());
  EXPECT_EQ(payload.value(), "left.right.{\"claims\":[1,2]}\n");
}

TEST(session_value_is_cookie_safe) {
  const auto value{sourcemeta::one::session_seal(
      "payload with spaces; and = signs", "session-secret", LATER)};
  for (const auto character : value) {
    const auto safe{(character >= 'a' && character <= 'z') ||
                    (character >= 'A' && character <= 'Z') ||
                    (character >= '0' && character <= '9') ||
                    character == '.' || character == '-' || character == '_'};
    EXPECT_TRUE(safe);
  }
}

TEST(session_denies_at_and_after_the_expiry_instant) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  EXPECT_TRUE(sourcemeta::one::session_open(value, SECRETS, NOW).has_value());
  EXPECT_FALSE(
      sourcemeta::one::session_open(value, SECRETS, LATER).has_value());
  const std::chrono::sys_seconds after{LATER + std::chrono::seconds{1}};
  EXPECT_FALSE(
      sourcemeta::one::session_open(value, SECRETS, after).has_value());
}

TEST(session_denies_a_wrong_secret) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "other-secret", LATER)};
  EXPECT_FALSE(sourcemeta::one::session_open(value, SECRETS, NOW).has_value());
}

TEST(session_denies_with_no_secrets) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  const std::array<std::string_view, 0> no_secrets{};
  EXPECT_FALSE(
      sourcemeta::one::session_open(value, no_secrets, NOW).has_value());
}

TEST(session_admits_a_value_sealed_under_an_older_secret) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "old-secret", LATER)};
  const std::array<std::string_view, 2> rotated{{"new-secret", "old-secret"}};
  const auto payload{sourcemeta::one::session_open(value, rotated, NOW)};
  EXPECT_TRUE(payload.has_value());
  EXPECT_EQ(payload.value(), "the-payload");
  const std::array<std::string_view, 1> retired{{"new-secret"}};
  EXPECT_FALSE(sourcemeta::one::session_open(value, retired, NOW).has_value());
}

TEST(session_denies_a_tampered_payload) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  auto tampered{value};
  const auto payload_start{tampered.find('.', tampered.find('.') + 1) + 1};
  tampered[payload_start] = tampered[payload_start] == 'A' ? 'B' : 'A';
  EXPECT_FALSE(
      sourcemeta::one::session_open(tampered, SECRETS, NOW).has_value());
}

TEST(session_denies_a_tampered_expiry) {
  const auto expired{
      sourcemeta::one::session_seal("the-payload", "session-secret", NOW)};
  // Extending the lifetime of an expired value must break its signature
  const auto expiry_start{expired.find('.') + 1};
  auto tampered{expired};
  tampered.insert(expiry_start, "9");
  EXPECT_FALSE(
      sourcemeta::one::session_open(tampered, SECRETS, NOW).has_value());
}

TEST(session_denies_a_tampered_version) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  auto tampered{value};
  tampered[0] = '2';
  EXPECT_FALSE(
      sourcemeta::one::session_open(tampered, SECRETS, NOW).has_value());
}

TEST(session_denies_a_transplanted_signature) {
  const auto first{
      sourcemeta::one::session_seal("first-payload", "session-secret", LATER)};
  const auto second{
      sourcemeta::one::session_seal("second-payload", "session-secret", LATER)};
  const auto first_body{first.substr(0, first.rfind('.'))};
  const auto second_signature{second.substr(second.rfind('.') + 1)};
  const auto spliced{first_body + "." + second_signature};
  EXPECT_FALSE(
      sourcemeta::one::session_open(spliced, SECRETS, NOW).has_value());
}

TEST(session_denies_a_truncated_signature) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  const auto truncated{value.substr(0, value.size() - 2)};
  EXPECT_FALSE(
      sourcemeta::one::session_open(truncated, SECRETS, NOW).has_value());
}

TEST(session_denies_a_lengthened_signature) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  const auto lengthened{value + "AA"};
  EXPECT_FALSE(
      sourcemeta::one::session_open(lengthened, SECRETS, NOW).has_value());
}

TEST(session_denies_a_value_sealed_with_a_pre_epoch_expiry) {
  const std::chrono::sys_seconds before_epoch{std::chrono::seconds{-1}};
  const auto value{sourcemeta::one::session_seal(
      "the-payload", "session-secret", before_epoch)};
  EXPECT_FALSE(sourcemeta::one::session_open(value, SECRETS, NOW).has_value());
  EXPECT_FALSE(
      sourcemeta::one::session_open(value, SECRETS, before_epoch).has_value());
}

TEST(session_denies_everything_under_a_pre_epoch_clock) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  const std::chrono::sys_seconds before_epoch{std::chrono::seconds{-1}};
  EXPECT_FALSE(
      sourcemeta::one::session_open(value, SECRETS, before_epoch).has_value());
}

TEST(session_denies_malformed_values) {
  EXPECT_FALSE(sourcemeta::one::session_open("", SECRETS, NOW).has_value());
  EXPECT_FALSE(sourcemeta::one::session_open(".", SECRETS, NOW).has_value());
  EXPECT_FALSE(sourcemeta::one::session_open("...", SECRETS, NOW).has_value());
  EXPECT_FALSE(sourcemeta::one::session_open("no-dots-at-all", SECRETS, NOW)
                   .has_value());
  EXPECT_FALSE(
      sourcemeta::one::session_open("1.123.AA", SECRETS, NOW).has_value());
  EXPECT_FALSE(sourcemeta::one::session_open("1.123.AA.BB.CC", SECRETS, NOW)
                   .has_value());
}

TEST(session_denies_a_malformed_expiry) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  const auto expiry_start{value.find('.') + 1};
  const auto expiry_end{value.find('.', expiry_start)};

  auto empty_expiry{value};
  empty_expiry.erase(expiry_start, expiry_end - expiry_start);
  EXPECT_FALSE(
      sourcemeta::one::session_open(empty_expiry, SECRETS, NOW).has_value());

  auto textual_expiry{value};
  textual_expiry.replace(expiry_start, expiry_end - expiry_start, "later");
  EXPECT_FALSE(
      sourcemeta::one::session_open(textual_expiry, SECRETS, NOW).has_value());

  auto negative_expiry{value};
  negative_expiry.replace(expiry_start, expiry_end - expiry_start, "-1");
  EXPECT_FALSE(
      sourcemeta::one::session_open(negative_expiry, SECRETS, NOW).has_value());

  auto overflowing_expiry{value};
  overflowing_expiry.replace(expiry_start, expiry_end - expiry_start,
                             "99999999999999999999999999");
  EXPECT_FALSE(sourcemeta::one::session_open(overflowing_expiry, SECRETS, NOW)
                   .has_value());
}

TEST(session_denies_a_signature_that_is_not_base64url) {
  const auto value{
      sourcemeta::one::session_seal("the-payload", "session-secret", LATER)};
  const auto body{value.substr(0, value.rfind('.'))};
  const auto invalid{body + ".!!!not-base64url!!!"};
  EXPECT_FALSE(
      sourcemeta::one::session_open(invalid, SECRETS, NOW).has_value());
}
