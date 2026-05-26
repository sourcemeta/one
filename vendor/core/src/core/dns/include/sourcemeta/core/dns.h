#ifndef SOURCEMETA_CORE_DNS_H_
#define SOURCEMETA_CORE_DNS_H_

#ifndef SOURCEMETA_CORE_DNS_EXPORT
#include <sourcemeta/core/dns_export.h>
#endif

#include <string_view> // std::string_view

/// @defgroup dns DNS
/// @brief DNS and hostname validation utilities.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/dns.h>
/// ```

namespace sourcemeta::core {

/// @ingroup dns
/// Check whether the given string is a valid Internet host name per
/// RFC 1123 Section 2.1, which relaxes the first-character rule of
/// RFC 952 to allow either a letter or a digit. For example:
///
/// ```cpp
/// #include <sourcemeta/core/dns.h>
///
/// #include <cassert>
///
/// assert(sourcemeta::core::is_hostname("www.example.com"));
/// assert(sourcemeta::core::is_hostname("1host"));
/// assert(!sourcemeta::core::is_hostname("-bad"));
/// assert(!sourcemeta::core::is_hostname("example."));
/// ```
///
/// This function implements RFC 1123 §2.1 (ASCII only). It does not
/// perform A-label or Punycode decoding. For internationalized host
/// names see `is_idn_hostname`.
SOURCEMETA_CORE_DNS_EXPORT
auto is_hostname(const std::string_view value) -> bool;

/// @ingroup dns
/// Check whether the given string is a valid internationalized host name.
/// Accepts every input that `is_hostname` accepts, and additionally allows
/// each label to contain valid UTF-8 non-ASCII byte sequences (RFC 6532
/// Section 3.1), modelling the U-label extension of RFC 5890 Section
/// 2.3.2.3. For example:
///
/// ```cpp
/// #include <sourcemeta/core/dns.h>
///
/// #include <cassert>
///
/// assert(sourcemeta::core::is_idn_hostname("www.example.com"));
/// assert(sourcemeta::core::is_idn_hostname(
///     "\xec\x8b\xa4\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
/// assert(!sourcemeta::core::is_idn_hostname("-bad"));
/// ```
///
/// This is a best-effort lexical check: it accepts the byte-level structure
/// of an RFC 5890 U-label but does not perform full IDNA2008 validation
/// (no NFC normalization, no Bidi rule, no ContextJ/O checks, no Punycode
/// round-trip).
SOURCEMETA_CORE_DNS_EXPORT
auto is_idn_hostname(const std::string_view value) -> bool;

} // namespace sourcemeta::core

#endif
