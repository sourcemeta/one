#ifndef SOURCEMETA_ONE_ENTERPRISE_UNIT_AUTHENTICATION_HELPERS_H_
#define SOURCEMETA_ONE_ENTERPRISE_UNIT_AUTHENTICATION_HELPERS_H_

#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::sys_seconds, std::chrono::seconds
#include <cstddef>     // std::byte, std::size_t
#include <cstdint>     // std::uint32_t
#include <cstdlib>     // setenv
#include <cstring>     // std::memcpy
#include <filesystem>  // std::filesystem::path
#include <fstream>     // std::ofstream, std::fstream, std::ifstream
#include <iterator>    // std::istreambuf_iterator
#include <map>         // std::map
#include <memory>      // std::shared_ptr, std::make_shared
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

// Every gate question is asked about a canonical location, so the tests name
// one the same way a request would
inline auto AT(const std::string_view input)
    -> sourcemeta::one::Authentication::Path {
  return sourcemeta::one::Authentication::Path::parse(input,
                                                      "http://localhost:8000")
      .value();
}

// These tests name locations directly rather than modelling what an instance
// serves, so every path a policy is scoped to is one to gate
inline auto ANYWHERE(const std::string_view) -> bool { return true; }

// One cookie field, which is what a browser conforming to RFC 6265 sends
inline auto FIELDS(const std::string_view value)
    -> std::array<std::string_view, 1> {
  return {{value}};
}

// The artifact is compiled and then persisted, which the tests below do
// together because they read it back through a file
inline auto
SAVE(const std::span<const sourcemeta::one::Authentication::Policy> policies,
     const std::filesystem::path &configuration,
     const std::filesystem::path &destination,
     const sourcemeta::one::Authentication::PathGuard &gateable) -> void {
  sourcemeta::one::Authentication::Table::write(
      sourcemeta::one::Authentication::Table::compile(policies, configuration,
                                                      gateable),
      destination);
}

// How many policies naming one issuer a table will combine, which the artifact
// decides rather than these cases
inline constexpr std::size_t COMBINABLE_CEILING{16};

// Where a configuration would have been read from. Nothing opens it: compiling
// only names it when refusing something, so a case that is not about a refusal
// never sees it
inline constexpr std::string_view CONFIGURATION_PATH{"/test/one.json"};

// A table compiled in this process rather than written out and mapped back,
// which is what every case that is not about the file itself wants
inline auto
TABLE(const std::span<const sourcemeta::one::Authentication::Policy> policies)
    -> sourcemeta::one::Authentication::Table {
  return sourcemeta::one::Authentication::Table{
      sourcemeta::one::Authentication::Table::compile(
          policies, CONFIGURATION_PATH, ANYWHERE)};
}

inline auto TEST_PATH(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{AUTHENTICATION_TEST_DIRECTORY} / name;
}

// A locally signed RS256 access token (typ "at+jwt") with issuer "acme",
// audience "client", and an expiry far in the future, alongside the key set
// that verifies it and an unrelated key set that does not. Reused from the core
// JOSE test vectors so no signing happens here
inline constexpr std::string_view SIGNED_TOKEN{
    "eyJhbGciOiJSUzI1NiIsInR5cCI6ImF0K2p3dCJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "U3ZBo7MvSW0U099gJ_"
    "vIA5T8HJ2XnKSzYmqkx7SDxgxQfmxQyu3QZIeKT68AAH7wQjWRvNWQ7f3Es57UUNUQAMs-"
    "z5TWlVBKtYZf5ZcbYqc4KrQ-ApwpjoFGJxurnd1R_"
    "tz02WssnvrZNKnxNPuGoYIkJKNCl59yLFJwRLf3nK_Jcxs-"
    "1m2MvKsm647PuXqhYOKlZkHOvkIV0RV8cLJ56_gDVjj7TlKQgwbTdW_"
    "71QLwLWRFGftU2EAWuqayTSpPeUA6kB4sfn7JNsweqDs7uev30m6y8BE9uzwzHuuovaN1cZz0o"
    "TAGXcx64sfbPs6HEMp5_FoU0SccxArAbnHSjA"};
