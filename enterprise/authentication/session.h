#ifndef SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_SESSION_H_
#define SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_SESSION_H_

#include <sourcemeta/one/authentication.h>

#include <chrono>      // std::chrono::sys_seconds
#include <cstdint>     // std::uint8_t
#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

// The sealing primitive the sessions and login transactions of this module are
// built out of. It sits beside the sources that use it rather than under the
// installed headers, so nothing outside this directory can reach it, and the
// interface next door stays what a consumer may call
namespace sourcemeta::one {

// What a sealed value is for. A value is only ever opened for the purpose it
// was sealed under, because the two derive different keys from the policy's
// secret, so one kind of value cannot be presented as the other
enum class SealPurpose : std::uint8_t { Session = 0, Transaction = 1 };

// Bind a payload and an expiry under a key derived from the secret and the
// purpose, producing a value that is safe to transport as a cookie. Only a
// holder of the secret can produce or alter such a value, though anyone can
// read its contents
[[nodiscard]] auto seal_value(std::string_view payload, SealPurpose purpose,
                              std::string_view secret,
                              std::chrono::sys_seconds issued,
                              std::chrono::sys_seconds expiry) -> std::string;

// Recover the payload of a sealed value, returning nothing for a value that was
// not produced for that purpose under one of the given secrets, was altered in
// any way, or has expired. Accepting several secrets lets a newly introduced
// secret coexist with the one it replaces until every value sealed under the
// old secret has expired
[[nodiscard]] auto open_value(std::string_view value, SealPurpose purpose,
                              std::span<const std::string_view> secrets,
                              std::chrono::sys_seconds now)
    -> std::optional<std::string>;

} // namespace sourcemeta::one

#endif
