#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/oidc.h>

#include "authentication_format.h"

#include <algorithm>     // std::ranges::all_of
#include <bit>           // std::countr_zero
#include <chrono>        // std::chrono::system_clock, std::chrono::seconds
#include <cstddef>       // std::byte, std::size_t
#include <cstdint>       // std::uint32_t, std::uint64_t, std::uint8_t
#include <cstdlib>       // std::getenv
#include <cstring>       // std::memcpy
#include <filesystem>    // std::filesystem::path
#include <limits>        // std::numeric_limits
#include <map>           // std::map
#include <memory>        // std::unique_ptr, std::make_unique
#include <mutex>         // std::mutex, std::scoped_lock
#include <optional>      // std::optional, std::nullopt
#include <span>          // std::span
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move, std::pair
#include <vector>        // std::vector

namespace {

constexpr std::uint32_t NO_CHILD{std::numeric_limits<std::uint32_t>::max()};

// A base that is not a whole-segment prefix is left in place, where it matches
// no policy. The spelling is otherwise untouched, since a route is matched on
// the target as it arrived
auto strip_base_path(const std::string_view path,
                     const std::string_view base) noexcept -> std::string_view {
  if (base.empty() || !path.starts_with(base)) {
    return path;
  }

  auto remainder{path};
  remainder.remove_prefix(base.size());
  if (remainder.empty() || base.back() == '/' || remainder.front() == '/') {
    return remainder;
  }

  return path;
}

auto find_child(const sourcemeta::one::AuthenticationNode &node,
                const sourcemeta::one::AuthenticationEdge *edges,
                const char *strings, const std::string_view segment) noexcept
    -> std::uint32_t {
  std::uint32_t low{node.first_edge};
  std::uint32_t high{node.first_edge + node.edge_count};
  while (low < high) {
    const auto middle{low + ((high - low) / 2)};
    const auto &edge{edges[middle]};
    const std::string_view value{strings + edge.segment_offset,
                                 edge.segment_length};
    const auto comparison{value.compare(segment)};
    if (comparison == 0) {
      return edge.child;
    } else if (comparison < 0) {
      low = middle + 1;
    } else {
      high = middle;
    }
  }

  return NO_CHILD;
}

// The artifact is produced by a trusted indexer, but on-disk truncation or
// corruption could leave a header whose magic and version still match while
// its offsets and counts point out of bounds. Validating the whole layout
// once here keeps the matching hot path free of any per-call bounds checks
auto structurally_valid(const sourcemeta::core::FileView &view) noexcept
    -> bool {
  using namespace sourcemeta::one;
  const auto size{view.size()};
  if (size < sizeof(AuthenticationHeader)) {
    return false;
  }

  const auto *header{view.as<AuthenticationHeader>()};
  // The policy count must fit the bitmask width, otherwise matching would
  // shift past the width of a PolicySet, which is undefined behavior
  if (header->magic != AUTHENTICATION_MAGIC ||
      header->version != AUTHENTICATION_VERSION || header->node_count == 0 ||
      header->policy_count > Authentication::MAXIMUM_POLICIES) {
    return false;
  }

  // The artifact is produced by a single serializer with a fixed section
  // order, so recompute every offset from the counts and require the header
  // to match exactly. This rejects a corrupted layout whose sections overlap
  // or sit out of order, rather than reinterpreting unrelated bytes. The
  // per-policy metadata blob is appended after the string section
  const auto policies_offset{
      static_cast<std::size_t>(sizeof(AuthenticationHeader))};
  const auto policies_bytes{static_cast<std::size_t>(header->policy_count) *
                            sizeof(AuthenticationPolicyEntry)};
  const auto nodes_offset{(policies_offset + policies_bytes + 7U) &
                          ~static_cast<std::size_t>(7U)};
  const auto nodes_bytes{static_cast<std::size_t>(header->node_count) *
                         sizeof(AuthenticationNode)};
  const auto edges_offset{nodes_offset + nodes_bytes};
  const auto edges_bytes{static_cast<std::size_t>(header->edge_count) *
                         sizeof(AuthenticationEdge)};
  const auto strings_offset{edges_offset + edges_bytes};
  const auto strings_length{static_cast<std::size_t>(header->strings_length)};
  if (header->policies_offset != policies_offset ||
      header->nodes_offset != nodes_offset ||
      header->edges_offset != edges_offset ||
      header->strings_offset != strings_offset ||
      strings_offset + strings_length > size) {
    return false;
  }

  // A nested prefix is stored as at least one edge labelled by a non-empty
  // segment, so the string blob is never empty when edges are present. This
  // also keeps the string base in bounds for the matcher
  if (header->edge_count > 0 && strings_length == 0) {
    return false;
  }

  if (header->policy_count > 0) {
    const auto *policies{
        view.as<AuthenticationPolicyEntry>(header->policies_offset)};
    // Each policy's metadata is appended in declaration order after the string
    // blob, so a valid artifact lays them out contiguously from there onward
    auto metadata_cursor{strings_offset + strings_length};
    for (std::uint32_t index{0}; index < header->policy_count; index += 1) {
      const auto &entry{policies[index]};
      if (entry.metadata_offset != metadata_cursor ||
          entry.metadata_length > size - metadata_cursor ||
          entry.algorithm >
              static_cast<std::uint8_t>(Authentication::Algorithm::Sha256) ||
          entry.type > static_cast<std::uint8_t>(Authentication::Type::OIDC)) {
        return false;
      }

      metadata_cursor += entry.metadata_length;
    }
  }

  const auto *nodes{view.as<AuthenticationNode>(header->nodes_offset)};
  for (std::uint32_t index{0}; index < header->node_count; index += 1) {
    const auto &node{nodes[index]};
    if (node.edge_count > header->edge_count ||
        node.first_edge > header->edge_count - node.edge_count) {
      return false;
    }
  }

  if (header->edge_count > 0) {
    const auto *edges{view.as<AuthenticationEdge>(header->edges_offset)};
    for (std::uint32_t index{0}; index < header->edge_count; index += 1) {
      const auto &edge{edges[index]};
      if (edge.child >= header->node_count ||
          edge.segment_offset > strings_length ||
          edge.segment_length > strings_length - edge.segment_offset) {
        return false;
      }
    }
  }

  return true;
}

auto read_u32(const std::span<const std::byte> metadata, std::size_t &cursor,
              std::uint32_t &value) -> bool {
  if (cursor > metadata.size() || metadata.size() - cursor < sizeof(value)) {
    return false;
  }

  std::memcpy(&value, metadata.data() + cursor, sizeof(value));
  cursor += sizeof(value);
  return true;
}

// The one place a secret is read. A name that is empty names no variable, and
// a variable set to nothing holds no secret, so neither reaches a caller as a
// value it might compare something against
auto resolve_environment(const std::string_view variable)
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

auto admits_apikey(const std::span<const std::byte> metadata,
                   const std::string_view credential,
                   const sourcemeta::one::Authentication::Algorithm algorithm)
    -> bool {
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
  if (algorithm != sourcemeta::one::Authentication::Algorithm::Identity) {
    hashed = sourcemeta::core::sha256(credential);
  }
  const std::string_view subject{
      algorithm == sourcemeta::one::Authentication::Algorithm::Identity
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

auto collect_keys(const std::span<const std::byte> metadata,
                  std::unordered_set<std::string_view> &keys) -> void {
  std::size_t cursor{0};
  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count)) {
    return;
  }

  for (std::uint32_t index{0}; index < count; index += 1) {
    std::uint32_t length{0};
    if (!read_u32(metadata, cursor, length) ||
        metadata.size() - cursor < length) {
      return;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    keys.emplace(reinterpret_cast<const char *>(metadata.data() + cursor),
                 length);
    cursor += length;
  }
}

constexpr std::chrono::seconds JWT_CLOCK_SKEW{60};

auto read_string(const std::span<const std::byte> metadata, std::size_t &cursor,
                 std::string_view &value) -> bool {
  std::uint32_t length{0};
  if (!read_u32(metadata, cursor, length) ||
      metadata.size() - cursor < length) {
    return false;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  value = std::string_view{
      reinterpret_cast<const char *>(metadata.data() + cursor), length};
  cursor += length;
  return true;
}

struct JWTPolicy {
  std::string_view issuer;
  std::string_view audience;
  std::string_view jwks_uri;
  std::string_view token_type;
  std::vector<sourcemeta::core::JWSAlgorithm> algorithms;
};

auto decode_jwt_metadata(const std::span<const std::byte> metadata,
                         JWTPolicy &result) -> bool {
  std::size_t cursor{0};
  if (!read_string(metadata, cursor, result.issuer) ||
      !read_string(metadata, cursor, result.audience) ||
      !read_string(metadata, cursor, result.jwks_uri) ||
      !read_string(metadata, cursor, result.token_type)) {
    return false;
  }

  std::uint32_t count{0};
  // Each algorithm is a single byte, so a count larger than the bytes that
  // remain is corrupt and must not drive an allocation
  if (!read_u32(metadata, cursor, count) || count > metadata.size() - cursor) {
    return false;
  }

  result.algorithms.reserve(count);
  for (std::uint32_t index{0}; index < count; index += 1) {
    if (cursor >= metadata.size()) {
      return false;
    }

    const auto value{static_cast<std::uint8_t>(metadata[cursor])};
    cursor += 1;
    if (value >
        static_cast<std::uint8_t>(sourcemeta::core::JWSAlgorithm::EdDSA)) {
      return false;
    }

    result.algorithms.push_back(
        static_cast<sourcemeta::core::JWSAlgorithm>(value));
  }

  return true;
}

// The serialised claim rules of a JWT policy, which sit past the algorithms.
// They are read on their own, since the gate parses them once at startup while
// admission itself never touches these bytes again
auto read_jwt_claims(const std::span<const std::byte> metadata,
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

// A single claim value, already reduced from whatever container held it. RFC
// 9068 Section 2.2.3.1 gives group, role, and entitlement claims the shape RFC
// 7643 Section 4.1.2 defines, where a member is an object whose `value` is the
// identifier and whose `display` is a label that is neither unique nor stable,
// so only the identifier is ever compared. Admitting on a label would let
// whoever can rename a group grant access
auto claim_scalar_accepts(const sourcemeta::core::JSON &request,
                          const sourcemeta::core::JSON &value) -> bool {
  if (value.is_string()) {
    return sourcemeta::core::oidc_claim_request_accepts(request, value);
  }

  if (value.is_object()) {
    const auto *identifier{value.try_at("value")};
    return identifier != nullptr && identifier->is_string() &&
           sourcemeta::core::oidc_claim_request_accepts(request, *identifier);
  }

  return false;
}

// An array carries a set the caller belongs to, so any one member satisfying
// the rule satisfies it
auto claim_accepts(const sourcemeta::core::JSON &request,
                   const sourcemeta::core::JSON &value) -> bool {
  if (value.is_array()) {
    return std::ranges::any_of(value.as_array(),
                               [&request](const auto &entry) -> bool {
                                 return claim_scalar_accepts(request, entry);
                               });
  }

  return claim_scalar_accepts(request, value);
}

// The one claim whose value is a set rather than a value. RFC 6749 Section 3.3
// makes it a space-delimited, case-sensitive, unordered list, so a rule naming
// values is satisfied by any one of them being granted, while a rule
// constraining nothing asks only that a scope be carried at all.
//
// A constraint this cannot read denies rather than being passed over. Passing
// over one would widen the rule to every token carrying any scope, so the
// nearer a rule is to unreadable the more it would admit
auto scope_accepts(const sourcemeta::core::JSON &payload,
                   const sourcemeta::core::JSON &request) -> bool {
  const auto *granted{payload.try_at("scope")};
  if (granted == nullptr || !granted->is_string()) {
    return false;
  }

  if (!request.is_object()) {
    return true;
  }

  const auto *single{request.try_at("value")};
  const auto *values{request.try_at("values")};
  if (single == nullptr && values == nullptr) {
    return true;
  }

  if (single != nullptr) {
    if (!single->is_string()) {
      return false;
    }

    if (sourcemeta::core::oauth_has_scope(payload, single->to_string())) {
      return true;
    }
  }

  if (values != nullptr) {
    if (!values->is_array()) {
      return false;
    }

    for (const auto &entry : values->as_array()) {
      if (!entry.is_string()) {
        return false;
      }

      if (sourcemeta::core::oauth_has_scope(payload, entry.to_string())) {
        return true;
      }
    }
  }

  return false;
}

// Whether a claim arrived carrying objects rather than the strings a rule
// names. Question 6's reading compares such a claim on its `value`
// sub-attribute alone, so a rule naming a display name matches nothing
auto carries_objects(const sourcemeta::core::JSON &value) -> bool {
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
auto admits_claims(const sourcemeta::core::JSON &payload,
                   const sourcemeta::core::JSON &rules)
    -> sourcemeta::one::Authentication::Admission {
  using Admission = sourcemeta::one::Authentication::Admission;
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

    const auto accepted{rule.first == "scope"
                            ? scope_accepts(payload, rule.second)
                            : claim_accepts(rule.second, *value)};
    if (!accepted) {
      return Admission::Refused;
    }
  }

  return outcome;
}

// The reference check treats two JWT policies as the same scope only when
// every parameter that decides admission matches, so the issuer, audience, key
// set location, required token type, allowed algorithms and claim rules count
// as one indivisible identity, never as separate keys that several policies
// could satisfy piecewise or in swapped roles.
//
// The rules belong in that identity for the same reason the token type does. A
// policy carrying them admits a narrower set than one alike but for them, so
// leaving them out would let a schema under the looser policy reference one
// the stricter policy guards, and the check would see two identical audiences
// where there are two different ones.
//
// A policy requiring a token type admits a narrower set than one that does
// not, so two policies alike but for it are not the same audience either. That
// refuses a reference from the stricter of the two to the looser one, which
// every holder of the stricter credential could have followed anyway, and the
// cost of refusing is a build that has to say so against disclosing a referent
// to somebody the referrer never admitted
auto collect_jwt_identifiers(const std::span<const std::byte> metadata,
                             std::unordered_set<std::string_view> &keys)
    -> void {
  std::size_t cursor{0};
  std::string_view issuer;
  std::string_view audience;
  std::string_view jwks_uri;
  std::string_view token_type;
  if (!read_string(metadata, cursor, issuer) ||
      !read_string(metadata, cursor, audience) ||
      !read_string(metadata, cursor, jwks_uri) ||
      !read_string(metadata, cursor, token_type)) {
    return;
  }

  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count) || count > metadata.size() - cursor) {
    return;
  }

  cursor += count;
  std::string_view claims;
  if (!read_string(metadata, cursor, claims)) {
    return;
  }

  // The serialized run itself is the key. Its length prefixes keep the fields
  // delimited, so exactly the equal identities compare equal, and the rules
  // arrive already canonical so that two spellings of one rule cannot part
  // policies that admit the same callers
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  keys.emplace(reinterpret_cast<const char *>(metadata.data()), cursor);
}

struct OIDCPolicyMetadata {
  std::string_view issuer;
  std::string_view client_id;
  std::string_view claims;
  // The domains this policy admits an address at, kept as the bytes they
  // occupy so that reading a policy costs nothing until one is wanted
  std::span<const std::byte> email_domains;
  std::string_view client_secret_variable;
  std::string_view name;
  // The variables holding this policy's session secrets, kept as the bytes
  // they occupy so that reading a policy costs nothing until one is wanted
  std::span<const std::byte> session_secrets;
  std::string_view default_path;
};

// Walk a counted run of strings, handing each to the caller, answering whether
// the whole run could be read
template <typename Callback>
  requires std::invocable<Callback, std::string_view>
[[nodiscard]] auto
each_counted_string(const std::span<const std::byte> metadata,
                    Callback callback) -> bool {
  std::size_t cursor{0};
  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count)) {
    return false;
  }

