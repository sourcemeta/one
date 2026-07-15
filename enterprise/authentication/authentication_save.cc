#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/uritemplate.h>

#include "authentication_format.h"

#include <algorithm>   // std::ranges::sort
#include <array>       // std::array
#include <cstddef>     // std::byte, std::size_t
#include <cstdint>     // std::uint32_t, std::uint64_t, std::uint8_t
#include <cstring>     // std::memcpy
#include <filesystem>  // std::filesystem::create_directories
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

auto append_string(std::vector<std::byte> &output, const std::string_view value)
    -> void {
  append_u32(output, static_cast<std::uint32_t>(value.size()));
  for (const char character : value) {
    output.push_back(static_cast<std::byte>(character));
  }
}

auto encode_apikey_metadata(
    const std::span<const std::string_view> environment_variables)
    -> std::vector<std::byte> {
  std::vector<std::byte> result;
  append_u32(result, static_cast<std::uint32_t>(environment_variables.size()));
  for (const auto variable : environment_variables) {
    append_string(result, variable);
  }

  return result;
}

// The issuer, client identifier, the name of the environment variable
// holding the client secret, and the policy name are stored as
// length-prefixed strings. The policy name comes last so that the leading
// bytes keep spanning exactly the provider client identity
auto encode_oidc_metadata(const std::string_view issuer,
                          const std::string_view client_id,
                          const std::string_view client_secret_variable,
                          const std::string_view name)
    -> std::vector<std::byte> {
  std::vector<std::byte> result;
  append_string(result, issuer);
  append_string(result, client_id);
  append_string(result, client_secret_variable);
  append_string(result, name);
  return result;
}

// The issuer, audience, and key set location are stored as length-prefixed
// strings, followed by the allow-listed signature algorithms as one byte each.
// An empty key set location means the location is discovered from the issuer
auto encode_jwt_metadata(
    const std::string_view issuer, const std::string_view audience,
    const std::string_view jwks_uri,
    const std::span<const sourcemeta::core::JWSAlgorithm> algorithms)
    -> std::vector<std::byte> {
  std::vector<std::byte> result;
  append_string(result, issuer);
  append_string(result, audience);
  append_string(result, jwks_uri);
  append_u32(result, static_cast<std::uint32_t>(algorithms.size()));
  for (const auto algorithm : algorithms) {
    result.push_back(
        static_cast<std::byte>(static_cast<std::uint8_t>(algorithm)));
  }

  return result;
}

} // namespace

namespace sourcemeta::one {

auto Authentication::save(std::span<const Authentication::Policy> policies,
                          const std::filesystem::path &,
                          const std::filesystem::path &destination) -> void {
  // Each policy occupies one bit of the node masks, so exceeding the ceiling
  // would shift past the width of a PolicySet
  if (policies.size() > Authentication::MAXIMUM_POLICIES) {
    throw std::runtime_error("Too many authentication policies");
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
    std::vector<std::byte> policy_metadata;
    if (policy.type == Authentication::Type::JWT) {
      policy_metadata = encode_jwt_metadata(policy.issuer, policy.audience,
                                            policy.jwks_uri, policy.algorithms);
    } else if (policy.type == Authentication::Type::OIDC) {
      // A nameless interactive policy could never match a session cookie, so
      // it fails loudly here rather than silently denying at the gate
      if (policy.name.empty()) {
        throw std::runtime_error(
            "Interactive authentication policies require a name");
      }

      policy_metadata =
          encode_oidc_metadata(policy.issuer, policy.client_id,
                               policy.client_secret_variable, policy.name);
    } else if (!policy.keys.empty()) {
      policy_metadata = encode_apikey_metadata(policy.keys);
    }

    AuthenticationPolicyEntry entry{};
    entry.metadata_offset =
        metadata_start + static_cast<std::uint32_t>(metadata.size());
    entry.metadata_length = static_cast<std::uint32_t>(policy_metadata.size());
    entry.algorithm = static_cast<std::uint8_t>(policy.algorithm);
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

auto Authentication::save(const Configuration &configuration,
                          const sourcemeta::core::URITemplateRouterView &routes,
                          const std::filesystem::path &destination) -> void {
  // The policy paths and keys are borrowed by the policies, so keep them alive
  // alongside the policies that reference them
  std::vector<std::vector<std::string_view>> policy_paths;
  policy_paths.reserve(configuration.authentication.size());
  std::vector<std::vector<std::string_view>> policy_keys;
  policy_keys.reserve(configuration.authentication.size());
  std::vector<Authentication::Policy> policies;
  policies.reserve(configuration.authentication.size());
  for (const auto &entry : configuration.authentication) {
    std::vector<std::string_view> paths;
    paths.reserve(entry.paths.size());
    for (const auto &path : entry.paths) {
      paths.push_back(path);
    }

    policy_paths.push_back(std::move(paths));

    if (entry.type == Configuration::AuthenticationEntry::Type::JWT) {
      policies.push_back(
          {.paths = policy_paths.back(),
           .type = Authentication::Type::JWT,
           .issuer = entry.issuer,
           .audience = entry.audience,
           .jwks_uri = entry.jwks_uri.has_value()
                           ? std::string_view{entry.jwks_uri.value()}
                           : std::string_view{},
           .algorithms = entry.algorithms});
    } else {
      std::vector<std::string_view> keys;
      keys.reserve(entry.keys.size());
      for (const auto &key : entry.keys) {
        keys.push_back(key);
      }

      policy_keys.push_back(std::move(keys));
      const auto algorithm{
          entry.algorithm ==
                  Configuration::AuthenticationEntry::Algorithm::Sha256
              ? Authentication::Algorithm::Sha256
              : Authentication::Algorithm::Identity};
      policies.push_back({.paths = policy_paths.back(),
                          .keys = policy_keys.back(),
                          .algorithm = algorithm});
    }
  }

  // A policy gates a route or a declared collection or page (or a namespace
  // above one), never a path inside a collection. Anything else, a typo, a
  // stray extension, or a schema-level scope, names nothing the matcher would
  // gate, which under the fail-open default would leave the target public
  for (const auto &entry : configuration.authentication) {
    for (const auto &policy_path : entry.paths) {
      if (!routes.describes(policy_path, configuration.base_path) &&
          !configuration.covers_entry(policy_path)) {
        throw AuthenticationUnknownPathError(configuration.path,
                                             std::string{policy_path});
      }
    }
  }

  std::filesystem::create_directories(destination.parent_path());
  Authentication::save(policies, configuration.path, destination);
}

} // namespace sourcemeta::one
