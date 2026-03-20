#ifndef SOURCEMETA_ONE_GZIP_H_
#define SOURCEMETA_ONE_GZIP_H_

#include <sourcemeta/one/gzip_error.h>

#include <cstddef> // std::size_t
#include <cstdint> // std::uint8_t
#include <string>  // std::string

namespace sourcemeta::one {

auto gzip(const std::uint8_t *input, std::size_t size) -> std::string;

auto gunzip(const std::uint8_t *input, std::size_t size,
            std::size_t output_hint = 0) -> std::string;

} // namespace sourcemeta::one

#endif