  for (std::uint32_t index{0}; index < count; index += 1) {
    std::string_view value;
    if (!read_string(metadata, cursor, value)) {
      return false;
    }

    callback(value);
  }

  return true;
}

// Advance past a counted run of strings, answering the bytes it occupies
auto read_counted_strings(const std::span<const std::byte> metadata,
                          std::size_t &cursor,
                          std::span<const std::byte> &result) -> bool {
  const auto start{cursor};
  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count)) {
    return false;
  }

  for (std::uint32_t index{0}; index < count; index += 1) {
    std::string_view value;
    if (!read_string(metadata, cursor, value)) {
      return false;
    }
  }

  result = metadata.subspan(start, cursor - start);
  return true;
}

auto decode_oidc_metadata(const std::span<const std::byte> metadata,
                          OIDCPolicyMetadata &result) -> bool {
  std::size_t cursor{0};
  if (!read_string(metadata, cursor, result.issuer) ||
      !read_string(metadata, cursor, result.client_id) ||
      !read_string(metadata, cursor, result.claims) ||
      !read_counted_strings(metadata, cursor, result.email_domains) ||
      !read_string(metadata, cursor, result.client_secret_variable) ||
      !read_string(metadata, cursor, result.name)) {
    return false;
  }

  const auto secrets_start{cursor};
  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count)) {
    return false;
  }

  for (std::uint32_t index{0}; index < count; index += 1) {
    std::string_view variable;
    if (!read_string(metadata, cursor, variable)) {
      return false;
    }
  }

  result.session_secrets =
      metadata.subspan(secrets_start, cursor - secrets_start);
  return read_string(metadata, cursor, result.default_path);
}

