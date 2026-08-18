#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/uri.h>

#include "authentication_claims.h"
#include "authentication_format.h"
#include "authentication_provider.h"
#include "authentication_session.h"
#include "authentication_table.h"

#include <algorithm>     // std::ranges::all_of
#include <bit>           // std::countr_zero
#include <chrono>        // std::chrono::system_clock, std::chrono::seconds
#include <cstddef>       // std::byte, std::size_t
#include <cstdint>       // std::uint32_t, std::uint64_t, std::uint8_t
#include <cstdlib>       // std::getenv
#include <cstring>       // std::memcpy
#include <filesystem>    // std::filesystem::path
#include <limits>        // std::numeric_limits
#include <map>           // std::map
#include <memory>        // std::unique_ptr, std::make_unique
#include <mutex>         // std::mutex, std::scoped_lock
#include <optional>      // std::optional, std::nullopt
#include <span>          // std::span
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move, std::pair
#include <vector>        // std::vector

namespace {} // namespace

namespace sourcemeta::one {

Authentication::Table::Table(const std::filesystem::path &path)
    : impl_{std::make_unique<Impl>(path)} {}

Authentication::Table::Table(const std::span<const std::byte> bytes)
    : impl_{std::make_unique<Impl>(bytes)} {}

Authentication::Table::~Table() = default;

Authentication::Table::Table(Authentication::Table &&) noexcept = default;

auto Authentication::Table::operator=(Authentication::Table &&) noexcept
    -> Authentication::Table & = default;

// The table is taken rather than borrowed, and the fetcher is installed on it,
// so that what performs the protocol and what the protocol reads cannot come
// apart for as long as either exists
Authentication::Authentication(Authentication::Table &&table,
                               Authentication::Fetcher fetcher)
    : table_{std::move(table)} {
  this->table_.impl_->fetcher_ = std::move(fetcher);
}

auto Authentication::table() const noexcept -> const Authentication::Table & {
  return this->table_;
}

Authentication::~Authentication() = default;

namespace {

// A view is spelled as the names of the policies it comprises, ordered so that
// one combination has one spelling whatever order they were declared in
auto view_name(const std::span<const Authentication::Policy> policies,
               const std::span<const std::size_t> members) -> std::string {
  std::vector<std::string_view> names;
  names.reserve(members.size());
  for (const auto member : members) {
    names.emplace_back(policies[member].name);
  }

  std::ranges::sort(names);
  std::string result;
  for (const auto name : names) {
    if (!result.empty()) {
      result += '+';
    }

    result += name;
  }

  return result;
}

} // namespace

auto Authentication::Table::enumerate(
    const std::span<const Authentication::Policy> policies)
    -> std::vector<Authentication::Table::View> {
  std::vector<Authentication::Table::View> result;
  // Always present, and the only entry when nothing is declared. A registry
  // whose every path is governed still has one, since it is what a caller
  // holding nothing is shown and what a lookup falls back to
  result.push_back({.name = std::string{VIEW_PUBLIC}, .policies = {}});

  // A credential carries one issuer and is checked against it before any rule
  // is read, so only token policies declared against the same issuer can ever
  // be satisfied together. Every other policy stands alone, since a caller
  // presents one key or holds one session
  // A view is spelled from the names of the policies it comprises, so a policy
  // without a name leaves a view naming nowhere, and one sharing a name with
  // another, or taking the name a caller holding nothing is served under,
  // leaves two views under one name. A configuration is refused for all three
  // before any of this is reached, so these say what has been established
  assert(std::ranges::none_of(policies, [](const auto &policy) -> bool {
    return policy.name.empty() || policy.name == VIEW_PUBLIC;
  }));
  assert(std::ranges::all_of(policies, [&policies](const auto &policy) -> bool {
    return std::ranges::count(policies, policy.name,
                              &Authentication::Policy::name) == 1;
  }));

  std::vector<std::string_view> issuers;
  std::vector<std::vector<std::size_t>> groups;
  std::vector<Authentication::Table::View> named;

  for (std::size_t index{0}; index < policies.size(); index++) {
    const auto &policy{policies[index]};
    const auto *token{
        std::get_if<Authentication::Policy::Token>(&policy.credential)};
    if (token == nullptr) {
      named.push_back({.name = std::string{policy.name}, .policies = {index}});
      continue;
    }

    const auto match{std::ranges::find(issuers, token->issuer)};
    if (match == issuers.cend()) {
      issuers.emplace_back(token->issuer);
      groups.push_back({index});
    } else {
      groups[static_cast<std::size_t>(match - issuers.begin())].push_back(
          index);
    }
  }

  for (std::size_t group{0}; group < groups.size(); group++) {
    const auto &members{groups[group]};
    if (members.size() > Authentication::Table::MAXIMUM_COMBINABLE_POLICIES) {
      throw AuthenticationTooManyViewsError(std::string{issuers[group]},
                                            members.size());
    }

    // Every non-empty combination of the group, since a credential satisfying
    // several of them is shown what all of them admit
    const auto total{std::uint64_t{1} << members.size()};
    for (std::uint64_t mask{1}; mask < total; mask++) {
      std::vector<std::size_t> combination;
      for (std::size_t offset{0}; offset < members.size(); offset++) {
        if ((mask & (std::uint64_t{1} << offset)) != 0) {
          combination.push_back(members[offset]);
        }
      }

      named.push_back({.name = view_name(policies, combination),
                       .policies = std::move(combination)});
    }
  }

  std::ranges::sort(named, [](const auto &left, const auto &right) -> bool {
    return left.name < right.name;
  });
  result.insert(result.cend(), std::make_move_iterator(named.begin()),
                std::make_move_iterator(named.end()));
  return result;
}

namespace {

// What redeeming an authorization code yields, of which only the identity token
// decides anything. The access token comes along solely so that a claim missing
// from that token can be asked for at the UserInfo endpoint
struct Grant {
  std::string id_token;
  std::string access_token;
};

// RFC 6749 Section 2.3.1 has every server accept the client secret in an
// authorization header, and asks that carrying it in the request body be
// limited to clients that cannot send one. A body is the part of a request that
// logging and proxies keep, while an authorization header is the part they
// already know to redact, so the header is used wherever the provider takes it
auto exchange(const sourcemeta::one::Authentication::Fetcher &fetcher,
              const std::string_view token_endpoint,
              const std::string_view client_id,
              const sourcemeta::core::SecureString &client_secret,
              const std::string_view redirect_uri, const std::string_view code,
              const std::string_view code_verifier, const bool basic_auth)
    -> std::optional<Grant> {
  if (!fetcher) {
    return std::nullopt;
  }

  sourcemeta::one::Authentication::ProviderRequest request{.url =
                                                               token_endpoint};
  sourcemeta::core::oauth_build_token_request_code(
      code, redirect_uri, code_verifier, {}, request.body);
  if (basic_auth) {
    sourcemeta::core::oauth_client_secret_basic(client_id, client_secret,
                                                request.authorization);
  } else {
    sourcemeta::core::oauth_client_secret_post(client_id, client_secret,
                                               request.body);
  }

  const auto result{fetcher(std::move(request))};
  if (!result.has_value() || result.value().status < 200 ||
      result.value().status >= 300) {
    return std::nullopt;
  }

  const auto document{sourcemeta::core::try_parse_json(result.value().body)};
  if (!document.has_value()) {
    return std::nullopt;
  }

  auto identity{sourcemeta::core::oidc_parse_id_token(document.value())};
  if (!identity.has_value()) {
    return std::nullopt;
  }

  // The access token is kept only so that the UserInfo endpoint can be asked
  // for a claim the identity token did not carry. It is never stored, never
  // logged, and never leaves this exchange
  Grant grant;
  grant.id_token = std::move(identity).value();
  const sourcemeta::core::OAuthTokenResponse response{document.value()};
  if (response.access_token().has_value()) {
    grant.access_token = response.access_token().value();
  }

  return grant;
}

// What a provider answers at its UserInfo endpoint, which under the
// authorization code flow is where the claims a scope requested arrive by
// default (OpenID Connect Core Section 5.4).
//
// OpenID Connect Core Section 5.3.2 requires the subject it returns to match
// the one the identity token asserted, and to be checked before anything it
// says is used. Without that a response about somebody else would be read as
// being about this person, which is the whole of what this is for
auto userinfo(const sourcemeta::one::Authentication::Fetcher &fetcher,
              const std::string_view endpoint,
              const std::string_view access_token,
              const std::string_view subject, std::vector<std::string> &log)
    -> std::optional<sourcemeta::core::JSON> {
  if (!fetcher || access_token.empty()) {
    return std::nullopt;
  }

  std::string authorization;
  if (!sourcemeta::core::oauth_bearer_header(access_token, authorization)) {
    return std::nullopt;
  }

  // An access token is a secret, so what carries it travels in wiping storage
  // even though what built it did not
  sourcemeta::one::Authentication::ProviderRequest request{.url = endpoint};
  request.authorization.append(authorization);

  const auto result{fetcher(std::move(request))};
  if (!result.has_value() || result.value().status < 200 ||
      result.value().status >= 300) {
    return std::nullopt;
  }

  auto document{sourcemeta::core::try_parse_json(result.value().body)};
  if (!document.has_value() || !document.value().is_object()) {
    return std::nullopt;
  }

  const auto *asserted{document.value().try_at("sub")};
  if (asserted == nullptr || !asserted->is_string() ||
      asserted->to_string() != subject) {
    log.emplace_back("The UserInfo endpoint answered about a different subject "
                     "than the identity token did");
    return std::nullopt;
  }

  return document;
}

auto id_token_keys(std::string location,
                   sourcemeta::core::JWKSProvider::Fetcher fetcher)
    -> sourcemeta::core::JWKSProvider {
  return sourcemeta::core::JWKSProvider{std::move(location),
                                        std::move(fetcher),
                                        {.clock_skew = ID_TOKEN_CLOCK_SKEW}};
}

// Expire a cookie under the attributes it was minted with, so the browser

// replaces it rather than keeping a second one of the same name beside it
auto expired_cookie(const std::string_view name, const bool secure)
    -> std::optional<std::string> {
  return sourcemeta::core::http_serialize_cookie(
      {.name = name,
       .value = "",
       .path = COOKIE_PATH,
       .max_age = std::chrono::seconds{0},
       .http_only = true,
       .secure = secure,
       .same_site = sourcemeta::core::HTTPCookieSameSite::Lax});
}

} // namespace

auto Authentication::login(const std::string_view policy_name,
                           const std::string_view instance_url,
                           const std::string_view redirect_uri,
                           const bool silent,
                           const std::string_view return_to) const
    -> Authentication::Outcome {
  Authentication::Outcome result;

  // An unknown or non-interactive policy name reveals nothing
  const auto policy{this->table_.impl_->interactive(policy_name)};
  if (!policy.has_value()) {
    result.result = Authentication::Outcome::Result::Missing;
    return result;
  }

  // Starting a login that cannot be completed only strands the person at the
  // provider, so the secret the exchange will need is required up front
  if (!this->table_.impl_->client_secret(policy_name).has_value()) {
    result.log.emplace_back("No client secret is set for the policy");
    return result;
  }

  const auto endpoints{this->table_.impl_->endpoints(policy_name)};
  if (!endpoints.has_value() || endpoints.value().authorization.empty()) {
    result.log.emplace_back("The provider named no authorization endpoint, or "
                            "could not be reached, for the policy");
    return result;
  }

  const auto secrets{sourcemeta::core::oauth_transaction_mint()};
  const std::string_view state{secrets.state.data(), secrets.state.size()};
  const std::string_view verifier{secrets.code_verifier.data(),
                                  secrets.code_verifier.size()};
  const auto nonce_token{sourcemeta::core::oidc_nonce()};
  const std::string_view nonce{nonce_token.data(), nonce_token.size()};

  auto payload{sourcemeta::core::JSON::make_object()};
  payload.assign_assume_new("policy",
                            sourcemeta::core::JSON{std::string{policy_name}});
  if (silent) {
    payload.assign_assume_new("silent", sourcemeta::core::JSON{true});
  }

  payload.assign_assume_new("state", sourcemeta::core::JSON{state});
  payload.assign_assume_new("nonce", sourcemeta::core::JSON{nonce});
  payload.assign_assume_new("verifier", sourcemeta::core::JSON{verifier});
  // Sealed so that a callback completing this login has to name the same place
  // the provider was told to come back to. The provider checks it too, and this
  // is what lets the check hold before a code is ever redeemed
  payload.assign_assume_new("redirect_uri",
                            sourcemeta::core::JSON{std::string{redirect_uri}});

  // Where the browser goes once this completes. What the request asked for
  // wins, and what the policy governs stands in where it asked for nothing
  if (!return_to.empty()) {
    payload.assign_assume_new("to",
                              sourcemeta::core::JSON{std::string{return_to}});
  } else if (!policy->default_path.empty()) {
    payload.assign_assume_new(
        "to", sourcemeta::core::JSON{std::string{policy->default_path}});
  }

  std::ostringstream payload_text;
  sourcemeta::core::stringify(payload, payload_text);

  const auto expiry{std::chrono::time_point_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now()) +
                    TRANSACTION_LIFETIME};
  const auto sealed{this->table_.impl_->seal(
      policy_name, SealPurpose::Transaction, payload_text.str(), expiry)};
  if (!sealed.has_value()) {
    result.log.emplace_back("No session secret is set for the policy");
    return result;
  }

