#ifndef SOURCEMETA_ONE_BUILD_TYPES_H_
#define SOURCEMETA_ONE_BUILD_TYPES_H_

#include <functional> // std::function
#include <vector>     // std::vector

namespace sourcemeta::one {

/// @ingroup build
template <typename NodeType>
using BuildDynamicCallback = std::function<void(const NodeType &)>;

/// @ingroup build
template <typename NodeType> using BuildDependencies = std::vector<NodeType>;

/// @ingroup build
template <typename Context, typename NodeType>
using BuildHandler =
    std::function<void(const NodeType &, const BuildDependencies<NodeType> &,
                       const BuildDynamicCallback<NodeType> &,
                       const Context &)>;

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_TYPES_H_