// What a login and its callback need of a policy, lifted out of the bytes it
// is stored as. The domains are copied out, since a caller matching an address
// against them should not have to know how they are laid out
auto interactive_policy(const OIDCPolicyMetadata &decoded)
    -> sourcemeta::one::Authentication::InteractivePolicy {
  sourcemeta::one::Authentication::InteractivePolicy result;
  result.issuer = decoded.issuer;
  result.client_id = decoded.client_id;
  result.default_path = decoded.default_path;
  result.claims = decoded.claims;
  if (!each_counted_string(decoded.email_domains,
                           [&result](const auto domain) -> void {
                             result.email_domains.push_back(domain);
                           })) {
    result.email_domains.clear();
  }

  return result;
}

// Whether an address the provider vouched for sits at one of the domains a
// policy admits. OpenID Connect Core Section 5.1 has a provider assert it
// verified ownership of an address only when `email_verified` is true, so
// without that the address is whatever its holder typed and proves nothing.
//
// The comparison takes the last separator, since a quoted local part may
// legally carry one, and the domain names a host, so it is compared without
// regard to case against domains the artifact already holds in lower case
auto admits_email_domain(const sourcemeta::core::JSON &claims,
                         const std::span<const std::byte> domains)
    -> sourcemeta::one::Authentication::Admission {
  using Admission = sourcemeta::one::Authentication::Admission;
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

  const auto &value{address->to_string()};
  const auto separator{value.rfind('@')};
  if (separator == std::string::npos || separator + 1 >= value.size()) {
    return Admission::Refused;
  }

  std::string domain{value.substr(separator + 1)};
  sourcemeta::core::to_lowercase(domain);
  bool admitted{false};
  if (!each_counted_string(domains,
                           [&admitted, &domain](const auto candidate) -> void {
                             admitted = admitted || candidate == domain;
                           })) {
    return Admission::Refused;
  }

  return admitted ? Admission::Admitted : Admission::Refused;
}

