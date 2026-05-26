#ifndef SOURCEMETA_CORE_UNICODE_H_
#define SOURCEMETA_CORE_UNICODE_H_

#ifndef SOURCEMETA_CORE_UNICODE_EXPORT
#include <sourcemeta/core/unicode_export.h>
#endif

#include <sourcemeta/core/unicode_ucd.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <istream>     // std::istream
#include <optional>    // std::optional
#include <ostream>     // std::ostream
#include <string>      // std::string, std::u32string
#include <string_view> // std::string_view

/// @defgroup unicode Unicode
/// @brief Unicode encoding utilities.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// ```

namespace sourcemeta::core {

/// @ingroup unicode
/// Encode a single Unicode codepoint as a UTF-8 string. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::codepoint_to_utf8(0x41) == "A");
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto codepoint_to_utf8(const char32_t codepoint) -> std::string;

/// @ingroup unicode
/// Encode a single Unicode codepoint as UTF-8 into an output stream.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <sstream>
/// #include <cassert>
///
/// std::ostringstream output;
/// sourcemeta::core::codepoint_to_utf8(0x41, output);
/// assert(output.str() == "A");
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto codepoint_to_utf8(const char32_t codepoint, std::ostream &output) -> void;

/// @ingroup unicode
/// Encode a single Unicode codepoint as UTF-8, appending to an existing string.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// std::string output;
/// sourcemeta::core::codepoint_to_utf8(0x41, output);
/// assert(output == "A");
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto codepoint_to_utf8(const char32_t codepoint, std::string &output) -> void;

/// @ingroup unicode
/// Decode a UTF-8 byte stream into a sequence of Unicode codepoints (UTF-32).
/// Returns std::nullopt if the input contains invalid UTF-8. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <sstream>
/// #include <cassert>
///
/// std::istringstream input{"A"};
/// const auto result{sourcemeta::core::utf8_to_utf32(input)};
/// assert(result.has_value());
/// assert(result.value() == std::u32string{0x41});
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto utf8_to_utf32(std::istream &input) -> std::optional<std::u32string>;

/// @ingroup unicode
/// Decode a UTF-8 string into a sequence of Unicode codepoints (UTF-32).
/// Returns std::nullopt if the input contains invalid UTF-8. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// const auto result{sourcemeta::core::utf8_to_utf32("A")};
/// assert(result.has_value());
/// assert(result.value() == std::u32string{0x41});
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto utf8_to_utf32(const std::string_view input)
    -> std::optional<std::u32string>;

/// @ingroup unicode
/// Determine the byte length encoded by a UTF-8 lead byte. Returns 1 for an
/// ASCII byte (%x00-7F), 2 for a 2-byte lead (%xC2-DF), 3 for a 3-byte lead
/// (%xE0-EF), 4 for a 4-byte lead (%xF0-F4), or 0 for any other byte
/// (continuation byte, overlong %xC0/%xC1, or out-of-range %xF5-FF).
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::utf8_lead_byte_size(0x41) == 1);
/// assert(sourcemeta::core::utf8_lead_byte_size(0xCE) == 2);
/// assert(sourcemeta::core::utf8_lead_byte_size(0xE4) == 3);
/// assert(sourcemeta::core::utf8_lead_byte_size(0xF0) == 4);
/// assert(sourcemeta::core::utf8_lead_byte_size(0x80) == 0);
/// ```
inline constexpr auto utf8_lead_byte_size(const unsigned char byte)
    -> std::uint8_t {
  if (byte < 0x80) {
    return 1;
  }
  if (byte >= 0xC2 && byte <= 0xDF) {
    return 2;
  }
  if (byte >= 0xE0 && byte <= 0xEF) {
    return 3;
  }
  if (byte >= 0xF0 && byte <= 0xF4) {
    return 4;
  }
  return 0;
}

