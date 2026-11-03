#ifndef SOURCEMETA_ONE_SHARED_LEAF_H
#define SOURCEMETA_ONE_SHARED_LEAF_H

#include <cstdint> // std::uint8_t

namespace sourcemeta::one {

// What a leaf of the catalog holds, which decides which artifacts the build
// writes for it, how to read what each one carries, and what the server serves
// from it
enum class LeafKind : std::uint8_t { Schema = 0 };

} // namespace sourcemeta::one

#endif