// The reference check treats two interactive policies as the same scope only
// when they admit the same people, so the issuer, client identifier, claim
// rules and email domains count as one indivisible identity, never as separate
// keys that several policies could satisfy piecewise or in swapped roles.
//
// The rules belong in that identity for the same reason a JWT policy's do. A
// policy carrying them admits a narrower set than one alike but for them, so
// leaving them out would let a schema under the looser policy reference one
// the stricter policy guards, and the check would see two identical audiences
// where there are two different ones
auto collect_oidc_identifiers(const std::span<const std::byte> metadata,
                              std::unordered_set<std::string_view> &keys)
    -> void {
  std::size_t cursor{0};
  std::string_view issuer;
  std::string_view client_id;
  std::string_view claims;
  std::span<const std::byte> email_domains;
  if (!read_string(metadata, cursor, issuer) ||
      !read_string(metadata, cursor, client_id) ||
      !read_string(metadata, cursor, claims) ||
      !read_counted_strings(metadata, cursor, email_domains)) {
    return;
  }

  // The serialized run itself is the key. Its length prefixes keep the fields
  // delimited, so exactly the equal identities compare equal, and both the
  // rules and the domains arrive already canonical so that two spellings of
  // one rule cannot part policies that admit the same people
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  keys.emplace(reinterpret_cast<const char *>(metadata.data()), cursor);
}

} // namespace

namespace sourcemeta::one {

// A set of policy entries, one bit per entry. Entries are assigned
// monotonically increasing identifiers in configuration declaration order, so
// a governing set is a single machine word
using PolicySet = std::uint64_t;

struct Authentication::Impl {
  // The indexer always emits this artifact. A missing, unreadable, or
  // malformed file means it was deleted, corrupted, or produced by an older
  // indexer. Rather than failing open and serving every path publicly, or
  // crashing the server into a restart loop, leave the policy denying
  // everything: the section pointers below stay null, so matching yields the
  // empty set and admits no one. Opening the file covers the missing and
  // unreadable cases without a separate, throwing existence check
  Impl(const std::filesystem::path &path,
       sourcemeta::core::JWKSProvider::Fetcher fetcher)
      : fetcher_{std::move(fetcher)} {
    std::unique_ptr<sourcemeta::core::FileView> view;
    try {
      view = std::make_unique<sourcemeta::core::FileView>(path);
    } catch (const sourcemeta::core::FileViewError &) {
      return;
    }

    if (!structurally_valid(*view)) {
      return;
    }

    const auto *header{view->as<AuthenticationHeader>()};

    // Claim rules are read and parsed once here rather than on each request,
    // since a request answers a rule by lookup while the bytes behind it never
    // change. A policy naming none is left null, which every rule vacuously
    // satisfies.
    //
    // Rules that are present but unreadable leave the whole artifact denying
    // everything, exactly as a malformed header does. Passing over them would
    // instead drop the restriction and admit every caller the policy would
    // otherwise have narrowed, which is the one outcome worse than refusing
    std::vector<sourcemeta::core::JSON> claims;
    claims.assign(header->policy_count, sourcemeta::core::JSON{nullptr});
    const auto *policies{
        view->as<AuthenticationPolicyEntry>(header->policies_offset)};
    for (std::uint32_t index{0}; index < header->policy_count; index += 1) {
      const auto &entry{policies[index]};
      const auto type{static_cast<Authentication::Type>(entry.type)};
      if ((type != Authentication::Type::JWT &&
           type != Authentication::Type::OIDC) ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          view->as<std::byte>(entry.metadata_offset), entry.metadata_length};
      std::string_view serialized;
      if (type == Authentication::Type::JWT) {
        if (!read_jwt_claims(metadata, serialized)) {
          return;
        }
      } else {
        OIDCPolicyMetadata decoded;
        if (!decode_oidc_metadata(metadata, decoded)) {
          return;
        }

        serialized = decoded.claims;
      }

      if (serialized.empty()) {
        continue;
      }

      auto document{sourcemeta::core::try_parse_json(serialized)};
      if (!document.has_value() || !document.value().is_object()) {
        return;
      }

      claims[index] = std::move(document).value();
    }

    this->nodes_ = view->as<AuthenticationNode>(header->nodes_offset);
    // The edge and string sections are empty when no policy declares a nested
    // prefix, in which case they sit at the end of the buffer and must not be
    // addressed
    if (header->edge_count > 0) {
      this->edges_ = view->as<AuthenticationEdge>(header->edges_offset);
      this->strings_ = view->as<char>(header->strings_offset);
    }

    if (header->policy_count > 0) {
      this->policies_ =
          view->as<AuthenticationPolicyEntry>(header->policies_offset);
      this->policy_count_ = header->policy_count;
    }

    this->claims_ = std::move(claims);
    this->view_ = std::move(view);
  }

  // The policies that govern a registry path, accumulated from every prefix
  // covering it. An unconfigured instance yields the empty set
  [[nodiscard]] auto match(const std::string_view registry_path) const noexcept
      -> PolicySet {
    if (this->nodes_ == nullptr) {
      return 0;
    }

    const auto *nodes{static_cast<const AuthenticationNode *>(this->nodes_)};
    const auto *edges{static_cast<const AuthenticationEdge *>(this->edges_)};

    PolicySet result{nodes[0].mask};
    std::uint32_t current{0};
    std::size_t cursor{0};
    auto segment{authentication_next_segment(registry_path, cursor)};
    while (!segment.empty()) {
      auto lookahead{cursor};
      const auto next{authentication_next_segment(registry_path, lookahead)};
      const auto &node{nodes[current]};

      const auto exact{find_child(node, edges, this->strings_, segment)};
      // An extension is content negotiation on the resource itself, so only the
      // terminal segment also matches the extensionless resource policy, with
      // union semantics admitting both. An intermediate dotted segment is a
      // distinct directory and must not inherit its stem's policies
      auto stem{NO_CHILD};
      if (next.empty()) {
        const auto extension{segment.rfind('.')};
        if (extension != std::string_view::npos && extension > 0) {
          stem = find_child(
              node, edges, this->strings_,
              std::string_view{segment.begin(), segment.begin() + extension});
        }
      }

      if (exact != NO_CHILD) {
        result |= nodes[exact].mask;
      }

      if (stem != NO_CHILD) {
        result |= nodes[stem].mask;
      }

      if (exact != NO_CHILD) {
        current = exact;
      } else if (stem != NO_CHILD) {
        current = stem;
      } else {
        break;
      }

      segment = next;
      cursor = lookahead;
    }

    return result;
  }