  // A provider sends only the claims a request asks for, so a policy whose
  // rules name any is asked for them here. The standard way is the claims
  // request parameter, and where a provider does not honour that, the scopes
  // that carry the standard claims are the fallback. A claim no standard scope
  // carries is then arranged at the provider instead, since inventing a scope
  // name risks a request refused outright.
  // The rules outlive every request built from them, since a claim request
  // names its claim by pointing into them rather than copying
  const auto rules{policy->claims.empty()
                       ? std::optional<sourcemeta::core::JSON>{std::nullopt}
                       : sourcemeta::core::try_parse_json(policy->claims)};
  const auto wanted{wanted_claims(policy.value(), rules)};
  std::string scope_request;
  std::string claims_parameter;
  if (endpoints.value().claims_parameter_supported && !wanted.empty()) {
    std::ostringstream text;
    sourcemeta::core::stringify(
        sourcemeta::core::oidc_build_claims_parameter({}, wanted), text);
    claims_parameter = text.str();
  }

  requested_scope(wanted, scope_request);
  report_unadvertised_claims(wanted, endpoints.value(), policy_name,
                             result.log);

  const auto challenge{sourcemeta::core::oauth_pkce_challenge(verifier)};
  sourcemeta::core::OIDCAuthenticationRequest authentication_request{};
  authentication_request.client_id = policy->client_id;
  authentication_request.redirect_uri = redirect_uri;
  authentication_request.scope = scope_request;
  authentication_request.claims = claims_parameter;
  authentication_request.response_type = "code";
  authentication_request.state = state;
  authentication_request.code_challenge = {challenge.data(), challenge.size()};
  authentication_request.code_challenge_method = "S256";
  authentication_request.nonce = nonce;
  if (silent) {
    authentication_request.prompt = "none";
  }

