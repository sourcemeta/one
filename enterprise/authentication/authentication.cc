#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>

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
// no policy and is denied
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

auto resolve_environment(const std::string_view variable)
    -> std::optional<std::string> {
  static std::mutex mutex;
  static std::unordered_map<std::string, std::optional<std::string>> cache;
  const std::string name{variable};
  const std::scoped_lock lock{mutex};
  const auto existing{cache.find(name)};
  if (existing != cache.cend()) {
    return existing->second;
  }

  std::optional<std::string> resolved;
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const char *value{std::getenv(name.c_str())};
  if (value != nullptr) {
    resolved = value;
  }

  cache.emplace(name, resolved);
  return resolved;
}

auto admits_apikey(const std::span<const std::byte> metadata,
                   const std::string_view credential,
                   const sourcemeta::one::Authentication::Algorithm algorithm)
    -> bool {
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
  std::vector<sourcemeta::core::JWSAlgorithm> algorithms;
};

auto decode_jwt_metadata(const std::span<const std::byte> metadata,
                         JWTPolicy &result) -> bool {
  std::size_t cursor{0};
  if (!read_string(metadata, cursor, result.issuer) ||
      !read_string(metadata, cursor, result.audience) ||
      !read_string(metadata, cursor, result.jwks_uri)) {
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

// The reference check treats two JWT policies as the same scope only when every
// parameter that decides admission matches, so the identity spans the issuer,
// audience, key set location, and the exact allowed algorithm bytes
auto collect_jwt_identifiers(const std::span<const std::byte> metadata,
                             std::unordered_set<std::string_view> &keys)
    -> void {
  std::size_t cursor{0};
  std::string_view issuer;
  std::string_view audience;
  std::string_view jwks_uri;
  if (!read_string(metadata, cursor, issuer) ||
      !read_string(metadata, cursor, audience) ||
      !read_string(metadata, cursor, jwks_uri)) {
    return;
  }

  keys.emplace(issuer);
  keys.emplace(audience);
  keys.emplace(jwks_uri);

  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count) || count > metadata.size() - cursor) {
    return;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  keys.emplace(reinterpret_cast<const char *>(metadata.data() + cursor), count);
}

// The reference check treats two interactive policies as the same scope only
// when they authenticate against the same provider client, so the identity
// spans the issuer and the client identifier
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

  keys.emplace(issuer);
  keys.emplace(client_id);
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
                            const std::string_view credential) const
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
        if (token.has_value() && this->admits_jwt(metadata, token.value())) {
          return {.allowed = true,
                  .principal = Authentication::Principal{
                      .type = type, .policy = static_cast<std::size_t>(index)}};
        }
      } else if (type == Authentication::Type::OIDC) {
        // An interactive policy authenticates a person through a browser
        // login, never a presented credential, so it can never open a path
        // here
        continue;
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
                                const sourcemeta::core::JWT &token) const
      -> bool {
    JWTPolicy policy;
    if (!decode_jwt_metadata(metadata, policy)) {
      return false;
    }

    auto *provider{this->provider_for(policy.issuer, policy.jwks_uri)};
    if (provider == nullptr) {
      return false;
    }

    const auto error{provider->verify(token, policy.algorithms, policy.issuer,
                                      policy.audience)};
    return !error.has_value();
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
      const auto metadata{this->fetcher_(discovery_url(issuer))};
      if (!metadata.has_value()) {
        return nullptr;
      }

      auto document{discovery_parse(metadata.value().body)};
      if (!document.has_value() || !document.value().jwks_uri.has_value()) {
        return nullptr;
      }

      location = std::move(document.value().jwks_uri).value();
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
};

Authentication::Authentication(const std::filesystem::path &path,
                               sourcemeta::core::JWKSProvider::Fetcher fetcher)
    : impl_{std::make_unique<Impl>(path, std::move(fetcher))} {}

Authentication::~Authentication() = default;

auto Authentication::admits(const std::string_view registry_path,
                            const std::string_view credential,
                            const std::string_view base_path) const
    -> Authentication::Verdict {
  return this->impl_->admits(strip_base_path(registry_path, base_path),
                             credential);
}

auto Authentication::governing(const std::string_view registry_path,
                               const std::string_view base_path) const
    -> std::vector<std::size_t> {
  auto mask{this->impl_->match(strip_base_path(registry_path, base_path))};
  std::vector<std::size_t> result;
  while (mask != 0) {
    result.push_back(static_cast<std::size_t>(std::countr_zero(mask)));
    mask &= mask - 1;
  }

  return result;
}

auto Authentication::reference_permitted(
    const std::string_view referrer_path,
    const std::string_view referent_path) const -> bool {
  return this->impl_->reference_permitted(referrer_path, referent_path);
}

} // namespace sourcemeta::one
