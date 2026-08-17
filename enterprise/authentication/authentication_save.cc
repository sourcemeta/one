#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/text.h>

#include "authentication_format.h"

#include <algorithm>   // std::ranges::sort
#include <array>       // std::array
#include <cassert>     // assert
#include <cstddef>     // std::byte, std::size_t
#include <cstdint>     // std::uint32_t, std::uint64_t, std::uint8_t
#include <cstring>     // std::memcpy
#include <filesystem>  // std::filesystem::path
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

// Every field is stored as a length-prefixed string. Everything deciding who a
// policy admits leads, so that those bytes keep spanning exactly the audience
// it denotes: the provider client, and the rules narrowing which of its people
// are let in. Both the session secrets and the domains are counted, so that
// the field after each is found whatever their number.
//
// A domain is compared case-insensitively, since it names a host, and the
// order rules were written in says nothing about who they admit, so both are
// reduced here to the single spelling the artifact carries. Two policies
// admitting the same people must serialise identically, since that is what
// decides whether one may reference the other
auto encode_oidc_metadata(
    const std::string_view issuer, const std::string_view client_id,
    const std::string_view claims,
    const std::span<const std::string_view> email_domains,
    const std::string_view client_secret_variable, const std::string_view name,
    const std::span<const std::string_view> session_secret_variables,
    const std::string_view default_path) -> std::vector<std::byte> {
  std::vector<std::byte> result;
  append_string(result, issuer);
  append_string(result, client_id);
  append_string(result, claims);

  std::vector<std::string> domains;
  domains.reserve(email_domains.size());
  for (const auto domain : email_domains) {
    domains.emplace_back(domain);
    sourcemeta::core::to_lowercase(domains.back());
  }

  std::ranges::sort(domains);
  const auto repeated{std::ranges::unique(domains)};
  domains.erase(repeated.begin(), repeated.end());
  append_u32(result, static_cast<std::uint32_t>(domains.size()));
  for (const auto &domain : domains) {
    append_string(result, domain);
  }

  append_string(result, client_secret_variable);
  append_string(result, name);
  append_u32(result,
             static_cast<std::uint32_t>(session_secret_variables.size()));
  for (const auto variable : session_secret_variables) {
    append_string(result, variable);
  }

  append_string(result, default_path);
  return result;
}

// A media type is compared case-insensitively and with the `application/`
// prefix optional, so two spellings that admit exactly the same tokens are
// reduced to one here. The prefix only comes off a bare subtype, matching how
// a presented token's type is read: a value that is the prefix alone keeps it,
// since dropping it would leave nothing and turn a policy that names a type
// into one that accepts every type, and a value carrying a further separator
// keeps it too, since what follows is then not a subtype
auto canonical_token_type(const std::string_view token_type) -> std::string {
  std::string result{token_type};
  sourcemeta::core::to_lowercase(result);
  constexpr std::string_view MEDIA_TYPE_PREFIX{"application/"};
  if (result.size() > MEDIA_TYPE_PREFIX.size() &&
      result.starts_with(MEDIA_TYPE_PREFIX) &&
      result.find('/', MEDIA_TYPE_PREFIX.size()) == std::string::npos) {
    result.erase(0, MEDIA_TYPE_PREFIX.size());
  }

  return result;
}