  std::string authorization_url;
  if (!sourcemeta::core::oidc_build_authentication_url(
          endpoints.value().authorization, authentication_request,
          authorization_url)) {
    result.log.emplace_back("The authorization endpoint is not a URL a request "
                            "can be built against, for the policy");
    return result;
  }

  // A redirect without the transaction cookie could never complete at the
  // callback, so it is not worth sending
  auto cookie{sourcemeta::core::http_serialize_cookie(
      {.name = TRANSACTION_COOKIE,
       .value = sealed.value(),
       .path = COOKIE_PATH,
       .max_age = TRANSACTION_LIFETIME,
       .http_only = true,
       .secure = sourcemeta::core::URI{std::string{instance_url}}.is_https(),
       .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
  if (!cookie.has_value()) {
    result.log.emplace_back(
        "The login transaction could not be put in a cookie, for the policy");
    return result;
  }

  // Where the browser was going to be sent back to is carried in the sealed
  // transaction, and whoever asked for the login chose it, so this can be made
  // to outgrow what a browser will keep. A cookie the browser discards would
  // send somebody to their provider and refuse them on the way back, with
  // nothing anywhere to explain it, so it is refused before the redirect
  if (cookie.value().size() > MAXIMUM_COOKIE_LENGTH) {
    result.log.emplace_back(
        "The login transaction is larger than a cookie can hold, for the "
        "policy");
    return result;
  }

  result.cookies.push_back(std::move(cookie).value());
  result.location = std::move(authorization_url);
  result.result = Authentication::Outcome::Result::Redirect;
  return result;
}

auto Authentication::renewal(const Authentication::Path &path,
                             const Credentials &credentials) const
    -> std::optional<std::string_view> {
  if (credentials.cookies.empty()) {
    return std::nullopt;
  }

  std::vector<std::string_view> candidates;
  for (const auto field : credentials.cookies) {
    sourcemeta::core::http_cookie_values(field, RENEWAL_COOKIE, candidates);
  }

  for (const auto candidate : candidates) {
    auto policy{this->table_.impl_->renewal_marker(path.value(), candidate)};
    if (policy.has_value()) {
      return policy;
    }
  }

  return std::nullopt;
}

auto Authentication::callback(const std::string_view policy_name,
                              const std::string_view instance_url,
                              const std::string_view redirect_uri,
                              const Authentication::CallbackRequest &incoming,
                              const Credentials &credentials) const
    -> Authentication::Outcome {
  Authentication::Outcome result;
  result.result = Authentication::Outcome::Result::Invalid;
  const auto secure{
      sourcemeta::core::URI{std::string{instance_url}}.is_https()};

  // The transaction is the only proof that this response belongs to a login
  // this instance started, and the state it carries must match the one the
  // provider echoes back. This runs before either the success or the decline is
  // honoured, so a cross-site callback cannot even trigger an error on a
  // person's behalf, per RFC 6749 Section 4.1.2.1
  const auto transaction{this->table_.impl_->transaction(
      policy_name, incoming.state, redirect_uri, credentials)};
  if (!transaction.has_value()) {
    return result;
  }

  // Nobody asked for a silent attempt, so from here on nothing it does is shown
  // to them: every way this can end without a session puts the browser back
  // where it started instead
  const auto silent{transaction.value().try_at("silent") != nullptr};
  const auto *nonce{transaction.value().try_at("nonce")};
  const auto *verifier{transaction.value().try_at("verifier")};

  // Where a silent attempt leaves the browser when it did not come back with a
  // grant: back where it was denied, carrying neither a session nor the marker
  // that would send it here again. The marker goes whatever the reason, so an
  // attempt that failed is not made again on the next navigation, and every
  // navigation after that
  const auto abandon{[&](const Authentication::Outcome::Result outcome)
                         -> Authentication::Outcome {
    if (!silent) {
      result.result = outcome;
      return result;
    }

    result.result = Authentication::Outcome::Result::Redirect;
    result.location = "";
    const auto *sealed{transaction.value().try_at("to")};
    if (sealed != nullptr && sealed->is_string() &&
        is_local_path(sealed->to_string())) {
      result.location = sealed->to_string();
    }

    for (const auto name : {RENEWAL_COOKIE, TRANSACTION_COOKIE}) {
      auto cookie{expired_cookie(name, secure)};
      if (cookie.has_value()) {
        result.cookies.push_back(std::move(cookie).value());
      }
    }

    result.log.emplace_back("A silent renewal did not end in a session");
    return result;
  }};

  // Which policy a callback belongs to is settled by opening its transaction,
  // so nothing reaching here names one this instance does not serve
  const auto policy{this->table_.impl_->interactive(policy_name)};
  if (!policy.has_value()) {
    return abandon(Authentication::Outcome::Result::Invalid);
  }

  // RFC 9207 Section 2.4 has a client compare the issuer an answer names
  // against the one it addressed the request to, which is what catches an
  // answer relayed from somewhere else. A provider naming none cannot be
  // checked that way, so this runs only when one arrives, and it runs ahead of
  // the outcome so that no answer is acted on before it is placed
  if (incoming.has_issuer && incoming.issuer != policy->issuer) {
    return abandon(Authentication::Outcome::Result::Invalid);
  }

  // Only once the callback is proven to belong to a real login is the
  // provider's outcome honoured: a decline returns an error instead of a code,
  // and a success without a code is malformed. RFC 6749 Section 4.1.2.1 has a
  // decline name itself with an error code, so one that names nothing is not a
  // decision this can report as the provider's. It is no grant either, and a
  // code arriving beside it is left unredeemed
  if (incoming.has_error) {
    return abandon(incoming.error.empty()
                       ? Authentication::Outcome::Result::Invalid
                       : Authentication::Outcome::Result::Declined);
  }

  if (incoming.code.empty()) {
    return abandon(Authentication::Outcome::Result::Invalid);
  }

  const auto client_secret{this->table_.impl_->client_secret(policy_name)};
  if (!client_secret.has_value()) {
    result.log.emplace_back("No client secret is set for the policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  const auto endpoints{this->table_.impl_->endpoints(policy_name)};
  if (!endpoints.has_value() || endpoints.value().token.empty()) {
    result.log.emplace_back("The provider named no token endpoint, or could "
                            "not be reached, for the policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  const auto grant{exchange(
      this->table_.impl_->fetcher(), endpoints.value().token, policy->client_id,
      client_secret.value(), redirect_uri, incoming.code, verifier->to_string(),
      endpoints.value().token_endpoint_basic_auth)};
  if (!grant.has_value()) {
    result.log.emplace_back(
        "The authorization code could not be redeemed for the policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  const auto token{sourcemeta::core::JWT::from(grant.value().id_token)};
  if (!token.has_value()) {
    result.log.emplace_back("The provider returned an identity token that "
                            "could not be read, for the policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  auto provider{id_token_keys(endpoints.value().jwks_uri,
                              this->table_.impl_->key_fetcher())};
  sourcemeta::core::OIDCValidationOptions options;
  options.nonce = nonce->to_string();
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      provider, token.value(), ID_TOKEN_ALGORITHMS, policy->issuer,
      policy->client_id, options)};
  if (!identity.has_value()) {
    result.log.emplace_back("The identity token did not validate for the "
                            "policy");
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  // A policy's rules are answered here rather than at the gate, so that a
  // session only ever exists for somebody the policy admits. Answering it
  // afterwards would leave a valid session denied on every request, and a
  // denial asks the provider again, which is a loop rather than an answer
  std::optional<sourcemeta::core::JSON> combined;
  auto admission{this->table_.impl_->admits_identity(policy_name,
                                                     token.value().payload())};

  // OpenID Connect Core Section 5.4 has a provider answer for the claims a
  // scope requested at its UserInfo endpoint rather than in the token, by
  // default, under the flow this is completing. So a rule naming a claim the
  // token does not carry is asked there before it is refused, and only then,
  // since a token carrying everything needed spares the round trip
  if (admission == Admission::Incomplete &&
      !endpoints.value().userinfo.empty()) {
    const auto extra{userinfo(
        this->table_.impl_->fetcher(), endpoints.value().userinfo,
        grant.value().access_token, identity.value().subject, result.log)};
    if (extra.has_value()) {
      combined = combine_claims(token.value().payload(), extra.value());
      admission =
          this->table_.impl_->admits_identity(policy_name, combined.value());
    }
  }

  if (admission != Admission::Admitted) {
    result.log.emplace_back("The provider authenticated somebody the policy "
                            "does not admit, for the policy");
    // Whatever the decision was actually made against, which is the pair taken
    // together once a second answer arrived. Explaining a refusal against the
    // token alone would miss a claim the UserInfo endpoint supplied
    const auto &asserted{combined.has_value() ? combined.value()
                                              : token.value().payload()};
    this->table_.impl_->report_object_shaped_claims(policy_name, asserted,
                                                    result.log);
    return abandon(Authentication::Outcome::Result::NotAdmitted);
  }

  const auto expiry{std::chrono::time_point_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now()) +
                    SESSION_LIFETIME};

  // The identity token is kept so that logging out can prove whose session it
  // is asking the provider to end, which is what spares the person a
  // confirmation page there. A provider that mints a large one can push the
  // cookie past what a browser will store, and a browser drops such a cookie
  // without saying so, which would look like signing in and then not being
  // signed in. So the whole cookie is measured, and the token is left out when
  // it does not fit, which costs the confirmation page and nothing else
  auto session{this->table_.impl_->session_cookie(
      policy_name, identity.value().subject, grant.value().id_token, expiry,
      secure, result.log)};
  if (session.has_value() && session.value().size() > MAXIMUM_COOKIE_LENGTH) {
    session = this->table_.impl_->session_cookie(
        policy_name, identity.value().subject, "", expiry, secure, result.log);
  }

  // Without the token there is very little left, so exceeding the limit here
  // takes something extraordinary, such as a provider that identifies people by
  // something enormous. Answering plainly beats handing over a cookie the
  // browser discards, which would look like signing in and then not being
  // signed in, with nothing anywhere to explain it
  if (!session.has_value() || session.value().size() > MAXIMUM_COOKIE_LENGTH) {
    if (session.has_value()) {
      result.log.emplace_back("The provider returned more than a session can "
                              "hold, for the policy");
    }

    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  result.cookies.push_back(std::move(session).value());

  // Signing in is what earns a browser a silent renewal later, and the marker
  // outlives the session it accompanies because it is only of use once that
  // session has expired
  auto marker{sourcemeta::core::JSON::make_object()};
  marker.assign_assume_new("policy",
                           sourcemeta::core::JSON{std::string{policy_name}});
  std::ostringstream marker_text;
  sourcemeta::core::stringify(marker, marker_text);
  const auto marker_expiry{std::chrono::time_point_cast<std::chrono::seconds>(
                               std::chrono::system_clock::now()) +
                           RENEWAL_LIFETIME};
  const auto sealed_marker{this->table_.impl_->seal(
      policy_name, SealPurpose::Renewal, marker_text.str(), marker_expiry)};
  if (!sealed_marker.has_value()) {
    return abandon(Authentication::Outcome::Result::Incomplete);
  }

  auto renewal{sourcemeta::core::http_serialize_cookie(
      {.name = RENEWAL_COOKIE,
       .value = sealed_marker.value(),
       .path = COOKIE_PATH,
       .max_age = RENEWAL_LIFETIME,
       .http_only = true,
       .secure = secure,
       .same_site = sourcemeta::core::HTTPCookieSameSite::Lax})};
  if (renewal.has_value()) {
    result.cookies.push_back(std::move(renewal).value());
  }

  // The single-use transaction has served its purpose, so it is expired
  // alongside minting the session
  auto spent{expired_cookie(TRANSACTION_COOKIE, secure)};
  if (spent.has_value()) {
    result.cookies.push_back(std::move(spent).value());
  }

  // The login may have sealed a page to return to. It came through this
  // instance's own signature, yet it is re-checked as a local path before being
  // trusted as a redirect target
  result.location = "";
  const auto *sealed_destination{transaction.value().try_at("to")};
  if (sealed_destination != nullptr && sealed_destination->is_string() &&
      is_local_path(sealed_destination->to_string())) {
    result.location = sealed_destination->to_string();
  }

  result.result = Authentication::Outcome::Result::Redirect;
  return result;
}

auto Authentication::logout(const Credentials &credentials,
                            const std::string_view instance_url,
                            const std::string_view return_to) const
    -> Authentication::Outcome {
  Authentication::Outcome result;
  const auto secure{
      sourcemeta::core::URI{std::string{instance_url}}.is_https()};

  // Composed before anything that could fail, so no path below ends with a
  // session surviving. Somebody who has signed out is asking not to be signed
  // in, so the marker that would have renewed them silently goes with it
  for (const auto name : {SESSION_COOKIE, TRANSACTION_COOKIE, RENEWAL_COOKIE}) {
    auto cookie{expired_cookie(name, secure)};
    if (cookie.has_value()) {
      result.cookies.push_back(std::move(cookie).value());
    }
  }

  result.location = return_to;

  // Ending the session here leaves the provider's own untouched, so signing in
  // again would not ask who you are. Where the session names a policy whose
  // provider offers to end it, the browser finishes the job there, carrying the
  // identity token as the proof of whose session it is. Every step that cannot
  // be completed simply stops, since the local session is already gone
  std::vector<std::string_view> candidates;
  for (const auto field : credentials.cookies) {
    sourcemeta::core::http_cookie_values(field, SESSION_COOKIE, candidates);
  }

  for (const auto sealed : candidates) {
    const auto payload{this->table_.impl_->open_session(sealed)};
    if (!payload.has_value()) {
      continue;
    }

    const auto document{sourcemeta::core::try_parse_json(payload.value())};
    if (!document.has_value() || !document.value().is_object()) {
      continue;
    }

    const auto *policy{document.value().try_at("policy")};
    const auto *token{document.value().try_at("id_token")};
    if (policy == nullptr || !policy->is_string()) {
      continue;
    }

    const auto endpoints{this->table_.impl_->endpoints(policy->to_string())};
    if (!endpoints.has_value() || endpoints.value().end_session.empty()) {
      continue;
    }

    sourcemeta::core::OIDCLogoutRequest logout;
    if (token != nullptr && token->is_string()) {
      logout.id_token_hint = token->to_string();
    }

    // Where the provider sends the browser back to once it is done, which is
    // this instance rather than the local path a caller without a provider
    // session lands on
    logout.post_logout_redirect_uri = instance_url;
    std::string url;
    sourcemeta::core::oidc_build_logout_url(endpoints.value().end_session,
                                            logout, url);
    result.location = std::move(url);
    break;
  }

  return result;
}

auto Authentication::caller(const Credentials &credentials) const
    -> Authentication::Caller {
  Authentication::Caller result;
  result.policies_ =
      this->table_.impl_->classify(credentials.bearer, credentials.cookies);
  result.view_ = this->table_.impl_->view_name(result.policies_);
  result.bearer_ = credentials.bearer;
  return result;
}

auto Authentication::permits(const Authentication::Path &path,
                             const Authentication::Caller &caller) const
    -> bool {
  return this->table_.impl_->permits(path.value(), caller.policies_);
}

auto Authentication::permits(const RouteTarget &target,
                             const Authentication::Caller &caller,
                             const std::string_view required_audience) const
    -> bool {
  const auto governing{this->table_.impl_->governing_mask(target.value())};
  if (!governing.has_value()) {
    return false;
  }

  // A route nobody governs is reached by anybody, so there is nothing for an
  // audience to narrow. Requiring one here would refuse a caller for presenting
  // a credential they did not need, at a route they could have reached by
  // presenting nothing at all
  if (governing.value() == 0) {
    return true;
  }

  if ((governing.value() & caller.policies_) == 0) {
    return false;
  }

  if (required_audience.empty()) {
    return true;
  }

  // Only a token carries an audience, so a caller admitted by anything else is
  // unaffected by a requirement it could never have satisfied or broken. The
  // signature is already established, so this is one more claim read from a
  // token that has been verified rather than a second verification
  const auto token{sourcemeta::core::JWT::from(caller.bearer_)};
  return !token.has_value() || token.value().has_audience(required_audience);
}

auto Authentication::Table::views() const
    -> std::vector<Authentication::RecordedView> {
  std::vector<Authentication::RecordedView> result;
  result.reserve(this->impl_->view_count());
  for (std::size_t index{0}; index < this->impl_->view_count(); index += 1) {
    result.push_back(this->impl_->view_at(index));
  }

  return result;
}

auto Authentication::Table::view(const std::string_view name) const
    -> Authentication::RecordedView {
  for (std::size_t index{0}; index < this->impl_->view_count(); index += 1) {
    const auto candidate{this->impl_->view_at(index)};
    if (candidate.name() == name) {
      return candidate;
    }
  }

  Authentication::RecordedView result;
  result.name_ = VIEW_PUBLIC;
  return result;
}

auto Authentication::Table::visible(
    const Authentication::Path &path,
    const Authentication::RecordedView &view) const -> bool {
  // An instance that could not read its artifact shows nothing at all, which
  // this answers rather than whoever asks. Otherwise what nobody governs would
  // be shown by an instance knowing nothing about who governs what, which is
  // the one way this could disclose more than the gate admits
  return this->impl_->permits(path.value(), view.policies_);
}

auto Authentication::Table::governing(const Authentication::Path &path) const
    -> std::optional<std::vector<std::string_view>> {
  const auto governing{this->impl_->governing_mask(path.value())};
  if (!governing.has_value()) {
    return std::nullopt;
  }

  auto mask{governing.value()};
  std::vector<std::string_view> result;
  while (mask != 0) {
    // A policy standing on its own is a view, and the table names every one, so
    // what a policy is called is read from there rather than stored twice
    const auto policy{Authentication::PolicySet{1} << std::countr_zero(mask)};
    result.push_back(this->impl_->view_name(policy));
    mask &= mask - 1;
  }

  return result;
}

auto Authentication::Table::reference_permitted(
    const Authentication::Path &referrer,
    const Authentication::Path &referent) const -> bool {
  return this->impl_->reference_permitted(referrer.value(), referent.value());
}

} // namespace sourcemeta::one