/// @ingroup unicode
/// Check whether the given byte is a UTF-8 continuation byte (%x80-BF per
/// RFC 6532 Section 3.1). For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::is_utf8_continuation(0x80));
/// assert(sourcemeta::core::is_utf8_continuation(0xBF));
/// assert(!sourcemeta::core::is_utf8_continuation(0x7F));
/// assert(!sourcemeta::core::is_utf8_continuation(0xC0));
/// ```
inline constexpr auto is_utf8_continuation(const unsigned char byte) -> bool {
  return byte >= 0x80 && byte <= 0xBF;
}

/// @ingroup unicode
/// Check whether the given codepoint is in the UTF-16 surrogate range
/// (U+D800 to U+DFFF), which is forbidden in scalar Unicode text.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::is_surrogate(0xD800));
/// assert(sourcemeta::core::is_surrogate(0xDFFF));
/// assert(!sourcemeta::core::is_surrogate(0xD7FF));
/// assert(!sourcemeta::core::is_surrogate(0xE000));
/// ```
inline constexpr auto is_surrogate(const char32_t codepoint) -> bool {
  return codepoint >= 0xD800 && codepoint <= 0xDFFF;
}

/// @ingroup unicode
/// Check whether the given value is a valid Unicode codepoint: in the range
/// U+0000 to U+10FFFF, excluding the UTF-16 surrogate range (U+D800 to
/// U+DFFF). For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::is_valid_codepoint(0x0000));
/// assert(sourcemeta::core::is_valid_codepoint(0x10FFFF));
/// assert(!sourcemeta::core::is_valid_codepoint(0xD800));
/// assert(!sourcemeta::core::is_valid_codepoint(0x110000));
/// ```
inline constexpr auto is_valid_codepoint(const char32_t codepoint) -> bool {
  return codepoint <= 0x10FFFF && !is_surrogate(codepoint);
}

/// @ingroup unicode
/// Determine the number of UTF-8 bytes that a codepoint encodes to per
/// RFC 3629: 1 byte for U+0000-U+007F, 2 bytes for U+0080-U+07FF, 3 bytes
/// for U+0800-U+FFFF, and 4 bytes for U+10000 and above. The caller is
/// responsible for ensuring the codepoint is in range. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::utf8_codepoint_byte_count(0x0041) == 1);
/// assert(sourcemeta::core::utf8_codepoint_byte_count(0x00E9) == 2);
/// assert(sourcemeta::core::utf8_codepoint_byte_count(0x4E2D) == 3);
/// assert(sourcemeta::core::utf8_codepoint_byte_count(0x1F600) == 4);
/// ```
inline constexpr auto utf8_codepoint_byte_count(const char32_t codepoint)
    -> std::uint8_t {
  if (codepoint < 0x80) {
    return 1;
  }
  if (codepoint < 0x800) {
    return 2;
  }
  if (codepoint < 0x10000) {
    return 3;
  }
  return 4;
}

/// @ingroup unicode
/// Return the canonical combining class of a Unicode codepoint. See
/// https://www.unicode.org/reports/tr44/ for the property's definition.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::combining_class(U'\u094D') == 9);
/// assert(sourcemeta::core::combining_class(U'\u0301') == 230);
/// assert(sourcemeta::core::combining_class(U'A') == 0);
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto combining_class(const char32_t codepoint) -> std::uint8_t;

/// @ingroup unicode
/// Return the joining type of a Unicode codepoint. See
/// https://www.unicode.org/reports/tr44/ for the property's definition.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::joining_type(U'\u0628') ==
///        sourcemeta::core::JoiningType::DualJoining);
/// assert(sourcemeta::core::joining_type(U'\u200D') ==
///        sourcemeta::core::JoiningType::JoinCausing);
/// assert(sourcemeta::core::joining_type(U'A') ==
///        sourcemeta::core::JoiningType::NonJoining);
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto joining_type(const char32_t codepoint) -> JoiningType;

