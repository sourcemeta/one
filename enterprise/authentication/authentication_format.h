#ifndef SOURCEMETA_ONE_AUTHENTICATION_FORMAT_H_
#define SOURCEMETA_ONE_AUTHENTICATION_FORMAT_H_

#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/jose.h>

#include <limits> // std::numeric_limits

#include <cstddef>       // std::byte, std::size_t
#include <cstdint>       // std::uint32_t, std::uint64_t, std::uint8_t
#include <span>          // std::span
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

namespace sourcemeta::one {

// What a policy authenticates, stored as one byte per entry, so these values
// are the artifact's rather than a matter of taste
enum class AuthenticationPolicyType : std::uint8_t {
  ApiKey = 0,
  JWT = 1,
  OIDC = 2
};

// How many policies one artifact can name. Each occupies a bit of the node
// masks, so this is what a mask has room for rather than a matter of taste
constexpr std::size_t AUTHENTICATION_MAXIMUM_POLICIES{64};

constexpr std::uint32_t NO_CHILD{std::numeric_limits<std::uint32_t>::max()};

// Address a section of the artifact wherever it was read from, so that mapping
// a file and holding the bytes outright reach the same structures
template <typename T>
inline auto at_offset(const std::span<const std::byte> bytes,
                      const std::size_t offset = 0) noexcept -> const T * {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const T *>(bytes.data() + offset);
}

constexpr std::uint32_t AUTHENTICATION_MAGIC{0x48545541};
constexpr std::uint32_t AUTHENTICATION_VERSION{14};

// The artifact begins with this header. Every variable-length section is
// located through an absolute byte offset so the matcher can address it
// directly in the memory-mapped buffer
struct AuthenticationHeader {
  std::uint32_t magic;
  std::uint32_t version;
  std::uint32_t policy_count;
  std::uint32_t node_count;
  std::uint32_t edge_count;
  std::uint32_t view_count;
  std::uint32_t policies_offset;
  std::uint32_t nodes_offset;
  std::uint32_t views_offset;
  std::uint32_t edges_offset;
  std::uint32_t strings_offset;
  std::uint32_t strings_length;
};

// A node in the prefix trie. The mask holds the policies whose path prefix
// terminates exactly at this node, so matching accumulates the masks of
// every node visited from the root to the deepest matching prefix
struct alignas(8) AuthenticationNode {
  std::uint64_t mask;
  std::uint32_t first_edge;
  std::uint32_t edge_count;
};

// An edge from a node to one of its children, labelled by a path segment
// stored in the string blob
struct AuthenticationEdge {
  std::uint32_t segment_offset;
  std::uint32_t segment_length;
  std::uint32_t child;
};

// One entry per policy, in declaration order, mirroring the bit assigned to
// it in the node masks. The metadata range locates the policy's parameters by
// absolute file offset, the type selects how the metadata is interpreted, and
// the algorithm selects how an apiKey credential is compared against its keys.
// The name is held here rather than inside the metadata so that naming a view
// after the policies it comprises reads one field per policy, whatever their
// types
struct AuthenticationPolicyEntry {
  std::uint32_t metadata_offset;
  std::uint32_t metadata_length;
  std::uint32_t name_offset;
  std::uint32_t name_length;
  std::uint8_t algorithm;
  std::uint8_t type;
};

// One entry per view, ordered as the table is built. The set names the
// policies a caller must satisfy to be placed here, so resolving a view is a
// comparison against what classification returned rather than a search
struct alignas(8) AuthenticationViewEntry {
  std::uint64_t policies;
  std::uint32_t name_offset;
  std::uint32_t name_length;
};

// The structures are cast directly out of the memory-mapped buffer, so their
// layout must stay fixed across edits and compilers
static_assert(sizeof(AuthenticationHeader) == 48);
static_assert(sizeof(AuthenticationNode) == 16);
static_assert(alignof(AuthenticationNode) == 8);
static_assert(sizeof(AuthenticationEdge) == 12);
static_assert(sizeof(AuthenticationPolicyEntry) == 20);
static_assert(alignof(AuthenticationPolicyEntry) == 4);
static_assert(sizeof(AuthenticationViewEntry) == 16);
static_assert(alignof(AuthenticationViewEntry) == 8);

// Advance the cursor to the next non-empty path segment and return it. The
// returned view is empty once the path is exhausted. Leading, trailing, and
// repeated slashes are ignored, so "/a/b", "a/b", and "a/b/" all yield the
// segments "a" then "b"
inline auto authentication_next_segment(const std::string_view path,
                                        std::size_t &cursor) noexcept
    -> std::string_view {
  while (cursor < path.size() && path[cursor] == '/') {
    cursor += 1;
  }

  if (cursor >= path.size()) {
    return {};
  }

  const auto start{cursor};
  while (cursor < path.size() && path[cursor] != '/') {
    cursor += 1;
  }

  return {path.data() + start, cursor - start};
}

struct JWTPolicy {
  std::string_view issuer;
  std::string_view audience;
  std::string_view jwks_uri;
  std::string_view token_type;
  std::vector<sourcemeta::core::JWSAlgorithm> algorithms;
};
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

// Reading the artifact back, which is the other half of what writes it.
// Each is a pure function of the bytes it is handed, and each answers
// whether what it was handed was well formed
inline auto find_child(const AuthenticationNode &node,
                       const AuthenticationEdge *edges, const char *strings,
                       const std::string_view segment) noexcept
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

inline auto structurally_valid(const std::span<const std::byte> bytes) noexcept
    -> bool {
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

    // A node's edges are searched by halving rather than scanned, so labels out
    // of order would lose a child that is present. Losing one leaves the
    // subtree below it ungoverned, which is the one direction this must not
    // fail in, so the order the search relies on is established here rather
    // than assumed of whoever wrote the file
    const char *strings{at_offset<char>(bytes, header->strings_offset)};
    for (std::uint32_t index{0}; index < header->node_count; index += 1) {
      const auto &node{nodes[index]};
      for (std::uint32_t edge{1}; edge < node.edge_count; edge += 1) {
        const auto &previous{edges[node.first_edge + edge - 1]};
        const auto &current{edges[node.first_edge + edge]};
        const std::string_view before{strings + previous.segment_offset,
                                      previous.segment_length};
        const std::string_view after{strings + current.segment_offset,
                                     current.segment_length};
        if (before.compare(after) >= 0) {
          return false;
        }
      }
    }
  }

