#ifndef SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_SESSION_H_
#define SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_SESSION_H_

#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>

#include <algorithm> // std::max
#include <array>     // std::array
#include <charconv>  // std::from_chars, std::errc
#include <chrono>    // std::chrono::sys_seconds, std::chrono::seconds
#include <cstddef>   // std::size_t
#include <cstdint>   // std::int64_t, std::uint64_t, std::uint8_t
#include <limits>    // std::numeric_limits
#include <optional>  // std::optional, std::nullopt
#include <sourcemeta/core/jose.h>
#include <span>        // std::span
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

// The sealing primitive the sessions and login transactions of this module are
// built out of. It sits beside the sources that use it rather than under the
// installed headers, so nothing outside this directory can reach it, and the
// interface next door stays what a consumer may call
namespace sourcemeta::one {

// What a sealed value is for. A value is only ever opened for the purpose it
// was sealed under, because the two derive different keys from the policy's
// secret, so one kind of value cannot be presented as the other
enum class SealPurpose : std::uint8_t { Session = 0, Transaction = 1 };

// The artifact is produced by a trusted indexer, but on-disk truncation or
// corruption could leave a header whose magic and version still match while
// its offsets and counts point out of bounds. Validating the whole layout
// once here keeps the matching hot path free of any per-call bounds checks
// A browser holds one session, whichever policy established it, and the policy
// travels inside the sealed value rather than in the name. One name means
// signing out has a single thing to end, and means a caller cannot choose which
// policy a value is read as by choosing what to call it
inline constexpr std::string_view SESSION_COOKIE{"sourcemeta_one_session"};

// A login transaction follows the same shape for the short window between the
// login redirect and the callback
inline constexpr std::string_view TRANSACTION_COOKIE{
    "sourcemeta_one_transaction"};

// Names the policy a browser last signed in under, so that a denial can ask the
// provider whether that sign-in still stands rather than asking the person
// again. It outlives a session, since it is only of use once one has expired,
// and it carries no credential: whoever holds it can start a login they were
// free to start anyway
inline constexpr std::string_view RENEWAL_COOKIE{"sourcemeta_one_renewal"};

// An instance names an origin and nothing more, so every location it serves is
// below the root, and a cookie scoped there travels to all of them
inline constexpr std::string_view COOKIE_PATH{"/"};

// How long a login has to come back before the transaction that started it
// stops opening
inline constexpr std::chrono::seconds TRANSACTION_LIFETIME{
    std::chrono::minutes{10}};

// The signature algorithms an identity token may be signed with: every
// asymmetric one, since a provider picks from these and an instance that named
// a narrower set would refuse a provider it could otherwise serve, after the
// person had already signed in. The symmetric ones are left out deliberately.
// They sign with the client secret rather than a key from the provider's
// published set, so admitting them alongside the rest is the shape that lets
// one algorithm be verified as though it were another
inline constexpr std::array<sourcemeta::core::JWSAlgorithm, 10>
    ID_TOKEN_ALGORITHMS{{sourcemeta::core::JWSAlgorithm::RS256,
                         sourcemeta::core::JWSAlgorithm::RS384,
                         sourcemeta::core::JWSAlgorithm::RS512,
                         sourcemeta::core::JWSAlgorithm::PS256,
                         sourcemeta::core::JWSAlgorithm::PS384,
                         sourcemeta::core::JWSAlgorithm::PS512,
                         sourcemeta::core::JWSAlgorithm::ES256,
                         sourcemeta::core::JWSAlgorithm::ES384,
                         sourcemeta::core::JWSAlgorithm::ES512,
                         sourcemeta::core::JWSAlgorithm::EdDSA}};

// The tolerance allowed on an identity token's time-based claims, matching what
// a presented access token is already given. A provider whose clock runs a
// little fast otherwise mints a token this refuses the instant it arrives,
// which ends a login that did everything right
inline constexpr std::chrono::seconds ID_TOKEN_CLOCK_SKEW{60};

// A session lasts an hour, kept short so that a lost cookie cannot outlive its
// usefulness, with silent re-authentication as the eventual refresh
inline constexpr std::chrono::seconds SESSION_LIFETIME{3600};

// How long a browser stays eligible for a silent renewal after signing in.
// Long enough to outlast a provider session, since the provider is the one that
// decides whether a renewal succeeds, and losing it early only costs a sign-in
// page that would otherwise have been skipped
inline constexpr std::chrono::seconds RENEWAL_LIFETIME{43200};

// RFC 6265 Section 6.1 asks a user agent to support "at least 4096 bytes per
// cookie (as measured by the sum of the length of the cookie's name, value, and
// attributes)". That is a floor they should honour rather than a ceiling they
// must enforce, and what happens above it is left unsaid, so the whole
// serialised cookie is kept under it with room to spare
inline constexpr std::size_t MAXIMUM_COOKIE_LENGTH{4000};

inline constexpr std::chrono::seconds JWT_CLOCK_SKEW{60};

// The version prefix lets the sealed format evolve without a value produced
// under one format ever verifying under another
inline constexpr std::string_view SESSION_VERSION{"1"};

inline constexpr std::size_t SESSION_SIGNATURE_LENGTH{32};

// A sealed value carries the instant it was minted alongside the instant it
// stops being honoured, so a lifetime is a fact about the value rather than
// whatever its expiry happens to claim. Nothing this system mints lives
// anywhere near this long, and refusing more bounds what a leaked secret is
// worth: a forged value cannot outlive this no matter what expiry it names
inline constexpr std::chrono::seconds MAXIMUM_LIFETIME{std::chrono::hours{24}};

// Replicas seal and open each other's values, so a clock that reads slightly
// ahead of the one opening must not produce values nobody accepts yet
inline constexpr std::chrono::seconds CLOCK_SKEW{60};

// The canonical unpadded encoding of a signature has exactly one length, so
// anything else is rejected before it is even decoded
inline constexpr std::size_t SESSION_SIGNATURE_ENCODED_LENGTH{
    ((SESSION_SIGNATURE_LENGTH * 4) + 2) / 3};

// A value is only ever opened for the purpose it was sealed under, and the
// cookie name that would otherwise distinguish the two travels outside the
// signature where a client chooses it. So the purpose picks the key rather
// than being asserted alongside it: a value sealed for one purpose cannot
// verify under another's key, whether or not a caller remembers to check
inline auto purpose_label(const SealPurpose purpose) -> std::string_view {
  switch (purpose) {
    case SealPurpose::Session:
      return "sourcemeta/one/session";
    case SealPurpose::Transaction:
      return "sourcemeta/one/transaction";
  }

  std::unreachable();
}

inline auto derive_key(const std::string_view secret, const SealPurpose purpose)
    -> std::array<std::uint8_t, 32> {
  return sourcemeta::core::hmac_sha256_digest(secret, purpose_label(purpose));
}

inline auto digest_view(const std::array<std::uint8_t, 32> &digest)
    -> std::string_view {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return {reinterpret_cast<const char *>(digest.data()), digest.size()};
}

inline auto parse_expiry(const std::string_view input)
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

inline auto seal_value(const std::string_view payload,
                       const SealPurpose purpose, const std::string_view secret,
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

inline auto open_value(const std::string_view value, const SealPurpose purpose,
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

#endif
