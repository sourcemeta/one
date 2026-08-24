#ifndef SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_CLAIMS_H_
#define SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_CLAIMS_H_

#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/email.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/text.h>

#include "authentication_format.h"

#include <algorithm>   // std::ranges::find
#include <cstddef>     // std::size_t
#include <cstdlib>     // std::getenv
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

// Whether what a caller presented satisfies what a policy asks of it. Every
// rule a policy can name is read here, and nothing about reaching a provider
// or about who governs what belongs in this file
namespace sourcemeta::one {

// What a policy's rules make of the claims a provider asserted
enum class Admission : std::uint8_t {
  // Every rule holds
  Admitted,
  // A rule names values that what arrived does not carry
  Refused,
  // A rule names a claim absent altogether. A provider answering the
  // authorization code flow returns a scope's claims from its UserInfo endpoint
  // rather than in the token by default, so this is the one outcome worth
  // asking a second question about
  Incomplete
};

// The one place a secret is read. A name that is empty names no variable, and
// a variable set to nothing holds no secret, so neither reaches a caller as a
// value it might compare something against
inline auto resolve_environment(const std::string_view variable)
    -> std::optional<sourcemeta::core::SecureString> {
  if (variable.empty()) {
    return std::nullopt;
  }

  static std::mutex mutex;
  static std::unordered_map<std::string,
                            std::optional<sourcemeta::core::SecureString>>
      cache;
  const std::string name{variable};
  const std::scoped_lock lock{mutex};
  const auto existing{cache.find(name)};
  if (existing != cache.cend()) {
    return existing->second;
  }

  std::optional<sourcemeta::core::SecureString> resolved;
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const char *value{std::getenv(name.c_str())};
  // A variable set to nothing holds no secret, so it reads here the same as
  // one that was never set at all
  if (value != nullptr && *value != '\0') {
    resolved.emplace(std::string_view{value});
  }

  cache.emplace(name, resolved);
  return resolved;
}

inline auto admits_apikey(const std::span<const std::byte> metadata,
                          const std::string_view credential,
                          const Authentication::Algorithm algorithm) -> bool {
  // Presenting nothing is not presenting a credential, whatever any policy
  // happens to hold
  if (credential.empty()) {
    return false;
  }

  std::size_t cursor{0};
  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count)) {
    return false;
  }

  // Identity compares the credential directly with no allocation. Every other
  // algorithm hashes it once, and only then is a string needed to hold the
  // digest the stored value is compared against
  std::string hashed;
  if (algorithm != Authentication::Algorithm::Identity) {
    hashed = sourcemeta::core::sha256(credential);
  }
  const std::string_view subject{algorithm ==
                                         Authentication::Algorithm::Identity
                                     ? credential
                                     : std::string_view{hashed}};

  for (std::uint32_t index{0}; index < count; index += 1) {
    std::uint32_t length{0};
    if (!read_u32(metadata, cursor, length) ||
        metadata.size() - cursor < length) {
      return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view variable{
        reinterpret_cast<const char *>(metadata.data() + cursor), length};
    cursor += length;

    const auto stored{resolve_environment(variable)};
    if (stored.has_value() &&
        sourcemeta::core::secure_equals(subject, stored.value())) {
      return true;
    }
  }

  return false;
}

// The serialised claim rules of a JWT policy, which sit past the algorithms.
// They are read on their own, since the gate parses them once at startup while
// admission itself never touches these bytes again
inline auto read_jwt_claims(const std::span<const std::byte> metadata,
                            std::string_view &result) -> bool {
  std::size_t cursor{0};
  std::string_view discarded;
  if (!read_string(metadata, cursor, discarded) ||
      !read_string(metadata, cursor, discarded) ||
      !read_string(metadata, cursor, discarded) ||
      !read_string(metadata, cursor, discarded)) {
    return false;
  }

  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count) || count > metadata.size() - cursor) {
    return false;
  }

  cursor += count;
  return read_string(metadata, cursor, result);
}

// The one claim whose value is a set rather than a value, which RFC 6749
// Section 3.3 makes a space-delimited, case-sensitive, unordered list.
//
// A rule this cannot read denies rather than being passed over, which is a
// policy choice rather than anything the specifications settle. Passing one
// over would widen the rule to every token carrying any scope, so the nearer a
// rule is to unreadable the more it would admit
inline auto scope_accepts(const sourcemeta::core::JSON &payload,
                          const sourcemeta::core::JSON &request) -> bool {
  return sourcemeta::core::oauth_scope_request_accepts(payload, request) ==
         sourcemeta::core::OAuthScopeDecision::Accepted;
}

