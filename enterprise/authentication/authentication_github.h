#ifndef SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_GITHUB_H_
#define SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_GITHUB_H_

#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/text.h>

#include "authentication_claims.h"
#include "authentication_format.h"

#include <algorithm>   // std::ranges::find
#include <concepts>    // std::invocable
#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::pair
#include <vector>      // std::vector

// What this knows about GitHub. It is a product rather than a standard, so
// every departure from what a standard would have is settled here, and nothing
// about who governs what or about the session that follows belongs in this file
namespace sourcemeta::one {

// Where the public deployment is served, and where it answers its API. Every
// other deployment answers below its own origin instead, which is the whole of
// what parts them as far as this is concerned
inline constexpr std::string_view GITHUB_HOST{"https://github.com"};
inline constexpr std::string_view GITHUB_PUBLIC_API{"https://api.github.com"};
inline constexpr std::string_view GITHUB_PRIVATE_API{"/api/v3"};

// The API refuses a request naming no user agent outright, and answers in
// whichever representation a request pins, so both travel on every call this
// makes
inline constexpr std::string_view GITHUB_USER_AGENT{"sourcemeta-one"};
inline constexpr std::string_view GITHUB_API_VERSION{"2022-11-28"};

// How much of a listing this reads before it stops. A listing is read inside a
// request handler, so one that never ends would leave a login making outbound
// calls without end. This sits far above what an account can plausibly belong
// to and far below where reading becomes a burden
inline constexpr std::size_t GITHUB_MAXIMUM_PAGES{10};
inline constexpr std::size_t GITHUB_PAGE_SIZE{100};

// What a GitHub policy declares, lifted out of the bytes it is stored as. The
// views point into the artifact and remain valid for as long as it is mapped
struct GitHubPolicy {
  std::string_view host{};
  std::string_view client_id{};
  // The first registry path the policy governs
  std::string_view default_path{};
  std::vector<std::string_view> users{};
  std::vector<std::string_view> organizations{};
  std::vector<std::string_view> teams{};
  std::vector<std::string_view> email_domains{};
};

// Who the API says a token was issued for. The account identifier is the
// subject, since an account may be renamed and its handle taken by somebody
// else, while the identifier is the account
struct GitHubIdentity {
  std::string subject{};
  std::string login{};
};

inline auto github_policy(const GitHubPolicyMetadata &decoded) -> GitHubPolicy {
  GitHubPolicy result;
  result.host = decoded.host;
  result.client_id = decoded.client_id;
  result.default_path = decoded.default_path;
  // A rule this cannot read admits nobody, so a run that cannot be read to its
  // end leaves the rule it belongs to empty rather than partly answered
  if (!each_counted_string(decoded.users, [&result](const auto handle) -> void {
        result.users.push_back(handle);
      })) {
    result.users.clear();
  }

  if (!each_counted_string(decoded.organizations,
                           [&result](const auto handle) -> void {
                             result.organizations.push_back(handle);
                           })) {
    result.organizations.clear();
  }

  if (!each_counted_string(decoded.teams, [&result](const auto handle) -> void {
        result.teams.push_back(handle);
      })) {
    result.teams.clear();
  }

  if (!each_counted_string(decoded.email_domains,
                           [&result](const auto domain) -> void {
                             result.email_domains.push_back(domain);
                           })) {
    result.email_domains.clear();
  }

  return result;
}

inline auto github_authorization_endpoint(const std::string_view host)
    -> std::string {
  std::string result{host};
  result += "/login/oauth/authorize";
  return result;
}

inline auto github_token_endpoint(const std::string_view host) -> std::string {
  std::string result{host};
  result += "/login/oauth/access_token";
  return result;
}

// The public deployment answers its API under a host of its own, while every
// other answers below the origin it is served at
inline auto github_api_endpoint(const std::string_view host,
                                const std::string_view path) -> std::string {
  std::string result;
  if (host == GITHUB_HOST) {
    result = GITHUB_PUBLIC_API;
  } else {
    result = host;
    result += GITHUB_PRIVATE_API;
  }

  result += path;
  return result;
}

// The least a login can ask for and still answer the rules a policy names. A
// policy naming only accounts needs nothing beyond signing in, since the
// account is what a token is issued for
inline auto github_scope(const GitHubPolicy &policy) -> std::string {
  std::string result;
  if (!policy.organizations.empty() || !policy.teams.empty()) {
    result = "read:org";
  }

  if (!policy.email_domains.empty()) {
    if (!result.empty()) {
      result += " ";
    }

    result += "user:email";
  }

  return result;
}

// What every call to the API carries beyond the credential
inline constexpr std::pair<std::string_view, std::string_view>
    GITHUB_API_HEADERS[]{{"user-agent", GITHUB_USER_AGENT},
                         {"accept", "application/vnd.github+json"},
                         {"x-github-api-version", GITHUB_API_VERSION}};

// What redeeming an authorization code carries. The token endpoint answers in a
// form encoding unless a request pins one, so the representation is asked for
// here rather than assumed to be the one RFC 6749 Section 5.1 mandates
inline constexpr std::pair<std::string_view, std::string_view>
    GITHUB_TOKEN_HEADERS[]{{"user-agent", GITHUB_USER_AGENT},
                           {"accept", "application/json"}};

// Redeem an authorization code for an access token.
//
// The token endpoint answers a failure with a 200 and a body naming the error,
// where RFC 6749 Section 5.2 has a failure carry a 400. So the body decides
// this rather than the status, and reading it the other way round would take a
// refused code for a grant and fail further along for a reason nobody could
// place
inline auto github_exchange(const Authentication::Fetcher &fetcher,
                            const std::string_view token_endpoint,
                            const std::string_view client_id,
                            const sourcemeta::core::SecureString &client_secret,
                            const std::string_view redirect_uri,
                            const std::string_view code,
                            const std::string_view code_verifier,
                            std::vector<std::string> &log)
    -> std::optional<sourcemeta::core::SecureString> {
  if (!fetcher) {
    return std::nullopt;
  }

  Authentication::ProviderRequest request{.url = token_endpoint};
  sourcemeta::core::oauth_build_token_request_code(
      code, redirect_uri, code_verifier, {}, request.body);
  // The body rather than an authorization header, which is what the deployment
  // documents and what every other client of it sends. There is nothing to
  // discover here that would say whether the header is taken, so the form known
  // to work is the one that is used
  sourcemeta::core::oauth_client_secret_post(client_id, client_secret,
                                             request.body);
  request.headers = GITHUB_TOKEN_HEADERS;

  const auto result{fetcher(std::move(request))};
  if (!result.has_value()) {
    return std::nullopt;
  }

  const auto document{sourcemeta::core::try_parse_json(result.value().body)};
  if (!document.has_value() || !document.value().is_object()) {
    return std::nullopt;
  }

  const auto *error{document.value().try_at("error")};
  if (error != nullptr) {
    std::string message{"The token endpoint refused the authorization code, "
                        "naming "};
    message += error->is_string() ? error->to_string() : "no reason";
    log.push_back(std::move(message));
    return std::nullopt;
  }

  const sourcemeta::core::OAuthTokenResponse response{document.value()};
  if (!response.access_token().has_value()) {
    return std::nullopt;
  }

  sourcemeta::core::SecureString token;
  token.append(response.access_token().value());
  return token;
}

// One call to the API, which is the only way anything is learned about who
// signed in. A redirect is not followed, so a membership the caller may not see
// reads as the absence it is rather than as somewhere else to look
inline auto github_get(const Authentication::Fetcher &fetcher,
                       const std::string_view url,
                       const sourcemeta::core::SecureString &access_token)
    -> std::optional<sourcemeta::core::JSON> {
  if (!fetcher || access_token.empty()) {
    return std::nullopt;
  }

  std::string authorization;
  if (!sourcemeta::core::oauth_bearer_header(access_token, authorization)) {
    return std::nullopt;
  }

  Authentication::ProviderRequest request{.url = url};
  request.authorization.append(authorization);
  request.headers = GITHUB_API_HEADERS;

  const auto result{fetcher(std::move(request))};
  if (!result.has_value() || result.value().status < 200 ||
      result.value().status >= 300) {
    return std::nullopt;
  }

  return sourcemeta::core::try_parse_json(result.value().body);
}

inline auto github_identity(const Authentication::Fetcher &fetcher,
                            const std::string_view host,
                            const sourcemeta::core::SecureString &access_token)
    -> std::optional<GitHubIdentity> {
  const auto document{
      github_get(fetcher, github_api_endpoint(host, "/user"), access_token)};
  if (!document.has_value() || !document.value().is_object()) {
    return std::nullopt;
  }

  const auto *identifier{document.value().try_at("id")};
  const auto *login{document.value().try_at("login")};
  if (identifier == nullptr || !identifier->is_integer() || login == nullptr ||
      !login->is_string() || login->to_string().empty()) {
    return std::nullopt;
  }

  GitHubIdentity result;
  result.subject = std::to_string(identifier->to_integer());
  result.login = login->to_string();
  return result;
}

// How an entry of a listing is spelled for comparison against what a policy
// names. An organisation is named by its handle, and a team by the handle of
// the organisation holding it alongside its own
inline auto github_organization_handle(const sourcemeta::core::JSON &entry)
    -> std::string {
  const auto *login{entry.try_at("login")};
  if (login == nullptr || !login->is_string()) {
    return {};
  }

  std::string result{login->to_string()};
  sourcemeta::core::to_lowercase(result);
  return result;
}

inline auto github_team_handle(const sourcemeta::core::JSON &entry)
    -> std::string {
  const auto *organization{entry.try_at("organization")};
  const auto *slug{entry.try_at("slug")};
  if (organization == nullptr || !organization->is_object() ||
      slug == nullptr || !slug->is_string()) {
    return {};
  }

  auto result{github_organization_handle(*organization)};
  if (result.empty()) {
    return {};
  }

  result += "/";
  result += slug->to_string();
  sourcemeta::core::to_lowercase(result);
  return result;
}

// Whether a listing carries an entry a policy names.
//
// The listing is walked a page at a time until one comes back shorter than the
// page asked for, which is the last one. Reading the `Link` header instead
// would follow wherever the answer pointed, for as long as it kept pointing
// somewhere, so the number of pages is bounded here and exceeding the bound is
// a refusal rather than a further call
template <typename Speller>
  requires std::invocable<Speller, const sourcemeta::core::JSON &>
[[nodiscard]] auto
github_admits_listing(const Authentication::Fetcher &fetcher,
                      const std::string_view host, const std::string_view path,
                      const sourcemeta::core::SecureString &access_token,
                      const std::span<const std::string_view> admitted,
                      Speller speller, std::vector<std::string> &log)
    -> std::optional<bool> {
  for (std::size_t page{1}; page <= GITHUB_MAXIMUM_PAGES; page += 1) {
    std::string url{github_api_endpoint(host, path)};
    url += "?per_page=";
    url += std::to_string(GITHUB_PAGE_SIZE);
    url += "&page=";
    url += std::to_string(page);

    const auto document{github_get(fetcher, url, access_token)};
    if (!document.has_value() || !document.value().is_array()) {
      return std::nullopt;
    }

    for (const auto &entry : document.value().as_array()) {
      if (!entry.is_object()) {
        continue;
      }

      const auto handle{speller(entry)};
      if (!handle.empty() &&
          std::ranges::find(admitted, handle) != admitted.end()) {
        return true;
      }
    }

    if (document.value().size() < GITHUB_PAGE_SIZE) {
      return false;
    }
  }

  log.emplace_back("A listing of what an account belongs to did not end within "
                   "the pages this reads, for the policy");
  return std::nullopt;
}

// The address a policy answers a domain rule against, shaped as the pair
// OpenID Connect Core Section 5.1 defines for one, so that the rule itself is
// read exactly where every other one is.
//
// The address on the account is the public one and is frequently absent, so the
// addresses the account holder keeps are asked for instead, and only one that
// is both primary and verified stands for the person
inline auto github_email(const Authentication::Fetcher &fetcher,
                         const std::string_view host,
                         const sourcemeta::core::SecureString &access_token)
    -> std::optional<sourcemeta::core::JSON> {
  const auto document{github_get(
      fetcher, github_api_endpoint(host, "/user/emails"), access_token)};
  if (!document.has_value() || !document.value().is_array()) {
    return std::nullopt;
  }

  for (const auto &entry : document.value().as_array()) {
    if (!entry.is_object()) {
      continue;
    }

    const auto *address{entry.try_at("email")};
    const auto *primary{entry.try_at("primary")};
    const auto *verified{entry.try_at("verified")};
    if (address == nullptr || !address->is_string() || primary == nullptr ||
        !primary->is_boolean() || !primary->to_boolean()) {
      continue;
    }

    auto result{sourcemeta::core::JSON::make_object()};
    result.assign_assume_new("email", sourcemeta::core::JSON{*address});
    result.assign_assume_new("email_verified",
                             sourcemeta::core::JSON{verified != nullptr &&
                                                    verified->is_boolean() &&
                                                    verified->to_boolean()});
    return result;
  }

  return std::nullopt;
}

// Whether a policy admits the person an access token was issued for.
//
// The values within one rule are alternatives and the rules themselves are
// cumulative, which is the rule an interactive policy's claims already follow.
// A rule is answered with the fewest calls that can settle it, in the order
// that puts the cheapest first, and the first refusal ends the questioning
inline auto github_admits(const Authentication::Fetcher &fetcher,
                          const GitHubPolicy &policy,
                          const GitHubIdentity &identity,
                          const sourcemeta::core::SecureString &access_token,
                          std::vector<std::string> &log) -> Admission {
  if (!policy.users.empty()) {
    std::string login{identity.login};
    sourcemeta::core::to_lowercase(login);
    if (std::ranges::find(policy.users, login) == policy.users.cend()) {
      return Admission::Refused;
    }
  }

  if (!policy.organizations.empty()) {
    const auto admitted{github_admits_listing(
        fetcher, policy.host, "/user/orgs", access_token, policy.organizations,
        github_organization_handle, log)};
    // An organisation can be made to refuse an application its own data, under
    // which a member reads as a stranger and nothing in the exchange says why.
    // That is a rule which can only ever deny, so it is reported where an
    // operator looks rather than only refused
    if (!admitted.has_value()) {
      log.emplace_back("The organisations an account belongs to could not be "
                       "read, which is also how an organisation refusing this "
                       "application its own data reads, for the policy");
      return Admission::Refused;
    }

    if (!admitted.value()) {
      return Admission::Refused;
    }
  }

  if (!policy.teams.empty()) {
    const auto admitted{
        github_admits_listing(fetcher, policy.host, "/user/teams", access_token,
                              policy.teams, github_team_handle, log)};
    if (!admitted.has_value()) {
      log.emplace_back("The teams an account belongs to could not be read, "
                       "which is also how an organisation refusing this "
                       "application its own data reads, for the policy");
      return Admission::Refused;
    }

    if (!admitted.value()) {
      return Admission::Refused;
    }
  }

  if (!policy.email_domains.empty()) {
    const auto address{github_email(fetcher, policy.host, access_token)};
    if (!address.has_value()) {
      log.emplace_back("The account holds no primary address, or none could be "
                       "read, for the policy");
      return Admission::Refused;
    }

    if (admits_email_domain(address.value(), policy.email_domains) !=
        Admission::Admitted) {
      return Admission::Refused;
    }
  }

  return Admission::Admitted;
}

} // namespace sourcemeta::one

#endif