inline constexpr std::string_view SIGNED_KEYS{
    R"JSON({
      "keys": [
        {
          "kty": "RSA",
          "n": "oHTpl-jfNfBuXmBp58sW8s_77UP6j2jA0mjjKjhDkxhp7Agk-xLNGgfPCS_bjdZ6YU6FGeab8uVjkSgo9_0OCJUaF4vzEGwXmNuGawANxnZtiYjWvbJlq-2mn_L7rsqGQcSkMmyM0g4aX7dF8wB6DVrXShJ78fcrNtpeoU72YGEdjehA8qVclDFwBdpCGynxxnWJePk72lQb6gkVMqKMc3jBF8GkWf8oP_sjss-fpOjSUMR1c8_0JlTYWO46KWOZa0EO2t8H1V3imMyzbhoxRd_qZHmo46gJkG-ZdebjX0vGQllaCwu0z4kLcXIfAZhqPEkdssDGhC_txwJuhaPDFQ",
          "e": "AQAB"
        }
      ]
    })JSON"};
// A claim rule is matched against whatever a provider put in a token, so these
// tests mint their own rather than reuse a fixed vector. The curve keeps the
// key small enough to read
inline constexpr std::string_view CLAIMS_PRIVATE_KEY{
    R"JSON({
      "kty": "EC",
      "crv": "P-256",
      "x": "sQbBlwx8VKtzct6SJjoYb4hmXMRIhBdC_rQtfrA7GdU",
      "y": "gE3c3l2Uux8jUbm0DVEnXwPQlsD7ln4CLWt6FGxNbhk",
      "d": "n0c-5YK2MYjEvSiF8OOaQOiheqm14U4iN6PdZAGLXOE"
    })JSON"};
inline constexpr std::string_view CLAIMS_KEYS{
    R"JSON({
      "keys": [
        {
          "kty": "EC",
          "crv": "P-256",
          "x": "sQbBlwx8VKtzct6SJjoYb4hmXMRIhBdC_rQtfrA7GdU",
          "y": "gE3c3l2Uux8jUbm0DVEnXwPQlsD7ln4CLWt6FGxNbhk"
        }
      ]
    })JSON"};

// Claim rules reach a policy already compiled into individual claim requests
// (OpenID Connect Core 1.0 Section 5.5.1), with their names and their values
// sorted, since the serialised bytes are what decide whether two policies
// count as one audience
inline constexpr std::string_view CLAIMS_ONE_GROUP{
    R"JSON({
      "groups": {
        "essential": true,
        "values": [ "platform" ]
      }
    })JSON"};
inline constexpr std::string_view CLAIMS_TWO_GROUPS{
    R"JSON({
      "groups": {
        "essential": true,
        "values": [ "oncall", "platform" ]
      }
    })JSON"};
inline constexpr std::string_view CLAIMS_SCOPE{
    R"JSON({
      "scope": {
        "essential": true,
        "values": [ "registry:read" ]
      }
    })JSON"};
// A token from the issuer and audience the claim tests configure, carrying
// whatever additional claims a case is about
inline auto TOKEN_WITH(const std::string_view claims) -> std::string {
  auto payload{sourcemeta::core::parse_json(claims)};
  payload.assign("iss", sourcemeta::core::JSON{"acme"});
  payload.assign("aud", sourcemeta::core::JSON{"client"});
  payload.assign("exp", sourcemeta::core::JSON{2000000000});
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(CLAIMS_PRIVATE_KEY))};
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{"ES256"});
  return sourcemeta::core::jwt_sign(header, payload, key.value()).value();
}

// What a Set-Cookie carries, and what a URL said, which these read back out of
// what an operation produced
// The value a `Set-Cookie` carries, read with the same tokenizer a browser
// would use rather than by hand. What a header sets comes first, so whatever
// attributes follow are passed over
inline auto COOKIE_VALUE(const std::string_view cookie) -> std::string {
  std::vector<std::pair<std::string_view, std::string_view>> parsed;
  sourcemeta::core::http_parse_cookies(cookie, parsed);
  return std::string{parsed.front().second};
}

// What a query parameter names, read as RFC 3986 defines one. Searching the
// URL for the name would also match it inside another parameter's name
inline auto QUERY_OF(const std::string_view url, const std::string_view name)
    -> std::string {
  const sourcemeta::core::URI target{std::string{url}};
  return std::string{target.query().value().at(name).value()};
}