/// @ingroup unicode
/// Return the bidirectional class of a Unicode codepoint. See
/// https://www.unicode.org/reports/tr44/ for the property's definition.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::bidi_class(U'A') ==
///        sourcemeta::core::BidiClass::LeftToRight);
/// assert(sourcemeta::core::bidi_class(U'\u05D0') ==
///        sourcemeta::core::BidiClass::RightToLeft);
/// assert(sourcemeta::core::bidi_class(U'\u0627') ==
///        sourcemeta::core::BidiClass::ArabicLetter);
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto bidi_class(const char32_t codepoint) -> BidiClass;

/// @ingroup unicode
/// Return the script of a Unicode codepoint. See
/// https://www.unicode.org/reports/tr24/ for the property's definition.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::script(U'A') ==
///        sourcemeta::core::UnicodeScript::Latin);
/// assert(sourcemeta::core::script(U'\u0391') ==
///        sourcemeta::core::UnicodeScript::Greek);
/// assert(sourcemeta::core::script(U'\u05D0') ==
///        sourcemeta::core::UnicodeScript::Hebrew);
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto script(const char32_t codepoint) -> UnicodeScript;

/// @ingroup unicode
/// Determine the byte length of the valid UTF-8 codepoint starting at the
/// given position within the input. Returns 1 for an ASCII byte, 2/3/4 for a
/// valid multi-byte UTF-8 sequence (RFC 6532 Section 3.1, excluding overlong
/// encodings, surrogates, and code points above U+10FFFF), or 0 if the bytes
/// at that position do not start a valid UTF-8 codepoint. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::utf8_codepoint_length("A", 0) == 1);
/// assert(sourcemeta::core::utf8_codepoint_length("\xce\xb1", 0) == 2);
/// assert(sourcemeta::core::utf8_codepoint_length("\xe4\xb8\xad", 0) == 3);
/// assert(sourcemeta::core::utf8_codepoint_length("\xf0\x9f\x98\x80", 0) == 4);
/// assert(sourcemeta::core::utf8_codepoint_length("\xed\xa0\x80", 0) == 0);
/// ```
inline constexpr auto
utf8_codepoint_length(const std::string_view input,
                      const std::string_view::size_type position)
    -> std::size_t {
  if (position >= input.size()) {
    return 0;
  }
  const auto byte_0{static_cast<unsigned char>(input[position])};
  const auto size{utf8_lead_byte_size(byte_0)};
  if (size == 0 || position + size > input.size()) {
    return 0;
  }
  if (size == 1) {
    return 1;
  }

  // The second byte after the lead has tighter sub-ranges for specific leads
  // (RFC 6532 §3.1) that exclude overlong encodings, surrogates, and code
  // points above U+10FFFF
  const auto byte_1{static_cast<unsigned char>(input[position + 1])};
  bool byte_1_ok{false};
  if (size == 2) {
    byte_1_ok = is_utf8_continuation(byte_1);
  } else if (size == 3) {
    if (byte_0 == 0xE0) {
      byte_1_ok = byte_1 >= 0xA0 && byte_1 <= 0xBF;
    } else if (byte_0 == 0xED) {
      byte_1_ok = byte_1 >= 0x80 && byte_1 <= 0x9F;
    } else {
      byte_1_ok = is_utf8_continuation(byte_1);
    }
  } else {
    if (byte_0 == 0xF0) {
      byte_1_ok = byte_1 >= 0x90 && byte_1 <= 0xBF;
    } else if (byte_0 == 0xF4) {
      byte_1_ok = byte_1 >= 0x80 && byte_1 <= 0x8F;
    } else {
      byte_1_ok = is_utf8_continuation(byte_1);
    }
  }

  if (!byte_1_ok) {
    return 0;
  }

  // Remaining continuation bytes (if any) are unconstrained beyond the
  // continuation byte range
  for (std::size_t index{2}; index < size; ++index) {
    if (!is_utf8_continuation(
            static_cast<unsigned char>(input[position + index]))) {
      return 0;
    }
  }

  return size;
}

} // namespace sourcemeta::core

#endif
