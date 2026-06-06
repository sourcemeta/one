#ifndef SOURCEMETA_CORE_HTTP_H_
#define SOURCEMETA_CORE_HTTP_H_

#ifndef SOURCEMETA_CORE_HTTP_EXPORT
#include <sourcemeta/core/http_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/http_problem.h>
#include <sourcemeta/core/http_status.h>
// NOLINTEND(misc-include-cleaner)

#include <chrono>           // std::chrono::system_clock
#include <cstdint>          // std::uint8_t
#include <initializer_list> // std::initializer_list
#include <optional>         // std::optional
#include <span>             // std::span
#include <string>           // std::string
#include <string_view>      // std::string_view
#include <utility>          // std::pair

/// @defgroup http HTTP
/// @brief An implementation of HTTP-protocol parsing, formatting, and
/// validation primitives per RFC 9110.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// ```

namespace sourcemeta::core {

/// @ingroup http
/// A content coding the server can produce. Mirrors the encoding modules
/// currently present in this project.
enum class HTTPContentEncoding : std::uint8_t {
  Identity,
  GZIP,
};

/// @ingroup http
/// Pick the best media-type candidate against an `Accept` header per RFC
/// 9110 §12.5.1. Highest effective `q` wins. Ties are broken by
/// media-range specificity, then by candidate order. Returns an empty
/// value when no candidate is acceptable. The returned view borrows from
/// the input `candidates` list. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto best{sourcemeta::core::http_match_accept(
///     "text/html, application/json;q=0.9",
///     {"text/html", "application/json"})};
/// assert(best == "text/html");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_match_accept(const std::string_view accept_header,
                       std::initializer_list<std::string_view> candidates)
    -> std::string_view;

/// @ingroup http
/// Pick the best language-tag candidate against an `Accept-Language`
/// header per RFC 9110 §12.5.4 and RFC 4647 §3.4 Lookup. A client tag
/// matches a candidate when the candidate is a prefix of the tag at a
/// subtag boundary (so `en-US` client matches `en` candidate). Returns
/// an empty value when no candidate matches. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto best{sourcemeta::core::http_match_accept_language(
///     "fr-CA;q=0.9, en;q=0.8", {"en", "fr"})};
/// assert(best == "fr");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_match_accept_language(
    const std::string_view accept_language_header,
    std::initializer_list<std::string_view> candidates) -> std::string_view;

/// @ingroup http
/// Resolve a content coding against an `Accept-Encoding` header per RFC
/// 9110 §12.5.3. Identity is acceptable by default unless explicitly
/// excluded. Ties are broken by `server_preference`. Returns no value
/// when no coding is acceptable. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto chosen{sourcemeta::core::http_negotiate_encoding(
///     "gzip, identity;q=0.5", sourcemeta::core::HTTPContentEncoding::GZIP)};
/// assert(chosen.has_value());
/// assert(chosen.value() == sourcemeta::core::HTTPContentEncoding::GZIP);
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_negotiate_encoding(
    const std::string_view accept_encoding_header,
    const HTTPContentEncoding server_preference) noexcept
    -> std::optional<HTTPContentEncoding>;

/// @ingroup http
/// Parse an HTTP-date string per RFC 9110 §5.6.7. Tries the IMF-fixdate,
/// RFC 850, and ANSI C asctime grammars in that order and returns the
/// first successful parse. Returns no value if none match. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_from_date(
///     "Sun, 06 Nov 1994 08:49:37 GMT").has_value());
/// assert(sourcemeta::core::http_from_date(
///     "Sunday, 06-Nov-94 08:49:37 GMT").has_value());
/// assert(sourcemeta::core::http_from_date(
///     "Sun Nov  6 08:49:37 1994").has_value());
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_from_date(const std::string_view value) noexcept
    -> std::optional<std::chrono::system_clock::time_point>;

/// @ingroup http
/// A typed RFC 8288 §3 link-value. The caller owns the backing storage
/// for every field. The caller is responsible for URI escaping `target`
/// and for ensuring parameter values are valid `quoted-string` content.
struct HTTPLink {
  std::string_view target;
  std::string_view rel;
  std::span<const std::pair<std::string_view, std::string_view>> parameters{};
};

/// @ingroup http
/// Append an RFC 8288 §3 link-value to `out`. Existing contents are
/// preserved. Lets callers amortise allocations across requests by
/// reusing a buffer. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
/// #include <string>
///
/// std::string buffer{"prefix:"};
/// sourcemeta::core::http_format_link(
///     {.target = "/schema.json", .rel = "describedby"}, buffer);
/// assert(buffer == "prefix:</schema.json>; rel=\"describedby\"");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_link(const HTTPLink &link, std::string &out) -> void;

/// @ingroup http
/// Convenience overload that wraps the sink form. Returns the formatted
/// link as a new string. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto value{sourcemeta::core::http_format_link(
///     {.target = "https://example.com/schema.json", .rel = "describedby"})};
/// assert(value ==
///   "<https://example.com/schema.json>; rel=\"describedby\"");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_link(const HTTPLink &link) -> std::string;

/// @ingroup http
/// Append an RFC 8288 §3.5 comma-separated multi-link value to `out`.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
/// #include <string>
///
/// const sourcemeta::core::HTTPLink links[]{
///     {.target = "/here", .rel = "self"},
///     {.target = "/next", .rel = "next"}};
/// std::string buffer;
/// sourcemeta::core::http_format_links(links, buffer);
/// assert(buffer ==
///   "</here>; rel=\"self\", </next>; rel=\"next\"");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_links(std::span<const HTTPLink> links, std::string &out)
    -> void;

/// @ingroup http
/// Convenience overload that wraps the sink form. Returns the multi-link
/// value as a new string. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const sourcemeta::core::HTTPLink links[]{
///     {.target = "/here", .rel = "self"},
///     {.target = "/next", .rel = "next"}};
/// const auto value{sourcemeta::core::http_format_links(links)};
/// assert(value ==
///   "</here>; rel=\"self\", </next>; rel=\"next\"");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_links(std::span<const HTTPLink> links) -> std::string;

/// @ingroup http
/// Test whether a comma-separated header value per RFC 9110 §5.6.1 lists
/// any of the given tokens. Each entry's value (the portion before any
/// `;parameters`) is compared verbatim against `tokens`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_field_list_contains_any(
///     "\"abc\", W/\"def\", *", {"*"}));
/// assert(!sourcemeta::core::http_field_list_contains_any(
///     "\"abc\", \"def\"", {"\"xyz\""}));
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_field_list_contains_any(
    const std::string_view header_value,
    std::initializer_list<std::string_view> tokens) noexcept -> bool;

} // namespace sourcemeta::core

#endif