// A provider a case has control of, and what it says. Everything this module
// reaches goes through one function, so a case configures what the provider
// advertises and answers with, and then reads what was made of it. Nothing
// here reaches a network
struct TestProvider {
  std::string_view issuer{"https://provider.test"};
  // Members spliced into the discovery document, beyond the ones every valid
  // one carries
  // A JSON object merged into what discovery answers with
  std::string advertises{};
  // What the UserInfo endpoint answers, empty where the provider offers none
  std::string userinfo{};
  // The identity token the exchange returns, put here once a login has said
  // what nonce it must carry
  std::shared_ptr<std::string> identity{std::make_shared<std::string>()};
  // Whether the token request carried the client secret in a header, which is
  // how a case reads the way it travelled
  std::shared_ptr<bool> secret_in_header{std::make_shared<bool>(false)};
  std::shared_ptr<int> discoveries{std::make_shared<int>(0)};
  // A provider that answers nothing at all, which is one that cannot be reached
  bool reachable{true};

  [[nodiscard]] auto fetcher() const
      -> sourcemeta::one::Authentication::Fetcher {
    return [issuer = std::string{this->issuer}, advertises = this->advertises,
            userinfo = this->userinfo, identity = this->identity,
            secret_in_header = this->secret_in_header,
            discoveries = this->discoveries, reachable = this->reachable](
               sourcemeta::one::Authentication::ProviderRequest &&request)
               -> std::optional<
                   sourcemeta::one::Authentication::ProviderResponse> {
      if (!reachable) {
        return std::nullopt;
      }

      if (request.url == issuer + "/.well-known/openid-configuration") {
        *discoveries += 1;
        auto document{sourcemeta::core::JSON::make_object()};
        document.assign("issuer", sourcemeta::core::JSON{issuer});
        document.assign("authorization_endpoint",
                        sourcemeta::core::JSON{issuer + "/authorize"});
        document.assign("token_endpoint",
                        sourcemeta::core::JSON{issuer + "/token"});
        document.assign("jwks_uri", sourcemeta::core::JSON{issuer + "/keys"});
        if (!userinfo.empty()) {
          document.assign("userinfo_endpoint",
                          sourcemeta::core::JSON{issuer + "/userinfo"});
        }

        // Whatever else the case wants this provider to say about itself
        if (!advertises.empty()) {
          const auto extra{sourcemeta::core::parse_json(advertises)};
          for (const auto &entry : extra.as_object()) {
            document.assign(entry.first, entry.second);
          }
        }

        auto responses{sourcemeta::core::JSON::make_array()};
        responses.push_back(sourcemeta::core::JSON{"code"});
        document.assign("response_types_supported", std::move(responses));
        auto subjects{sourcemeta::core::JSON::make_array()};
        subjects.push_back(sourcemeta::core::JSON{"public"});
        document.assign("subject_types_supported", std::move(subjects));
        auto algorithms{sourcemeta::core::JSON::make_array()};
        algorithms.push_back(sourcemeta::core::JSON{"RS256"});
        algorithms.push_back(sourcemeta::core::JSON{"ES256"});
        document.assign("id_token_signing_alg_values_supported",
                        std::move(algorithms));
        std::ostringstream serialized;
        sourcemeta::core::stringify(document, serialized);
        return sourcemeta::one::Authentication::ProviderResponse{
            .status = 200, .body = serialized.str()};
      }

      if (request.url == issuer + "/keys") {
        return sourcemeta::one::Authentication::ProviderResponse{
            .status = 200, .body = std::string{CLAIMS_KEYS}};
      }

      if (request.url == issuer + "/token") {
        *secret_in_header = !request.authorization.empty();
        auto body{sourcemeta::core::JSON::make_object()};
        body.assign("id_token", sourcemeta::core::JSON{*identity});
        body.assign("access_token", sourcemeta::core::JSON{"an-access-token"});
        body.assign("token_type", sourcemeta::core::JSON{"Bearer"});
        std::ostringstream serialized;
        sourcemeta::core::stringify(body, serialized);
        return sourcemeta::one::Authentication::ProviderResponse{
            .status = 200, .body = serialized.str()};
      }

      if (request.url == issuer + "/userinfo" && !userinfo.empty()) {
        return sourcemeta::one::Authentication::ProviderResponse{
            .status = 200, .body = userinfo};
      }

      return std::nullopt;
    };
  }
};

inline constexpr std::string_view INSTANCE_URL{"https://registry.test"};
inline constexpr std::string_view REDIRECT_URI{
    "https://registry.test/self/v1/auth/callback/okta"};

// Whether any of what an operation reported names this, which is how a case
// reads a reason that never reaches a response
inline auto REPORTED(const sourcemeta::one::Authentication::Outcome &outcome,
                     const std::string_view fragment) -> bool {
  return std::ranges::any_of(
      outcome.log, [fragment](const std::string &message) -> bool {
        return message.find(fragment) != std::string::npos;
      });
}

