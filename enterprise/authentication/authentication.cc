#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/io.h>

#include "authentication_format.h"

#include <algorithm>     // std::ranges::all_of
#include <bit>           // std::countr_zero
#include <cstddef>       // std::byte, std::size_t
#include <cstdint>       // std::uint32_t, std::uint64_t
#include <cstdlib>       // std::getenv
#include <cstring>       // std::memcpy
#include <filesystem>    // std::filesystem::path
#include <limits>        // std::numeric_limits
#include <memory>        // std::unique_ptr, std::make_unique
#include <mutex>         // std::mutex, std::lock_guard
#include <optional>      // std::optional
#include <span>          // std::span
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move
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
          entry.metadata_length > size - metadata_cursor) {
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

auto constant_time_equal(const std::string_view left,
                         const std::string_view right) -> bool {
  if (left.size() != right.size()) {
    return false;
  }

  int difference{0};
  for (std::size_t index{0}; index < left.size(); index += 1) {
    difference |= static_cast<unsigned char>(left[index]) ^
                  static_cast<unsigned char>(right[index]);
  }

  return difference == 0;
}

auto resolve_environment(const std::string_view variable)
    -> std::optional<std::string> {
  static std::mutex mutex;
  static std::unordered_map<std::string, std::optional<std::string>> cache;
  const std::string name{variable};
  const std::lock_guard<std::mutex> lock{mutex};
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
                   const std::string_view credential) -> bool {
  std::size_t cursor{0};
  std::uint32_t count{0};
  if (!read_u32(metadata, cursor, count)) {
    return false;
  }

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
    if (stored.has_value() && constant_time_equal(credential, stored.value())) {
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
  explicit Impl(const std::filesystem::path &path) {
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
          stem = find_child(node, edges, this->strings_,
                            segment.substr(0, extension));
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
                            const std::string_view credential) const -> bool {
    // A missing or structurally broken artifact leaves the section pointers
    // null and denies everything. Only a valid policy fails open below
    if (this->nodes_ == nullptr) {
      return false;
    }

    const auto governing{this->match(registry_path)};
    if (governing == 0) {
      // No policy covers this path, so it is public
      return true;
    }

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

      if (admits_apikey(metadata, credential)) {
        return true;
      }
    }

    return false;
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
        collect_keys(metadata, result.keys);
      }
    }

    return result;
  }

  [[nodiscard]] auto
  reference_permitted(const std::string_view referrer_path,
                      const std::string_view referent_path) const -> bool {
    const auto referent{this->audience(referent_path)};
    if (referent.is_public) {
      return true;
    }

    const auto referrer{this->audience(referrer_path)};
    if (referrer.is_public) {
      return false;
    }

    return std::ranges::all_of(referrer.keys, [&referent](const auto key) {
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

  // The policy table, resolved once at construction, holding the type of each
  // policy so that admits can dispatch on it. Null when the instance is
  // unconfigured or declares no policies
  const void *policies_{nullptr};
  std::uint32_t policy_count_{0};

  std::unique_ptr<sourcemeta::core::FileView> view_;
};

Authentication::Authentication(const std::filesystem::path &path)
    : impl_{std::make_unique<Impl>(path)} {}

Authentication::~Authentication() = default;

auto Authentication::admits(const std::string_view registry_path,
                            const std::string_view credential,
                            const std::string_view base_path) const
    -> Authentication::Verdict {
  return {.allowed = this->impl_->admits(
              strip_base_path(registry_path, base_path), credential)};
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
