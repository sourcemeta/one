#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/uri.h>

#include "authentication_format.h"
#include "session.h"

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
// A browser holds one session, whichever policy established it, and the policy
// travels inside the sealed value rather than in the name. One name means
// signing out has a single thing to end, and means a caller cannot choose which
// policy a value is read as by choosing what to call it
constexpr std::string_view SESSION_COOKIE{"sourcemeta_one_session"};

// A login transaction follows the same shape for the short window between the
// login redirect and the callback
constexpr std::string_view TRANSACTION_COOKIE{"sourcemeta_one_transaction"};

// Names the policy a browser last signed in under, so that a denial can ask the
// provider whether that sign-in still stands rather than asking the person
// again. It outlives a session, since it is only of use once one has expired,
// and it carries no credential: whoever holds it can start a login they were
// free to start anyway
constexpr std::string_view RENEWAL_COOKIE{"sourcemeta_one_renewal"};

// An instance names an origin and nothing more, so every location it serves is
// below the root, and a cookie scoped there travels to all of them
constexpr std::string_view COOKIE_PATH{"/"};

// Where a provider says its endpoints are. The values are copies, so they stay
// usable across a refresh of what the provider last said
struct ProviderEndpoints {
  std::string authorization{};
  std::string token{};
  std::string jwks_uri{};
  // Absent from a provider that does not offer to end its own session
  std::string end_session{};
  // Where a provider answers for the claims a scope requested, which under the
  // authorization code flow is where they arrive by default rather than in the
  // token itself
  std::string userinfo{};
  // Whether the provider takes the client secret in an authorization header
  // rather than in the request body
  bool token_endpoint_basic_auth{true};
  // Whether the provider honours the claims request parameter, which is the
  // standard way to ask for a claim no standard scope carries
  bool claims_parameter_supported{false};
  // The claims the provider says it may be able to supply. OpenID Connect
  // Discovery Section 3 calls this list non-exhaustive, so a claim missing from
  // it is worth reporting and never worth refusing over, and a provider
  // publishing none says nothing at all
  std::vector<std::string> claims_supported{};
};