// An interactive policy names one variable per secret it accepts, newest
// first, so each set of policies points at the variables it needs
// An interactive policy names the environment variable holding the secret
// that signs its session and transaction cookies, so the tests set that
// variable and mint cookies under its value
inline constexpr const char *SESSION_SECRET_VARIABLE{"ONE_TEST_SESSION_SECRET"};

inline constexpr std::array<std::string_view, 1> SESSION_SECRETS{
    {SESSION_SECRET_VARIABLE}};
inline constexpr std::array<std::string_view, 1> SESSION_SECRETS_UNUSED{
    {"ONE_TEST_OIDC_SESSION_UNUSED"}};
// Every outbound call this module makes goes through one function, so a test
// answers them all from a map and nothing here reaches a network
inline auto STUB_FETCHER(std::map<std::string, std::string> responses,
                         std::shared_ptr<int> calls)
    -> sourcemeta::one::Authentication::Fetcher {
  return
      [responses = std::move(responses), calls = std::move(calls)](
          sourcemeta::one::Authentication::ProviderRequest &&request)
          -> std::optional<sourcemeta::one::Authentication::ProviderResponse> {
        if (calls != nullptr) {
          *calls += 1;
        }

        const auto match{responses.find(std::string{request.url})};
        if (match == responses.cend()) {
          return std::nullopt;
        }

        return sourcemeta::one::Authentication::ProviderResponse{
            .status = 200, .body = match->second, .max_age = std::nullopt};
      };
}

// The sealed values this instance mints are its own workings rather than its
// interface, so these reach them the only way anybody can: by starting a login
// and handing what it produced back to the callback that reads it. That is also
// the only way they are ever reached in service of a request.
//
// A value that opens carries the callback past the one gate that reads it, and
// a value that does not is refused there. So the two answers below are what
// every case here distinguishes, and the provider is deliberately given no
// token endpoint, which stops the callback one step after that gate and keeps
// every test off a network

inline const std::string_view INSTANCE{"https://registry.test"};
inline const std::string_view REDIRECT{
    "https://registry.test/self/v1/auth/callback/okta"};

// A request carries cookie fields rather than bare values, so a test that
// wants a value read has to present it the way a browser would
inline auto field(const std::string_view value) -> std::string {
  std::string result{"sourcemeta_one_transaction="};
  result += value;
  return result;
}

// A provider complete enough to start a login against, which answers nothing
// at its token endpoint. A callback that got past the seal stops there, and
// one that did not is refused before it is ever consulted
inline auto provider() -> sourcemeta::one::Authentication::Fetcher {
  return
      [](sourcemeta::one::Authentication::ProviderRequest &&request)
          -> std::optional<sourcemeta::one::Authentication::ProviderResponse> {
        if (request.url !=
            "https://acme.test/.well-known/openid-configuration") {
          return std::nullopt;
        }

        return sourcemeta::one::Authentication::ProviderResponse{
            .status = 200,
            .body = R"JSON({
          "issuer": "https://acme.test",
          "authorization_endpoint": "https://acme.test/authorize",
          "token_endpoint": "https://acme.test/token",
          "jwks_uri": "https://acme.test/keys",
          "response_types_supported": [ "code" ],
          "subject_types_supported": [ "public" ],
          "id_token_signing_alg_values_supported": [ "RS256" ]
        })JSON",
            .max_age = std::nullopt};
      };
}

inline auto instance(const std::string &name,
                     const std::span<const std::string_view> secrets)
    -> sourcemeta::one::Authentication {
  setenv("ONE_TEST_SEAL_SECRET", "session-secret", 1);
  setenv("ONE_TEST_SEAL_ROTATED", "rotated-secret", 1);
  setenv("ONE_TEST_SEAL_FOREIGN", "foreign-secret", 1);
  setenv("ONE_TEST_SEAL_CLIENT", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_SEAL_CLIENT",
            .session_secrets = secrets}}}};
  return sourcemeta::one::Authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH(name), ANYWHERE)},
      provider()};
}

// What a login handed the browser: the sealed value it must bring back, and the
// state the provider is expected to echo beside it
struct TestStarted {
  std::string sealed;
  std::string state;
};