// The issuer, audience, and key set location are stored as length-prefixed
// strings, followed by the allow-listed signature algorithms as one byte each,
// and last the claim rules, which are counted so that a reader reaches them
// whatever the number of algorithms before them. An empty key set location
// means the location is discovered from the issuer. Both the algorithms and
// the token type arrive in whatever shape the caller held them, and are
// reduced here to the single spelling the artifact carries, exactly as a path
// is. Two policies admitting the same tokens must serialise identically, since
// that is what decides whether one may reference the other
auto encode_jwt_metadata(
    const std::string_view issuer, const std::string_view audience,
    const std::string_view jwks_uri,
    const std::span<const sourcemeta::core::JWSAlgorithm> algorithms,
    const std::string_view token_type, const std::string_view claims)
    -> std::vector<std::byte> {
  std::vector<std::byte> result;
  append_string(result, issuer);
  append_string(result, audience);
  append_string(result, jwks_uri);
  append_string(result, canonical_token_type(token_type));

  // The allow-list decides admission by membership, so neither its order nor a
  // repeated entry carries any meaning, and only the set it denotes may
  std::vector<sourcemeta::core::JWSAlgorithm> sorted{algorithms.begin(),
                                                     algorithms.end()};
  std::ranges::sort(sorted);
  const auto repeated{std::ranges::unique(sorted)};
  sorted.erase(repeated.begin(), repeated.end());
  append_u32(result, static_cast<std::uint32_t>(sorted.size()));
  for (const auto algorithm : sorted) {
    result.push_back(
        static_cast<std::byte>(static_cast<std::uint8_t>(algorithm)));
  }

  append_string(result, claims);
  return result;
}

} // namespace

