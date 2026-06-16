#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/io.h>

#include "authentication_format.h"

#include <cassert> // assert
#include <cstddef> // std::byte, std::size_t
#include <cstdint> // std::uint32_t, std::uint64_t, std::uint8_t
#include <cstring> // std::memcpy
#include <span>    // std::span
#include <string>  // std::string
#include <utility> // std::pair
#include <vector>  // std::vector

namespace {

struct BuildNode {
  std::uint64_t mask{0};
  std::vector<std::pair<std::string, std::uint32_t>> edges;
};

auto find_or_create_child(std::vector<BuildNode> &nodes,
                          const std::uint32_t parent,
                          const std::string_view segment) -> std::uint32_t {
  for (const auto &edge : nodes[parent].edges) {
    if (edge.first == segment) {
      return edge.second;
    }
  }

  const auto child{static_cast<std::uint32_t>(nodes.size())};
  nodes.emplace_back();
  nodes[parent].edges.emplace_back(std::string{segment}, child);
  return child;
}

auto align_to_word(const std::uint32_t offset) -> std::uint32_t {
  return (offset + 7U) & ~static_cast<std::uint32_t>(7U);
}

} // namespace

namespace sourcemeta::one {

auto Authentication::save(std::span<const Authentication::Policy> policies,
                          const std::filesystem::path &path) -> void {
  assert(policies.size() <= Authentication::MAXIMUM_POLICIES);

  std::vector<BuildNode> nodes;
  nodes.emplace_back();

  for (std::size_t index{0}; index < policies.size(); index += 1) {
    const auto bit{static_cast<std::uint64_t>(1) << index};
    for (const auto &policy_path : policies[index].paths) {
      std::uint32_t current{0};
      std::size_t cursor{0};
      for (auto segment{authentication_next_segment(policy_path, cursor)};
           !segment.empty();
           segment = authentication_next_segment(policy_path, cursor)) {
        current = find_or_create_child(nodes, current, segment);
      }

      nodes[current].mask |= bit;
    }
  }

  std::string strings;
  std::vector<AuthenticationEdge> edges;
  std::vector<AuthenticationNode> serialized;
  serialized.reserve(nodes.size());
  for (const auto &node : nodes) {
    AuthenticationNode entry{};
    entry.mask = node.mask;
    entry.first_edge = static_cast<std::uint32_t>(edges.size());
    entry.edge_count = static_cast<std::uint32_t>(node.edges.size());
    for (const auto &edge : node.edges) {
      AuthenticationEdge serialized_edge{};
      serialized_edge.segment_offset =
          static_cast<std::uint32_t>(strings.size());
      serialized_edge.segment_length =
          static_cast<std::uint32_t>(edge.first.size());
      serialized_edge.child = edge.second;
      strings += edge.first;
      edges.push_back(serialized_edge);
    }

    serialized.push_back(entry);
  }

  AuthenticationHeader header{};
  header.magic = AUTHENTICATION_MAGIC;
  header.version = AUTHENTICATION_VERSION;
  header.policy_count = static_cast<std::uint32_t>(policies.size());
  header.node_count = static_cast<std::uint32_t>(serialized.size());
  header.edge_count = static_cast<std::uint32_t>(edges.size());
  header.policies_offset =
      static_cast<std::uint32_t>(sizeof(AuthenticationHeader));
  // The node array begins the word-aligned region the matcher addresses
  // directly, so pad past the byte-packed policy table
  header.nodes_offset = align_to_word(
      header.policies_offset +
      header.policy_count *
          static_cast<std::uint32_t>(sizeof(AuthenticationPolicyEntry)));
  header.edges_offset =
      header.nodes_offset + header.node_count * static_cast<std::uint32_t>(
                                                    sizeof(AuthenticationNode));
  header.strings_offset =
      header.edges_offset + header.edge_count * static_cast<std::uint32_t>(
                                                    sizeof(AuthenticationEdge));
  header.strings_length = static_cast<std::uint32_t>(strings.size());

  std::vector<std::byte> buffer(header.strings_offset + header.strings_length,
                                std::byte{0});
  std::memcpy(buffer.data(), &header, sizeof(header));
  for (std::size_t index{0}; index < policies.size(); index += 1) {
    buffer[header.policies_offset + index] =
        static_cast<std::byte>(policies[index].type);
  }

  std::memcpy(buffer.data() + header.nodes_offset, serialized.data(),
              serialized.size() * sizeof(AuthenticationNode));
  std::memcpy(buffer.data() + header.edges_offset, edges.data(),
              edges.size() * sizeof(AuthenticationEdge));
  std::memcpy(buffer.data() + header.strings_offset, strings.data(),
              strings.size());

  sourcemeta::core::write_file(path, buffer);
}

} // namespace sourcemeta::one