  [[nodiscard]] auto admits(const std::string_view registry_path,
                            const std::string_view credential,
                            const std::span<const std::string_view> cookies,
                            const std::string_view required_audience = {}) const
      -> Authentication::Verdict {
    // A missing or structurally broken artifact leaves the section pointers
    // null and denies everything. Only a valid policy fails open below
    if (this->nodes_ == nullptr) {
      return {.allowed = false, .principal = std::nullopt};
    }

    const auto governing{this->match(registry_path)};
    if (governing == 0) {
      // No policy covers this path, so it is public and the caller anonymous
      return {.allowed = true, .principal = std::nullopt};
    }

    const auto token{sourcemeta::core::JWT::from(credential)};

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      if ((governing & (PolicySet{1} << index)) == 0) {
        continue;
      }

      const auto &entry{policies[index]};
      std::span<const std::byte> metadata;
      if (entry.metadata_length > 0) {
        metadata = {this->view_->as<std::byte>(entry.metadata_offset),
                    entry.metadata_length};
      }

      const auto type{static_cast<Authentication::Type>(entry.type)};
      if (type == Authentication::Type::JWT) {
        if (token.has_value() &&
            this->admits_jwt(metadata, token.value(), required_audience,
                             this->claims_[index])) {
          return {.allowed = true,
                  .principal = Authentication::Principal{
                      .type = type, .policy = static_cast<std::size_t>(index)}};
        }
      } else if (type == Authentication::Type::OIDC) {
        // An interactive policy authenticates a person through the session
        // its browser login established, never a presented credential
        if (this->admits_session(metadata, cookies)) {
          return {.allowed = true,
                  .principal = Authentication::Principal{
                      .type = type, .policy = static_cast<std::size_t>(index)}};
        }
      } else if (admits_apikey(
                     metadata, credential,
                     static_cast<Authentication::Algorithm>(entry.algorithm))) {
        return {.allowed = true,
                .principal = Authentication::Principal{
                    .type = type, .policy = static_cast<std::size_t>(index)}};
      }
    }

    return {.allowed = false, .principal = std::nullopt};
  }

  [[nodiscard]] auto admits_jwt(const std::span<const std::byte> metadata,
                                const sourcemeta::core::JWT &token,
                                const std::string_view required_audience,
                                const sourcemeta::core::JSON &claims) const
      -> bool {
    JWTPolicy policy;
    if (!decode_jwt_metadata(metadata, policy)) {
      return false;
    }

    auto *provider{this->provider_for(policy.issuer, policy.jwks_uri)};
    if (provider == nullptr) {
      return false;
    }

    // RFC 9068 Section 4 has a resource server refuse a token whose `typ` is
    // not the access token profile's, which is what keeps an identity token
    // from being spent as an API credential. A provider that does not stamp
    // the header at all cannot be told apart that way, so the policy says
    // which type it requires rather than one being assumed
    const auto expected_type{
        policy.token_type.empty()
            ? std::optional<std::string_view>{std::nullopt}
            : std::optional<std::string_view>{policy.token_type}};
    const auto error{provider->verify(token, policy.algorithms, policy.issuer,
                                      policy.audience, std::nullopt,
                                      expected_type)};
    if (error.has_value()) {
      return false;
    }

    // The signature and the policy's own audience are already established, so
    // the route's requirement is one more claim read from a token that has
    // been verified rather than a second verification
    if (!required_audience.empty() && !token.has_audience(required_audience)) {
      return false;
    }

    return admits_claims(token.payload(), claims) ==
           Authentication::Admission::Admitted;
  }

  [[nodiscard]] auto
  admits_session(const std::span<const std::byte> metadata,
                 const std::span<const std::string_view> cookies) const
      -> bool {
    if (cookies.empty()) {
      return false;
    }

    OIDCPolicyMetadata decoded;
    if (!decode_oidc_metadata(metadata, decoded) || decoded.name.empty()) {
      return false;
    }
    const auto policy_name{decoded.name};

    // A request can carry several cookies under one name, since a parent
    // domain and the host itself can each set one and neither the header nor
    // the order says which is which. Taking any single one lets whoever
    // controls a neighbouring host decide which session this instance reads,
    // so each is carried through the whole check and the caller is admitted
    // when any of them is a session for this policy
    std::vector<std::string_view> candidates;
    for (const auto field : cookies) {
      sourcemeta::core::http_cookie_values(
          field, Authentication::SESSION_COOKIE, candidates);
    }
    for (const auto sealed : candidates) {
      const auto payload{this->session_open(
          decoded.session_secrets, Authentication::Purpose::Session, sealed)};
      if (!payload.has_value()) {
        continue;
      }

      const auto document{sourcemeta::core::try_parse_json(payload.value())};
      if (!document.has_value() || !document.value().is_object()) {
        continue;
      }

      // Every policy governing this location reads the one session the browser
      // holds, so this is what keeps a session established under one policy
      // from admitting its holder under another. Two policies may share a
      // session secret, in which case a value minted elsewhere opens cleanly
      // here and nothing but the payload tells them apart. It is the control,
      // not a belt on top of one, and a value that is not for this policy is
      // passed over rather than ending the search
      const auto *minted_for{document.value().try_at("policy")};
      if (minted_for != nullptr && minted_for->is_string() &&
          minted_for->to_string() == policy_name) {
        return true;
      }
    }

    return false;
  }

  // The payload of a session value, whichever interactive policy minted it.
  // The policy travels inside the sealed value, so a caller learns which one
  // established the session rather than nominating one, and a value is only
  // ever accepted under the policy it names. Deciding that here rather than at
  // a call site keeps one answer to what a session is
  [[nodiscard]] auto open_session(const std::string_view value) const
      -> std::optional<std::string> {
    if (this->policy_count_ == 0) {
      return std::nullopt;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      if (static_cast<Authentication::Type>(entry.type) !=
              Authentication::Type::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          this->view_->as<std::byte>(entry.metadata_offset),
          entry.metadata_length};
      OIDCPolicyMetadata decoded;
      if (!decode_oidc_metadata(metadata, decoded) || decoded.name.empty()) {
        continue;
      }

      const auto payload{this->session_open(
          decoded.session_secrets, Authentication::Purpose::Session, value)};
      if (!payload.has_value()) {
        continue;
      }

      const auto document{sourcemeta::core::try_parse_json(payload.value())};
      if (!document.has_value() || !document.value().is_object()) {
        continue;
      }

      const auto *minted_for{document.value().try_at("policy")};
      if (minted_for == nullptr || !minted_for->is_string() ||
          minted_for->to_string() != decoded.name) {
        continue;
      }

      return payload;
    }

    return std::nullopt;
  }

  // The decoded metadata of the OIDC policy declared under the given name,
  // scanned out of the artifact
  [[nodiscard]] auto find_interactive(const std::string_view name,
                                      OIDCPolicyMetadata &result) const
      -> bool {
    if (this->policy_count_ == 0 || name.empty()) {
      return false;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      if (static_cast<Authentication::Type>(entry.type) !=
              Authentication::Type::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          this->view_->as<std::byte>(entry.metadata_offset),
          entry.metadata_length};
      if (decode_oidc_metadata(metadata, result) && result.name == name) {
        return true;
      }
    }

    return false;
  }

