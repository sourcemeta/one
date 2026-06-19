#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/io.h>

#include "authentication_format.h"

#include <algorithm>   // std::ranges::sort
#include <array>       // std::array
#include <cstddef>     // std::byte, std::size_t
#include <cstdint>     // std::uint32_t, std::uint64_t, std::uint8_t
#include <cstring>     // std::memcpy
#include <span>        // std::span
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

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

auto append_u32(std::vector<std::byte> &output, const std::uint32_t value)
    -> void {
  std::array<std::byte, sizeof(value)> bytes{};
  std::memcpy(bytes.data(), &value, sizeof(value));
  output.insert(output.end(), bytes.begin(), bytes.end());
}

auto encode_apikey_metadata(
    const std::span<const std::string_view> environment_variables)
    -> std::vector<std::byte> {
  std::vector<std::byte> result;
  append_u32(result, static_cast<std::uint32_t>(environment_variables.size()));
  for (const auto variable : environment_variables) {
    append_u32(result, static_cast<std::uint32_t>(variable.size()));
    for (const char character : variable) {
      result.push_back(static_cast<std::byte>(character));
    }
  }

  return result;
}

} // namespace

namespace sourcemeta::one {

auto Authentication::save(std::span<const Authentication::Policy> policies,
                          const std::filesystem::path &configuration,
                          const std::filesystem::path &destination) -> void {
  // Each policy occupies one bit of the node masks, so exceeding the ceiling
  // would shift past the width of a PolicySet
  if (policies.size() > Authentication::MAXIMUM_POLICIES) {
    throw std::runtime_error("Too many authentication policies");
  }

  // An apiKey policy whose entire scope is already covered by a public policy
  // can never deny anyone, so it is unreachable configuration. A public
  // carve-out nested deeper inside the apiKey scope is not a cover.
  for (const auto &candidate : policies) {
    if (candidate.type != Type::ApiKey) {
      continue;
    }

    std::string_view shadowed;
    std::string_view shadow;
    bool fully_shadowed{!candidate.paths.empty()};
    for (const auto scope : candidate.paths) {
      std::string_view covering;
      for (const auto &other : policies) {
        if (other.type != Type::Public) {
          continue;
        }

        for (const auto prefix : other.paths) {
          if (sourcemeta::core::is_lexically_under_path(scope, prefix)) {
            covering = prefix;
            break;
          }
        }

        if (!covering.empty()) {
          break;
        }
      }

      if (covering.empty()) {
        fully_shadowed = false;
        break;
      }

      if (shadowed.empty()) {
        shadowed = scope;
        shadow = covering;
      }
    }

    if (fully_shadowed) {
      throw AuthenticationShadowedError(configuration, std::string{shadowed},
                                        std::string{shadow});
    }
  }

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

  // Sort each node's edges by segment so the matcher can binary search them
  for (auto &node : nodes) {
    std::ranges::sort(node.edges, {},
                      &std::pair<std::string, std::uint32_t>::first);
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

  // The per-policy metadata is appended after the string blob and located by
  // absolute offset, so the header layout stays fixed
  const auto metadata_start{header.strings_offset + header.strings_length};
  std::vector<AuthenticationPolicyEntry> policy_table;
  policy_table.reserve(policies.size());
  std::vector<std::byte> metadata;
  for (const auto &policy : policies) {
    const auto policy_metadata{policy.keys.empty()
                                   ? std::vector<std::byte>{}
                                   : encode_apikey_metadata(policy.keys)};
    AuthenticationPolicyEntry entry{};
    entry.metadata_offset =
        metadata_start + static_cast<std::uint32_t>(metadata.size());
    entry.metadata_length = static_cast<std::uint32_t>(policy_metadata.size());
    entry.type = static_cast<std::uint8_t>(policy.type);
    policy_table.push_back(entry);
    metadata.insert(metadata.end(), policy_metadata.begin(),
                    policy_metadata.end());
  }

  std::vector<std::byte> buffer(
      static_cast<std::size_t>(metadata_start) + metadata.size(), std::byte{0});
  std::memcpy(buffer.data(), &header, sizeof(header));
  // The policy table, node, edge, string, and metadata sections are each empty
  // in some valid artifacts, and an empty vector may expose a null data pointer
  if (!policy_table.empty()) {
    std::memcpy(buffer.data() + header.policies_offset, policy_table.data(),
                policy_table.size() * sizeof(AuthenticationPolicyEntry));
  }

  std::memcpy(buffer.data() + header.nodes_offset, serialized.data(),
              serialized.size() * sizeof(AuthenticationNode));
  if (!edges.empty()) {
    std::memcpy(buffer.data() + header.edges_offset, edges.data(),
                edges.size() * sizeof(AuthenticationEdge));
  }

  if (!strings.empty()) {
    std::memcpy(buffer.data() + header.strings_offset, strings.data(),
                strings.size());
  }

  if (!metadata.empty()) {
    std::memcpy(buffer.data() + metadata_start, metadata.data(),
                metadata.size());
  }

  sourcemeta::core::write_file(destination, buffer);
}

} // namespace sourcemeta::one