inline auto start(const sourcemeta::one::Authentication &authentication)
    -> TestStarted {
  const auto outcome{
      authentication.login("okta", INSTANCE, REDIRECT, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(outcome.cookies.size(), 1);
  return {.sealed = COOKIE_VALUE(outcome.cookies.front()),
          .state = QUERY_OF(outcome.location, "state")};
}

// Everything a callback is given besides the value under test. A case about one
// of these names it and leaves the rest as the login that minted the value
// would have had them
struct Presented {
  std::string_view policy{"okta"};
  std::string_view redirect{REDIRECT};
  // Empty means whatever state the login actually minted
  std::string_view state{};
  std::string_view cookie{"sourcemeta_one_transaction"};
  // Whether the value is carried at all, since arriving with nothing is its own
  // case
  bool carried{true};
  // Another value under the same name, placed ahead of the real one, which is
  // what a parent domain setting its own cookie looks like
  std::string_view shadow{};
};

// Whether the callback read the value as the transaction it names. Anything it
// cannot open is refused before the provider is consulted at all, and anything
// it opens gets one step further, to a provider that named no token endpoint
inline auto OPENS(const sourcemeta::one::Authentication &authentication,
                  const TestStarted &started, const std::string_view sealed,
                  const Presented &given = {}) -> bool {
  std::string carried{given.cookie};
  carried += '=';
  carried += sealed;
  std::string shadowed{given.cookie};
  shadowed += '=';
  shadowed += given.shadow;
  std::vector<std::string_view> fields;
  if (!given.shadow.empty()) {
    fields.push_back(shadowed);
  }

  if (given.carried) {
    fields.push_back(carried);
  }

  const auto outcome{authentication.callback(
      given.policy, INSTANCE, given.redirect,
      {.state = given.state.empty() ? started.state : given.state,
       .code = "an-authorization-code"},
      {.cookies = fields})};
  return outcome.result !=
         sourcemeta::one::Authentication::Outcome::Result::Invalid;
}

// A session obtained the only way anybody obtains one: by starting a login and
// completing it against a provider that answers. The gate cases below are about
// what a session opens rather than about how one is got, so they take it from
// here and go on to say what they mean.
//
// The provider is a stub and the identity token is signed with the key its key
// set publishes, so nothing here reaches a network
inline auto SESSION_FOR(const std::string_view policy_name,
                        const std::span<const std::string_view> secrets,
                        const std::string_view subject) -> std::string {
  setenv("ONE_TEST_SIGN_IN_CLIENT", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = policy_name,
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://signin.test",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SIGN_IN_CLIENT",
            .session_secrets = secrets}}}};

  // The identity token has to name the nonce the login just minted, so it is
  // signed once that is known and left here for the exchange to answer with
  const auto identity{std::make_shared<std::string>()};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, TEST_PATH("sign_in"), ANYWHERE)},
      [identity](sourcemeta::one::Authentication::ProviderRequest &&request)
          -> std::optional<sourcemeta::one::Authentication::ProviderResponse> {
        if (request.url ==
            "https://signin.test/.well-known/openid-configuration") {
          return sourcemeta::one::Authentication::ProviderResponse{
              .status = 200, .body = R"JSON({
                "issuer": "https://signin.test",
                "authorization_endpoint": "https://signin.test/authorize",
                "token_endpoint": "https://signin.test/token",
                "jwks_uri": "https://signin.test/keys",
                "response_types_supported": [ "code" ],
                "subject_types_supported": [ "public" ],
                "id_token_signing_alg_values_supported": [ "RS256", "ES256" ]
              })JSON"};
        }

        if (request.url == "https://signin.test/keys") {
          return sourcemeta::one::Authentication::ProviderResponse{
              .status = 200, .body = std::string{CLAIMS_KEYS}};
        }

        if (request.url == "https://signin.test/token") {
          auto body{sourcemeta::core::JSON::make_object()};
          body.assign("id_token", sourcemeta::core::JSON{*identity});
          body.assign("access_token",
                      sourcemeta::core::JSON{"an-access-token"});
          body.assign("token_type", sourcemeta::core::JSON{"Bearer"});
          std::ostringstream text;
          sourcemeta::core::stringify(body, text);
          return sourcemeta::one::Authentication::ProviderResponse{
              .status = 200, .body = text.str()};
        }

        return std::nullopt;
      }};

  const std::string_view instance_url{"https://registry.test"};
  const std::string_view redirect{"https://registry.test/callback"};
  const auto started{
      authentication.login(policy_name, instance_url, redirect, false, "")};
  EXPECT_EQ(started.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);

  auto payload{sourcemeta::core::JSON::make_object()};
  payload.assign("iss", sourcemeta::core::JSON{"https://signin.test"});
  payload.assign("aud", sourcemeta::core::JSON{"client"});
  payload.assign("sub", sourcemeta::core::JSON{std::string{subject}});
  payload.assign("exp", sourcemeta::core::JSON{2000000000});
  payload.assign("iat", sourcemeta::core::JSON{1700000000});
  payload.assign("nonce",
                 sourcemeta::core::JSON{QUERY_OF(started.location, "nonce")});
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(CLAIMS_PRIVATE_KEY))};
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{"ES256"});
  *identity = sourcemeta::core::jwt_sign(header, payload, key.value()).value();

  const auto carried{"sourcemeta_one_transaction=" +
                     COOKIE_VALUE(started.cookies.front())};
  const std::array<std::string_view, 1> presented{{carried}};
  const auto completed{authentication.callback(
      policy_name, instance_url, redirect,
      {.state = QUERY_OF(started.location, "state"), .code = "a-code"},
      {.cookies = presented})};
  EXPECT_EQ(completed.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  return COOKIE_VALUE(completed.cookies.front());
}