  [[nodiscard]] auto interactive(const std::string_view name) const
      -> std::optional<Authentication::InteractivePolicy> {
    OIDCPolicyMetadata decoded;
    if (!this->find_interactive(name, decoded)) {
      return std::nullopt;
    }

    return interactive_policy(decoded);
  }

  [[nodiscard]] auto
  object_shaped_claims(const std::string_view policy,
                       const sourcemeta::core::JSON &claims) const
      -> std::vector<std::string_view> {
    std::vector<std::string_view> result;
    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      if (static_cast<Authentication::Type>(entry.type) !=
              Authentication::Type::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      OIDCPolicyMetadata decoded;
      if (!decode_oidc_metadata(
              {this->view_->as<std::byte>(entry.metadata_offset),
               entry.metadata_length},
              decoded) ||
          decoded.name != policy) {
        continue;
      }

      const auto &rules{this->claims_[index]};
      if (!rules.is_object()) {
        return result;
      }

      for (const auto &rule : rules.as_object()) {
        const auto *value{claims.try_at(rule.first)};
        if (value != nullptr && carries_objects(*value)) {
          result.push_back(rule.first);
        }
      }

      return result;
    }

    return result;
  }

  [[nodiscard]] auto admits_identity(const std::string_view policy,
                                     const sourcemeta::core::JSON &claims) const
      -> Authentication::Admission {
    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      if (static_cast<Authentication::Type>(entry.type) !=
              Authentication::Type::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      OIDCPolicyMetadata decoded;
      if (!decode_oidc_metadata(
              {this->view_->as<std::byte>(entry.metadata_offset),
               entry.metadata_length},
              decoded) ||
          decoded.name != policy) {
        continue;
      }

      // A policy naming domains admits nobody whose address it cannot place,
      // so one it cannot read denies rather than being passed over
      std::uint32_t domains{0};
      std::size_t cursor{0};
      if (!read_u32(decoded.email_domains, cursor, domains)) {
        return Authentication::Admission::Refused;
      }

      const auto address{
          domains > 0 ? admits_email_domain(claims, decoded.email_domains)
                      : Authentication::Admission::Admitted};
      if (address == Authentication::Admission::Refused) {
        return address;
      }

      const auto rules{admits_claims(claims, this->claims_[index])};
      if (rules == Authentication::Admission::Refused) {
        return rules;
      }

      // Either half wanting more is the whole wanting more
      return rules == Authentication::Admission::Incomplete ? rules : address;
    }

    // A name that no interactive policy answers to could never have minted a
    // session, so nothing it asserts is admitted
    return Authentication::Admission::Refused;
  }

  [[nodiscard]] auto interactive(const std::string_view path,
                                 const std::string_view name) const
      -> std::optional<Authentication::InteractivePolicy> {
    const auto mask{this->match(path)};
    if (mask == 0 || this->policy_count_ == 0 || name.empty()) {
      return std::nullopt;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      if ((mask & (PolicySet{1} << index)) == 0) {
        continue;
      }

      const auto &entry{policies[index]};
      if (static_cast<Authentication::Type>(entry.type) !=
              Authentication::Type::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          this->view_->as<std::byte>(entry.metadata_offset),
          entry.metadata_length};
      OIDCPolicyMetadata decoded;
      if (decode_oidc_metadata(metadata, decoded) && decoded.name == name) {
        return interactive_policy(decoded);
      }
    }

    return std::nullopt;
  }

  [[nodiscard]] auto client_secret(const std::string_view policy) const
      -> std::optional<sourcemeta::core::SecureString> {
    OIDCPolicyMetadata decoded;
    if (!this->find_interactive(policy, decoded)) {
      return std::nullopt;
    }

    return resolve_environment(decoded.client_secret_variable);
  }

  // The resolved values back the views handed to the sealing primitive, so
  // they are returned to the caller to keep alive alongside them. A run that
  // cannot be read to its end says nothing about which secrets a policy
  // accepts, so none of it is trusted rather than the part read before it
  static auto session_secrets(const std::span<const std::byte> variables)
      -> std::vector<sourcemeta::core::SecureString> {
    std::vector<sourcemeta::core::SecureString> result;
    if (!each_counted_string(variables, [&result](const auto variable) -> void {
          auto resolved{resolve_environment(variable)};
          if (resolved.has_value()) {
            result.push_back(std::move(resolved.value()));
          }
        })) {
      return {};
    }

    return result;
  }

