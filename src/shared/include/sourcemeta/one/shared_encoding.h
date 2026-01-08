#ifndef SOURCEMETA_ONE_SHARED_ENCODING_H_
#define SOURCEMETA_ONE_SHARED_ENCODING_H_

#include <cstdint> // std::uint8_t

namespace sourcemeta::one {

enum class Encoding : std::uint8_t { Identity, GZIP };

} // namespace sourcemeta::one

#endif