// What a configuration may declare is decided by the type as much as by the
// checks below, so these say what cannot be written down at all
// What a policy needs depends entirely on what it authenticates, and these say
// that the ones it does not need cannot be written down. A key policy carrying
// a key set location, or a machine policy carrying a client secret, was a
// configuration that meant nothing and is now one that does not compile
template <typename T>
concept names_a_key_set = requires(T value) { value.jwks_uri; };
template <typename T>
concept names_an_issuer = requires(T value) { value.issuer; };
template <typename T>
concept names_a_client_secret =
    requires(T value) { value.client_secret_variable; };
template <typename T>
concept names_keys = requires(T value) { value.keys; };
template <typename T>
concept names_session_secrets = requires(T value) { value.session_secrets; };
template <typename T>
concept names_an_audience = requires(T value) { value.audience; };
template <typename T>
concept names_algorithms = requires(T value) { value.algorithms; };

using ApiKeyPolicy = sourcemeta::one::Authentication::Policy::ApiKey;
using TokenPolicy = sourcemeta::one::Authentication::Policy::Token;
using InteractivePolicy = sourcemeta::one::Authentication::Policy::Interactive;

// A credential compared verbatim discovers nothing and signs nobody in
static_assert(!names_a_key_set<ApiKeyPolicy>);
static_assert(!names_an_issuer<ApiKeyPolicy>);
static_assert(!names_a_client_secret<ApiKeyPolicy>);
static_assert(!names_session_secrets<ApiKeyPolicy>);

// A token is validated rather than obtained here, so nothing about obtaining
// one belongs to it
static_assert(!names_keys<TokenPolicy>);
static_assert(!names_a_client_secret<TokenPolicy>);
static_assert(!names_session_secrets<TokenPolicy>);

// A person signs in, which is neither a key nor an audience this validates
static_assert(!names_keys<InteractivePolicy>);
static_assert(!names_an_audience<InteractivePolicy>);
static_assert(!names_algorithms<InteractivePolicy>);

// And what each does need is still there to be written
static_assert(names_keys<ApiKeyPolicy>);
static_assert(names_a_key_set<TokenPolicy>);
static_assert(names_an_audience<TokenPolicy>);
static_assert(names_session_secrets<InteractivePolicy>);
static_assert(names_a_client_secret<InteractivePolicy>);

// A view is what a table answers with rather than something a caller states,
// which the type is what enforces
template <typename T>
concept assembled_from_a_set =
    requires { T{std::string_view{}, std::uint64_t{}}; };
template <typename T>
concept states_a_set = requires(T value) { value.policies = std::uint64_t{}; };

// What the two above detect, so that the pair below says something
struct TestAssembledView {
  std::string_view name;
  std::uint64_t policies;
};
static_assert(assembled_from_a_set<TestAssembledView>);
static_assert(states_a_set<TestAssembledView>);

// A view is what a table answers with rather than something a caller states.
// A set assembled anywhere else names a view no build ever wrote, and would be
// read as admitting whatever it happened to hold
static_assert(
    !assembled_from_a_set<sourcemeta::one::Authentication::RecordedView>);
static_assert(!states_a_set<sourcemeta::one::Authentication::RecordedView>);

// An interactive policy names one variable per secret it accepts, newest
// first, so each set of policies points at the variables it needs

#endif
