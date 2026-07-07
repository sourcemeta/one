#include <sourcemeta/one/authentication_session.h>

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

namespace {

// The version prefix lets the sealed format evolve without a value produced
// under one format ever verifying under another
constexpr std::string_view SESSION_VERSION{"1"};

constexpr std::size_t SESSION_SIGNATURE_LENGTH{32};

// The canonical unpadded encoding of a signature has exactly one length, so
// anything else is rejected before it is even decoded
constexpr std::size_t SESSION_SIGNATURE_ENCODED_LENGTH{
    ((SESSION_SIGNATURE_LENGTH * 4) + 2) / 3};

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

auto session_seal(const std::string_view payload, const std::string_view secret,
                  const std::chrono::sys_seconds expiry) -> std::string {
  std::string result{SESSION_VERSION};
  result += '.';
  // A pre-epoch expiry is clamped so that every sealed value is well-formed,
  // remaining expired from the moment it is produced
  result += std::to_string(std::max(std::chrono::seconds::rep{0},
                                    expiry.time_since_epoch().count()));
  result += '.';
  result += sourcemeta::core::base64url_encode(payload);
  const auto digest{sourcemeta::core::hmac_sha256_digest(secret, result)};
  result += '.';
  result += sourcemeta::core::base64url_encode(digest_view(digest));
  return result;
}

auto session_open(const std::string_view value,
                  const std::span<const std::string_view> secrets,
                  const std::chrono::sys_seconds now)
    -> std::optional<std::string> {
  const auto version_end{value.find('.')};
  if (version_end == std::string_view::npos) {
    return std::nullopt;
  }

  const auto expiry_end{value.find('.', version_end + 1)};
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

  const auto expiry{parse_expiry(
      value.substr(version_end + 1, expiry_end - version_end - 1))};
  if (!expiry.has_value()) {
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
    const auto digest{
        sourcemeta::core::hmac_sha256_digest(secret, signing_input)};
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
