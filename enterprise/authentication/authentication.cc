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

// The reference check treats two JWT policies as the same scope only when
// every parameter that decides admission matches, so the issuer, audience, key
// set location, required token type and allowed algorithms count as one
// indivisible identity, never as separate keys that several policies could
// satisfy piecewise or in swapped roles.
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

  // The serialized run itself is the key. Its length prefixes keep the fields
  // delimited, so exactly the equal identities compare equal
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  keys.emplace(reinterpret_cast<const char *>(metadata.data()), cursor + count);
}

struct OIDCPolicyMetadata {
  std::string_view issuer;
  std::string_view client_id;
  std::string_view client_secret_variable;
  std::string_view name;
  // The variables holding this policy's session secrets, kept as the bytes
  // they occupy so that reading a policy costs nothing until one is wanted
  std::span<const std::byte> session_secrets;
  std::string_view default_path;
};

auto decode_oidc_metadata(const std::span<const std::byte> metadata,
                          OIDCPolicyMetadata &result) -> bool {
  std::size_t cursor{0};
  if (!read_string(metadata, cursor, result.issuer) ||
      !read_string(metadata, cursor, result.client_id) ||
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

// Walk the variables a policy names, newest first, handing each to the caller,
// answering whether the whole run could be read
template <typename Callback>
  requires std::invocable<Callback, std::string_view>
[[nodiscard]] auto
each_session_secret_variable(const std::span<const std::byte> metadata,
                             Callback callback) -> bool {
  std::size_t cursor{0};
  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count)) {
    return false;
  }

  for (std::uint32_t index{0}; index < count; index += 1) {
    std::string_view variable;
    if (!read_string(metadata, cursor, variable)) {
      return false;
    }

    callback(variable);
  }

  return true;
}

// The reference check treats two interactive policies as the same scope only
// when they authenticate against the same provider client, so the issuer and
// client identifier count as one indivisible identity, never as separate
// keys that several policies could satisfy piecewise or in swapped roles
auto collect_oidc_identifiers(const std::span<const std::byte> metadata,
                              std::unordered_set<std::string_view> &keys)
    -> void {
  std::size_t cursor{0};
  std::string_view issuer;
  std::string_view client_id;
  if (!read_string(metadata, cursor, issuer) ||
      !read_string(metadata, cursor, client_id)) {
    return;
  }

  // The serialized pair itself is the key. Its length prefixes keep the two
  // fields delimited, so exactly the equal pairs compare equal
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
            this->admits_jwt(metadata, token.value(), required_audience)) {
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
                                const std::string_view required_audience) const
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
    return required_audience.empty() || token.has_audience(required_audience);
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

    return Authentication::InteractivePolicy{.issuer = decoded.issuer,
                                             .client_id = decoded.client_id,
                                             .default_path =
                                                 decoded.default_path};
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
        return Authentication::InteractivePolicy{.issuer = decoded.issuer,
                                                 .client_id = decoded.client_id,
                                                 .default_path =
                                                     decoded.default_path};
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
    if (!each_session_secret_variable(
            variables, [&result](const auto variable) -> void {
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

      // RFC 6749 Section 2.3.1 requires every server to accept the client
      // secret in an authorization header and discourages carrying it in the
      // request body, so the body is used only where the header is refused.
      // A provider that lists nothing is taken to accept the header, which is
      // what the specification assigns to saying nothing
      resolved.token_endpoint_basic_auth =
          document.value().supports_token_endpoint_auth_method(
              "client_secret_basic");

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
