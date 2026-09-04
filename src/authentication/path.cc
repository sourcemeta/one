#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>

#include <cstddef>     // std::size_t
#include <exception>   // std::exception
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

namespace {

constexpr std::string_view HEX_DIGITS{"0123456789abcdefABCDEF"};

// The table lists the lowercase digits before the uppercase ones, so an index
// past the first sixteen names the same digit six positions along
auto hex_nibble(const std::size_t index) -> std::size_t {
  return index >= 16 ? index - 6 : index;
}

// RFC 3986 Section 6.2.2.2: only an escape that stands for an unreserved
// character is equivalent to that character, so only those are decoded. A
// reserved character keeps its escape, which is what the artifact tree stores
// and what stops an escaped separator or dot segment from taking effect
auto decode_unreserved(const std::string_view segment) -> std::string {
  std::string result;
  result.reserve(segment.size());
  std::size_t cursor{0};
  while (cursor < segment.size()) {
    if (segment[cursor] != '%' || cursor + 2 >= segment.size() ||
        HEX_DIGITS.find(segment[cursor + 1]) == std::string_view::npos ||
        HEX_DIGITS.find(segment[cursor + 2]) == std::string_view::npos) {
      result += segment[cursor];
      cursor += 1;
      continue;
    }

    const auto high{hex_nibble(HEX_DIGITS.find(segment[cursor + 1]))};
    const auto low{hex_nibble(HEX_DIGITS.find(segment[cursor + 2]))};
    const auto value{static_cast<char>((high * 16) + low)};
    const auto unreserved{(value >= 'A' && value <= 'Z') ||
                          (value >= 'a' && value <= 'z') ||
                          (value >= '0' && value <= '9') || value == '-' ||
                          value == '.' || value == '_' || value == '~'};
    if (unreserved) {
      result += value;
    } else {
      result.append(segment, cursor, 3);
    }

    cursor += 3;
  }

  return result;
}

// Reduce a path that is already relative to the instance to the one spelling
// the rest of the system reads. The input is a request target rather than a
// URL reference, so its separators are literal and a leading double separator
// introduces an empty segment rather than an authority
auto canonicalize(const std::string_view input) -> std::string {
  std::vector<std::string> segments;
  std::size_t cursor{0};
  while (cursor <= input.size()) {
    const auto next{input.find('/', cursor)};
    const auto raw{next == std::string_view::npos
                       ? input.substr(cursor)
                       : input.substr(cursor, next - cursor)};
    cursor = next == std::string_view::npos ? input.size() + 1 : next + 1;

    auto segment{decode_unreserved(raw)};

    // An empty segment carries no location and a single dot repeats the
    // current one, while a double dot climbs, which at the root goes nowhere
    if (segment.empty() || segment == ".") {
      continue;
    }

    if (segment == "..") {
      if (!segments.empty()) {
        segments.pop_back();
      }

      continue;
    }

    segments.push_back(std::move(segment));
  }

  std::string result;
  for (const auto &segment : segments) {
    if (!result.empty()) {
      result += '/';
    }

    result += segment;
  }

  sourcemeta::core::to_lowercase(result);
  return result;
}

} // namespace

namespace sourcemeta::one {

auto Authentication::Path::relative(const std::string_view input) -> Path {
  return Path{canonicalize(input)};
}

auto Authentication::Path::parse(const std::string_view input,
                                 const std::string_view instance_url)
    -> std::optional<Authentication::Path> {
  std::string_view target{input};

  // A caller may name a resource by its URL rather than by a request target,
  // and only a scheme tells the two apart, since a request target's
  // separators are always literal
  std::string reduced;
  std::optional<sourcemeta::core::URI> parsed;
  try {
    parsed.emplace(input);
  } catch (const std::exception &) {
    return std::nullopt;
  }

  if (parsed->scheme().has_value()) {
    std::string absolute;
    try {
      parsed->canonicalize();
      const auto path{parsed->path()};
      absolute = path.has_value() ? std::string{path.value()} : std::string{};
      parsed->relative_to(sourcemeta::core::URI{std::string{instance_url}});
    } catch (const std::exception &) {
      return std::nullopt;
    }

    // Still absolute after being made relative means another origin entirely
    if (parsed->is_absolute()) {
      return std::nullopt;
    }

    // Reducing against the instance answers whether the origin matches, and
    // an instance sits at its origin, so the URL continues as the target it
    // names
    reduced = absolute.empty() ? std::string{"/"} : std::move(absolute);
    target = reduced;
  }

  return Path{canonicalize(target)};
}

} // namespace sourcemeta::one
