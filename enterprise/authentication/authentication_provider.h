#ifndef SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_PROVIDER_H_
#define SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_PROVIDER_H_

#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>

#include "authentication_claims.h"
#include "authentication_format.h"

#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

// What this asks an identity provider for, and what it makes of the answer.
// The rules themselves are next door, so this is only about the conversation
namespace sourcemeta::one {

// Where a provider says its endpoints are. The values are copies, so they stay
// usable across a refresh of what the provider last said
struct ProviderEndpoints {
  std::string authorization{};
  std::string token{};
  std::string jwks_uri{};
  // Absent from a provider that does not offer to end its own session
  std::string end_session{};
  // Where a provider answers for the claims a scope requested, which under the
  // authorization code flow is where they arrive by default rather than in the
  // token itself
  std::string userinfo{};
  // Whether the provider takes the client secret in an authorization header
  // rather than in the request body
  bool token_endpoint_basic_auth{true};
  // Whether the provider honours the claims request parameter, which is the
  // standard way to ask for a claim no standard scope carries
  bool claims_parameter_supported{false};
  // The claims the provider says it may be able to supply. OpenID Connect
  // Discovery Section 3 calls this list non-exhaustive, so a claim missing from
  // it is worth reporting and never worth refusing over, and a provider
  // publishing none says nothing at all
  std::vector<std::string> claims_supported{};
};

// What an interactive policy declares about its provider client. The views
// point into the artifact and remain valid for as long as it is mapped
struct InteractivePolicy {
  std::string_view issuer{};
  std::string_view client_id{};
  // The first registry path the policy governs
  std::string_view default_path{};
  // The claims a person must carry to be admitted, serialised as the member map
  // of an OpenID Connect claims request parameter. Empty where the policy names
  // no rule
  std::string_view claims{};
  // The email domains that admit a person. A login asks its provider for an
  // address whenever this is non-empty, since a rule it cannot read admits
  // nobody
  std::vector<std::string_view> email_domains{};
};

// What a login and its callback need of a policy, lifted out of the bytes it
// is stored as. The domains are copied out, since a caller matching an address
// against them should not have to know how they are laid out
inline auto interactive_policy(const OIDCPolicyMetadata &decoded)
    -> InteractivePolicy {
  InteractivePolicy result;
  result.issuer = decoded.issuer;
  result.client_id = decoded.client_id;
  result.default_path = decoded.default_path;
  result.claims = decoded.claims;
  if (!each_counted_string(decoded.email_domains,
                           [&result](const auto domain) -> void {
                             result.email_domains.push_back(domain);
                           })) {
    result.email_domains.clear();
  }

  return result;
}

// The claims a policy's rules speak about, each asked for as essential, so that
// a provider is told what is actually needed rather than being left to guess
// from a scope. A domain rule reads an address, so it asks for the pair OpenID
// Connect Core Section 5.1 defines for one
inline auto wanted_claims(const InteractivePolicy &policy,
                          const std::optional<sourcemeta::core::JSON> &rules)
    -> std::vector<sourcemeta::core::OIDCClaimRequest> {
  std::vector<sourcemeta::core::OIDCClaimRequest> result;
  if (!policy.email_domains.empty()) {
    result.push_back({.name = "email", .essential = true});
    result.push_back({.name = "email_verified", .essential = true});
  }

  if (!rules.has_value() || !rules.value().is_object()) {
    return result;
  }

  for (const auto &rule : rules.value().as_object()) {
    result.push_back({.name = rule.first, .essential = true});
  }

  return result;
}

// The scope a login asks for. Every request carries `openid`, and a claim one
// of the standard scopes carries adds that scope, which is the only mapping a
// specification defines. A claim outside them adds nothing, since a scope this
// invented could be refused outright by the provider
inline auto
requested_scope(const std::vector<sourcemeta::core::OIDCClaimRequest> &wanted,
                std::string &sink) -> void {
  sink += "openid";
  std::vector<std::string_view> scopes;
  for (const auto &claim : wanted) {
    const auto scope{sourcemeta::core::oidc_claim_to_scope(claim.name)};
    if (scope.has_value() && scope.value() != "openid" &&
        std::ranges::find(scopes, scope.value()) == scopes.cend()) {
      scopes.push_back(scope.value());
    }
  }

  std::ranges::sort(scopes);
  for (const auto scope : scopes) {
    sink += " ";
    sink += scope;
  }
}

// A rule naming a claim the provider never sends is one that can only ever
// deny, and nothing in the exchange would say so: the login succeeds, the token
// arrives, and admission fails for a reason nobody can see. So the provider's
// own account of what it may supply is compared against what the rules ask for,
// and a gap is named where an operator will find it.
//
// This reports and never refuses. OpenID Connect Discovery Section 3 says the
// list "might not be an exhaustive list", so a claim missing from it is a hint
// rather than a verdict, and a provider publishing no list at all is saying
// nothing rather than saying no
inline auto report_unadvertised_claims(
    const std::vector<sourcemeta::core::OIDCClaimRequest> &wanted,
    const ProviderEndpoints &endpoints, const std::string_view policy_name,
    std::vector<std::string> &log) -> void {
  if (endpoints.claims_supported.empty()) {
    return;
  }

  // Anybody at all may start a login, so saying this on every attempt would
  // leave a stranger able to bury everything else in the log. What it says
  // concerns a policy and its provider rather than the attempt that surfaced
  // it, so saying it once says all of it
  static std::mutex mutex;
  static std::set<std::string, std::less<>> reported;

  for (const auto &claim : wanted) {
    if (std::ranges::find(endpoints.claims_supported, claim.name) !=
        endpoints.claims_supported.cend()) {
      continue;
    }

    std::string subject{claim.name};
    subject += " of the policy ";
    subject += policy_name;
    const std::scoped_lock guard{mutex};
    if (!reported.insert(subject).second) {
      continue;
    }

    std::string message{"The provider does not advertise a claim a rule "
                        "requires, so the rule may never match. The claim is "};
    message += subject;
    log.push_back(std::move(message));
  }
}

} // namespace sourcemeta::one

#endif
