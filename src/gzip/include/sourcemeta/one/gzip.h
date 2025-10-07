#ifndef SOURCEMETA_ONE_GZIP_H_
#define SOURCEMETA_ONE_GZIP_H_

#include <sourcemeta/one/gzip_error.h>

#include <istream>     // std::istream
#include <ostream>     // std::ostream
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

auto gzip(std::istream &input, std::ostream &output) -> void;

auto gzip(std::istream &stream) -> std::string;

auto gzip(const std::string &input) -> std::string;

auto gunzip(std::istream &input, std::ostream &output) -> void;

auto gunzip(std::istream &stream) -> std::string;

auto gunzip(const std::string &input) -> std::string;

} // namespace sourcemeta::one

#endif
