#ifndef SOURCEMETA_ONE_AUTHENTICATION_SESSION_H
#define SOURCEMETA_ONE_AUTHENTICATION_SESSION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <chrono>      // std::chrono::sys_seconds
#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

// Bind a payload and an expiry under a keyed signature, producing a value that
// is safe to transport as a cookie. Only a holder of the secret can produce or
// alter such a value, though anyone can read its contents
auto SOURCEMETA_ONE_AUTHENTICATION_EXPORT
session_seal(std::string_view payload, std::string_view secret,
             std::chrono::sys_seconds expiry) -> std::string;

// Recover the payload of a sealed value, returning nothing for a value that
// was not produced under one of the given secrets, was altered in any way, or
// has expired. Accepting several secrets lets a newly introduced secret
// coexist with the one it replaces until every value sealed under the old
// secret has expired
auto SOURCEMETA_ONE_AUTHENTICATION_EXPORT
session_open(std::string_view value, std::span<const std::string_view> secrets,
             std::chrono::sys_seconds now) -> std::optional<std::string>;

} // namespace sourcemeta::one

#endif