// A provider answering twice about one person is two halves of one account,
// but only one of them is signed. The address pair is carved out of the merge
// because `email_verified` speaks for the address delivered with it, so the
// pair is only ever taken from an answer that carried the address, and an
// assertion left on its own is dropped whichever answer it came from
auto combine_claims(const sourcemeta::core::JSON &token,
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

// How long a login has to come back before the transaction that started it
// stops opening
constexpr std::chrono::seconds TRANSACTION_LIFETIME{std::chrono::minutes{10}};

// What an interactive policy declares about its provider client. The views
// point into the artifact and remain valid for as long as it is mapped
struct InteractivePolicy {
  std::string_view issuer{};
  std::string_view client_id{};
  // The first registry path the policy governs
  std::string_view default_path{};
  // The claims a person must carry to be admitted, serialised as the member map
  // of an OpenID Connect claims request parameter. Empty where the policy names
  // no rule
  std::string_view claims{};
  // The email domains that admit a person. A login asks its provider for an
  // address whenever this is non-empty, since a rule it cannot read admits
  // nobody
  std::vector<std::string_view> email_domains{};
};

// The claims a policy's rules speak about, each asked for as essential, so that
// a provider is told what is actually needed rather than being left to guess
// from a scope. A domain rule reads an address, so it asks for the pair OpenID
// Connect Core Section 5.1 defines for one
auto wanted_claims(const InteractivePolicy &policy,
                   const std::optional<sourcemeta::core::JSON> &rules)
    -> std::vector<sourcemeta::core::OIDCClaimRequest> {
  std::vector<sourcemeta::core::OIDCClaimRequest> result;
  if (!policy.email_domains.empty()) {
    result.push_back({.name = "email", .essential = true});
    result.push_back({.name = "email_verified", .essential = true});
  }

  if (!rules.has_value() || !rules.value().is_object()) {
    return result;
  }

  for (const auto &rule : rules.value().as_object()) {
    result.push_back({.name = rule.first, .essential = true});
  }

  return result;
}

// The scope a login asks for. Every request carries `openid`, and a claim one
// of the standard scopes carries adds that scope, which is the only mapping a
// specification defines. A claim outside them adds nothing, since a scope this
// invented could be refused outright by the provider
auto requested_scope(
    const std::vector<sourcemeta::core::OIDCClaimRequest> &wanted,
    std::string &sink) -> void {
  sink += "openid";
  std::vector<std::string_view> scopes;
  for (const auto &claim : wanted) {
    const auto scope{sourcemeta::core::oidc_claim_to_scope(claim.name)};
    if (scope.has_value() && scope.value() != "openid" &&
        std::ranges::find(scopes, scope.value()) == scopes.cend()) {
      scopes.push_back(scope.value());
    }
  }

  std::ranges::sort(scopes);
  for (const auto scope : scopes) {
    sink += " ";
    sink += scope;
  }
}

// A rule naming a claim the provider never sends is one that can only ever
// deny, and nothing in the exchange would say so: the login succeeds, the token
// arrives, and admission fails for a reason nobody can see. So the provider's
// own account of what it may supply is compared against what the rules ask for,
// and a gap is named where an operator will find it.
//
// This reports and never refuses. OpenID Connect Discovery Section 3 says the
// list "might not be an exhaustive list", so a claim missing from it is a hint
// rather than a verdict, and a provider publishing no list at all is saying
// nothing rather than saying no
auto report_unadvertised_claims(
    const std::vector<sourcemeta::core::OIDCClaimRequest> &wanted,
    const ProviderEndpoints &endpoints, const std::string_view policy_name,
    std::vector<std::string> &log) -> void {
  if (endpoints.claims_supported.empty()) {
    return;
  }

  // Anybody at all may start a login, so saying this on every attempt would
  // leave a stranger able to bury everything else in the log. What it says
  // concerns a policy and its provider rather than the attempt that surfaced
  // it, so saying it once says all of it
  static std::mutex mutex;
  static std::set<std::string, std::less<>> reported;

  for (const auto &claim : wanted) {
    if (std::ranges::find(endpoints.claims_supported, claim.name) !=
        endpoints.claims_supported.cend()) {
      continue;
    }

    std::string subject{claim.name};
    subject += " of the policy ";
    subject += policy_name;
    const std::scoped_lock guard{mutex};
    if (!reported.insert(subject).second) {
      continue;
    }

    std::string message{"The provider does not advertise a claim a rule "
                        "requires, so the rule may never match. The claim is "};
    message += subject;
    log.push_back(std::move(message));
  }
}

// The signature algorithms an identity token may be signed with: every
// asymmetric one, since a provider picks from these and an instance that named
// a narrower set would refuse a provider it could otherwise serve, after the
// person had already signed in. The symmetric ones are left out deliberately.
// They sign with the client secret rather than a key from the provider's
// published set, so admitting them alongside the rest is the shape that lets
// one algorithm be verified as though it were another
constexpr std::array<sourcemeta::core::JWSAlgorithm, 10> ID_TOKEN_ALGORITHMS{
    {sourcemeta::core::JWSAlgorithm::RS256,
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
constexpr std::chrono::seconds ID_TOKEN_CLOCK_SKEW{60};

// A session lasts an hour, kept short so that a lost cookie cannot outlive its
// usefulness, with silent re-authentication as the eventual refresh
constexpr std::chrono::seconds SESSION_LIFETIME{3600};

// How long a browser stays eligible for a silent renewal after signing in.
// Long enough to outlast a provider session, since the provider is the one that
// decides whether a renewal succeeds, and losing it early only costs a sign-in
// page that would otherwise have been skipped
constexpr std::chrono::seconds RENEWAL_LIFETIME{43200};

// RFC 6265 Section 6.1 asks a user agent to support "at least 4096 bytes per
// cookie (as measured by the sum of the length of the cookie's name, value, and
// attributes)". That is a floor they should honour rather than a ceiling they
// must enforce, and what happens above it is left unsaid, so the whole
// serialised cookie is kept under it with room to spare
constexpr std::size_t MAXIMUM_COOKIE_LENGTH{4000};

// Address a section of the artifact wherever it was read from, so that mapping
// a file and holding the bytes outright reach the same structures
template <typename T>
auto at_offset(const std::span<const std::byte> bytes,
               const std::size_t offset = 0) noexcept -> const T * {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const T *>(bytes.data() + offset);
}

auto structurally_valid(const std::span<const std::byte> bytes) noexcept
    -> bool {
  using namespace sourcemeta::one;
  const auto size{bytes.size()};
  if (size < sizeof(AuthenticationHeader)) {
    return false;
  }

  const auto *header{at_offset<AuthenticationHeader>(bytes)};
  // The policy count must fit the bitmask width, otherwise matching would
  // shift past the width of a PolicySet, which is undefined behavior
  if (header->magic != AUTHENTICATION_MAGIC ||
      header->version != AUTHENTICATION_VERSION || header->node_count == 0 ||
      header->policy_count > AUTHENTICATION_MAXIMUM_POLICIES) {
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
  const auto views_offset{nodes_offset + nodes_bytes};
  const auto views_bytes{static_cast<std::size_t>(header->view_count) *
                         sizeof(AuthenticationViewEntry)};
  const auto edges_offset{views_offset + views_bytes};
  const auto edges_bytes{static_cast<std::size_t>(header->edge_count) *
                         sizeof(AuthenticationEdge)};
  const auto strings_offset{edges_offset + edges_bytes};
  const auto strings_length{static_cast<std::size_t>(header->strings_length)};
  if (header->policies_offset != policies_offset ||
      header->nodes_offset != nodes_offset ||
      header->views_offset != views_offset ||
      header->edges_offset != edges_offset ||
      header->strings_offset != strings_offset ||
      strings_offset + strings_length > size) {
    return false;
  }

  // A caller satisfying nothing is placed somewhere, so a table that could not
  // name that is one no request could be resolved against. Every table names at
  // least that view, so a table with nothing to name it from is one whose names
  // would be read against no bytes at all
  if (header->view_count == 0 || strings_length == 0) {
    return false;
  }

  const auto *views{
      at_offset<AuthenticationViewEntry>(bytes, header->views_offset)};
  bool names_the_anonymous{false};
  for (std::uint32_t index{0}; index < header->view_count; index += 1) {
    const auto &entry{views[index]};
    // A name is read as a range into the string section, so an empty one would
    // be served as a directory that is not there
    if (entry.name_length == 0 || entry.name_offset > strings_length ||
        entry.name_length > strings_length - entry.name_offset) {
      return false;
    }

    // A view naming a policy the artifact does not carry could never be
    // resolved to, and on a full mask would shift past the width of a set
    if (header->policy_count < AUTHENTICATION_MAXIMUM_POLICIES &&
        (entry.policies >> header->policy_count) != 0) {
      return false;
    }

    names_the_anonymous = names_the_anonymous || entry.policies == 0;
  }

  if (!names_the_anonymous) {
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
        at_offset<AuthenticationPolicyEntry>(bytes, header->policies_offset)};
    // Each policy's metadata is appended in declaration order after the string
    // blob, so a valid artifact lays them out contiguously from there onward
    auto metadata_cursor{strings_offset + strings_length};
    for (std::uint32_t index{0}; index < header->policy_count; index += 1) {
      const auto &entry{policies[index]};
      if (entry.metadata_offset != metadata_cursor ||
          entry.metadata_length > size - metadata_cursor ||
          entry.name_offset > strings_length ||
          entry.name_length > strings_length - entry.name_offset ||
          entry.algorithm >
              static_cast<std::uint8_t>(Authentication::Algorithm::Sha256) ||
          entry.type >
              static_cast<std::uint8_t>(AuthenticationPolicyType::OIDC)) {
        return false;
      }

      metadata_cursor += entry.metadata_length;
    }
  }

  const auto *nodes{at_offset<AuthenticationNode>(bytes, header->nodes_offset)};
  for (std::uint32_t index{0}; index < header->node_count; index += 1) {
    const auto &node{nodes[index]};
    if (node.edge_count > header->edge_count ||
        node.first_edge > header->edge_count - node.edge_count) {
      return false;
    }
  }

  if (header->edge_count > 0) {
    const auto *edges{
        at_offset<AuthenticationEdge>(bytes, header->edges_offset)};
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
    -> InteractivePolicy {
  InteractivePolicy result;
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

struct Authentication::Table::Impl {
  // The indexer always emits this artifact. A missing, unreadable, or
  // malformed file means it was deleted, corrupted, or produced by an older
  // indexer. Rather than failing open and serving every path publicly, or
  // crashing the server into a restart loop, leave the policy denying
  // everything: the section pointers below stay null, so matching yields the
  // empty set and admits no one. Opening the file covers the missing and
  // unreadable cases without a separate, throwing existence check
  explicit Impl(const std::filesystem::path &path) {
    std::unique_ptr<sourcemeta::core::FileView> view;
    try {
      view = std::make_unique<sourcemeta::core::FileView>(path);
    } catch (const sourcemeta::core::FileViewError &) {
      return;
    }

    if (this->adopt({view->as<std::byte>(), view->size()})) {
      this->view_ = std::move(view);
    }
  }

  // A table compiled in this process is held rather than mapped, so the bytes
  // are copied once and never move again, which is what keeps every section
  // pointer below valid for as long as this exists
  explicit Impl(const std::span<const std::byte> bytes) {
    this->owned_.assign(bytes.begin(), bytes.end());
    if (!this->adopt(this->owned_)) {
      this->owned_.clear();
    }
  }

  // Whether the artifact could be read, and every section pointer set where it
  // could. Nothing is set on a refusal, so a caller that ignores the answer
  // still denies everything
  auto adopt(const std::span<const std::byte> bytes) -> bool {
    if (!structurally_valid(bytes)) {
      return false;
    }

    const auto *header{at_offset<AuthenticationHeader>(bytes)};

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
        at_offset<AuthenticationPolicyEntry>(bytes, header->policies_offset)};
    for (std::uint32_t index{0}; index < header->policy_count; index += 1) {
      const auto &entry{policies[index]};
      const auto type{static_cast<AuthenticationPolicyType>(entry.type)};
      if ((type != AuthenticationPolicyType::JWT &&
           type != AuthenticationPolicyType::OIDC) ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(bytes, entry.metadata_offset),
          entry.metadata_length};
      std::string_view serialized;
      if (type == AuthenticationPolicyType::JWT) {
        if (!read_jwt_claims(metadata, serialized)) {
          return false;
        }
      } else {
        OIDCPolicyMetadata decoded;
        if (!decode_oidc_metadata(metadata, decoded)) {
          return false;
        }

        serialized = decoded.claims;
      }

      if (serialized.empty()) {
        continue;
      }

      auto document{sourcemeta::core::try_parse_json(serialized)};
      if (!document.has_value() || !document.value().is_object()) {
        return false;
      }

      claims[index] = std::move(document).value();
    }

    this->nodes_ = at_offset<AuthenticationNode>(bytes, header->nodes_offset);
    // The edge section is empty when no policy declares a nested prefix, in
    // which case it sits at the end of the buffer and must not be addressed
    if (header->edge_count > 0) {
      this->edges_ = at_offset<AuthenticationEdge>(bytes, header->edges_offset);
    }

    // Every name is a range into the string section, so it is addressed
    // whenever anything was named rather than only when a prefix was nested
    if (header->strings_length > 0) {
      this->strings_ = at_offset<char>(bytes, header->strings_offset);
    }

    this->views_ =
        at_offset<AuthenticationViewEntry>(bytes, header->views_offset);
    this->view_count_ = header->view_count;

    if (header->policy_count > 0) {
      this->policies_ =
          at_offset<AuthenticationPolicyEntry>(bytes, header->policies_offset);
      this->policy_count_ = header->policy_count;
    }

    this->claims_ = std::move(claims);
    this->bytes_ = bytes;
    return true;
  }

  // The transaction a callback belongs to, if the request carries one. A
  // request can present several cookies under one name, since a parent domain
  // and the host itself can each set one and neither the header nor the order
  // says which is which, so every value is tried. Letting whoever controls a
  // neighbouring host decide which transaction this reads is what turns the
  // cookie from a defence against a forged callback into the way to mount one
  [[nodiscard]] auto transaction(const std::string_view policy_name,
                                 const std::string_view state,
                                 const std::string_view redirect_uri,
                                 const Credentials &credentials) const
      -> std::optional<sourcemeta::core::JSON> {
    if (state.empty()) {
      return std::nullopt;
    }

    std::vector<std::string_view> candidates;
    for (const auto field : credentials.cookies) {
      sourcemeta::core::http_cookie_values(field, TRANSACTION_COOKIE,
                                           candidates);
    }

    for (const auto sealed : candidates) {
      auto opened{this->open(policy_name, SealPurpose::Transaction, sealed)};
      if (!opened.has_value()) {
        continue;
      }

      auto document{sourcemeta::core::try_parse_json(opened.value())};
      if (!document.has_value() || !document.value().is_object()) {
        continue;
      }

      const auto *sealed_policy{document.value().try_at("policy")};
      const auto *sealed_state{document.value().try_at("state")};
      const auto *sealed_redirect{document.value().try_at("redirect_uri")};
      const auto *nonce{document.value().try_at("nonce")};
      const auto *verifier{document.value().try_at("verifier")};
      if (sealed_policy == nullptr || !sealed_policy->is_string() ||
          sealed_policy->to_string() != policy_name ||
          sealed_state == nullptr || !sealed_state->is_string() ||
          sealed_state->to_string() != state || sealed_redirect == nullptr ||
          !sealed_redirect->is_string() ||
          sealed_redirect->to_string() != redirect_uri || nonce == nullptr ||
          !nonce->is_string() || nonce->to_string().empty() ||
          verifier == nullptr || !verifier->is_string() ||
          verifier->to_string().empty()) {
        continue;
      }

      return document;
    }

    return std::nullopt;
  }

  // The session cookie for a login, carrying the identity token when one is
  // given. Building it is separated out so the caller can measure the result
  // and ask for a smaller one
  [[nodiscard]] auto session_cookie(const std::string_view policy_name,
                                    const std::string_view subject,
                                    const std::string_view id_token,
                                    const std::chrono::sys_seconds expiry,
                                    const bool secure,
                                    std::vector<std::string> &log) const
      -> std::optional<std::string> {
    auto payload{sourcemeta::core::JSON::make_object()};
    payload.assign_assume_new("policy",
                              sourcemeta::core::JSON{std::string{policy_name}});
    payload.assign_assume_new("subject",
                              sourcemeta::core::JSON{std::string{subject}});
    if (!id_token.empty()) {
      payload.assign_assume_new("id_token",
                                sourcemeta::core::JSON{std::string{id_token}});
    }

    std::ostringstream payload_text;
    sourcemeta::core::stringify(payload, payload_text);
    const auto sealed{this->seal(policy_name, SealPurpose::Session,
                                 payload_text.str(), expiry)};
    if (!sealed.has_value()) {
      log.emplace_back("No session secret is set for the policy");
      return std::nullopt;
    }

    auto cookie{sourcemeta::core::http_serialize_cookie(
        {.name = SESSION_COOKIE,
         .value = sealed.value(),
         .path = COOKIE_PATH,
         .max_age = SESSION_LIFETIME,
         .http_only = true,
         .secure = secure,
         .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
    if (!cookie.has_value()) {
      log.emplace_back("The session could not be put in a cookie, for the "
                       "policy");
    }

    return cookie;
  }

  // A rule compared against a claim carrying objects is compared on the `value`
  // sub-attribute alone, so a rule naming what a person sees rather than what
  // identifies them matches nothing. A denial cannot show that, and the token
  // it concerns is sealed inside a cookie where an operator cannot look, so it
  // is said here. Only a refusal reaches this, so a working policy stays quiet,
  // and each claim is named once however often somebody signs in
  auto report_object_shaped_claims(const std::string_view policy_name,
                                   const sourcemeta::core::JSON &claims,
                                   std::vector<std::string> &log) const
      -> void {
    static std::mutex mutex;
    static std::set<std::string, std::less<>> reported;
    for (const auto claim : this->object_shaped_claims(policy_name, claims)) {
      std::string subject{claim};
      subject += " of the policy ";
      subject += policy_name;
      const std::scoped_lock guard{mutex};
      if (!reported.insert(subject).second) {
        continue;
      }

      std::string message{
          "A rule names a claim the provider answers with objects, which are "
          "compared on their identifier rather than on any name shown to a "
          "person. The claim is "};
      message += subject;
      log.push_back(std::move(message));
    }
  }

  // Which policies govern a location, or nothing at all when the artifact
  // could not be read. A missing or structurally broken one leaves the section
  // pointers null and governs nothing it could answer for, which is why an
  // empty set and an unreadable table are told apart here rather than by
  // whoever asks
  [[nodiscard]] auto governing_mask(const std::string_view path) const
      -> std::optional<Authentication::PolicySet> {
    if (this->nodes_ == nullptr) {
      return std::nullopt;
    }

    return this->match(path);
  }

  // Whether a caller satisfying these policies is shown a location
  [[nodiscard]] auto permits(const std::string_view path,
                             const Authentication::PolicySet policies) const
      -> bool {
    const auto governing{this->governing_mask(path)};
    return governing.has_value() &&
           (governing.value() == 0 || (governing.value() & policies) != 0);
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

  // Whether one policy accepts what a request presented, which is the whole of
  // reading a credential and is asked both of a policy governing a path and of
  // every policy when placing a caller. Two answers to this could disagree,
  // so there is one
  [[nodiscard]] auto
  admits_policy(const std::uint32_t index, const std::string_view credential,
                const std::span<const std::string_view> cookies,
                const std::optional<sourcemeta::core::JWT> &token,
                const std::string_view required_audience) const -> bool {
    const auto &entry{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)[index]};
    std::span<const std::byte> metadata;
    if (entry.metadata_length > 0) {
      metadata = {at_offset<std::byte>(this->bytes_, entry.metadata_offset),
                  entry.metadata_length};
    }

    const auto type{static_cast<AuthenticationPolicyType>(entry.type)};
    if (type == AuthenticationPolicyType::JWT) {
      return token.has_value() &&
             this->admits_jwt(metadata, token.value(), required_audience,
                              this->claims_[index]);
    }

    // An interactive policy authenticates a person through the session its
    // browser login established, never a presented credential. A request that
    // presented one is asking to be read as that credential, so its session is
    // not consulted at all rather than quietly widening what it reaches
    if (type == AuthenticationPolicyType::OIDC) {
      return credential.empty() && this->admits_session(metadata, cookies);
    }

    return admits_apikey(
        metadata, credential,
        static_cast<Authentication::Algorithm>(entry.algorithm));
  }

  // Which policies a caller satisfies, asked of every policy rather than of
  // those governing one path, since a view describes the whole registry
  [[nodiscard]] auto
  classify(const std::string_view credential,
           const std::span<const std::string_view> cookies) const
      -> Authentication::PolicySet {
    // Presenting nothing satisfies nothing, and this is the common request, so
    // it answers without reading a policy at all
    if (this->nodes_ == nullptr || (credential.empty() && cookies.empty())) {
      return 0;
    }

    const auto token{sourcemeta::core::JWT::from(credential)};
    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    Authentication::PolicySet result{0};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      if (!this->admits_policy(index, credential, cookies, token, {})) {
        continue;
      }

      // Only token policies combine, so anything else stands for the caller on
      // its own and the first one reached is the one read
      if (static_cast<AuthenticationPolicyType>(policies[index].type) !=
          AuthenticationPolicyType::JWT) {
        if (result == 0) {
          return Authentication::PolicySet{1} << index;
        }

        continue;
      }

      result |= Authentication::PolicySet{1} << index;
    }

    return result;
  }

  [[nodiscard]] auto view_count() const noexcept -> std::size_t {
    return this->view_count_;
  }

  [[nodiscard]] auto view_at(const std::size_t index) const
      -> Authentication::RecordedView {
    assert(index < this->view_count_);
    const auto &entry{
        static_cast<const AuthenticationViewEntry *>(this->views_)[index]};
    Authentication::RecordedView result;
    result.name_ = {this->strings_ + entry.name_offset, entry.name_length};
    result.policies_ = entry.policies;
    return result;
  }

  // The name a set of policies is served under, read from the recorded table
  // rather than spelled again here
  [[nodiscard]] auto view_name(const Authentication::PolicySet policies) const
      -> std::string_view {
    const auto *views{
        static_cast<const AuthenticationViewEntry *>(this->views_)};
    for (std::uint32_t index{0}; index < this->view_count_; index += 1) {
      if (views[index].policies == policies) {
        return {this->strings_ + views[index].name_offset,
                views[index].name_length};
      }
    }

    // Every set a caller can be placed in is named, since the table holds one
    // entry per policy and one per combination that can be satisfied at once.
    // An unconfigured instance has no table at all, and serves the one view
    // everything is served under
    return VIEW_PUBLIC;
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

    return admits_claims(token.payload(), claims) == Admission::Admitted;
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
      sourcemeta::core::http_cookie_values(field, SESSION_COOKIE, candidates);
    }
    for (const auto sealed : candidates) {
      const auto payload{this->session_open(decoded.session_secrets,
                                            SealPurpose::Session, sealed)};
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
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(this->bytes_, entry.metadata_offset),
          entry.metadata_length};
      OIDCPolicyMetadata decoded;
      if (!decode_oidc_metadata(metadata, decoded) || decoded.name.empty()) {
        continue;
      }

      const auto payload{this->session_open(decoded.session_secrets,
                                            SealPurpose::Session, value)};
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
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(this->bytes_, entry.metadata_offset),
          entry.metadata_length};
      if (decode_oidc_metadata(metadata, result) && result.name == name) {
        return true;
      }
    }

    return false;
  }

  [[nodiscard]] auto interactive(const std::string_view name) const
      -> std::optional<InteractivePolicy> {
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
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      OIDCPolicyMetadata decoded;
      if (!decode_oidc_metadata(
              {at_offset<std::byte>(this->bytes_, entry.metadata_offset),
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
        // A scope is read as one space-delimited string rather than compared
        // member by member, so one arriving as anything else is refused rather
        // than compared on an identifier, and saying otherwise would send an
        // operator looking for the wrong mistake
        if (rule.first == "scope") {
          continue;
        }

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
      -> Admission {
    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      OIDCPolicyMetadata decoded;
      if (!decode_oidc_metadata(
              {at_offset<std::byte>(this->bytes_, entry.metadata_offset),
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
        return Admission::Refused;
      }

      const auto address{
          domains > 0 ? admits_email_domain(claims, decoded.email_domains)
                      : Admission::Admitted};
      if (address == Admission::Refused) {
        return address;
      }

      const auto rules{admits_claims(claims, this->claims_[index])};
      if (rules == Admission::Refused) {
        return rules;
      }

      // Either half wanting more is the whole wanting more
      return rules == Admission::Incomplete ? rules : address;
    }

    // A name that no interactive policy answers to could never have minted a
    // session, so nothing it asserts is admitted
    return Admission::Refused;
  }

  [[nodiscard]] auto interactive(const std::string_view path,
                                 const std::string_view name) const
      -> std::optional<InteractivePolicy> {
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
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(this->bytes_, entry.metadata_offset),
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
                                  const SealPurpose purpose,
                                  const std::string_view payload,
                                  const std::chrono::sys_seconds expiry) const
      -> std::optional<std::string> {
    const auto resolved{session_secrets(variables)};
    if (resolved.empty()) {
      return std::nullopt;
    }

    const auto issued{std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now())};
    return seal_value(payload, purpose, resolved.front(), issued, expiry);
  }

  [[nodiscard]] auto session_open(const std::span<const std::byte> variables,
                                  const SealPurpose purpose,
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
    return open_value(value, purpose, secrets, now);
  }

  [[nodiscard]] auto seal(const std::string_view policy,
                          const SealPurpose purpose,
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
                          const SealPurpose purpose,
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
      -> std::optional<ProviderEndpoints> {
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
            [fetcher = this->key_fetcher()](const std::string_view url)
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

      ProviderEndpoints resolved;
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

      const auto metadata{this->retrieve(url.value())};
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
        std::move(location), this->key_fetcher(), options)};

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
            at_offset<std::byte>(this->bytes_, entry.metadata_offset),
            entry.metadata_length};
        const auto type{static_cast<AuthenticationPolicyType>(entry.type)};
        if (type == AuthenticationPolicyType::JWT) {
          collect_jwt_identifiers(metadata, result.keys);
        } else if (type == AuthenticationPolicyType::OIDC) {
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

  // The view table, computed where the policies were read and only looked up
  // here, so that what a build wrote and what this serves cannot disagree
  const void *views_{nullptr};
  std::uint32_t view_count_{0};

  // The parsed claim rules of each policy, in the same order as the table
  // above, null where a policy declares none
  std::vector<sourcemeta::core::JSON> claims_;

  // Whichever of the two holds the bytes every section above points into. A
  // mapped table keeps the mapping, a compiled one keeps its own copy, and the
  // span is how anything reads them without caring which
  std::unique_ptr<sourcemeta::core::FileView> view_;
  std::vector<std::byte> owned_;
  std::span<const std::byte> bytes_{};

  Authentication::Fetcher fetcher_;

  [[nodiscard]] auto fetcher() const -> const Authentication::Fetcher & {
    return this->fetcher_;
  }

  // A plain retrieval, which is what discovery and a key set both are
  [[nodiscard]] auto retrieve(const std::string_view url) const
      -> std::optional<Authentication::ProviderResponse> {
    if (!this->fetcher_) {
      return std::nullopt;
    }

    auto response{this->fetcher_({.url = url})};
    if (!response.has_value() || response.value().status < 200 ||
        response.value().status >= 300) {
      return std::nullopt;
    }

    return response;
  }

  // What a key set is retrieved with, which is the one fetcher asked for a
  // plain retrieval. Adapting here rather than holding two keeps every
  // outbound call this makes going through the same place
  [[nodiscard]] auto key_fetcher() const
      -> sourcemeta::core::JWKSProvider::Fetcher {
    return [fetcher = this->fetcher_](const std::string_view url)
               -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
      auto response{fetcher({.url = url})};
      if (!response.has_value() || response.value().status < 200 ||
          response.value().status >= 300) {
        return std::nullopt;
      }

      const auto max_age{response.value().max_age};
      return sourcemeta::core::JWKSProvider::FetchResult{
          .body = std::move(response).value().body, .max_age = max_age};
    };
  }

  mutable std::mutex jwks_mutex_;
  mutable std::map<std::pair<std::string, std::string>,
                   std::unique_ptr<sourcemeta::core::JWKSProvider>>
      jwks_providers_;

  struct ResolvedEndpoints {
    std::shared_ptr<const sourcemeta::core::OAuthServerMetadata> source;
    ProviderEndpoints resolved;
  };

  mutable std::mutex metadata_mutex_;
  mutable std::map<std::string,
                   std::unique_ptr<sourcemeta::core::OAuthMetadataProvider>>
      metadata_providers_;
  mutable std::map<std::string, ResolvedEndpoints> endpoints_;
};

Authentication::Table::Table(const std::filesystem::path &path)
    : impl_{std::make_unique<Impl>(path)} {}

Authentication::Table::Table(const std::span<const std::byte> bytes)
    : impl_{std::make_unique<Impl>(bytes)} {}

Authentication::Table::~Table() = default;

Authentication::Table::Table(Authentication::Table &&) noexcept = default;

auto Authentication::Table::operator=(Authentication::Table &&) noexcept
    -> Authentication::Table & = default;

// The table is taken rather than borrowed, and the fetcher is installed on it,
// so that what performs the protocol and what the protocol reads cannot come
// apart for as long as either exists
Authentication::Authentication(Authentication::Table &&table,
                               Authentication::Fetcher fetcher)
    : table_{std::move(table)} {
  this->table_.impl_->fetcher_ = std::move(fetcher);
}

auto Authentication::table() const noexcept -> const Authentication::Table & {
  return this->table_;
}

Authentication::~Authentication() = default;

namespace {

// A view is spelled as the names of the policies it comprises, ordered so that
// one combination has one spelling whatever order they were declared in
auto view_name(const std::span<const Authentication::Policy> policies,
               const std::span<const std::size_t> members) -> std::string {
  std::vector<std::string_view> names;
  names.reserve(members.size());
  for (const auto member : members) {
    names.emplace_back(policies[member].name);
  }

  std::ranges::sort(names);
  std::string result;
  for (const auto name : names) {
    if (!result.empty()) {
      result += '+';
    }

    result += name;
  }

  return result;
}

} // namespace

auto Authentication::Table::enumerate(
    const std::span<const Authentication::Policy> policies)
    -> std::vector<Authentication::Table::View> {
  std::vector<Authentication::Table::View> result;
  // Always present, and the only entry when nothing is declared. A registry
  // whose every path is governed still has one, since it is what a caller
  // holding nothing is shown and what a lookup falls back to
  result.push_back({.name = std::string{VIEW_PUBLIC}, .policies = {}});

  // A credential carries one issuer and is checked against it before any rule
  // is read, so only token policies declared against the same issuer can ever
  // be satisfied together. Every other policy stands alone, since a caller
  // presents one key or holds one session
  // A view is spelled from the names of the policies it comprises, so a policy
  // without a name leaves a view naming nowhere, and one sharing a name with
  // another, or taking the name a caller holding nothing is served under,
  // leaves two views under one name. A configuration is refused for all three
  // before any of this is reached, so these say what has been established
  assert(std::ranges::none_of(policies, [](const auto &policy) -> bool {
    return policy.name.empty() || policy.name == VIEW_PUBLIC;
  }));
  assert(std::ranges::all_of(policies, [&policies](const auto &policy) -> bool {
    return std::ranges::count(policies, policy.name,
                              &Authentication::Policy::name) == 1;
  }));

  std::vector<std::string_view> issuers;
  std::vector<std::vector<std::size_t>> groups;
  std::vector<Authentication::Table::View> named;

  for (std::size_t index{0}; index < policies.size(); index++) {
    const auto &policy{policies[index]};
    const auto *token{
        std::get_if<Authentication::Policy::Token>(&policy.credential)};
    if (token == nullptr) {
      named.push_back({.name = std::string{policy.name}, .policies = {index}});
      continue;
    }

    const auto match{std::ranges::find(issuers, token->issuer)};
    if (match == issuers.cend()) {
      issuers.emplace_back(token->issuer);
      groups.push_back({index});
    } else {
      groups[static_cast<std::size_t>(match - issuers.begin())].push_back(
          index);
    }
  }

  for (std::size_t group{0}; group < groups.size(); group++) {
    const auto &members{groups[group]};
    if (members.size() > Authentication::Table::MAXIMUM_COMBINABLE_POLICIES) {
      throw AuthenticationTooManyViewsError(std::string{issuers[group]},
                                            members.size());
    }

    // Every non-empty combination of the group, since a credential satisfying
    // several of them is shown what all of them admit
    const auto total{std::uint64_t{1} << members.size()};
    for (std::uint64_t mask{1}; mask < total; mask++) {
      std::vector<std::size_t> combination;
      for (std::size_t offset{0}; offset < members.size(); offset++) {
        if ((mask & (std::uint64_t{1} << offset)) != 0) {
          combination.push_back(members[offset]);
        }
      }

      named.push_back({.name = view_name(policies, combination),
                       .policies = std::move(combination)});
    }
  }

  std::ranges::sort(named, [](const auto &left, const auto &right) -> bool {
    return left.name < right.name;
  });
  result.insert(result.cend(), std::make_move_iterator(named.begin()),
                std::make_move_iterator(named.end()));
  return result;
}

namespace {

// Whether a value is somewhere on this instance rather than somewhere else, so
// that what a login sealed cannot become a redirect to another origin
auto is_local_path(const std::string_view value) -> bool {
  if (value.empty() || value.front() != '/') {
    return false;
  }

  if (value.size() >= 2 && (value[1] == '/' || value[1] == '\\')) {
    return false;
  }

  for (const auto character : value) {
    const auto code{static_cast<unsigned char>(character)};
    if (code <= 0x20 || code == 0x7f || character == '\\') {
      return false;
    }
  }

  return true;
}

// What redeeming an authorization code yields, of which only the identity token
// decides anything. The access token comes along solely so that a claim missing
// from that token can be asked for at the UserInfo endpoint
struct Grant {
  std::string id_token;
  std::string access_token;
};

// RFC 6749 Section 2.3.1 has every server accept the client secret in an
// authorization header, and asks that carrying it in the request body be
// limited to clients that cannot send one. A body is the part of a request that
// logging and proxies keep, while an authorization header is the part they
// already know to redact, so the header is used wherever the provider takes it
auto exchange(const sourcemeta::one::Authentication::Fetcher &fetcher,
              const std::string_view token_endpoint,
              const std::string_view client_id,
              const sourcemeta::core::SecureString &client_secret,
              const std::string_view redirect_uri, const std::string_view code,
              const std::string_view code_verifier, const bool basic_auth)
    -> std::optional<Grant> {
  if (!fetcher) {
    return std::nullopt;
  }

  sourcemeta::one::Authentication::ProviderRequest request{.url =
                                                               token_endpoint};
  sourcemeta::core::oauth_build_token_request_code(
      code, redirect_uri, code_verifier, {}, request.body);
  if (basic_auth) {
    sourcemeta::core::oauth_client_secret_basic(client_id, client_secret,
                                                request.authorization);
  } else {
    sourcemeta::core::oauth_client_secret_post(client_id, client_secret,
                                               request.body);
  }

  const auto result{fetcher(std::move(request))};
  if (!result.has_value() || result.value().status < 200 ||
      result.value().status >= 300) {
    return std::nullopt;
  }

  const auto document{sourcemeta::core::try_parse_json(result.value().body)};
  if (!document.has_value()) {
    return std::nullopt;
  }

  auto identity{sourcemeta::core::oidc_parse_id_token(document.value())};
  if (!identity.has_value()) {
    return std::nullopt;
  }

  // The access token is kept only so that the UserInfo endpoint can be asked
  // for a claim the identity token did not carry. It is never stored, never
  // logged, and never leaves this exchange
  Grant grant;
  grant.id_token = std::move(identity).value();
  const sourcemeta::core::OAuthTokenResponse response{document.value()};
  if (response.access_token().has_value()) {
    grant.access_token = response.access_token().value();
  }

  return grant;
}

// What a provider answers at its UserInfo endpoint, which under the
// authorization code flow is where the claims a scope requested arrive by
// default (OpenID Connect Core Section 5.4).
//
// OpenID Connect Core Section 5.3.2 requires the subject it returns to match
// the one the identity token asserted, and to be checked before anything it
// says is used. Without that a response about somebody else would be read as
// being about this person, which is the whole of what this is for
auto userinfo(const sourcemeta::one::Authentication::Fetcher &fetcher,
              const std::string_view endpoint,
              const std::string_view access_token,
              const std::string_view subject, std::vector<std::string> &log)
    -> std::optional<sourcemeta::core::JSON> {
  if (!fetcher || access_token.empty()) {
    return std::nullopt;
  }

  std::string authorization;
  if (!sourcemeta::core::oauth_bearer_header(access_token, authorization)) {
    return std::nullopt;
  }

  // An access token is a secret, so what carries it travels in wiping storage
  // even though what built it did not
  sourcemeta::one::Authentication::ProviderRequest request{.url = endpoint};
  request.authorization.append(authorization);

  const auto result{fetcher(std::move(request))};
  if (!result.has_value() || result.value().status < 200 ||
      result.value().status >= 300) {
    return std::nullopt;
  }

  auto document{sourcemeta::core::try_parse_json(result.value().body)};
  if (!document.has_value() || !document.value().is_object()) {
    return std::nullopt;
  }

  const auto *asserted{document.value().try_at("sub")};
  if (asserted == nullptr || !asserted->is_string() ||
      asserted->to_string() != subject) {
    log.emplace_back("The UserInfo endpoint answered about a different subject "
                     "than the identity token did");
    return std::nullopt;
  }

  return document;
}

auto id_token_keys(std::string location,
                   sourcemeta::core::JWKSProvider::Fetcher fetcher)
    -> sourcemeta::core::JWKSProvider {
  return sourcemeta::core::JWKSProvider{std::move(location),
                                        std::move(fetcher),
                                        {.clock_skew = ID_TOKEN_CLOCK_SKEW}};
}

// Expire a cookie under the attributes it was minted with, so the browser

// replaces it rather than keeping a second one of the same name beside it
auto expired_cookie(const std::string_view name, const bool secure)
    -> std::optional<std::string> {
  return sourcemeta::core::http_serialize_cookie(
      {.name = name,
       .value = "",
       .path = COOKIE_PATH,
       .max_age = std::chrono::seconds{0},
       .http_only = true,
       .secure = secure,
       .same_site = sourcemeta::core::HTTPCookieSameSite::Lax});
}

} // namespace

auto Authentication::login(const std::string_view policy_name,
                           const std::string_view instance_url,
                           const std::string_view redirect_uri,
                           const bool silent,
                           const std::string_view return_to) const
    -> Authentication::Outcome {
  Authentication::Outcome result;

  // An unknown or non-interactive policy name reveals nothing
  const auto policy{this->table_.impl_->interactive(policy_name)};
  if (!policy.has_value()) {
    result.result = Authentication::Outcome::Result::Missing;
    return result;
  }

  // Starting a login that cannot be completed only strands the person at the
  // provider, so the secret the exchange will need is required up front
  if (!this->table_.impl_->client_secret(policy_name).has_value()) {
    result.log.emplace_back("No client secret is set for the policy");
    return result;
  }

  const auto endpoints{this->table_.impl_->endpoints(policy_name)};
  if (!endpoints.has_value() || endpoints.value().authorization.empty()) {
    result.log.emplace_back("The provider named no authorization endpoint, or "
                            "could not be reached, for the policy");
    return result;
  }

  const auto secrets{sourcemeta::core::oauth_transaction_mint()};
  const std::string_view state{secrets.state.data(), secrets.state.size()};
  const std::string_view verifier{secrets.code_verifier.data(),
                                  secrets.code_verifier.size()};
  const auto nonce_token{sourcemeta::core::oidc_nonce()};
  const std::string_view nonce{nonce_token.data(), nonce_token.size()};

  auto payload{sourcemeta::core::JSON::make_object()};
  payload.assign_assume_new("policy",
                            sourcemeta::core::JSON{std::string{policy_name}});
  if (silent) {
    payload.assign_assume_new("silent", sourcemeta::core::JSON{true});
  }

  payload.assign_assume_new("state", sourcemeta::core::JSON{state});
  payload.assign_assume_new("nonce", sourcemeta::core::JSON{nonce});
  payload.assign_assume_new("verifier", sourcemeta::core::JSON{verifier});
  // Sealed so that a callback completing this login has to name the same place
  // the provider was told to come back to. The provider checks it too, and this
  // is what lets the check hold before a code is ever redeemed
  payload.assign_assume_new("redirect_uri",
                            sourcemeta::core::JSON{std::string{redirect_uri}});

  // Where the browser goes once this completes. What the request asked for
  // wins, and what the policy governs stands in where it asked for nothing
  if (!return_to.empty()) {
    payload.assign_assume_new("to",
                              sourcemeta::core::JSON{std::string{return_to}});
  } else if (!policy->default_path.empty()) {
    payload.assign_assume_new(
        "to", sourcemeta::core::JSON{std::string{policy->default_path}});
  }

  std::ostringstream payload_text;
  sourcemeta::core::stringify(payload, payload_text);

  const auto expiry{std::chrono::time_point_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now()) +
                    TRANSACTION_LIFETIME};
  const auto sealed{this->table_.impl_->seal(
      policy_name, SealPurpose::Transaction, payload_text.str(), expiry)};
  if (!sealed.has_value()) {
    result.log.emplace_back("No session secret is set for the policy");
    return result;
  }

  // A provider sends only the claims a request asks for, so a policy whose
  // rules name any is asked for them here. The standard way is the claims
  // request parameter, and where a provider does not honour that, the scopes
  // that carry the standard claims are the fallback. A claim no standard scope
  // carries is then arranged at the provider instead, since inventing a scope
  // name risks a request refused outright.
  // The rules outlive every request built from them, since a claim request
  // names its claim by pointing into them rather than copying
  const auto rules{policy->claims.empty()
                       ? std::optional<sourcemeta::core::JSON>{std::nullopt}
                       : sourcemeta::core::try_parse_json(policy->claims)};
  const auto wanted{wanted_claims(policy.value(), rules)};
  std::string scope_request;
  std::string claims_parameter;
  if (endpoints.value().claims_parameter_supported && !wanted.empty()) {
    std::ostringstream text;
    sourcemeta::core::stringify(
        sourcemeta::core::oidc_build_claims_parameter({}, wanted), text);
    claims_parameter = text.str();
  }

  requested_scope(wanted, scope_request);
  report_unadvertised_claims(wanted, endpoints.value(), policy_name,
                             result.log);

  const auto challenge{sourcemeta::core::oauth_pkce_challenge(verifier)};
  sourcemeta::core::OIDCAuthenticationRequest authentication_request{};
  authentication_request.client_id = policy->client_id;
  authentication_request.redirect_uri = redirect_uri;
  authentication_request.scope = scope_request;
  authentication_request.claims = claims_parameter;
  authentication_request.response_type = "code";
  authentication_request.state = state;
  authentication_request.code_challenge = {challenge.data(), challenge.size()};
  authentication_request.code_challenge_method = "S256";
  authentication_request.nonce = nonce;
  if (silent) {
    authentication_request.prompt = "none";
  }

  std::string authorization_url;
  if (!sourcemeta::core::oidc_build_authentication_url(
          endpoints.value().authorization, authentication_request,
          authorization_url)) {
    result.log.emplace_back("The authorization endpoint is not a URL a request "
                            "can be built against, for the policy");
    return result;
  }

  // A redirect without the transaction cookie could never complete at the
  // callback, so it is not worth sending
  auto cookie{sourcemeta::core::http_serialize_cookie(
      {.name = TRANSACTION_COOKIE,
       .value = sealed.value(),
       .path = COOKIE_PATH,
       .max_age = TRANSACTION_LIFETIME,
       .http_only = true,
       .secure = sourcemeta::core::URI{std::string{instance_url}}.is_https(),
       .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
  if (!cookie.has_value()) {
    result.log.emplace_back(
        "The login transaction could not be put in a cookie, for the policy");
    return result;
  }

  result.cookies.push_back(std::move(cookie).value());
  result.location = std::move(authorization_url);
  result.result = Authentication::Outcome::Result::Redirect;
  return result;
}

auto Authentication::renewal(const Authentication::Path &path,
                             const Credentials &credentials) const
    -> std::optional<std::string_view> {
  if (credentials.cookies.empty()) {
    return std::nullopt;
  }

  std::vector<std::string_view> candidates;
  for (const auto field : credentials.cookies) {
    sourcemeta::core::http_cookie_values(field, RENEWAL_COOKIE, candidates);
  }

  for (const auto candidate : candidates) {
    const auto policy{this->table_.impl_->interactive(path.value(), candidate)};
    if (policy.has_value()) {
      return candidate;
    }
  }

  return std::nullopt;
}

auto Authentication::callback(const std::string_view policy_name,
                              const std::string_view instance_url,
                              const std::string_view redirect_uri,
                              const Authentication::CallbackRequest &incoming,
                              const Credentials &credentials) const
    -> Authentication::Outcome {
  Authentication::Outcome result;
  result.result = Authentication::Outcome::Result::Invalid;
  const auto secure{
      sourcemeta::core::URI{std::string{instance_url}}.is_https()};

  // The transaction is the only proof that this response belongs to a login
  // this instance started, and the state it carries must match the one the
  // provider echoes back. This runs before either the success or the decline is
  // honoured, so a cross-site callback cannot even trigger an error on a
  // person's behalf, per RFC 6749 Section 4.1.2.1
  const auto transaction{this->table_.impl_->transaction(
      policy_name, incoming.state, redirect_uri, credentials)};
  if (!transaction.has_value()) {
    return result;
  }

  // Nobody asked for a silent attempt, so from here on nothing it does is shown
  // to them: every way this can end without a session puts the browser back
  // where it started instead
  const auto silent{transaction.value().try_at("silent") != nullptr};
  const auto *nonce{transaction.value().try_at("nonce")};
  const auto *verifier{transaction.value().try_at("verifier")};

  // Where a silent attempt leaves the browser when it did not come back with a
  // grant: back where it was denied, carrying neither a session nor the marker
  // that would send it here again. The marker goes whatever the reason, so an
  // attempt that failed is not made again on the next navigation, and every
  // navigation after that
  const auto abandon{[&](const Authentication::Outcome::Result outcome)
                         -> Authentication::Outcome {
    if (!silent) {
      result.result = outcome;
      return result;
    }

    result.result = Authentication::Outcome::Result::Redirect;
    result.location = "";
    const auto *sealed{transaction.value().try_at("to")};
    if (sealed != nullptr && sealed->is_string() &&
        is_local_path(sealed->to_string())) {
      result.location = sealed->to_string();
    }

    for (const auto name : {RENEWAL_COOKIE, TRANSACTION_COOKIE}) {
      auto cookie{expired_cookie(name, secure)};
      if (cookie.has_value()) {
        result.cookies.push_back(std::move(cookie).value());
      }
    }

    result.log.emplace_back("A silent renewal did not end in a session");
    return result;
  }};

  // Which policy a callback belongs to is settled by opening its transaction,
  // so nothing reaching here names one this instance does not serve
  const auto policy{this->table_.impl_->interactive(policy_name)};
  if (!policy.has_value()) {
    return abandon(Authentication::Outcome::Result::Invalid);
  }

  // RFC 9207 Section 2.4 has a client compare the issuer an answer names
  // against the one it addressed the request to, which is what catches an
  // answer relayed from somewhere else. A provider naming none cannot be
  // checked that way, so this runs only when one arrives, and it runs ahead of
  // the outcome so that no answer is acted on before it is placed
  if (incoming.has_issuer && incoming.issuer != policy->issuer) {
    return abandon(Authentication::Outcome::Result::Invalid);
  }

  // Only once the callback is proven to belong to a real login is the
  // provider's outcome honoured: a decline returns an error instead of a code,
  // and a success without a code is malformed. RFC 6749 Section 4.1.2.1 has a
  // decline name itself with an error code, so one that names nothing is not a
  // decision this can report as the provider's. It is no grant either, and a
  // code arriving beside it is left unredeemed
  if (incoming.has_error) {
    return abandon(incoming.error.empty()
                       ? Authentication::Outcome::Result::Invalid
                       : Authentication::Outcome::Result::Declined);
  }

  if (incoming.code.empty()) {
    return abandon(Authentication::Outcome::Result::Invalid);
  }

  const auto client_secret{this->table_.impl_->client_secret(policy_name)};
  if (!client_secret.has_value()) {
    result.log.emplace_back("No client secret is set for the policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  const auto endpoints{this->table_.impl_->endpoints(policy_name)};
  if (!endpoints.has_value() || endpoints.value().token.empty()) {
    result.log.emplace_back("The provider named no token endpoint, or could "
                            "not be reached, for the policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  const auto grant{exchange(
      this->table_.impl_->fetcher(), endpoints.value().token, policy->client_id,
      client_secret.value(), redirect_uri, incoming.code, verifier->to_string(),
      endpoints.value().token_endpoint_basic_auth)};
  if (!grant.has_value()) {
    result.log.emplace_back(
        "The authorization code could not be redeemed for the policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  const auto token{sourcemeta::core::JWT::from(grant.value().id_token)};
  if (!token.has_value()) {
    result.log.emplace_back("The provider returned an identity token that "
                            "could not be read, for the policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  auto provider{id_token_keys(endpoints.value().jwks_uri,
                              this->table_.impl_->key_fetcher())};
  sourcemeta::core::OIDCValidationOptions options;
  options.nonce = nonce->to_string();
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      provider, token.value(), ID_TOKEN_ALGORITHMS, policy->issuer,
      policy->client_id, options)};
  if (!identity.has_value()) {
    result.log.emplace_back("The identity token did not validate for the "
                            "policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  // A policy's rules are answered here rather than at the gate, so that a
  // session only ever exists for somebody the policy admits. Answering it
  // afterwards would leave a valid session denied on every request, and a
  // denial asks the provider again, which is a loop rather than an answer
  std::optional<sourcemeta::core::JSON> combined;
  auto admission{this->table_.impl_->admits_identity(policy_name,
                                                     token.value().payload())};

  // OpenID Connect Core Section 5.4 has a provider answer for the claims a
  // scope requested at its UserInfo endpoint rather than in the token, by
  // default, under the flow this is completing. So a rule naming a claim the
  // token does not carry is asked there before it is refused, and only then,
  // since a token carrying everything needed spares the round trip
  if (admission == Admission::Incomplete &&
      !endpoints.value().userinfo.empty()) {
    const auto extra{userinfo(
        this->table_.impl_->fetcher(), endpoints.value().userinfo,
        grant.value().access_token, identity.value().subject, result.log)};
    if (extra.has_value()) {
      combined = combine_claims(token.value().payload(), extra.value());
      admission =
          this->table_.impl_->admits_identity(policy_name, combined.value());
    }
  }

  if (admission != Admission::Admitted) {
    result.log.emplace_back("The provider authenticated somebody the policy "
                            "does not admit, for the policy");
    // Whatever the decision was actually made against, which is the pair taken
    // together once a second answer arrived. Explaining a refusal against the
    // token alone would miss a claim the UserInfo endpoint supplied
    const auto &asserted{combined.has_value() ? combined.value()
                                              : token.value().payload()};
    this->table_.impl_->report_object_shaped_claims(policy_name, asserted,
                                                    result.log);
    return abandon(Authentication::Outcome::Result::NotAdmitted);
  }

  const auto expiry{std::chrono::time_point_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now()) +
                    SESSION_LIFETIME};

  // The identity token is kept so that logging out can prove whose session it
  // is asking the provider to end, which is what spares the person a
  // confirmation page there. A provider that mints a large one can push the
  // cookie past what a browser will store, and a browser drops such a cookie
  // without saying so, which would look like signing in and then not being
  // signed in. So the whole cookie is measured, and the token is left out when
  // it does not fit, which costs the confirmation page and nothing else
  auto session{this->table_.impl_->session_cookie(
      policy_name, identity.value().subject, grant.value().id_token, expiry,
      secure, result.log)};
  if (session.has_value() && session.value().size() > MAXIMUM_COOKIE_LENGTH) {
    session = this->table_.impl_->session_cookie(
        policy_name, identity.value().subject, "", expiry, secure, result.log);
  }

  // Without the token there is very little left, so exceeding the limit here
  // takes something extraordinary, such as a provider that identifies people by
  // something enormous. Answering plainly beats handing over a cookie the
  // browser discards, which would look like signing in and then not being
  // signed in, with nothing anywhere to explain it
  if (!session.has_value() || session.value().size() > MAXIMUM_COOKIE_LENGTH) {
    if (session.has_value()) {
      result.log.emplace_back("The provider returned more than a session can "
                              "hold, for the policy");
    }

    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  result.cookies.push_back(std::move(session).value());

  // Signing in is what earns a browser a silent renewal later, and the marker
  // outlives the session it accompanies because it is only of use once that
  // session has expired
  auto renewal{sourcemeta::core::http_serialize_cookie(
      {.name = RENEWAL_COOKIE,
       .value = policy_name,
       .path = COOKIE_PATH,
       .max_age = RENEWAL_LIFETIME,
       .http_only = true,
       .secure = secure,
       .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
  if (renewal.has_value()) {
    result.cookies.push_back(std::move(renewal).value());
  }

  // The single-use transaction has served its purpose, so it is expired
  // alongside minting the session
  auto spent{expired_cookie(TRANSACTION_COOKIE, secure)};
  if (spent.has_value()) {
    result.cookies.push_back(std::move(spent).value());
  }

  // The login may have sealed a page to return to. It came through this
  // instance's own signature, yet it is re-checked as a local path before being
  // trusted as a redirect target
  result.location = "";
  const auto *sealed_destination{transaction.value().try_at("to")};
  if (sealed_destination != nullptr && sealed_destination->is_string() &&
      is_local_path(sealed_destination->to_string())) {
    result.location = sealed_destination->to_string();
  }

  result.result = Authentication::Outcome::Result::Redirect;
  return result;
}

auto Authentication::logout(const Credentials &credentials,
                            const std::string_view instance_url,
                            const std::string_view return_to) const
    -> Authentication::Outcome {
  Authentication::Outcome result;
  const auto secure{
      sourcemeta::core::URI{std::string{instance_url}}.is_https()};

  // Composed before anything that could fail, so no path below ends with a
  // session surviving. Somebody who has signed out is asking not to be signed
  // in, so the marker that would have renewed them silently goes with it
  for (const auto name : {SESSION_COOKIE, TRANSACTION_COOKIE, RENEWAL_COOKIE}) {
    auto cookie{expired_cookie(name, secure)};
    if (cookie.has_value()) {
      result.cookies.push_back(std::move(cookie).value());
    }
  }

  result.location = return_to;

  // Ending the session here leaves the provider's own untouched, so signing in
  // again would not ask who you are. Where the session names a policy whose
  // provider offers to end it, the browser finishes the job there, carrying the
  // identity token as the proof of whose session it is. Every step that cannot
  // be completed simply stops, since the local session is already gone
  std::vector<std::string_view> candidates;
  for (const auto field : credentials.cookies) {
    sourcemeta::core::http_cookie_values(field, SESSION_COOKIE, candidates);
  }

  for (const auto sealed : candidates) {
    const auto payload{this->table_.impl_->open_session(sealed)};
    if (!payload.has_value()) {
      continue;
    }

    const auto document{sourcemeta::core::try_parse_json(payload.value())};
    if (!document.has_value() || !document.value().is_object()) {
      continue;
    }

    const auto *policy{document.value().try_at("policy")};
    const auto *token{document.value().try_at("id_token")};
    if (policy == nullptr || !policy->is_string()) {
      continue;
    }

    const auto endpoints{this->table_.impl_->endpoints(policy->to_string())};
    if (!endpoints.has_value() || endpoints.value().end_session.empty()) {
      continue;
    }

    sourcemeta::core::OIDCLogoutRequest logout;
    if (token != nullptr && token->is_string()) {
      logout.id_token_hint = token->to_string();
    }

    // Where the provider sends the browser back to once it is done, which is
    // this instance rather than the local path a caller without a provider
    // session lands on
    logout.post_logout_redirect_uri = instance_url;
    std::string url;
    sourcemeta::core::oidc_build_logout_url(endpoints.value().end_session,
                                            logout, url);
    result.location = std::move(url);
    break;
  }

  return result;
}

auto Authentication::caller(const Credentials &credentials) const
    -> Authentication::Caller {
  Authentication::Caller result;
  result.policies_ =
      this->table_.impl_->classify(credentials.bearer, credentials.cookies);
  result.view_ = this->table_.impl_->view_name(result.policies_);
  result.bearer_ = credentials.bearer;
  return result;
}

auto Authentication::permits(const Authentication::Path &path,
                             const Authentication::Caller &caller) const
    -> bool {
  return this->table_.impl_->permits(path.value(), caller.policies_);
}

auto Authentication::permits(const RouteTarget &target,
                             const Authentication::Caller &caller,
                             const std::string_view required_audience) const
    -> bool {
  const auto governing{this->table_.impl_->governing_mask(target.value())};
  if (!governing.has_value()) {
    return false;
  }

  // A route nobody governs is reached by anybody, so there is nothing for an
  // audience to narrow. Requiring one here would refuse a caller for presenting
  // a credential they did not need, at a route they could have reached by
  // presenting nothing at all
  if (governing.value() == 0) {
    return true;
  }

  if ((governing.value() & caller.policies_) == 0) {
    return false;
  }

  if (required_audience.empty()) {
    return true;
  }

  // Only a token carries an audience, so a caller admitted by anything else is
  // unaffected by a requirement it could never have satisfied or broken. The
  // signature is already established, so this is one more claim read from a
  // token that has been verified rather than a second verification
  const auto token{sourcemeta::core::JWT::from(caller.bearer_)};
  return !token.has_value() || token.value().has_audience(required_audience);
}

auto Authentication::Table::views() const
    -> std::vector<Authentication::RecordedView> {
  std::vector<Authentication::RecordedView> result;
  result.reserve(this->impl_->view_count());
  for (std::size_t index{0}; index < this->impl_->view_count(); index += 1) {
    result.push_back(this->impl_->view_at(index));
  }

  return result;
}

auto Authentication::Table::view(const std::string_view name) const
    -> Authentication::RecordedView {
  for (std::size_t index{0}; index < this->impl_->view_count(); index += 1) {
    const auto candidate{this->impl_->view_at(index)};
    if (candidate.name() == name) {
      return candidate;
    }
  }

  Authentication::RecordedView result;
  result.name_ = VIEW_PUBLIC;
  return result;
}

auto Authentication::Table::visible(
    const Authentication::Path &path,
    const Authentication::RecordedView &view) const -> bool {
  // An instance that could not read its artifact shows nothing at all, which
  // this answers rather than whoever asks. Otherwise what nobody governs would
  // be shown by an instance knowing nothing about who governs what, which is
  // the one way this could disclose more than the gate admits
  return this->impl_->permits(path.value(), view.policies_);
}

auto Authentication::Table::governing(const Authentication::Path &path) const
    -> std::vector<std::string_view> {
  auto mask{this->impl_->match(path.value())};
  std::vector<std::string_view> result;
  while (mask != 0) {
    // A policy standing on its own is a view, and the table names every one, so
    // what a policy is called is read from there rather than stored twice
    const auto policy{Authentication::PolicySet{1} << std::countr_zero(mask)};
    result.push_back(this->impl_->view_name(policy));
    mask &= mask - 1;
  }

  return result;
}

auto Authentication::Table::reference_permitted(
    const Authentication::Path &referrer,
    const Authentication::Path &referent) const -> bool {
  return this->impl_->reference_permitted(referrer.value(), referent.value());
}

} // namespace sourcemeta::one
