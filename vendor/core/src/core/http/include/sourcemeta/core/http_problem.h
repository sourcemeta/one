#ifndef SOURCEMETA_CORE_HTTP_PROBLEM_H_
#define SOURCEMETA_CORE_HTTP_PROBLEM_H_

#ifndef SOURCEMETA_CORE_HTTP_EXPORT
#include <sourcemeta/core/http_export.h>
#endif

#include <sourcemeta/core/http_status.h>
#include <sourcemeta/core/json.h>

namespace sourcemeta::core {

/// @ingroup http
/// Fields of an RFC 9457 §3.1 Problem Details object. When `type` is
/// empty the builder substitutes `"about:blank"` (the RFC 9457 §4.2
/// default). When `title` is empty the builder substitutes the status
/// reason-phrase. Empty `detail` and `instance` are omitted from the
/// output. The `status` field is mandatory.
struct HTTPProblemDetails {
  HTTPStatus status;
  JSON::StringView type{"about:blank"};
  JSON::StringView title{};
  JSON::StringView detail{};
  JSON::StringView instance{};
};

/// @ingroup http
/// Build an RFC 9457 §3.1 Problem Details JSON object. The caller writes
/// the `Content-Type: application/problem+json` header and the body to
/// the wire. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto body{sourcemeta::core::http_make_problem_details({
///     .status = sourcemeta::core::HTTP_STATUS_NOT_FOUND,
///     .type   = "https://example.com/probs/not-found",
///     .detail = "The requested resource does not exist."})};
///
/// assert(body.at("status").to_integer() == 404);
/// assert(body.at("title").to_string() == "Not Found");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_make_problem_details(const HTTPProblemDetails &problem)
    -> sourcemeta::core::JSON;

} // namespace sourcemeta::core

#endif
