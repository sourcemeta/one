#ifndef SOURCEMETA_CORE_YAML_STRINGIFY_H_
#define SOURCEMETA_CORE_YAML_STRINGIFY_H_

#include <sourcemeta/core/json.h>

#include <array>   // std::array
#include <cassert> // assert
#include <cmath>   // std::modf
#include <cstddef> // std::size_t
#include <iomanip> // std::setprecision
#include <ios>     // std::noshowpoint, std::fixed
#include <ostream> // std::basic_ostream
#include <string>  // std::to_string

namespace sourcemeta::core::yaml {

using OutputStream = std::basic_ostream<JSON::Char, JSON::CharTraits>;

static constexpr std::size_t INDENT_WIDTH{2};
static constexpr std::array<char, 16> HEX_DIGITS{{'0', '1', '2', '3', '4', '5',
                                                  '6', '7', '8', '9', 'a', 'b',
                                                  'c', 'd', 'e', 'f'}};

inline auto write_indent(OutputStream &stream, const std::size_t indent)
    -> void {
  for (std::size_t index{0}; index < indent * INDENT_WIDTH; ++index) {
    stream.put(' ');
  }
}

inline auto looks_like_number(const std::string &value) -> bool {
  std::size_t start{0};
  if (value[0] == '-' || value[0] == '+') {
    start = 1;
  }

  if (start >= value.size()) {
    return false;
  }

  if (value.size() > start + 1 && value[start] == '0') {
    const char second{value[start + 1]};
    if (second == 'x' || second == 'X' || second == 'o' || second == 'O') {
      return true;
    }
  }

  bool has_digit{false};
  bool has_dot{false};
  bool has_exponent{false};

  for (std::size_t index{start}; index < value.size(); ++index) {
    const char character{value[index]};
    if (character >= '0' && character <= '9') {
      has_digit = true;
    } else if (character == '.' && !has_dot && !has_exponent) {
      has_dot = true;
    } else if ((character == 'e' || character == 'E') && !has_exponent &&
               has_digit) {
      has_exponent = true;
      if (index + 1 < value.size() &&
          (value[index + 1] == '+' || value[index + 1] == '-')) {
        ++index;
      }
    } else {
      return false;
    }
  }

  return has_digit;
}

inline auto needs_quoting(const std::string &value) -> bool {
  if (value.empty()) {
    return true;
  }

  if (value == "null" || value == "Null" || value == "NULL" || value == "~" ||
      value == "true" || value == "True" || value == "TRUE" ||
      value == "false" || value == "False" || value == "FALSE") {
    return true;
  }

  if (value == ".inf" || value == ".Inf" || value == ".INF" ||
      value == "+.inf" || value == "+.Inf" || value == "+.INF" ||
      value == "-.inf" || value == "-.Inf" || value == "-.INF" ||
      value == ".nan" || value == ".NaN" || value == ".NAN") {
    return true;
  }

  if (value.size() >= 3 &&
      ((value[0] == '-' && value[1] == '-' && value[2] == '-') ||
       (value[0] == '.' && value[1] == '.' && value[2] == '.')) &&
      (value.size() == 3 || value[3] == ' ' || value[3] == '\t')) {
    return true;
  }

  if (looks_like_number(value)) {
    return true;
  }

  const char first{value[0]};

  if (first == ',' || first == '[' || first == ']' || first == '{' ||
      first == '}' || first == '#' || first == '&' || first == '*' ||
      first == '!' || first == '|' || first == '>' || first == '\'' ||
      first == '"' || first == '%' || first == '@' || first == '`') {
    return true;
  }

  if (first == '-' || first == '?' || first == ':') {
    if (value.size() == 1 || value[1] == ' ') {
      return true;
    }
  }

  if (value.front() == ' ' || value.back() == ' ') {
    return true;
  }

  for (std::size_t index{0}; index < value.size(); ++index) {
    const char character{value[index]};
    if (character < ' ') {
      return true;
    }

    if (character == ':' &&
        (index + 1 >= value.size() || value[index + 1] == ' ')) {
      return true;
    }

    if (character == ' ' && index + 1 < value.size() &&
        value[index + 1] == '#') {
      return true;
    }
  }

  return false;
}

inline auto write_double_quoted(OutputStream &stream, const std::string &value)
    -> void {
  stream.put('"');
  for (const char character : value) {
    switch (character) {
      case '"':
        stream.write("\\\"", 2);
        break;
      case '\\':
        stream.write("\\\\", 2);
        break;
      case '\n':
        stream.write("\\n", 2);
        break;
      case '\r':
        stream.write("\\r", 2);
        break;
      case '\t':
        stream.write("\\t", 2);
        break;
      case '\0':
        stream.write("\\0", 2);
        break;
      default:
        if (character >= '\x01' && character < '\x20') {
          const auto byte{static_cast<unsigned char>(character)};
          stream.write("\\x", 2);
          stream.put(HEX_DIGITS[byte >> 4u]);
          stream.put(HEX_DIGITS[byte & 0x0Fu]);
        } else {
          stream.put(character);
        }
        break;
    }
  }
  stream.put('"');
}

inline auto write_string(OutputStream &stream, const std::string &value)
    -> void {
  if (needs_quoting(value)) {
    write_double_quoted(stream, value);
  } else {
    stream.write(value.data(), static_cast<std::streamsize>(value.size()));
  }
}

inline auto write_inline_value(OutputStream &stream, const JSON &value)
    -> void {
  switch (value.type()) {
    case JSON::Type::Null:
      stream.write("null", 4);
      break;
    case JSON::Type::Boolean:
      if (value.to_boolean()) {
        stream.write("true", 4);
      } else {
        stream.write("false", 5);
      }
      break;
    case JSON::Type::Integer: {
      const auto string{std::to_string(value.to_integer())};
      stream.write(string.c_str(), static_cast<std::streamsize>(string.size()));
    } break;
    case JSON::Type::Real: {
      const auto real{value.to_real()};
      if (real == 0.0) {
        stream.write("0.0", 3);
      } else {
        const auto flags{stream.flags()};
        const auto precision{stream.precision()};
        double integer_part;
        if (std::modf(real, &integer_part) == 0.0) {
          stream << std::fixed << std::setprecision(1) << real;
        } else {
          stream << std::noshowpoint << real;
        }
        stream.flags(flags);
        stream.precision(precision);
      }
    } break;
    case JSON::Type::Decimal:
      stream << value.to_decimal().to_scientific_string();
      break;
    case JSON::Type::String:
      write_string(stream, value.to_string());
      break;
    case JSON::Type::Object:
      assert(value.empty());
      stream.write("{}", 2);
      break;
    case JSON::Type::Array:
      assert(value.empty());
      stream.write("[]", 2);
      break;
  }
}

inline auto write_block_mapping(OutputStream &stream, const JSON &value,
                                std::size_t indent, bool skip_first_indent)
    -> void;
inline auto write_block_sequence(OutputStream &stream, const JSON &value,
                                 std::size_t indent, bool skip_first_indent)
    -> void;

inline auto write_node(OutputStream &stream, const JSON &value,
                       const std::size_t indent, const bool skip_first_indent)
    -> void {
  if (value.is_object() && !value.empty()) {
    write_block_mapping(stream, value, indent, skip_first_indent);
  } else if (value.is_array() && !value.empty()) {
    write_block_sequence(stream, value, indent, skip_first_indent);
  } else {
    write_inline_value(stream, value);
    stream.put('\n');
  }
}

inline auto write_block_mapping(OutputStream &stream, const JSON &value,
                                const std::size_t indent,
                                const bool skip_first_indent) -> void {
  assert(value.is_object() && !value.empty());
  bool first{true};
  for (const auto &entry : value.as_object()) {
    if (!first || !skip_first_indent) {
      write_indent(stream, indent);
    }
    first = false;

    write_string(stream, entry.first);
    stream.put(':');

    const bool nested{(entry.second.is_object() || entry.second.is_array()) &&
                      !entry.second.empty()};
    stream.put(nested ? '\n' : ' ');
    write_node(stream, entry.second, indent + 1, false);
  }
}

inline auto write_block_sequence(OutputStream &stream, const JSON &value,
                                 const std::size_t indent,
                                 const bool skip_first_indent) -> void {
  assert(value.is_array() && !value.empty());
  bool first{true};
  for (const auto &item : value.as_array()) {
    if (!first || !skip_first_indent) {
      write_indent(stream, indent);
    }
    first = false;

    stream.write("- ", 2);
    write_node(stream, item, indent + 1, true);
  }
}

template <template <typename T> typename Allocator>
auto stringify_yaml(const JSON &document, OutputStream &stream) -> void {
  write_node(stream, document, 0, false);
}

} // namespace sourcemeta::core::yaml

#endif
