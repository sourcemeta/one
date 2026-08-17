#include <sourcemeta/one/authentication.h>

#include "session.h"

#include "session.h"

#include <sourcemeta/core/crypto.h>

#include <algorithm>   // std::max
#include <array>       // std::array
#include <charconv>    // std::from_chars, std::errc
#include <chrono>      // std::chrono::sys_seconds, std::chrono::seconds
#include <cstddef>     // std::size_t
#include <cstdint>     // std::int64_t, std::uint64_t, std::uint8_t
#include <limits>      // std::numeric_limits
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace {

// The version prefix lets the sealed format evolve without a value produced
// under one format ever verifying under another
constexpr std::string_view SESSION_VERSION{"1"};

constexpr std::size_t SESSION_SIGNATURE_LENGTH{32};

// A sealed value carries the instant it was minted alongside the instant it
// stops being honoured, so a lifetime is a fact about the value rather than
// whatever its expiry happens to claim. Nothing this system mints lives
// anywhere near this long, and refusing more bounds what a leaked secret is
// worth: a forged value cannot outlive this no matter what expiry it names
constexpr std::chrono::seconds MAXIMUM_LIFETIME{std::chrono::hours{24}};

// Replicas seal and open each other's values, so a clock that reads slightly
// ahead of the one opening must not produce values nobody accepts yet
constexpr std::chrono::seconds CLOCK_SKEW{60};

// The canonical unpadded encoding of a signature has exactly one length, so
// anything else is rejected before it is even decoded
constexpr std::size_t SESSION_SIGNATURE_ENCODED_LENGTH{
    ((SESSION_SIGNATURE_LENGTH * 4) + 2) / 3};

// A value is only ever opened for the purpose it was sealed under, and the
// cookie name that would otherwise distinguish the two travels outside the
// signature where a client chooses it. So the purpose picks the key rather
// than being asserted alongside it: a value sealed for one purpose cannot
// verify under another's key, whether or not a caller remembers to check
auto purpose_label(const sourcemeta::one::Authentication::Purpose purpose)
    -> std::string_view {
  switch (purpose) {
    case sourcemeta::one::Authentication::Purpose::Session:
      return "sourcemeta/one/session";
    case sourcemeta::one::Authentication::Purpose::Transaction:
      return "sourcemeta/one/transaction";
  }

  std::unreachable();
}

auto derive_key(const std::string_view secret,
                const sourcemeta::one::Authentication::Purpose purpose)
    -> std::array<std::uint8_t, 32> {
  return sourcemeta::core::hmac_sha256_digest(secret, purpose_label(purpose));
}

auto digest_view(const std::array<std::uint8_t, 32> &digest)
    -> std::string_view {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return {reinterpret_cast<const char *>(digest.data()), digest.size()};
}

auto parse_expiry(const std::string_view input)
    -> std::optional<std::chrono::sys_seconds> {
  if (input.empty()) {
    return std::nullopt;
  }

  std::uint64_t count{0};
  const auto *begin{input.data()};
  const auto *end{input.data() + input.size()};
  const auto result{std::from_chars(begin, end, count)};
  if (result.ec != std::errc{} || result.ptr != end ||
      count > static_cast<std::uint64_t>(
                  std::numeric_limits<std::int64_t>::max())) {
    return std::nullopt;
  }

  return std::chrono::sys_seconds{
      std::chrono::seconds{static_cast<std::int64_t>(count)}};
}

} // namespace

namespace sourcemeta::one {

auto seal_value(const std::string_view payload,
                const Authentication::Purpose purpose,
                const std::string_view secret,
                const std::chrono::sys_seconds issued,
                const std::chrono::sys_seconds expiry) -> std::string {
  const auto key{derive_key(secret, purpose)};
  std::string result{SESSION_VERSION};
  result += '.';
  result += std::to_string(std::max(std::chrono::seconds::rep{0},
                                    issued.time_since_epoch().count()));
  result += '.';
  // A pre-epoch expiry is clamped so that every sealed value is well-formed,
  // remaining expired from the moment it is produced
  result += std::to_string(std::max(std::chrono::seconds::rep{0},
                                    expiry.time_since_epoch().count()));
  result += '.';
  result += sourcemeta::core::base64url_encode(payload);
  const auto digest{
      sourcemeta::core::hmac_sha256_digest(digest_view(key), result)};
  result += '.';
  result += sourcemeta::core::base64url_encode(digest_view(digest));
  return result;
}

auto open_value(const std::string_view value,
                const Authentication::Purpose purpose,
                const std::span<const std::string_view> secrets,
                const std::chrono::sys_seconds now)
    -> std::optional<std::string> {
  // Expiry comparisons are meaningless under a clock that reads before the
  // epoch, so such a clock validates nothing
  if (now.time_since_epoch().count() < 0) {
    return std::nullopt;
  }

  const auto version_end{value.find('.')};
  if (version_end == std::string_view::npos) {
    return std::nullopt;
  }

  const auto issued_end{value.find('.', version_end + 1)};
  if (issued_end == std::string_view::npos) {
    return std::nullopt;
  }

  const auto expiry_end{value.find('.', issued_end + 1)};
  if (expiry_end == std::string_view::npos) {
    return std::nullopt;
  }

  const auto payload_end{value.find('.', expiry_end + 1)};
  if (payload_end == std::string_view::npos) {
    return std::nullopt;
  }

  const auto signature{value.substr(payload_end + 1)};
  if (signature.find('.') != std::string_view::npos) {
    return std::nullopt;
  }

  if (value.substr(0, version_end) != SESSION_VERSION) {
    return std::nullopt;
  }

  const auto issued{parse_expiry(
      value.substr(version_end + 1, issued_end - version_end - 1))};
  if (!issued.has_value()) {
    return std::nullopt;
  }

  const auto expiry{
      parse_expiry(value.substr(issued_end + 1, expiry_end - issued_end - 1))};
  if (!expiry.has_value()) {
    return std::nullopt;
  }

  // A value that claims to outlive anything this system mints was not minted
  // by it, whatever signature it carries, and one that expires before it was
  // issued names no interval at all. Bounding the interval is only worth
  // anything alongside refusing one issued in the future, since a lifetime
  // measured from an instant that has not arrived would otherwise start
  // whenever its holder chose
  if (expiry.value() < issued.value() ||
      expiry.value() - issued.value() > MAXIMUM_LIFETIME ||
      issued.value() > now + CLOCK_SKEW) {
    return std::nullopt;
  }

  if (signature.size() != SESSION_SIGNATURE_ENCODED_LENGTH) {
    return std::nullopt;
  }

  const auto presented{sourcemeta::core::base64url_decode(signature)};
  if (!presented.has_value() ||
      presented.value().size() != SESSION_SIGNATURE_LENGTH) {
    return std::nullopt;
  }

  // The signature covers everything before it, so the expiry and the payload
  // are bound together and neither can be transplanted from another value
  const auto signing_input{value.substr(0, payload_end)};
  auto authentic{false};
  for (const auto secret : secrets) {
    const auto key{derive_key(secret, purpose)};
    const auto digest{
        sourcemeta::core::hmac_sha256_digest(digest_view(key), signing_input)};
    if (sourcemeta::core::secure_equals(digest_view(digest),
                                        presented.value())) {
      authentic = true;
      break;
    }
  }

  if (!authentic) {
    return std::nullopt;
  }

  // A value is valid strictly before its expiry instant
  if (now >= expiry.value()) {
    return std::nullopt;
  }

  return sourcemeta::core::base64url_decode(
      value.substr(expiry_end + 1, payload_end - expiry_end - 1));
}

} // namespace sourcemeta::one
