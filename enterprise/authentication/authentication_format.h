#ifndef SOURCEMETA_ONE_AUTHENTICATION_FORMAT_H_
#define SOURCEMETA_ONE_AUTHENTICATION_FORMAT_H_

#include <cstddef> // std::size_t
#include <cstdint> // std::uint32_t, std::uint64_t, std::uint8_t

namespace sourcemeta::one {

constexpr std::uint32_t AUTHENTICATION_MAGIC{0x48545541};
constexpr std::uint32_t AUTHENTICATION_VERSION{2};

// The artifact begins with this header. Every variable-length section is
// located through an absolute byte offset so the matcher can address it
// directly in the memory-mapped buffer
struct AuthenticationHeader {
  std::uint32_t magic;
  std::uint32_t version;
  std::uint32_t policy_count;
  std::uint32_t node_count;
  std::uint32_t edge_count;
  std::uint32_t policies_offset;
  std::uint32_t nodes_offset;
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
// it in the node masks. The metadata range locates the policy's opaque blob
// by absolute file offset, empty for public policies
struct AuthenticationPolicyEntry {
  std::uint32_t metadata_offset;
  std::uint32_t metadata_length;
  std::uint8_t type;
};

// The structures are cast directly out of the memory-mapped buffer, so their
// layout must stay fixed across edits and compilers
static_assert(sizeof(AuthenticationHeader) == 40);
static_assert(sizeof(AuthenticationNode) == 16);
static_assert(alignof(AuthenticationNode) == 8);
static_assert(sizeof(AuthenticationEdge) == 12);
static_assert(sizeof(AuthenticationPolicyEntry) == 12);
static_assert(alignof(AuthenticationPolicyEntry) == 4);

} // namespace sourcemeta::one

#endif
