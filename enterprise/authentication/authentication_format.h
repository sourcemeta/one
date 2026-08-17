#ifndef SOURCEMETA_ONE_AUTHENTICATION_FORMAT_H_
#define SOURCEMETA_ONE_AUTHENTICATION_FORMAT_H_

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint32_t, std::uint64_t, std::uint8_t
#include <string_view> // std::string_view

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

} // namespace sourcemeta::one

#endif
