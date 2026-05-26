#include <sourcemeta/core/dns.h>

#include <sourcemeta/core/unicode.h>

namespace sourcemeta::core {

// RFC 952 §B: let-dig = ALPHA / DIGIT
// RFC 1123 §2.1: first character of a label is letter or digit
static constexpr auto is_let_dig(const char character) -> bool {
  return (character >= 'A' && character <= 'Z') ||
         (character >= 'a' && character <= 'z') ||
         (character >= '0' && character <= '9');
}

// RFC 1123 §2.1: hostname grammar. When AllowUtf8 is true, RFC 5890 §2.3.2.3
// extends each label with UTF-8 non-ASCII bytes (RFC 6532 §3.1)
template <bool AllowUtf8>
static auto is_hostname_impl(const std::string_view value) -> bool {
  // RFC 952 §B: <hname> requires at least one <name>
  if (value.empty()) {
    return false;
  }

  // RFC 1123 §2.1: SHOULD handle host names of up to 255 characters
  if (value.size() > 255) {
    return false;
  }

  std::string_view::size_type position{0};
  while (position < value.size()) {
    const auto label_start{position};
    bool last_was_hyphen{false};
    bool label_has_content{false};

    while (position < value.size() && value[position] != '.') {
      const auto character{value[position]};
      if (character == '-') {
        // RFC 1123 §2.1: first character must be let-dig, never hyphen
        if (!label_has_content) {
          return false;
        }
        last_was_hyphen = true;
        position += 1;
        label_has_content = true;
        continue;
      }

      if (is_let_dig(character)) {
        last_was_hyphen = false;
        position += 1;
        label_has_content = true;
        continue;
      }

      if constexpr (AllowUtf8) {
        // RFC 5890 §2.3.2.3 / RFC 6532 §3.1: UTF-8 non-ASCII codepoint as a
        // U-label byte
        const auto utf8_length{utf8_codepoint_length(value, position)};
        if (utf8_length < 2) {
          return false;
        }
        last_was_hyphen = false;
        position += utf8_length;
        label_has_content = true;
      } else {
        return false;
      }
    }

    // RFC 1035 §2.3.4: per-label cap is 63 octets
    const auto label_length{position - label_start};
    if (label_length == 0 || label_length > 63 || last_was_hyphen) {
      return false;
    }

    if (position < value.size()) {
      // value[position] == '.'
      position += 1;
      if (position == value.size()) {
        // Trailing dot is not part of the host name grammar
        return false;
      }
    }
  }

  return true;
}

auto is_hostname(const std::string_view value) -> bool {
  return is_hostname_impl<false>(value);
}

auto is_idn_hostname(const std::string_view value) -> bool {
  return is_hostname_impl<true>(value);
}

} // namespace sourcemeta::core