  // A value is signed under the newest secret and accepted under any of them,
  // so a secret can be replaced by putting the new one first and dropping the
  // old once every value signed under it has expired
  [[nodiscard]] auto session_seal(const std::span<const std::byte> variables,
                                  const Authentication::Purpose purpose,
                                  const std::string_view payload,
                                  const std::chrono::sys_seconds expiry) const
      -> std::optional<std::string> {
    const auto resolved{session_secrets(variables)};
    if (resolved.empty()) {
      return std::nullopt;
    }

    const auto issued{std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now())};
    return Authentication::seal_value(payload, purpose, resolved.front(),
                                      issued, expiry);
  }

  [[nodiscard]] auto session_open(const std::span<const std::byte> variables,
                                  const Authentication::Purpose purpose,
                                  const std::string_view value) const
      -> std::optional<std::string> {
    const auto resolved{session_secrets(variables)};
    if (resolved.empty()) {
      return std::nullopt;
    }

    // The views are taken once the values have stopped moving
    std::vector<std::string_view> secrets;
    secrets.reserve(resolved.size());
    for (const auto &secret : resolved) {
      secrets.emplace_back(secret);
    }

    const auto now{std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now())};
    return Authentication::open_value(value, purpose, secrets, now);
  }

  [[nodiscard]] auto seal(const std::string_view policy,
                          const Authentication::Purpose purpose,
                          const std::string_view payload,
                          const std::chrono::sys_seconds expiry) const
      -> std::optional<std::string> {
    OIDCPolicyMetadata decoded;
    if (!this->find_interactive(policy, decoded)) {
      return std::nullopt;
    }

    return this->session_seal(decoded.session_secrets, purpose, payload,
                              expiry);
  }

  [[nodiscard]] auto open(const std::string_view policy,
                          const Authentication::Purpose purpose,
                          const std::string_view value) const
      -> std::optional<std::string> {
    OIDCPolicyMetadata decoded;
    if (!this->find_interactive(policy, decoded)) {
      return std::nullopt;
    }

    return this->session_open(decoded.session_secrets, purpose, value);
  }

  // The provider's description of itself is fetched once and refreshed on the
  // freshness its own response advertises, rather than on every login. The
  // caching resolver hands back a snapshot of what it last read, so the OpenID
  // Connect layer is applied again only when that snapshot changes rather than
  // once per request
  [[nodiscard]] auto endpoints(const std::string_view policy) const
      -> std::optional<Authentication::ProviderEndpoints> {
    OIDCPolicyMetadata decoded;
    if (!this->find_interactive(policy, decoded) || !this->fetcher_) {
      return std::nullopt;
    }

    std::string issuer{decoded.issuer};
    sourcemeta::core::OAuthMetadataProvider *provider{nullptr};
    {
      const std::scoped_lock lock{this->metadata_mutex_};
      const auto existing{this->metadata_providers_.find(issuer)};
      if (existing == this->metadata_providers_.cend()) {
        // The two resolvers describe a retrieval with their own result type,
        // so the one transport this instance was given is adapted rather than
        // a second one being introduced
        auto transport{
            [fetcher = this->fetcher_](const std::string_view url)
                -> std::optional<
                    sourcemeta::core::OAuthMetadataProvider::FetchResult> {
              auto result{fetcher(url)};
              if (!result.has_value()) {
                return std::nullopt;
              }

              const auto max_age{result.value().max_age};
              return sourcemeta::core::OAuthMetadataProvider::FetchResult{
                  .body = std::move(result).value().body, .max_age = max_age};
            }};
        auto fresh{std::make_unique<sourcemeta::core::OAuthMetadataProvider>(
            issuer,
            sourcemeta::core::OAuthWellKnownKind::OpenIDConfigurationAppended,
            std::move(transport))};
        provider = fresh.get();
        this->metadata_providers_.emplace(issuer, std::move(fresh));
      } else {
        provider = existing->second.get();
      }
    }

    const auto server{provider->metadata()};
    if (!server) {
      return std::nullopt;
    }

    const std::scoped_lock lock{this->metadata_mutex_};
    auto &cached{this->endpoints_[issuer]};
    if (cached.source != server) {
      auto document{sourcemeta::core::OIDCProviderMetadata::from(
          sourcemeta::core::OAuthServerMetadata{*server})};
      if (!document.has_value()) {
        return std::nullopt;
      }

      Authentication::ProviderEndpoints resolved;
      if (document.value().authorization_endpoint().has_value()) {
        resolved.authorization =
            document.value().authorization_endpoint().value();
      }

      if (document.value().token_endpoint().has_value()) {
        resolved.token = document.value().token_endpoint().value();
      }

      resolved.jwks_uri = document.value().jwks_uri();
      if (document.value().end_session_endpoint().has_value()) {
        resolved.end_session = document.value().end_session_endpoint().value();
      }

      if (document.value().userinfo_endpoint().has_value()) {
        resolved.userinfo = document.value().userinfo_endpoint().value();
      }

      // RFC 6749 Section 2.3.1 requires every server to accept the client
      // secret in an authorization header and discourages carrying it in the
      // request body, so the body is used only where the header is refused.
      // A provider that lists nothing is taken to accept the header, which is
      // what the specification assigns to saying nothing
      resolved.token_endpoint_basic_auth =
          document.value().supports_token_endpoint_auth_method(
              "client_secret_basic");

      // OpenID Connect Discovery 1.0 Section 3 defaults this to false when a
      // provider says nothing, so a login falls back to asking through the
      // scopes that carry a claim rather than sending a parameter that would
      // be ignored, leaving it with no claims at all
      resolved.claims_parameter_supported =
          document.value().supports_claims_parameter();

      const auto *advertised{
          document.value().data().try_at("claims_supported")};
      if (advertised != nullptr && advertised->is_array()) {
        for (const auto &claim : advertised->as_array()) {
          if (claim.is_string()) {
            resolved.claims_supported.push_back(claim.to_string());
          }
        }
      }

      cached.source = server;
      cached.resolved = std::move(resolved);
    }

    return cached.resolved;
  }

  [[nodiscard]] auto provider_for(const std::string_view issuer,
                                  const std::string_view jwks_uri) const
      -> sourcemeta::core::JWKSProvider * {
    if (!this->fetcher_) {
      return nullptr;
    }

    std::pair<std::string, std::string> key{issuer, jwks_uri};

    {
      const std::scoped_lock lock{this->jwks_mutex_};
      const auto existing{this->jwks_providers_.find(key)};
      if (existing != this->jwks_providers_.cend()) {
        return existing->second.get();
      }
    }

    std::string location;
    if (jwks_uri.empty()) {
      const auto url{sourcemeta::core::oidc_discovery_url(issuer)};
      if (!url.has_value()) {
        return nullptr;
      }

      const auto metadata{this->fetcher_(url.value())};
      if (!metadata.has_value()) {
        return nullptr;
      }

      auto parsed{sourcemeta::core::try_parse_json(metadata.value().body)};
      if (!parsed.has_value()) {
        return nullptr;
      }

      const auto document{sourcemeta::core::OIDCProviderMetadata::from(
          std::move(parsed).value(), issuer)};
      if (!document.has_value()) {
        return nullptr;
      }

      location = document.value().jwks_uri();
    } else {
      location = jwks_uri;
    }

    sourcemeta::core::JWKSProvider::Options options;
    options.clock_skew = JWT_CLOCK_SKEW;
    auto provider{std::make_unique<sourcemeta::core::JWKSProvider>(
        std::move(location), this->fetcher_, options)};

    // A concurrent caller may have installed this key while the lock was
    // released, so its provider wins and ours is discarded
    const std::scoped_lock lock{this->jwks_mutex_};
    const auto existing{this->jwks_providers_.find(key)};
    if (existing != this->jwks_providers_.cend()) {
      return existing->second.get();
    }

    auto *raw{provider.get()};
    this->jwks_providers_.emplace(std::move(key), std::move(provider));
    return raw;
  }

  struct Audience {
    bool is_public;
    std::unordered_set<std::string_view> keys;
  };

  [[nodiscard]] auto audience(const std::string_view registry_path) const
      -> Audience {
    Audience result{.is_public = false, .keys = {}};
    // A missing or broken artifact is not public, keeping the reference check
    // conservative
    if (this->nodes_ == nullptr) {
      return result;
    }

    const auto governing{this->match(registry_path)};
    if (governing == 0) {
      // No policy covers this path, so it is public
      result.is_public = true;
      return result;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      if ((governing & (PolicySet{1} << index)) == 0) {
        continue;
      }

      const auto &entry{policies[index]};
      if (entry.metadata_length > 0) {
        const std::span<const std::byte> metadata{
            this->view_->as<std::byte>(entry.metadata_offset),
            entry.metadata_length};
        const auto type{static_cast<Authentication::Type>(entry.type)};
        if (type == Authentication::Type::JWT) {
          collect_jwt_identifiers(metadata, result.keys);
        } else if (type == Authentication::Type::OIDC) {
          collect_oidc_identifiers(metadata, result.keys);
        } else {
          collect_keys(metadata, result.keys);
        }
      }
    }

    return result;
  }

  [[nodiscard]] auto
  reference_permitted(const std::string_view referrer_path,
                      const std::string_view referent_path) const -> bool {
    // A missing or broken artifact denies every reference, since an empty key
    // set would otherwise vacuously satisfy the subset check below
    if (this->nodes_ == nullptr) {
      return false;
    }

    const auto referent{this->audience(referent_path)};
    if (referent.is_public) {
      return true;
    }

    const auto referrer{this->audience(referrer_path)};
    if (referrer.is_public) {
      return false;
    }

    return std::ranges::all_of(referrer.keys,
                               [&referent](const auto key) -> bool {
                                 return referent.keys.contains(key);
                               });
  }

  // The trie section bases, resolved from the header once at construction so
  // that matching never re-reads it. They are typed as the internal serialized
  // structures and point into the memory-mapped buffer below, remaining valid
  // for the lifetime of the view. All are null when the instance is
  // unconfigured, and the edge and string bases are null when no policy
  // declares a nested prefix
  const void *nodes_{nullptr};
  const void *edges_{nullptr};
  const char *strings_{nullptr};

  // The policy table, resolved once at construction, locating each policy's
  // keys. Null when the instance is unconfigured or declares no policies
  const void *policies_{nullptr};
  std::uint32_t policy_count_{0};

  // The parsed claim rules of each policy, in the same order as the table
  // above, null where a policy declares none
  std::vector<sourcemeta::core::JSON> claims_;

  std::unique_ptr<sourcemeta::core::FileView> view_;

  sourcemeta::core::JWKSProvider::Fetcher fetcher_;
  mutable std::mutex jwks_mutex_;
  mutable std::map<std::pair<std::string, std::string>,
                   std::unique_ptr<sourcemeta::core::JWKSProvider>>
      jwks_providers_;

  struct ResolvedEndpoints {
    std::shared_ptr<const sourcemeta::core::OAuthServerMetadata> source;
    Authentication::ProviderEndpoints resolved;
  };

  mutable std::mutex metadata_mutex_;
  mutable std::map<std::string,
                   std::unique_ptr<sourcemeta::core::OAuthMetadataProvider>>
      metadata_providers_;
  mutable std::map<std::string, ResolvedEndpoints> endpoints_;
};