namespace sourcemeta::one {

auto Authentication::Table::compile(
    std::span<const Authentication::Policy> policies,
    const std::filesystem::path &configuration,
    const Authentication::PathGuard &gateable) -> std::vector<std::byte> {
  assert(gateable);
  // Each policy occupies one bit of the node masks, so exceeding the ceiling
  // would shift past the width of a PolicySet
  if (policies.size() > Authentication::MAXIMUM_POLICIES) {
    throw std::runtime_error("Too many authentication policies");
  }

  // A typo, a stray extension, or a scope this instance does not serve names
  // nothing the matcher would gate, leaving the target it was meant to
  // protect reachable by anybody
  for (const auto &policy : policies) {
    for (const auto policy_path : policy.paths) {
      if (!gateable(policy_path)) {
        throw AuthenticationUnknownPathError(configuration,
                                             std::string{policy_path});
      }
    }
  }

  std::vector<BuildNode> nodes;
  nodes.emplace_back();

  for (std::size_t index{0}; index < policies.size(); index += 1) {
    const auto bit{static_cast<std::uint64_t>(1) << index};
    for (const auto &policy_path : policies[index].paths) {
      // The trie is keyed by the same spelling a request resolves to, so a
      // configured path is canonicalised here rather than stored as written.
      // Otherwise a path that only differs cosmetically would build segments
      // no request could ever traverse, and its target would be left public
      const auto canonical{Authentication::Path::relative(policy_path)};
      const auto value{canonical.value()};
      std::uint32_t current{0};
      std::size_t cursor{0};
      for (auto segment{authentication_next_segment(value, cursor)};
           !segment.empty();
           segment = authentication_next_segment(value, cursor)) {
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

  // The table is computed once here, so that the naming rule is applied where
  // the policies are read rather than by every server that later serves them
  const auto table{Authentication::Table::enumerate(policies)};

  // Distinct policy names do not by themselves make distinct view names, since
  // a view naming several is spelled by joining theirs, which a single policy
  // could be named to match. Two views sharing a name are two sets of policies
  // served from one directory, so what was actually spelled is checked rather
  // than what it was spelled from.
  //
  // The table arrives with the anonymous view first and every other in order of
  // name, so a repeated name can only sit beside the one it repeats. Comparing
  // every pair instead would square a table that a handful of issuer groups
  // already leaves with hundreds of thousands of entries
  for (std::size_t index{1}; index < table.size(); index += 1) {
    if (table[index - 1].name == table[index].name) {
      throw AuthenticationViewNameCollisionError(configuration,
                                                 table[index].name);
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

  // A policy is named in the same blob its path segments live in, and a view
  // after it, so that every name is one range into one section
  std::vector<AuthenticationViewEntry> view_table;
  view_table.reserve(table.size());
  std::vector<std::pair<std::uint32_t, std::uint32_t>> policy_names;
  policy_names.reserve(policies.size());
  for (const auto &policy : policies) {
    policy_names.emplace_back(static_cast<std::uint32_t>(strings.size()),
                              static_cast<std::uint32_t>(policy.name.size()));
    strings += policy.name;
  }

  for (const auto &view : table) {
    AuthenticationViewEntry entry{};
    for (const auto member : view.policies) {
      entry.policies |= std::uint64_t{1} << member;
    }

    entry.name_offset = static_cast<std::uint32_t>(strings.size());
    entry.name_length = static_cast<std::uint32_t>(view.name.size());
    strings += view.name;
    view_table.push_back(entry);
  }

  AuthenticationHeader header{};
  header.magic = AUTHENTICATION_MAGIC;
  header.version = AUTHENTICATION_VERSION;
  header.policy_count = static_cast<std::uint32_t>(policies.size());
  header.node_count = static_cast<std::uint32_t>(serialized.size());
  header.edge_count = static_cast<std::uint32_t>(edges.size());
  header.view_count = static_cast<std::uint32_t>(view_table.size());
  header.policies_offset =
      static_cast<std::uint32_t>(sizeof(AuthenticationHeader));
  // The node array begins the word-aligned region the matcher addresses
  // directly, so pad past the byte-packed policy table
  header.nodes_offset = align_to_word(
      header.policies_offset +
      header.policy_count *
          static_cast<std::uint32_t>(sizeof(AuthenticationPolicyEntry)));
  // Both the node and the view arrays hold eight-byte-aligned entries of the
  // same width, so placing the views between the nodes and the byte-packed
  // edges keeps every section aligned without padding between them
  header.views_offset =
      header.nodes_offset + header.node_count * static_cast<std::uint32_t>(
                                                    sizeof(AuthenticationNode));
  header.edges_offset =
      header.views_offset +
      header.view_count *
          static_cast<std::uint32_t>(sizeof(AuthenticationViewEntry));
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
  for (std::size_t index{0}; index < policies.size(); index += 1) {
    const auto &policy{policies[index]};
    std::vector<std::byte> policy_metadata;
    auto algorithm{Authentication::Algorithm::Identity};
    if (const auto *token{
            std::get_if<Authentication::Policy::Token>(&policy.credential)}) {
      policy_metadata = encode_jwt_metadata(token->issuer, token->audience,
                                            token->jwks_uri, token->algorithms,
                                            token->token_type, token->claims);
    } else if (const auto *interactive{
                   std::get_if<Authentication::Policy::Interactive>(
                       &policy.credential)}) {
      // An interactive policy without a session secret could never mint or
      // verify one, so it fails loudly here rather than silently denying every
      // login at runtime
      if (interactive->session_secrets.empty()) {
        throw std::runtime_error(
            "Interactive authentication policies require a session secret");
      }

      policy_metadata = encode_oidc_metadata(
          interactive->issuer, interactive->client_id, interactive->claims,
          interactive->email_domains, interactive->client_secret_variable,
          policy.name, interactive->session_secrets,
          policy.paths.empty() ? std::string_view{} : policy.paths.front());
    } else {
      const auto &key{
          std::get<Authentication::Policy::ApiKey>(policy.credential)};
      algorithm = key.algorithm;
      if (!key.keys.empty()) {
        policy_metadata = encode_apikey_metadata(key.keys);
      }
    }

    AuthenticationPolicyEntry entry{};
    entry.metadata_offset =
        metadata_start + static_cast<std::uint32_t>(metadata.size());
    entry.metadata_length = static_cast<std::uint32_t>(policy_metadata.size());
    entry.name_offset = policy_names[index].first;
    entry.name_length = policy_names[index].second;
    entry.algorithm = static_cast<std::uint8_t>(algorithm);
    entry.type = static_cast<std::uint8_t>(policy.type());
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
  if (!view_table.empty()) {
    std::memcpy(buffer.data() + header.views_offset, view_table.data(),
                view_table.size() * sizeof(AuthenticationViewEntry));
  }

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

  return buffer;
}

auto Authentication::Table::write(const std::span<const std::byte> bytes,
                                  const std::filesystem::path &destination)
    -> void {
  sourcemeta::core::write_file(destination, bytes);
}

} // namespace sourcemeta::one
