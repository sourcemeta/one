#include <sourcemeta/core/unicode.h>

#include <cassert> // assert
#include <cstdint> // std::uint8_t
#include <sstream> // std::istringstream, std::ostringstream

#include "unicode_data.h"

namespace sourcemeta::core {

auto codepoint_to_utf8(const char32_t codepoint, std::ostream &output) -> void {
  assert(is_valid_codepoint(codepoint));
  if (codepoint < 0x80) {
    output.put(static_cast<char>(codepoint));
  } else if (codepoint < 0x800) {
    output.put(static_cast<char>(0xC0 | (codepoint >> 6)));
    output.put(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else if (codepoint < 0x10000) {
    output.put(static_cast<char>(0xE0 | (codepoint >> 12)));
    output.put(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    output.put(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else {
    output.put(static_cast<char>(0xF0 | (codepoint >> 18)));
    output.put(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
    output.put(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    output.put(static_cast<char>(0x80 | (codepoint & 0x3F)));
  }
}

auto codepoint_to_utf8(const char32_t codepoint, std::string &output) -> void {
  assert(is_valid_codepoint(codepoint));
  if (codepoint < 0x80) {
    output.push_back(static_cast<char>(codepoint));
  } else if (codepoint < 0x800) {
    output.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
    output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else if (codepoint < 0x10000) {
    output.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
    output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else {
    output.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
    output.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
    output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  }
}

auto codepoint_to_utf8(const char32_t codepoint) -> std::string {
  std::string output;
  codepoint_to_utf8(codepoint, output);
  return output;
}

auto utf8_to_utf32(std::istream &input) -> std::optional<std::u32string> {
  std::u32string result;
  std::uint8_t byte{0};

  while (input.read(reinterpret_cast<char *>(&byte), 1)) {
    const auto size{utf8_lead_byte_size(byte)};
    if (size == 0) {
      return std::nullopt;
    }
    if (size == 1) {
      result.push_back(byte);
      continue;
    }

    char32_t code_point{0};
    char32_t minimum{0};
    if (size == 2) {
      code_point = byte & 0x1F;
      minimum = 0x80;
    } else if (size == 3) {
      code_point = byte & 0x0F;
      minimum = 0x800;
    } else {
      code_point = byte & 0x07;
      minimum = 0x10000;
    }

    for (std::uint8_t index{1}; index < size; ++index) {
      std::uint8_t continuation{0};
      if (!input.read(reinterpret_cast<char *>(&continuation), 1) ||
          !is_utf8_continuation(continuation)) {
        return std::nullopt;
      }

      code_point = (code_point << 6) | (continuation & 0x3F);
    }

    if (code_point < minimum || !is_valid_codepoint(code_point)) {
      return std::nullopt;
    }

    result.push_back(code_point);
  }

  if (!input.eof()) {
    return std::nullopt;
  }

  return result;
}

auto utf8_to_utf32(const std::string_view input)
    -> std::optional<std::u32string> {
  // TODO: Replace std::istringstream with std::ispanstream once libc++
  // supports it (__cpp_lib_spanstream), to avoid copying the input string
  std::istringstream stream{std::string{input}};
  return utf8_to_utf32(stream);
}

auto combining_class(const char32_t codepoint) -> std::uint8_t {
  if (codepoint > 0x10FFFF) {
    return 0;
  }
  const std::size_t page{COMBINING_CLASS_STAGE1[codepoint >> 10U]};
  return COMBINING_CLASS_STAGE2[(page << 10U) | (codepoint & 0x3FFU)];
}

auto joining_type(const char32_t codepoint) -> JoiningType {
  if (codepoint > 0x10FFFF) {
    return JoiningType::NonJoining;
  }
  const std::size_t page{JOINING_TYPE_STAGE1[codepoint >> 10U]};
  return static_cast<JoiningType>(
      JOINING_TYPE_STAGE2[(page << 10U) | (codepoint & 0x3FFU)]);
}

auto bidi_class(const char32_t codepoint) -> BidiClass {
  if (codepoint > 0x10FFFF) {
    return BidiClass::LeftToRight;
  }
  const std::size_t page{BIDI_CLASS_STAGE1[codepoint >> 10U]};
  return static_cast<BidiClass>(
      BIDI_CLASS_STAGE2[(page << 10U) | (codepoint & 0x3FFU)]);
}

auto script(const char32_t codepoint) -> UnicodeScript {
  if (codepoint > 0x10FFFF) {
    return UnicodeScript::Unknown;
  }
  const std::size_t page{UNICODE_SCRIPT_STAGE1[codepoint >> 10U]};
  return static_cast<UnicodeScript>(
      UNICODE_SCRIPT_STAGE2[(page << 10U) | (codepoint & 0x3FFU)]);
}

} // namespace sourcemeta::core
