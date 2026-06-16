#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/io.h>

#include "authentication_format.h"

#include <cstddef>   // std::size_t
#include <cstdint>   // std::uint32_t
#include <stdexcept> // std::runtime_error
#include <utility>   // std::move

namespace sourcemeta::one {

Authentication::Authentication(const std::filesystem::path &path) {
  // A missing artifact is the only unconfigured case, where the instance
  // admits everyone. An artifact that exists but cannot be mapped or
  // validated is a fatal misconfiguration: failing open would silently
  // expose every path that the policy was meant to restrict
  if (!std::filesystem::exists(path)) {
    return;
  }

  auto view{std::make_unique<sourcemeta::core::FileView>(path)};
  if (view->size() < sizeof(AuthenticationHeader)) {
    throw std::runtime_error("The authentication policy artifact is malformed");
  }

  const auto *header{view->as<AuthenticationHeader>()};
  if (header->magic != AUTHENTICATION_MAGIC ||
      header->version != AUTHENTICATION_VERSION) {
    throw std::runtime_error("The authentication policy artifact is malformed");
  }

  this->nodes_ = view->as<AuthenticationNode>(header->nodes_offset);
  // The edge and string sections are empty when no policy declares a nested
  // prefix, in which case they sit at the end of the buffer and must not be
  // addressed
  if (header->edge_count > 0) {
    this->edges_ = view->as<AuthenticationEdge>(header->edges_offset);
    this->strings_ = view->as<char>(header->strings_offset);
  }

  this->view_ = std::move(view);
}

Authentication::~Authentication() = default;

auto Authentication::match(const std::string_view registry_path) const noexcept
    -> Authentication::PolicySet {
  if (this->nodes_ == nullptr) {
    return 0;
  }

  const auto *nodes{static_cast<const AuthenticationNode *>(this->nodes_)};
  const auto *edges{static_cast<const AuthenticationEdge *>(this->edges_)};

  Authentication::PolicySet result{nodes[0].mask};
  std::uint32_t current{0};
  std::size_t cursor{0};
  for (auto segment{authentication_next_segment(registry_path, cursor)};
       !segment.empty();
       segment = authentication_next_segment(registry_path, cursor)) {
    const auto &node{nodes[current]};

    // A node's edges are serialized contiguously and sorted by segment, so
    // the matching child is found with a binary search
    std::uint32_t low{node.first_edge};
    std::uint32_t high{node.first_edge + node.edge_count};
    bool descended{false};
    while (low < high) {
      const auto middle{low + ((high - low) / 2)};
      const auto &edge{edges[middle]};
      const std::string_view value{this->strings_ + edge.segment_offset,
                                   edge.segment_length};
      const auto comparison{value.compare(segment)};
      if (comparison == 0) {
        current = edge.child;
        result |= nodes[current].mask;
        descended = true;
        break;
      } else if (comparison < 0) {
        low = middle + 1;
      } else {
        high = middle;
      }
    }

    if (!descended) {
      break;
    }
  }

  return result;
}

auto Authentication::admits(const std::string_view registry_path,
                            const std::string_view) const noexcept
    -> Authentication::Verdict {
  // An unconfigured instance admits everyone. Once configured, a path is
  // served only when at least one policy governs it, as every policy grants
  // anonymous public access for now
  if (this->nodes_ == nullptr) {
    return {.allowed = true, .key_name = {}};
  }

  return {.allowed = this->match(registry_path) != 0, .key_name = {}};
}

} // namespace sourcemeta::one