// Whether a claim arrived carrying objects rather than the strings a rule
// names. Question 6's reading compares such a claim on its `value`
// sub-attribute alone, so a rule naming a display name matches nothing
inline auto carries_objects(const sourcemeta::core::JSON &value) -> bool {
  if (value.is_object()) {
    return true;
  }

  if (!value.is_array()) {
    return false;
  }

  return std::ranges::any_of(value.as_array(), [](const auto &entry) -> bool {
    return entry.is_object();
  });
}

// Whether a verified token carries every claim a policy requires. The rules are
// the member map of a claims request parameter, so each member names a claim
// and the values it may carry. Values within one rule are alternatives and the
// rules themselves are cumulative, which is what lets a policy widen who it
// admits without also widening what they reach
inline auto admits_claims(const sourcemeta::core::JSON &payload,
                          const sourcemeta::core::JSON &rules) -> Admission {
  if (!rules.is_object()) {
    return Admission::Admitted;
  }

  // A claim that never arrived is a question somewhere else may still answer,
  // so it is held apart from one that arrived and fell short. Every rule is
  // still read, since a rule already refused settles the whole thing whatever
  // a later question would return
  auto outcome{Admission::Admitted};
  for (const auto &rule : rules.as_object()) {
    const auto *value{payload.try_at(rule.first)};
    if (value == nullptr) {
      if (outcome == Admission::Admitted) {
        outcome = Admission::Incomplete;
      }

      continue;
    }

    const auto accepted{
        rule.first == "scope"
            ? scope_accepts(payload, rule.second)
            : sourcemeta::core::oidc_claim_request_accepts_multi_valued(
                  rule.second, *value)};
    if (!accepted) {
      return Admission::Refused;
    }
  }

  return outcome;
}

// Whether an address the provider vouched for sits at one of the domains a
// policy admits. OpenID Connect Core Section 5.1 has a provider assert it
// verified ownership of an address only when `email_verified` is true, so
// without that the address is whatever its holder typed and proves nothing.
//
// The address is parsed rather than split on a separator, since RFC 5321
// Section 4.1.2 admits an at sign inside a quoted local part and Section 4.1.3
// admits one inside an address literal. An address that is no mailbox at all
// names no domain and so matches none, and a domain names a host, so it is
// compared without regard to case against domains the artifact already holds
// in lower case
inline auto admits_email_domain(const sourcemeta::core::JSON &claims,
                                const std::span<const std::string_view> domains)
    -> Admission {
  const auto *verified{claims.try_at("email_verified")};
  const auto *address{claims.try_at("email")};
  // Whatever arrived is judged before whatever did not. An address the
  // provider declines to vouch for is an answer already given, and it stays
  // given however much its companion might still turn up elsewhere
  if (verified != nullptr &&
      (!verified->is_boolean() || !verified->to_boolean())) {
    return Admission::Refused;
  }

  if (address != nullptr && !address->is_string()) {
    return Admission::Refused;
  }

  // Only absence is worth a second question
  if (verified == nullptr || address == nullptr) {
    return Admission::Incomplete;
  }

  const auto asserted{sourcemeta::core::email_domain(address->to_string())};
  if (asserted.empty()) {
    return Admission::Refused;
  }

  std::string domain{asserted};
  sourcemeta::core::to_lowercase(domain);
  return std::ranges::find(domains, domain) == domains.end()
             ? Admission::Refused
             : Admission::Admitted;
}

// The same question asked of the domains as the artifact stores them. A run
// this cannot read to its end says nothing about which domains a policy
// admits, so none of it is trusted rather than the part read before it
inline auto admits_email_domain(const sourcemeta::core::JSON &claims,
                                const std::span<const std::byte> domains)
    -> Admission {
  std::vector<std::string_view> decoded;
  if (!each_counted_string(domains, [&decoded](const auto domain) -> void {
        decoded.push_back(domain);
      })) {
    return Admission::Refused;
  }

  return admits_email_domain(claims, decoded);
}

} // namespace sourcemeta::one

#endif