Authentication::Authentication(const std::filesystem::path &path,
                               sourcemeta::core::JWKSProvider::Fetcher fetcher)
    : impl_{std::make_unique<Impl>(path, std::move(fetcher))} {}

Authentication::~Authentication() = default;

auto Authentication::admits(const Authentication::Path &path,
                            const Credentials &credentials) const
    -> Authentication::Verdict {
  return this->impl_->admits(path.value(), credentials.bearer,
                             credentials.cookies);
}

auto Authentication::interactive(const std::string_view name) const
    -> std::optional<Authentication::InteractivePolicy> {
  return this->impl_->interactive(name);
}

auto Authentication::interactive(const Authentication::Path &path,
                                 const std::string_view name) const
    -> std::optional<Authentication::InteractivePolicy> {
  return this->impl_->interactive(path.value(), name);
}

auto Authentication::client_secret(const std::string_view policy) const
    -> std::optional<sourcemeta::core::SecureString> {
  return this->impl_->client_secret(policy);
}

auto Authentication::endpoints(const std::string_view policy) const
    -> std::optional<Authentication::ProviderEndpoints> {
  return this->impl_->endpoints(policy);
}

auto Authentication::open_session(const std::string_view value) const
    -> std::optional<std::string> {
  return this->impl_->open_session(value);
}

// A provider answering twice about one person is two halves of one account,
// but only one of them is signed. The address pair is carved out of the merge
// because `email_verified` speaks for the address delivered with it, so the
// pair is only ever taken from an answer that carried the address, and an
// assertion left on its own is dropped whichever answer it came from
auto Authentication::combine_claims(const sourcemeta::core::JSON &token,
                                    const sourcemeta::core::JSON &extra)
    -> sourcemeta::core::JSON {
  if (!token.is_object()) {
    return extra.is_object() ? extra : token;
  }

  // Nothing to combine leaves what the token said untouched, since an
  // assertion it carried alone can vouch for no address but its own
  if (!extra.is_object()) {
    return token;
  }

  auto result{token};
  const auto token_has_address{token.defines("email")};
  const auto extra_has_address{extra.defines("email")};
  if (!token_has_address) {
    result.erase("email_verified");
  }

  for (const auto &claim : extra.as_object()) {
    const auto address_pair{claim.first == "email" ||
                            claim.first == "email_verified"};
    // The answer that carried the address carries the assertion about it, so
    // neither half is taken from an answer holding only one of them
    if (address_pair && (token_has_address || !extra_has_address)) {
      continue;
    }

    if (!result.defines(claim.first)) {
      result.assign(claim.first, claim.second);
    }
  }

  return result;
}

auto Authentication::object_shaped_claims(
    const std::string_view policy, const sourcemeta::core::JSON &claims) const
    -> std::vector<std::string_view> {
  return this->impl_->object_shaped_claims(policy, claims);
}

auto Authentication::admits_identity(const std::string_view policy,
                                     const sourcemeta::core::JSON &claims) const
    -> Authentication::Admission {
  return this->impl_->admits_identity(policy, claims);
}

auto Authentication::seal(const std::string_view policy, const Purpose purpose,
                          const std::string_view payload,
                          const std::chrono::sys_seconds expiry) const
    -> std::optional<std::string> {
  return this->impl_->seal(policy, purpose, payload, expiry);
}

auto Authentication::open(const std::string_view policy, const Purpose purpose,
                          const std::string_view value) const
    -> std::optional<std::string> {
  return this->impl_->open(policy, purpose, value);
}

auto Authentication::admits_route(
    const std::string_view target, const std::string_view base_path,
    const Credentials &credentials,
    const std::string_view required_audience) const -> Authentication::Verdict {
  return this->impl_->admits(strip_base_path(target, base_path),
                             credentials.bearer, credentials.cookies,
                             required_audience);
}

auto Authentication::governing(const Authentication::Path &path) const
    -> std::vector<std::size_t> {
  auto mask{this->impl_->match(path.value())};
  std::vector<std::size_t> result;
  while (mask != 0) {
    result.push_back(static_cast<std::size_t>(std::countr_zero(mask)));
    mask &= mask - 1;
  }

  return result;
}

auto Authentication::reference_permitted(
    const Authentication::Path &referrer,
    const Authentication::Path &referent) const -> bool {
  return this->impl_->reference_permitted(referrer.value(), referent.value());
}

} // namespace sourcemeta::one