  return true;
}

inline auto read_u32(const std::span<const std::byte> metadata,
                     std::size_t &cursor, std::uint32_t &value) -> bool {
  if (cursor > metadata.size() || metadata.size() - cursor < sizeof(value)) {
    return false;
  }

  std::memcpy(&value, metadata.data() + cursor, sizeof(value));
  cursor += sizeof(value);
  return true;
}

inline auto collect_keys(const std::span<const std::byte> metadata,
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

inline auto read_string(const std::span<const std::byte> metadata,
                        std::size_t &cursor, std::string_view &value) -> bool {
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

inline auto decode_jwt_metadata(const std::span<const std::byte> metadata,
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

    // A token is verified against a key set, which carries public keys, so only
    // an asymmetric algorithm can ever be meant here. Admitting a symmetric one
    // would let a public key be read as a shared secret, which is why a
    // configuration cannot name one either. The asymmetric values are the ones
    // up to and including this, so anything beyond it is refused
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
inline auto collect_jwt_identifiers(const std::span<const std::byte> metadata,
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

// Advance past a counted run of strings, answering the bytes it occupies
inline auto read_counted_strings(const std::span<const std::byte> metadata,
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

inline auto decode_oidc_metadata(const std::span<const std::byte> metadata,
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
inline auto collect_oidc_identifiers(const std::span<const std::byte> metadata,
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

} // namespace sourcemeta::one

#endif
