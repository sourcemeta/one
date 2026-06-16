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

  this->view_ = std::move(view);
}

Authentication::~Authentication() = default;

auto Authentication::match(const std::string_view registry_path) const noexcept
    -> Authentication::PolicySet {
  if (!this->view_) {
    return 0;
  }

  const auto *header{this->view_->as<AuthenticationHeader>()};
  const auto *nodes{this->view_->as<AuthenticationNode>(header->nodes_offset)};

  // The edge and string sections are empty when no policy declares a nested
  // prefix, in which case they sit at the end of the buffer and must not be
  // addressed
  const AuthenticationEdge *edges{nullptr};
  const char *strings{nullptr};
  if (header->edge_count > 0) {
    edges = this->view_->as<AuthenticationEdge>(header->edges_offset);
    strings = this->view_->as<char>(header->strings_offset);
  }

  Authentication::PolicySet result{nodes[0].mask};
  std::uint32_t current{0};
  std::size_t cursor{0};
  for (auto segment{authentication_next_segment(registry_path, cursor)};
       !segment.empty();
       segment = authentication_next_segment(registry_path, cursor)) {
    const auto &node{nodes[current]};
    bool descended{false};
    for (std::uint32_t index{0}; index < node.edge_count; index += 1) {
      const auto &edge{edges[node.first_edge + index]};
      const std::string_view value{strings + edge.segment_offset,
                                   edge.segment_length};
      if (value == segment) {
        current = edge.child;
        result |= nodes[current].mask;
        descended = true;
        break;
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
  if (!this->view_) {
    return {.allowed = true, .key_name = {}};
  }

  return {.allowed = this->match(registry_path) != 0, .key_name = {}};
}

} // namespace sourcemeta::one
