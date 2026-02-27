#ifndef SOURCEMETA_ONE_BUILD_TYPES_H_
#define SOURCEMETA_ONE_BUILD_TYPES_H_

#include <cstdint>    // std::uint8_t
#include <functional> // std::function
#include <utility>    // std::pair, std::move
#include <vector>     // std::vector

namespace sourcemeta::one {

enum class BuildDependencyKind : std::uint8_t { Static, Dynamic };

template <typename NodeType>
using BuildDynamicCallback = std::function<void(const NodeType &)>;

template <typename NodeType>
using BuildDependencies = std::vector<std::pair<BuildDependencyKind, NodeType>>;

template <typename NodeType>
auto make_dependency(NodeType node)
    -> std::pair<BuildDependencyKind, NodeType> {
  return {BuildDependencyKind::Static, std::move(node)};
}

template <typename Context, typename NodeType>
using BuildHandler =
    std::function<void(const NodeType &, const BuildDependencies<NodeType> &,
                       const BuildDynamicCallback<NodeType> &,
                       const Context &)>;

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_TYPES_H_
