#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <array>      // std::array
#include <chrono>     // std::chrono::sys_seconds, std::chrono::seconds
#include <cstddef>    // std::byte, std::size_t
#include <cstdint>    // std::uint32_t
#include <cstdlib>    // setenv
#include <cstring>    // std::memcpy
#include <filesystem> // std::filesystem::path
#include <fstream>    // std::ofstream, std::fstream, std::ifstream
#include <iostream>
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
static auto at(const std::string_view input)
    -> sourcemeta::one::Authentication::Path {
  return sourcemeta::one::Authentication::Path::parse(input,
                                                      "http://localhost:8000")
      .value();
}

// These tests name locations directly rather than modelling what an instance
// serves, so every path a policy is scoped to is one to gate
static auto anywhere(const std::string_view) -> bool { return true; }

// One cookie field, which is what a browser conforming to RFC 6265 sends
static auto fields(const std::string_view value)
    -> std::array<std::string_view, 1> {
  return {{value}};
}

// The artifact is compiled and then persisted, which the tests below do
// together because they read it back through a file
static auto
save(const std::span<const sourcemeta::one::Authentication::Policy> policies,
     const std::filesystem::path &configuration,
     const std::filesystem::path &destination,
     const sourcemeta::one::Authentication::PathGuard &gateable) -> void {
  sourcemeta::one::Authentication::Table::write(
      sourcemeta::one::Authentication::Table::compile(policies, configuration,
                                                      gateable),
      destination);
}

static auto test_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{AUTHENTICATION_TEST_DIRECTORY} / name;
}

// A locally signed RS256 access token (typ "at+jwt") with issuer "acme",
// audience "client", and an expiry far in the future, alongside the key set
// that verifies it and an unrelated key set that does not. Reused from the core
// JOSE test vectors so no signing happens here
static constexpr std::string_view SIGNED_TOKEN{
    "eyJhbGciOiJSUzI1NiIsInR5cCI6ImF0K2p3dCJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "U3ZBo7MvSW0U099gJ_"
    "vIA5T8HJ2XnKSzYmqkx7SDxgxQfmxQyu3QZIeKT68AAH7wQjWRvNWQ7f3Es57UUNUQAMs-"
    "z5TWlVBKtYZf5ZcbYqc4KrQ-ApwpjoFGJxurnd1R_"
    "tz02WssnvrZNKnxNPuGoYIkJKNCl59yLFJwRLf3nK_Jcxs-"
    "1m2MvKsm647PuXqhYOKlZkHOvkIV0RV8cLJ56_gDVjj7TlKQgwbTdW_"
    "71QLwLWRFGftU2EAWuqayTSpPeUA6kB4sfn7JNsweqDs7uev30m6y8BE9uzwzHuuovaN1cZz0o"
    "TAGXcx64sfbPs6HEMp5_FoU0SccxArAbnHSjA"};
static constexpr std::string_view SIGNED_KEYS{
    R"JSON({
      "keys": [
        {
          "kty": "RSA",
          "n": "oHTpl-jfNfBuXmBp58sW8s_77UP6j2jA0mjjKjhDkxhp7Agk-xLNGgfPCS_bjdZ6YU6FGeab8uVjkSgo9_0OCJUaF4vzEGwXmNuGawANxnZtiYjWvbJlq-2mn_L7rsqGQcSkMmyM0g4aX7dF8wB6DVrXShJ78fcrNtpeoU72YGEdjehA8qVclDFwBdpCGynxxnWJePk72lQb6gkVMqKMc3jBF8GkWf8oP_sjss-fpOjSUMR1c8_0JlTYWO46KWOZa0EO2t8H1V3imMyzbhoxRd_qZHmo46gJkG-ZdebjX0vGQllaCwu0z4kLcXIfAZhqPEkdssDGhC_txwJuhaPDFQ",
          "e": "AQAB"
        }
      ]
    })JSON"};
static constexpr std::string_view UNRELATED_KEYS{
    R"JSON({
      "keys": [
        {
          "kty": "RSA",
          "n": "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ",
          "e": "AQAB"
        }
      ]
    })JSON"};

// A claim rule is matched against whatever a provider put in a token, so these
// tests mint their own rather than reuse a fixed vector. The curve keeps the
// key small enough to read
static constexpr std::string_view CLAIMS_PRIVATE_KEY{
    R"JSON({
      "kty": "EC",
      "crv": "P-256",
      "x": "sQbBlwx8VKtzct6SJjoYb4hmXMRIhBdC_rQtfrA7GdU",
      "y": "gE3c3l2Uux8jUbm0DVEnXwPQlsD7ln4CLWt6FGxNbhk",
      "d": "n0c-5YK2MYjEvSiF8OOaQOiheqm14U4iN6PdZAGLXOE"
    })JSON"};
static constexpr std::string_view CLAIMS_KEYS{
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
static constexpr std::string_view CLAIMS_ONE_GROUP{
    R"JSON({
      "groups": {
        "essential": true,
        "values": [ "platform" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_ONCALL_GROUP{
    R"JSON({
      "groups": {
        "essential": true,
        "values": [ "oncall" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_TWO_GROUPS{
    R"JSON({
      "groups": {
        "essential": true,
        "values": [ "oncall", "platform" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_GROUP_AND_DEPARTMENT{
    R"JSON({
      "department": {
        "essential": true,
        "values": [ "engineering" ]
      },
      "groups": {
        "essential": true,
        "values": [ "platform" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_SCOPE{
    R"JSON({
      "scope": {
        "essential": true,
        "values": [ "registry:read" ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_VERIFIED{
    R"JSON({
      "verified": {
        "essential": true,
        "values": [ "true" ]
      }
    })JSON"};

// Rules the indexer never emits, since the configuration format refuses them,
// which a corrupt artifact could still carry into the gate
static constexpr std::string_view CLAIMS_SCOPE_NO_VALUES{
    R"JSON({
      "scope": {
        "essential": true,
        "values": []
      }
    })JSON"};
static constexpr std::string_view CLAIMS_SCOPE_UNREADABLE{
    R"JSON({
      "scope": {
        "essential": true,
        "values": [ 42 ]
      }
    })JSON"};
static constexpr std::string_view CLAIMS_SCOPE_UNCONSTRAINED{
    R"JSON({
      "scope": {
        "essential": true
      }
    })JSON"};
static constexpr std::string_view CLAIMS_GROUPS_NO_VALUES{
    R"JSON({
      "groups": {
        "essential": true,
        "values": []
      }
    })JSON"};

// A token from the issuer and audience the claim tests configure, carrying
// whatever additional claims a case is about
static auto token_with(const std::string_view claims) -> std::string {
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
static auto cookie_value(const std::string_view cookie) -> std::string {
  const auto equals{cookie.find('=')};
  const auto end{cookie.find(';', equals)};
  return std::string{cookie.substr(equals + 1, end - equals - 1)};
}

static auto query_of(const std::string_view url, const std::string_view name)
    -> std::string {
  std::string needle{name};
  needle += "=";
  const auto start{url.find(needle) + needle.size()};
  const auto end{url.find('&', start)};
  return std::string{url.substr(start, end - start)};
}

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

template <typename T>
concept assembled_from_a_set = requires {
  T{std::string_view{}, sourcemeta::one::Authentication::PolicySet{}};
};
template <typename T>
concept states_a_set = requires(T value) {
  value.policies = sourcemeta::one::Authentication::PolicySet{};
};

// What the two above detect, so that the pair below says something
struct AssembledView {
  std::string_view name;
  sourcemeta::one::Authentication::PolicySet policies;
};
static_assert(assembled_from_a_set<AssembledView>);
static_assert(states_a_set<AssembledView>);

// A view is what a table answers with rather than something a caller states.
// A set assembled anywhere else names a view no build ever wrote, and would be
// read as admitting whatever it happened to hold
static_assert(
    !assembled_from_a_set<sourcemeta::one::Authentication::RecordedView>);
static_assert(!states_a_set<sourcemeta::one::Authentication::RecordedView>);

// A provider a case has control of, and what it says. Everything this module
// reaches goes through one function, so a case configures what the provider
// advertises and answers with, and then reads what was made of it. Nothing
// here reaches a network
struct Provider {
  std::string_view issuer{"https://provider.test"};
  // Members spliced into the discovery document, beyond the ones every valid
  // one carries
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
        std::string document{"{"};
        document += R"("issuer": ")" + issuer + R"(",)";
        document +=
            R"("authorization_endpoint": ")" + issuer + R"(/authorize",)";
        document += R"("token_endpoint": ")" + issuer + R"(/token",)";
        document += R"("jwks_uri": ")" + issuer + R"(/keys",)";
        if (!userinfo.empty()) {
          document += R"("userinfo_endpoint": ")" + issuer + R"(/userinfo",)";
        }

        if (!advertises.empty()) {
          document += advertises;
          document += ",";
        }

        document += R"("response_types_supported": [ "code" ],)";
        document += R"("subject_types_supported": [ "public" ],)";
        document +=
            R"("id_token_signing_alg_values_supported": [ "RS256", "ES256" ])";
        document += "}";
        return sourcemeta::one::Authentication::ProviderResponse{
            .status = 200, .body = document};
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

static constexpr std::string_view INSTANCE_URL{"https://registry.test"};
static constexpr std::string_view REDIRECT_URI{
    "https://registry.test/self/v1/auth/callback/okta"};

// Start a login and complete it, with the identity token carrying whatever a
// case says the provider asserted about the person. A login that does not get
// as far as a redirect is returned as it is, since that is the answer a case
// asking about one wants
static auto sign_in(const sourcemeta::one::Authentication &authentication,
                    const Provider &provider, const std::string_view policy,
                    const std::string_view client_id,
                    const std::string_view asserted)
    -> sourcemeta::one::Authentication::Outcome {
  auto started{
      authentication.login(policy, INSTANCE_URL, REDIRECT_URI, false, "")};
  if (started.result !=
      sourcemeta::one::Authentication::Outcome::Result::Redirect) {
    return started;
  }

  auto payload{sourcemeta::core::parse_json(asserted)};
  payload.assign("iss", sourcemeta::core::JSON{std::string{provider.issuer}});
  payload.assign("aud", sourcemeta::core::JSON{std::string{client_id}});
  payload.assign("exp", sourcemeta::core::JSON{2000000000});
  payload.assign("iat", sourcemeta::core::JSON{1700000000});
  payload.assign("nonce",
                 sourcemeta::core::JSON{query_of(started.location, "nonce")});
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(CLAIMS_PRIVATE_KEY))};
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{"ES256"});
  *provider.identity =
      sourcemeta::core::jwt_sign(header, payload, key.value()).value();

  const auto carried{"sourcemeta_one_transaction=" +
                     cookie_value(started.cookies.front())};
  const std::array<std::string_view, 1> presented{{carried}};
  return authentication.callback(
      policy, INSTANCE_URL, REDIRECT_URI,
      {.state = query_of(started.location, "state"), .code = "a-code"},
      {.cookies = presented});
}

// Whether any of what an operation reported names this, which is how a case
// reads a reason that never reaches a response
static auto reported(const sourcemeta::one::Authentication::Outcome &outcome,
                     const std::string_view fragment) -> bool {
  return std::ranges::any_of(
      outcome.log, [fragment](const std::string &message) -> bool {
        return message.find(fragment) != std::string::npos;
      });
}

// A session obtained the only way anybody obtains one: by starting a login and
// completing it against a provider that answers. The gate cases below are about
// what a session opens rather than about how one is got, so they take it from
// here and go on to say what they mean.
//
// The provider is a stub and the identity token is signed with the key its key
// set publishes, so nothing here reaches a network
static auto session_for(const std::string_view policy_name,
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
              policies, test_path("sign_in"), anywhere)},
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
                 sourcemeta::core::JSON{query_of(started.location, "nonce")});
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(CLAIMS_PRIVATE_KEY))};
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{"ES256"});
  *identity = sourcemeta::core::jwt_sign(header, payload, key.value()).value();

  const auto carried{"sourcemeta_one_transaction=" +
                     cookie_value(started.cookies.front())};
  const std::array<std::string_view, 1> presented{{carried}};
  const auto completed{authentication.callback(
      policy_name, instance_url, redirect,
      {.state = query_of(started.location, "state"), .code = "a-code"},
      {.cookies = presented})};
  EXPECT_EQ(completed.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  return cookie_value(completed.cookies.front());
}

// An interactive policy names the environment variable holding the secret
// that signs its session and transaction cookies, so the tests set that
// variable and mint cookies under its value
static constexpr const char *SESSION_SECRET_VARIABLE{"ONE_TEST_SESSION_SECRET"};

// An interactive policy names one variable per secret it accepts, newest
// first, so each set of policies points at the variables it needs
static constexpr std::array<std::string_view, 1> SESSION_SECRETS{
    {SESSION_SECRET_VARIABLE}};
static constexpr std::array<std::string_view, 1> SESSION_SECRETS_UNUSED{
    {"ONE_TEST_OIDC_SESSION_UNUSED"}};
static constexpr std::array<std::string_view, 1> SESSION_SECRETS_BLANK{
    {"ONE_TEST_OIDC_BLANK_SECRET"}};
static constexpr std::array<std::string_view, 2> SESSION_SECRETS_ROTATED{
    {"ONE_TEST_OIDC_ROTATED_SECRET", "ONE_TEST_OIDC_ROTATED_SECRET_OLD"}};
static constexpr std::array<std::string_view, 1> SESSION_SECRETS_UNSET{
    {"ONE_TEST_OIDC_UNSET_SECRET"}};

// Every outbound call this module makes goes through one function, so a test
// answers them all from a map and nothing here reaches a network
static auto stub_fetcher(std::map<std::string, std::string> responses,
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

TEST(missing_artifact_denies_everything) {
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          std::filesystem::path{"/no/such/authentication.bin"}},
      stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.permits(at("/"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/acme/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(
      authentication.permits(at(""), authentication.caller({.bearer = ""})));
}

TEST(malformed_artifact_denies_everything) {
  const auto path{test_path("malformed.bin")};
  std::ofstream stream{path, std::ios::binary};
  const std::array<char, 64> garbage{};
  stream.write(garbage.data(), garbage.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.permits(at("/"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/acme/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(structurally_corrupt_artifact_denies_everything) {
  const auto path{test_path("corrupt.bin")};
  std::ofstream stream{path, std::ios::binary};
  // A valid header over an empty node table
  std::array<char, 40> header{};
  header[0] = 'A';
  header[1] = 'U';
  header[2] = 'T';
  header[3] = 'H';
  header[4] = 4;
  stream.write(header.data(), header.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.permits(at("/"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(artifact_exceeding_the_policy_ceiling_denies_everything) {
  const auto path{test_path("too-many-policies.bin")};
  std::ofstream stream{path, std::ios::binary};
  // A valid header declaring a policy count past the supported maximum
  std::array<char, 40> header{};
  header[0] = 'A';
  header[1] = 'U';
  header[2] = 'T';
  header[3] = 'H';
  header[4] = 4;
  header[8] =
      static_cast<char>(sourcemeta::one::Authentication::MAXIMUM_POLICIES + 1);
  header[12] = 1;
  stream.write(header.data(), header.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(
      authentication.permits(at("/"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/acme/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(corrupted_section_offset_denies_everything) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_OFFSET"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("corrupted_offset.bin")};
  save(policies, path, path, anywhere);

  // Overwrite the node section offset with a value that aliases the header
  std::fstream stream{path, std::ios::binary | std::ios::in | std::ios::out};
  stream.seekp(24);
  const std::array<char, 4> aliased{};
  stream.write(aliased.data(), aliased.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(
      authentication.permits(at("/"), authentication.caller({.bearer = ""})));
}

TEST(zero_policies_admits_every_path) {
  const std::array<sourcemeta::one::Authentication::Policy, 0> policies{};
  const auto path{test_path("zero_policies.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(
      authentication.permits(at("/"), authentication.caller({.bearer = ""})));
  EXPECT_TRUE(
      authentication.permits(at(""), authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(at("/acme/foo/bar"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_EQ(authentication.table().governing(at("/")),
            (std::vector<std::string_view>{}));
  EXPECT_EQ(authentication.table().governing(at("/acme")),
            (std::vector<std::string_view>{}));
}

TEST(uncovered_paths_are_public_around_a_gated_scope) {
  setenv("ONE_TEST_KEY_SCOPE", "scope-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SCOPE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("uncovered_public.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // The covered subtree is gated
  EXPECT_FALSE(authentication.permits(at("/internal"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      at("/internal"), authentication.caller({.bearer = "scope-secret"})));
  EXPECT_TRUE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "scope-secret"})));
  // Everything outside it is public
  EXPECT_TRUE(
      authentication.permits(at("/"), authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(at("/vendor"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(at("/vendor/foo"),
                                     authentication.caller({.bearer = ""})));
}

TEST(scope_matches_whole_segments_only) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SEGMENT"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("segment_boundary.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // The scope gates its own segment
  EXPECT_FALSE(authentication.permits(at("/internal"),
                                      authentication.caller({.bearer = ""})));
  // A textual prefix that is not a whole segment is a different path, so it is
  // uncovered and public
  EXPECT_TRUE(authentication.permits(at("/internalish"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(at("/int"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(at("/internal-team"),
                                     authentication.caller({.bearer = ""})));
}

TEST(distinct_policies_each_gate_their_scope) {
  const std::array<std::string_view, 1> alpha{{"/alpha"}};
  const std::array<std::string_view, 1> beta{{"/beta"}};
  const std::array<std::string_view, 1> gamma{{"/gamma"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_KEY_DA"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_KEY_DB"}};
  const std::array<std::string_view, 1> gamma_keys{{"ONE_TEST_KEY_DG"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = alpha,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                alpha_keys}},
       {.paths = beta,
        .name = "beta",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = beta_keys}},
       {.paths = gamma,
        .name = "gamma",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = gamma_keys}}}};
  const auto path{test_path("distinct_policies.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/alpha/one"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/beta/two"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/gamma/three"),
                                      authentication.caller({.bearer = ""})));
  // Between the scopes the registry is public
  EXPECT_TRUE(authentication.permits(at("/delta"),
                                     authentication.caller({.bearer = ""})));
}

TEST(nested_prefixes_gate_their_subtrees) {
  const std::array<std::string_view, 1> internal{{"/internal"}};
  const std::array<std::string_view, 1> secret{{"/internal/secret"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_NI"}};
  const std::array<std::string_view, 1> secret_keys{{"ONE_TEST_KEY_NS"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = internal,
        .name = "internal",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                internal_keys}},
       {.paths = secret,
        .name = "secret",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = secret_keys}}}};
  const auto path{test_path("nested_prefixes.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/internal"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/internal/other"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/internal/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/internal/secret/deep"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(
      authentication.permits(at("/"), authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(at("/public"),
                                     authentication.caller({.bearer = ""})));
}

TEST(nested_inner_key_widens_access) {
  setenv("ONE_TEST_KEY_WI", "wi-secret", 1);
  setenv("ONE_TEST_KEY_WO", "wo-secret", 1);
  const std::array<std::string_view, 1> outer{{"/internal"}};
  const std::array<std::string_view, 1> inner{{"/internal/secret"}};
  const std::array<std::string_view, 1> outer_keys{{"ONE_TEST_KEY_WO"}};
  const std::array<std::string_view, 1> inner_keys{{"ONE_TEST_KEY_WI"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = outer,
        .name = "outer",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                outer_keys}},
       {.paths = inner,
        .name = "inner",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = inner_keys}}}};
  const auto path{test_path("nested_widen.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // The inner path is covered by both, so either key admits it
  EXPECT_TRUE(authentication.permits(
      at("/internal/secret"), authentication.caller({.bearer = "wo-secret"})));
  EXPECT_TRUE(authentication.permits(
      at("/internal/secret"), authentication.caller({.bearer = "wi-secret"})));
  // The outer path is covered only by the outer policy
  EXPECT_TRUE(authentication.permits(
      at("/internal/other"), authentication.caller({.bearer = "wo-secret"})));
  EXPECT_FALSE(authentication.permits(
      at("/internal/other"), authentication.caller({.bearer = "wi-secret"})));
}

TEST(single_policy_with_multiple_prefixes) {
  const std::array<std::string_view, 2> paths{{"/internal", "/vendor"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MP"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("multiple_prefixes.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/vendor/bar"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(at("/public"),
                                     authentication.caller({.bearer = ""})));
}

TEST(extensionless_policy_gates_every_representation) {
  setenv("ONE_TEST_KEY_REPRESENTATION", "representation-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_REPRESENTATION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("representation_agnostic.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // The resource, every representation of it, and its subtree are all governed
  EXPECT_FALSE(authentication.permits(at("/secret/data"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/secret/data.json"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/secret/data.xml"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/secret/data/nested"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      at("/secret/data"),
      authentication.caller({.bearer = "representation-secret"})));
  EXPECT_TRUE(authentication.permits(
      at("/secret/data.json"),
      authentication.caller({.bearer = "representation-secret"})));
  EXPECT_TRUE(authentication.permits(
      at("/secret/data.xml"),
      authentication.caller({.bearer = "representation-secret"})));
  EXPECT_TRUE(authentication.permits(
      at("/secret/data/nested"),
      authentication.caller({.bearer = "representation-secret"})));
  // A sibling sharing a textual prefix is covered by no policy, so it is public
  EXPECT_TRUE(authentication.permits(at("/secret/database"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(at("/secret/data2.json"),
                                     authentication.caller({.bearer = ""})));
}

TEST(extension_specific_policy_gates_only_that_representation) {
  setenv("ONE_TEST_KEY_SPECIFIC", "specific-secret", 1);
  const std::array<std::string_view, 1> paths{{"/secret/data.json"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SPECIFIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("representation_specific.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // Only the named representation is gated
  EXPECT_FALSE(authentication.permits(at("/secret/data.json"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      at("/secret/data.json"),
      authentication.caller({.bearer = "specific-secret"})));
  EXPECT_TRUE(authentication.permits(
      at("/secret/data.json/nested"),
      authentication.caller({.bearer = "specific-secret"})));
  // The bare resource and other representations are uncovered, so public
  EXPECT_TRUE(authentication.permits(at("/secret/data"),
                                     authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(at("/secret/data.xml"),
                                     authentication.caller({.bearer = ""})));
}

TEST(extension_handling_is_confined_to_the_terminal_segment) {
  const std::array<std::string_view, 1> paths{{"/v1"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_V1"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("intermediate_dot.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // The policy on /v1 gates its own subtree
  EXPECT_FALSE(
      authentication.permits(at("/v1"), authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/v1/secret"),
                                      authentication.caller({.bearer = ""})));
  // As a terminal segment, /v1.0 is a representation of /v1 under the
  // content-negotiation rule, the same way /person.json represents /person
  EXPECT_FALSE(authentication.permits(at("/v1.0"),
                                      authentication.caller({.bearer = ""})));
  // But as an intermediate segment it is a distinct directory that does not
  // descend into the /v1 subtree, so its children are uncovered and public
  EXPECT_TRUE(authentication.permits(at("/v1.0/secret"),
                                     authentication.caller({.bearer = ""})));
}

TEST(an_explicit_route_is_gated_on_the_target_as_it_arrived) {
  setenv("ONE_TEST_KEY_ROUTE", "route-secret", 1);
  const std::array<std::string_view, 1> apikey_paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_ROUTE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = apikey_paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("explicit_route.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(
      sourcemeta::one::Authentication::RouteTarget{"/private/secret"},
      authentication.caller({.bearer = "", .cookies = {}})));
  EXPECT_TRUE(authentication.permits(
      sourcemeta::one::Authentication::RouteTarget{"/private/secret"},
      authentication.caller({.bearer = "route-secret", .cookies = {}})));

  // A target covered by no policy is admitted, including one whose spelling
  // only resembles a governed prefix
  EXPECT_TRUE(authentication.permits(
      sourcemeta::one::Authentication::RouteTarget{"/public/string"},
      authentication.caller({.bearer = "", .cookies = {}})));
  EXPECT_TRUE(authentication.permits(
      sourcemeta::one::Authentication::RouteTarget{"/privateextra/secret"},
      authentication.caller({.bearer = "", .cookies = {}})));
}

TEST(apikey_admits_matching_credential) {
  setenv("ONE_TEST_KEY_MATCH", "secret-match", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_MATCH"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("apikey_match.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "secret-match"})));
  EXPECT_FALSE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "wrong"})));
  EXPECT_FALSE(authentication.permits(at("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(apikey_with_multiple_keys_admits_any) {
  setenv("ONE_TEST_KEY_MULTI_A", "key-a", 1);
  setenv("ONE_TEST_KEY_MULTI_B", "key-b", 1);
  const std::array<std::string_view, 2> keys{
      {"ONE_TEST_KEY_MULTI_A", "ONE_TEST_KEY_MULTI_B"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("apikey_multi.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "key-a"})));
  EXPECT_TRUE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "key-b"})));
  EXPECT_FALSE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "key-c"})));
}

TEST(apikey_with_unset_variable_denies) {
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_UNSET"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("apikey_unset.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "anything"})));
  EXPECT_FALSE(authentication.permits(at("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
}

TEST(apikey_with_an_empty_variable_denies) {
  setenv("ONE_TEST_KEY_EMPTY", "", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_EMPTY"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("apikey_empty.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // A variable an operator meant to hold a key but left blank gates the path
  // exactly as an unset one does, rather than opening it to everyone
  EXPECT_FALSE(authentication.permits(at("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "anything"})));
}

TEST(apikey_ignores_an_empty_variable_beside_a_real_one) {
  setenv("ONE_TEST_KEY_PAIR_BLANK", "", 1);
  setenv("ONE_TEST_KEY_PAIR_REAL", "pair-secret", 1);
  const std::array<std::string_view, 2> keys{
      {"ONE_TEST_KEY_PAIR_BLANK", "ONE_TEST_KEY_PAIR_REAL"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("apikey_pair.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // The blank one neither admits anybody nor keeps the key beside it from
  // working
  EXPECT_TRUE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "pair-secret"})));
  EXPECT_FALSE(authentication.permits(at("/internal/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "wrong"})));
}

TEST(sha256_policy_with_an_empty_variable_denies) {
  setenv("ONE_TEST_KEY_SHA_EMPTY", "", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHA_EMPTY"}};
  const std::array<std::string_view, 1> paths{{"/secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = keys,
            .algorithm = sourcemeta::one::Authentication::Algorithm::Sha256}}}};
  const auto path{test_path("sha256_empty.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/secret/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(
      at("/secret/foo"), authentication.caller({.bearer = "anything"})));
  // Nor does the digest of nothing, which is what an empty credential hashes to
  EXPECT_FALSE(authentication.permits(
      at("/secret/foo"),
      authentication.caller({.bearer = sourcemeta::core::sha256("")})));
}

TEST(sha256_policy_admits_the_matching_credential) {
  const std::string raw{"raw-secret-key"};
  setenv("ONE_TEST_KEY_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SHA"}};
  const std::array<std::string_view, 1> paths{{"/secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = keys,
            .algorithm = sourcemeta::one::Authentication::Algorithm::Sha256}}}};
  const auto path{test_path("sha256_match.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.permits(at("/secret/foo"),
                                     authentication.caller({.bearer = raw})));
  EXPECT_FALSE(authentication.permits(
      at("/secret/foo"), authentication.caller({.bearer = "wrong"})));
  EXPECT_FALSE(authentication.permits(at("/secret/foo"),
                                      authentication.caller({.bearer = ""})));
  // Presenting the stored hash itself does not authenticate
  EXPECT_FALSE(authentication.permits(
      at("/secret/foo"),
      authentication.caller({.bearer = sourcemeta::core::sha256(raw)})));
}

TEST(mixed_algorithms_admit_either_key_with_identity_first) {
  setenv("ONE_TEST_KEY_MIXA_ID", "plain-a", 1);
  const std::string raw{"hashed-a"};
  setenv("ONE_TEST_KEY_MIXA_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> paths{{"/mixed"}};
  const std::array<std::string_view, 1> identity_keys{{"ONE_TEST_KEY_MIXA_ID"}};
  const std::array<std::string_view, 1> sha256_keys{{"ONE_TEST_KEY_MIXA_SHA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "identity",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{
                .keys = identity_keys,
                .algorithm =
                    sourcemeta::one::Authentication::Algorithm::Identity}},
       {.paths = paths,
        .name = "hashed",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = sha256_keys,
            .algorithm = sourcemeta::one::Authentication::Algorithm::Sha256}}}};
  const auto path{test_path("mixed_identity_first.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // Either key type opens the path regardless of declaration order. The sha256
  // key must work even though the identity policy is checked first and fails
  EXPECT_TRUE(authentication.permits(
      at("/mixed/x"), authentication.caller({.bearer = "plain-a"})));
  EXPECT_TRUE(authentication.permits(at("/mixed/x"),
                                     authentication.caller({.bearer = raw})));
  EXPECT_FALSE(authentication.permits(
      at("/mixed/x"), authentication.caller({.bearer = "neither"})));
}

TEST(mixed_algorithms_admit_either_key_with_sha256_first) {
  setenv("ONE_TEST_KEY_MIXB_ID", "plain-b", 1);
  const std::string raw{"hashed-b"};
  setenv("ONE_TEST_KEY_MIXB_SHA", sourcemeta::core::sha256(raw).c_str(), 1);
  const std::array<std::string_view, 1> paths{{"/mixed"}};
  const std::array<std::string_view, 1> identity_keys{{"ONE_TEST_KEY_MIXB_ID"}};
  const std::array<std::string_view, 1> sha256_keys{{"ONE_TEST_KEY_MIXB_SHA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "hashed",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{
                .keys = sha256_keys,
                .algorithm =
                    sourcemeta::one::Authentication::Algorithm::Sha256}},
       {.paths = paths,
        .name = "identity",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = identity_keys,
            .algorithm =
                sourcemeta::one::Authentication::Algorithm::Identity}}}};
  const auto path{test_path("mixed_sha256_first.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // The identity key must work even though the sha256 policy is checked first
  EXPECT_TRUE(authentication.permits(at("/mixed/x"),
                                     authentication.caller({.bearer = raw})));
  EXPECT_TRUE(authentication.permits(
      at("/mixed/x"), authentication.caller({.bearer = "plain-b"})));
  EXPECT_FALSE(authentication.permits(
      at("/mixed/x"), authentication.caller({.bearer = "neither"})));
}

TEST(supports_the_maximum_number_of_policies) {
  constexpr auto maximum{sourcemeta::one::Authentication::MAXIMUM_POLICIES};
  std::vector<std::string> path_storage;
  path_storage.reserve(maximum);
  for (std::size_t index{0}; index < maximum; index += 1) {
    path_storage.push_back("/p" + std::to_string(index));
  }

  std::vector<std::string_view> path_views;
  path_views.reserve(maximum);
  for (const auto &value : path_storage) {
    path_views.push_back(value);
  }

  std::vector<std::string> name_storage;
  name_storage.reserve(maximum);
  for (std::size_t index{0}; index < maximum; index += 1) {
    name_storage.push_back("p" + std::to_string(index));
  }

  std::vector<sourcemeta::one::Authentication::Policy> policies;
  policies.reserve(maximum);
  for (std::size_t index{0}; index < maximum; index += 1) {
    policies.push_back(
        {.paths = std::span<const std::string_view>{&path_views[index], 1},
         .name = name_storage[index],
         .credential = sourcemeta::one::Authentication::Policy::ApiKey{}});
  }

  const auto path{test_path("maximum_policies.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // The keyless policies gate their scope with no key that can open it
  EXPECT_FALSE(authentication.permits(at("/p0/foo"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_FALSE(authentication.permits(at("/p63/foo"),
                                      authentication.caller({.bearer = ""})));
  // An uncovered path is public
  EXPECT_TRUE(authentication.permits(at("/missing"),
                                     authentication.caller({.bearer = ""})));
}

TEST(governing_names_the_policies_in_declaration_order) {
  const std::array<std::string_view, 1> root_paths{{"/"}};
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> root_keys{{"ONE_TEST_KEY_GR"}};
  const std::array<std::string_view, 1> internal_keys{{"ONE_TEST_KEY_GI"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = root_paths,
        .name = "root",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = root_keys}},
       {.paths = internal_paths,
        .name = "internal",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = internal_keys}}}};
  const auto path{test_path("governing.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_EQ(gate.governing(at("/")), (std::vector<std::string_view>{"root"}));
  EXPECT_EQ(gate.governing(at("/vendor")),
            (std::vector<std::string_view>{"root"}));
  EXPECT_EQ(gate.governing(at("/internal")),
            (std::vector<std::string_view>{"root", "internal"}));
  EXPECT_EQ(gate.governing(at("/internal/foo")),
            (std::vector<std::string_view>{"root", "internal"}));
}

TEST(governing_of_an_ungoverned_path_is_empty) {
  const std::array<std::string_view, 1> internal_paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_GE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = internal_paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("governing_empty.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_EQ(gate.governing(at("/vendor")), (std::vector<std::string_view>{}));
  EXPECT_EQ(gate.governing(at("/internal")),
            (std::vector<std::string_view>{"policy"}));
}

TEST(reference_through_a_broken_artifact_is_rejected) {
  const sourcemeta::one::Authentication::Table gate{
      std::filesystem::path{"/no/such/authentication.bin"}};
  EXPECT_FALSE(gate.reference_permitted(at("/open/one"), at("/open/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/open/one"), at("/secret/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/secret/one"), at("/secret/two")));
}

TEST(reference_to_a_public_schema_is_permitted) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_PUBLIC"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = secret_paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("ref_to_public.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_TRUE(gate.reference_permitted(at("/secret/one"), at("/open/two")));
  EXPECT_TRUE(gate.reference_permitted(at("/open/one"), at("/open/two")));
}

TEST(public_schema_referencing_an_apikey_schema_is_rejected) {
  const std::array<std::string_view, 1> secret_paths{{"/secret"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_LEAK"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = secret_paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("ref_public_to_apikey.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/open/one"), at("/secret/two")));
}

TEST(reference_within_the_same_policy_is_permitted) {
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_REF_SAME"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("ref_same_policy.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_TRUE(
      gate.reference_permitted(at("/internal/one"), at("/internal/two")));
  EXPECT_TRUE(
      gate.reference_permitted(at("/internal/one"), at("/internal/one")));
}

TEST(reference_across_disjoint_policies_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_REF_ALPHA"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_REF_BETA"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                alpha_keys}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = beta_keys}}}};
  const auto path{test_path("ref_disjoint.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_from_a_narrower_to_a_wider_audience_is_permitted) {
  const std::array<std::string_view, 1> broad_paths{{"/p"}};
  const std::array<std::string_view, 1> nested_paths{{"/p/inner"}};
  const std::array<std::string_view, 1> broad_keys{{"ONE_TEST_REF_BROAD"}};
  const std::array<std::string_view, 1> nested_keys{{"ONE_TEST_REF_NESTED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = broad_paths,
        .name = "broad",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                broad_keys}},
       {.paths = nested_paths,
        .name = "nested",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = nested_keys}}}};
  const auto path{test_path("ref_narrow_to_wide.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_TRUE(gate.reference_permitted(at("/p/one"), at("/p/inner/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/p/inner/two"), at("/p/one")));
}

TEST(jwt_admits_a_valid_token_and_caches_the_key_set) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_valid.bin")};
  save(policies, path, path, anywhere);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   calls)};
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = "not-a-token"})));
  EXPECT_FALSE(authentication.permits(at("/secure/x"),
                                      authentication.caller({.bearer = ""})));
  // A second valid request reuses the cached key set rather than refetching
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  EXPECT_EQ(*calls, 1);
}

TEST(jwt_admits_a_token_whose_type_the_policy_requires) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .token_type = "at+jwt"}}}};
  const auto path{test_path("jwt_type_match.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_a_token_whose_type_is_not_the_required_one) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // An identity token is signed by the same provider under the same key, and
  // carries the client identifier as its audience, so where a policy names
  // that audience the type is the only thing telling the two apart
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .token_type = "JWT"}}}};
  const auto path{test_path("jwt_type_mismatch.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_without_a_required_type_admits_any_type) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // A provider that does not stamp the header cannot be told apart this way,
  // so a policy that names no type keeps working against one
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_type_absent.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_a_token_for_the_wrong_audience) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "different",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_audience.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_a_token_from_the_wrong_issuer) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "different",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_issuer.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_a_disallowed_algorithm) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_algorithm.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_when_the_signing_key_is_absent) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_unknown_key.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{UNRELATED_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_denies_when_the_key_set_cannot_be_fetched) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_fetch_fails.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(an_apikey_credential_never_triggers_a_jwt_fetch) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_no_fetch.bin")};
  save(policies, path, path, anywhere);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   calls)};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = "static-api-key"})));
  EXPECT_FALSE(authentication.permits(at("/secure/x"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_EQ(*calls, 0);
}

TEST(jwt_resolves_the_key_set_through_discovery) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // No key set location is pinned, so it is discovered from the issuer
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://acme.test",
            .audience = "client",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_discovery.bin")};
  save(policies, path, path, anywhere);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://acme.test/.well-known/openid-configuration",
                     R"JSON({
              "issuer": "https://acme.test",
              "jwks_uri": "https://acme.test/keys",
              "response_types_supported": [ "code" ],
              "subject_types_supported": [ "public" ],
              "id_token_signing_alg_values_supported": [ "RS256" ]
            })JSON"},
                    {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
                   calls)};
  // Both the provider metadata and the key set it names are retrieved, and
  // the token then fails only on its issuer claim, which names a different
  // issuer than the policy trusts
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  EXPECT_EQ(*calls, 2);
}

TEST(jwt_without_a_discoverable_issuer_fails_closed) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // The issuer claim is an opaque string rather than an https URL, so with no
  // pinned key set location there is nowhere trustworthy to discover one
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_discovery_issuer.bin")};
  save(policies, path, path, anywhere);
  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher(
          {{"acme/.well-known/openid-configuration",
            R"JSON({ "issuer": "acme", "jwks_uri": "https://acme.test/keys" })JSON"},
           {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
          calls)};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  EXPECT_EQ(*calls, 0);
}

TEST(mixed_apikey_and_jwt_policies_admit_either_credential) {
  setenv("ONE_TEST_KEY_BOTH", "static-secret", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_BOTH"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_mixed.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  // The static key opens the path
  EXPECT_TRUE(authentication.permits(
      at("/both/x"), authentication.caller({.bearer = "static-secret"})));
  // The token opens the path
  EXPECT_TRUE(authentication.permits(
      at("/both/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  // Neither a wrong key nor a wrong token opens it
  EXPECT_FALSE(authentication.permits(
      at("/both/x"), authentication.caller({.bearer = "wrong"})));
}

// A policy naming no rule admits whoever its provider vouched for, so signing
// in is the whole of it
TEST(a_policy_naming_no_rule_admits_whoever_signs_in) {
  setenv("ONE_TEST_ADMIT_OPEN", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_OPEN",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_admit_open"), anywhere)},
      provider.fetcher()};

  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "somebody-else" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// A claim that never arrived is a question the provider may still answer at its
// UserInfo endpoint, which is where a scope's claims land by default under this
// flow. A claim that arrived and fell short is an answer already given, so
// asking anywhere else would only repeat it.
//
// Whether asking again could still change the answer is exactly what tells the
// two apart, so that is what these say: the same provider answers the missing
// claim at UserInfo, and only the one that had not been answered is admitted
TEST(a_claim_that_never_arrived_is_asked_for_rather_than_refused) {
  setenv("ONE_TEST_ADMIT_PARTIAL", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  provider.userinfo = R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_PARTIAL",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_admit_partial"), anywhere)},
      provider.fetcher()};

  // It never arrived, so UserInfo is asked and answers
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);

  // It arrived and fell short, so nothing is asked and the answer stands, even
  // though the very same UserInfo answer would have satisfied the rule
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "support" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // And what the token carried on its own is enough where it satisfies the rule
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// The same claim, with a provider that answers nothing at its UserInfo
// endpoint, which is what shows the admission above came from asking
TEST(a_claim_that_never_arrived_and_is_nowhere_to_ask_refuses) {
  setenv("ONE_TEST_ADMIT_NO_USERINFO", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_NO_USERINFO",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_admit_no_userinfo"), anywhere)},
      provider.fetcher()};

  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// Rules are cumulative, so one that refuses settles it whatever another would
// have allowed
TEST(a_rule_that_refuses_settles_it_whatever_another_wants) {
  setenv("ONE_TEST_ADMIT_BOTH", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  provider.userinfo =
      R"JSON({ "sub": "a1b2", "groups": [ "platform" ], "email": "jane@acme.test", "email_verified": true })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_BOTH",
            .claims = CLAIMS_ONE_GROUP,
            .email_domains = domains,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_admit_both"), anywhere)},
      provider.fetcher()};

  // Both hold, which is the control
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ],
                       "email": "jane@acme.test", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);

  // The group falls short, and no address can make up for it
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "support" ],
                       "email": "jane@acme.test", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // The address is somewhere else, and no group can make up for that
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ],
                       "email": "jane@elsewhere.test", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
}

// A domain rule reads an address and the assertion that it was verified, which
// OpenID Connect Core Section 5.1 has speak for the address delivered with it
// and no other. A provider saying it will not vouch is an answer, so it settles
// the matter, while absence alone is what leaves the question open
TEST(an_address_the_provider_will_not_vouch_for_is_refused) {
  setenv("ONE_TEST_ADMIT_UNVOUCHED", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  provider.userinfo =
      R"JSON({ "sub": "a1b2", "email": "jane@acme.test", "email_verified": true })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_UNVOUCHED",
            .email_domains = domains,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_admit_unvouched"), anywhere)},
      provider.fetcher()};

  // The provider declined to vouch, which is an answer, so asking again cannot
  // change it even though UserInfo would have vouched
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "email": "jane@acme.test",
                             "email_verified": false })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // An address that is not one settles it the same way
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "email": 42 })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // Absence alone leaves the question open, so it is asked and answered
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// The token wins wherever both answers speak, since it arrives signed and
// verified while a UserInfo response is protected only by the transport that
// carried it. So the second fills gaps rather than overruling a signature
TEST(a_second_answer_fills_gaps_without_overruling_the_token) {
  setenv("ONE_TEST_COMBINE_TOKEN", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  provider.userinfo = R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_COMBINE_TOKEN",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_combine_token"), anywhere)},
      provider.fetcher()};

  // The token said something else about the very claim UserInfo would satisfy,
  // and the token is what stands
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "support" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // Where the token said nothing, the gap is filled
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// An address and the assertion that it was verified travel as a pair, from
// whichever answer carried the address. OpenID Connect Core Section 5.1 has
// `email_verified` speak for the `email` delivered alongside it and no other,
// so letting one answer's assertion vouch for the other answer's address would
// admit an address the provider never verified
// An address and the assertion that it was verified travel as a pair, from
// whichever answer carried the address. OpenID Connect Core Section 5.1 has
// `email_verified` speak for the `email` delivered alongside it and no other,
// so letting one answer's assertion vouch for the other answer's address would
// admit an address the provider never verified
TEST(an_address_arrives_with_its_own_assertion_or_not_at_all) {
  setenv("ONE_TEST_COMBINE_PAIR", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://provider.test",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_COMBINE_PAIR",
            .email_domains = domains,
            .session_secrets = SESSION_SECRETS}}}};

  // The token vouches for an address it never carried, and the second answer
  // supplies one without an assertion. Neither half may vouch for the other
  Provider orphaned;
  orphaned.userinfo = R"JSON({ "sub": "a1b2", "email": "jane@acme.test" })JSON";
  const sourcemeta::one::Authentication unvouched{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("combine_pair"), anywhere)},
      orphaned.fetcher()};
  EXPECT_EQ(sign_in(unvouched, orphaned, "okta", "client",
                    R"JSON({ "sub": "a1b2", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);

  // The control differs in one thing: the pair arrives whole from the answer
  // that carried the address
  Provider whole;
  whole.userinfo = R"JSON({ "sub": "a1b2", "email": "jane@acme.test",
                            "email_verified": true })JSON";
  const sourcemeta::one::Authentication vouched{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("combine_whole"), anywhere)},
      whole.fetcher()};
  EXPECT_EQ(sign_in(vouched, whole, "okta", "client",
                    R"JSON({ "sub": "a1b2", "email_verified": true })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

TEST(admitting_reads_two_answers_only_once_they_are_combined) {
  setenv("ONE_TEST_ADMIT_SPLIT", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  provider.userinfo =
      R"JSON({ "sub": "a1b2", "email": "jane@acme.test", "email_verified": true })JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> domains{{"acme.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_SPLIT",
            .claims = CLAIMS_ONE_GROUP,
            .email_domains = domains,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_admit_split"), anywhere)},
      provider.fetcher()};

  // The token carries the group and UserInfo carries the address, so only the
  // two together satisfy both rules
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "platform" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);

  // The same second answer cannot supply the group, so what the token carried
  // is what decided that half
  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2", "groups": [ "support" ] })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
}

// A rule compared against a claim carrying objects is compared on the `value`
// sub-attribute alone, which RFC 9068 gives group, role and entitlement claims
// by way of RFC 7643. So a rule naming what a person sees rather than what
// identifies them matches nothing, and a refusal cannot show that: the token it
// concerns is sealed inside a cookie where an operator cannot look. It is said
// in the log instead, which is the only place it can be said
TEST(a_claim_answered_with_objects_is_named_where_an_operator_looks) {
  setenv("ONE_TEST_SHAPE_OBJECTS", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "shapes",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SHAPE_OBJECTS",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("shape_objects"), anywhere)},
      provider.fetcher()};

  const auto objects{sign_in(authentication, provider, "shapes", "client",
                             R"JSON({ "sub": "a1b2",
               "groups": [ { "value": "g-1", "display": "platform" } ] })JSON")};
  EXPECT_EQ(objects.result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
  EXPECT_TRUE(reported(objects, "groups"));
  // Nothing about it reaches the person
  EXPECT_TRUE(objects.cookies.empty());
}

// The same rule, answered in the shape it names, says nothing at all. This
// names a policy of its own because what is said is said once per claim and
// policy however often somebody signs in
TEST(a_claim_answered_in_the_shape_a_rule_names_says_nothing) {
  setenv("ONE_TEST_SHAPE_STRINGS", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "strings",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SHAPE_STRINGS",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("shape_strings"), anywhere)},
      provider.fetcher()};

  // It matched, so there is nothing to explain
  const auto matched{sign_in(authentication, provider, "strings", "client",
                             R"JSON({ "sub": "a1b2",
                                      "groups": [ "platform" ] })JSON")};
  EXPECT_EQ(matched.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_FALSE(reported(matched, "groups"));

  // It fell short in the shape the rule names, which is an ordinary refusal
  // rather than a mistake worth naming
  const auto fell_short{sign_in(authentication, provider, "strings", "client",
                                R"JSON({ "sub": "a1b2",
                                         "groups": [ "support" ] })JSON")};
  EXPECT_EQ(fell_short.result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
  EXPECT_FALSE(reported(fell_short, "objects"));
}

// A rule on `scope` is never named that way. That claim is read as one
// space-delimited string rather than compared member by member, so one arriving
// as anything else is refused outright, and calling it an identifier mismatch
// would describe a mistake nobody made
TEST(a_scope_arriving_as_objects_is_refused_without_being_named) {
  setenv("ONE_TEST_SHAPE_SCOPE", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "scopes",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SHAPE_SCOPE",
            .claims = CLAIMS_SCOPE,
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("shape_scope"), anywhere)},
      provider.fetcher()};

  const auto objects{sign_in(
      authentication, provider, "scopes", "client",
      R"JSON({ "sub": "a1b2", "scope": [ { "value": "registry:read" } ] })JSON")};
  EXPECT_EQ(objects.result,
            sourcemeta::one::Authentication::Outcome::Result::NotAdmitted);
  EXPECT_FALSE(reported(objects, "scope"));
}

TEST(a_callback_under_a_name_this_instance_does_not_serve_is_refused) {
  setenv("ONE_TEST_ADMIT_UNKNOWN", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_ADMIT_UNKNOWN",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_admit_unknown"), anywhere)},
      provider.fetcher()};

  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(sign_in(authentication, provider, "nowhere", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Missing);
}

TEST(oidc_policy_admits_no_presented_credential) {
  setenv("ONE_TEST_OIDC_DENY", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_DENY",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{test_path("oidc_deny.bin")};
  save(policies, path, path, anywhere);

  // The provider is reachable and would verify the token, yet no presented
  // credential opens the path, not even one the equivalent token policy
  // would accept, and the provider is never contacted
  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"acme/.well-known/openid-configuration",
                     R"JSON({ "jwks_uri": "https://acme.test/keys" })JSON"},
                    {"https://acme.test/keys", std::string{SIGNED_KEYS}}},
                   calls)};

  const auto empty_permitted{authentication.permits(
      at("/portal/x"), authentication.caller({.bearer = ""}))};
  EXPECT_FALSE(empty_permitted);

  const auto secret_permitted{authentication.permits(
      at("/portal/x"), authentication.caller({.bearer = "confidential"}))};
  EXPECT_FALSE(secret_permitted);

  const auto token_permitted{authentication.permits(
      at("/portal/x"), authentication.caller({.bearer = SIGNED_TOKEN}))};
  EXPECT_FALSE(token_permitted);

  EXPECT_EQ(*calls, 0);
}

TEST(union_of_an_apikey_and_an_oidc_policy_admits_only_the_key) {
  setenv("ONE_TEST_KEY_OIDC_UNION", "union-secret", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_OIDC_UNION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_KEY_OIDC_UNION",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{test_path("oidc_union.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};

  const auto key_permitted{authentication.permits(
      at("/both/x"), authentication.caller({.bearer = "union-secret"}))};
  EXPECT_TRUE(key_permitted);

  const auto token_permitted{authentication.permits(
      at("/both/x"), authentication.caller({.bearer = SIGNED_TOKEN}))};
  EXPECT_FALSE(token_permitted);
}

TEST(oidc_policy_admits_its_session_cookie) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SESSION",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_session.bin")};
  save(policies, path, path, anywhere);

  const auto calls{std::make_shared<int>(0)};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, calls)};

  const auto sealed{session_for("okta", SESSION_SECRETS, "jane@acme.test")};
  const std::string cookies{"theme=dark; sourcemeta_one_session=" + sealed};

  const auto permitted{authentication.permits(
      at("/portal/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)}))};
  EXPECT_TRUE(permitted);
  EXPECT_EQ(*calls, 0);

  const auto anonymous_permitted{authentication.permits(
      at("/portal/x"), authentication.caller({.bearer = ""}))};
  EXPECT_FALSE(anonymous_permitted);
}

TEST(session_cookie_is_bound_to_the_policy_it_was_minted_for) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_OIDC_BIND_A",
                .session_secrets = SESSION_SECRETS}},
       {.paths = beta_paths,
        .name = "google",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_BIND_B",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_session_bound.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};

  const auto sealed{session_for("okta", SESSION_SECRETS, "")};

  // The session opens the path its policy governs
  const std::string okta_cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_TRUE(authentication.permits(
      at("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = fields(okta_cookies)})));

  // And not a path governed by another policy. Both policies here read the
  // same session secret, so the value verifies under either and the payload is
  // the only thing that tells them apart. There is no cookie name left to
  // separate them, which makes this the control rather than a second opinion
  EXPECT_FALSE(authentication.permits(
      at("/beta/x"),
      authentication.caller({.bearer = "", .cookies = fields(okta_cookies)})));
}

TEST(forged_session_cookie_is_denied) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_FOREIGN_SECRET", "other-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_FORGED",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("oidc_session_forged"), anywhere)},
      stub_fetcher({}, nullptr)};

  // The control, which is a session this policy did mint
  const auto genuine{session_for("okta", SESSION_SECRETS, "")};
  EXPECT_TRUE(authentication.permits(
      at("/portal/x"),
      authentication.caller(
          {.cookies = fields("sourcemeta_one_session=" + genuine)})));

  // A session minted under a secret this policy does not hold, which differs
  // from the one above in that alone
  const std::array<std::string_view, 1> foreign_secrets{
      {"ONE_TEST_FOREIGN_SECRET"}};
  const auto foreign{session_for("okta", foreign_secrets, "")};
  EXPECT_FALSE(authentication.permits(
      at("/portal/x"),
      authentication.caller(
          {.cookies = fields("sourcemeta_one_session=" + foreign)})));

  // A value whose signature no longer matches its contents
  auto tampered{genuine};
  tampered.back() = tampered.back() == 'A' ? 'B' : 'A';
  EXPECT_FALSE(authentication.permits(
      at("/portal/x"),
      authentication.caller(
          {.cookies = fields("sourcemeta_one_session=" + tampered)})));

  // A value that is not a sealed session at all
  EXPECT_FALSE(authentication.permits(
      at("/portal/x"),
      authentication.caller(
          {.cookies = fields("sourcemeta_one_session=garbage")})));
}

TEST(session_is_admitted_when_a_shadowing_cookie_precedes_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SHADOW_A",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_shadow_before.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto sealed{session_for("okta", SESSION_SECRETS, "")};

  // A parent domain can set a cookie the host also sets, and the header says
  // nothing about which is which, so the genuine one is honoured wherever it
  // appears
  const std::string cookies{"sourcemeta_one_session=not-a-session; "
                            "sourcemeta_one_session=" +
                            sealed};
  EXPECT_TRUE(authentication.permits(
      at("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));
}

TEST(session_is_admitted_when_a_shadowing_cookie_follows_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SHADOW_B",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_shadow_after.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto sealed{session_for("okta", SESSION_SECRETS, "")};

  // Taking the last match would deny here, which is the shape that lets a
  // neighbouring host lock somebody out of an instance it does not control
  const std::string cookies{"sourcemeta_one_session=" + sealed +
                            "; sourcemeta_one_session=not-a-session"};
  EXPECT_TRUE(authentication.permits(
      at("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));
}

TEST(session_is_admitted_when_it_arrives_in_a_later_cookie_field) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_FIELD_LATER",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_field_later.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto sealed{session_for("okta", SESSION_SECRETS, "")};

  // A request may carry its cookies across several fields rather than one, so
  // reading only the first would deny a session that did arrive
  const std::string second{"sourcemeta_one_session=" + sealed};
  const std::array<std::string_view, 2> carried{
      {"sourcemeta_one_session=not-a-session", second}};
  EXPECT_TRUE(authentication.permits(
      at("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = carried})));
}

TEST(session_is_admitted_when_it_arrives_in_an_earlier_cookie_field) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_FIELD_EARLIER",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_field_earlier.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto sealed{session_for("okta", SESSION_SECRETS, "")};

  // And neither field is the one that decides, so a later one carrying nothing
  // does not undo an earlier one that does
  const std::string first{"sourcemeta_one_session=" + sealed};
  const std::array<std::string_view, 2> carried{
      {first, "sourcemeta_one_session=not-a-session"}};
  EXPECT_TRUE(authentication.permits(
      at("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = carried})));
}

TEST(a_session_for_another_policy_does_not_end_the_search) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // Both policies read the same session secret, so a value minted for one
  // opens under the other and is only told apart by the payload
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_OIDC_SEARCH_A",
                .session_secrets = SESSION_SECRETS}},
       {.paths = beta_paths,
        .name = "google",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SEARCH_B",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_search.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto other{session_for("google", SESSION_SECRETS, "")};
  const auto mine{session_for("okta", SESSION_SECRETS, "")};

  // The first value opens but was minted elsewhere, so stopping there would
  // deny a caller who did present a session for this policy
  const std::string cookies{"sourcemeta_one_session=" + other +
                            "; sourcemeta_one_session=" + mine};
  EXPECT_TRUE(authentication.permits(
      at("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));

  // And a value minted elsewhere still opens nothing on its own
  const std::string alone{"sourcemeta_one_session=" + other};
  EXPECT_FALSE(authentication.permits(
      at("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = fields(alone)})));
}

TEST(a_shadowing_cookie_alone_never_admits) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/alpha"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SHADOW_C",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_shadow_only.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // Trying every value admits a caller if any one opens, and never because
  // several did not
  const std::string cookies{"sourcemeta_one_session=not-a-session; "
                            "sourcemeta_one_session=nor-is-this"};
  EXPECT_FALSE(authentication.permits(
      at("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));
}

TEST(a_session_never_admits_under_a_policy_sharing_its_secret) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // Deliberately the same secret for both, which the configuration permits.
  // The value therefore verifies under either policy and only the payload
  // distinguishes them
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_OIDC_SHARED_A",
                .session_secrets = SESSION_SECRETS}},
       {.paths = beta_paths,
        .name = "google",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SHARED_B",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_shared_secret.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto sealed{session_for("okta", SESSION_SECRETS, "")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  EXPECT_TRUE(authentication.permits(
      at("/alpha/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));
  EXPECT_FALSE(authentication.permits(
      at("/beta/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));
}

// Signing out asks the provider to end its own session, carrying the identity
// token as proof of whose it is asking about. Reaching that at all means the
// session opened and named the policy that minted it
TEST(signing_out_asks_the_provider_that_established_the_session) {
  setenv("ONE_TEST_LOGOUT_A", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  provider.advertises =
      R"JSON("end_session_endpoint": "https://provider.test/logout")JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_LOGOUT_A",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("logout_known"), anywhere)},
      provider.fetcher()};

  const auto established{session_for("okta", SESSION_SECRETS, "jane")};
  const auto carried{"sourcemeta_one_session=" + established};
  const auto outcome{
      authentication.logout({.cookies = fields(carried)}, INSTANCE_URL, "/")};
  EXPECT_TRUE(outcome.location.starts_with("https://provider.test/logout"));
  EXPECT_TRUE(outcome.location.find("id_token_hint=") != std::string::npos);
  // Both cookies expire whatever happened
  EXPECT_EQ(outcome.cookies.size(), 3);
}

// A session naming a policy this instance does not serve opens nothing, so
// there is nobody to ask and the browser is simply forgotten here
TEST(signing_out_with_a_session_naming_no_policy_here_asks_nobody) {
  setenv("ONE_TEST_LOGOUT_B", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  provider.advertises =
      R"JSON("end_session_endpoint": "https://provider.test/logout")JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_LOGOUT_B",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("logout_unknown"), anywhere)},
      provider.fetcher()};

  // A session another instance minted under a name this one never declared
  const auto elsewhere{session_for("nowhere", SESSION_SECRETS, "jane")};
  const auto carried{"sourcemeta_one_session=" + elsewhere};
  const auto outcome{
      authentication.logout({.cookies = fields(carried)}, INSTANCE_URL, "/")};
  EXPECT_EQ(outcome.location, "/");
  // The instance still forgets, which is the secure outcome either way
  EXPECT_EQ(outcome.cookies.size(), 3);
}

TEST(a_transaction_never_admits_as_a_session) {
  setenv("ONE_TEST_PURPOSE_CLIENT", "confidential", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_PURPOSE_CLIENT",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("oidc_open_session_purpose"), anywhere)},
      stub_fetcher({{"https://acme.test/.well-known/openid-configuration",
                     R"JSON({
              "issuer": "https://acme.test",
              "authorization_endpoint": "https://acme.test/authorize",
              "token_endpoint": "https://acme.test/token",
              "jwks_uri": "https://acme.test/keys",
              "response_types_supported": [ "code" ],
              "subject_types_supported": [ "public" ],
              "id_token_signing_alg_values_supported": [ "RS256", "ES256" ]
            })JSON"}},
                   nullptr)};

  const auto started{authentication.login("okta", "https://registry.test",
                                          "https://registry.test/callback",
                                          false, "")};
  EXPECT_EQ(started.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  const auto transaction{cookie_value(started.cookies.front())};

  // The control is a session, which does admit
  const auto session{session_for("okta", SESSION_SECRETS, "")};
  EXPECT_TRUE(authentication.permits(
      at("/portal/x"),
      authentication.caller(
          {.cookies = fields("sourcemeta_one_session=" + session)})));

  EXPECT_FALSE(authentication.permits(
      at("/portal/x"),
      authentication.caller(
          {.cookies = fields("sourcemeta_one_session=" + transaction)})));
}

TEST(session_cookie_without_a_configured_secret_is_denied) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  // The session secret variable is deliberately never set in the environment
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_NO_SECRETS",
            .session_secrets = SESSION_SECRETS_UNSET}}}};
  const auto path{test_path("oidc_session_no_secrets.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};

  const auto sealed{session_for("okta", SESSION_SECRETS, "")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_FALSE(authentication.permits(
      at("/portal/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));
}

TEST(session_admitted_under_a_rotated_secret) {
  // The policy names the newest secret first, then the one it replaces, so a
  // session established under the old secret still verifies
  setenv("ONE_TEST_OIDC_ROTATED_SECRET", "new-secret", 1);
  setenv("ONE_TEST_OIDC_ROTATED_SECRET_OLD", "old-secret", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_ROTATED",
            .session_secrets = SESSION_SECRETS_ROTATED}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("oidc_session_rotated"), anywhere)},
      stub_fetcher({}, nullptr)};

  // A session established when the old secret was the only one is still
  // admitted, which is what lets a secret be replaced without ending it
  const std::array<std::string_view, 1> old_only{
      {"ONE_TEST_OIDC_ROTATED_SECRET_OLD"}};
  const auto established{session_for("okta", old_only, "")};
  EXPECT_TRUE(authentication.permits(
      at("/portal/x"),
      authentication.caller(
          {.cookies = fields("sourcemeta_one_session=" + established)})));

  // A secret no longer in the set is refused, which differs from the one above
  // in exactly that
  setenv("ONE_TEST_OIDC_RETIRED_SECRET", "retired-secret", 1);
  const std::array<std::string_view, 1> retired_only{
      {"ONE_TEST_OIDC_RETIRED_SECRET"}};
  const auto retired{session_for("okta", retired_only, "")};
  EXPECT_FALSE(authentication.permits(
      at("/portal/x"),
      authentication.caller(
          {.cookies = fields("sourcemeta_one_session=" + retired)})));
}

TEST(session_with_a_blank_configured_secret_is_denied) {
  setenv("ONE_TEST_OIDC_BLANK_SECRET", "", 1);
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_BLANK",
            .session_secrets = SESSION_SECRETS_BLANK}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("oidc_session_blank"), anywhere)},
      stub_fetcher({}, nullptr)};

  // A blank secret would let anybody forge a session, so nothing verifies one,
  // not even a session this system established under a real secret
  const auto established{session_for("okta", SESSION_SECRETS, "")};
  EXPECT_FALSE(authentication.permits(
      at("/portal/x"),
      authentication.caller(
          {.cookies = fields("sourcemeta_one_session=" + established)})));
}

TEST(save_creates_the_directory_it_writes_into) {
  setenv("ONE_TEST_KEY_NESTED", "nested-secret", 1);
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NESTED"}};
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("nested") / "deeper" / "authentication.bin"};
  std::filesystem::remove_all(test_path("nested"));
  save(policies, path, path, anywhere);

  EXPECT_TRUE(std::filesystem::exists(path));
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "nested-secret"})));
}

TEST(an_artifact_whose_table_omits_the_anonymous_view_is_refused) {
  setenv("ONE_TEST_KEY_TABLE_ANONYMOUS", "table-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_TABLE_ANONYMOUS"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("table_anonymous.bin")};
  save(policies, path, path, anywhere);

  // The anonymous view is the entry naming no policy, so giving the entry that
  // holds it a policy is what a table naming nobody anonymous looks like. Where
  // the table sits is read from the header rather than recomputed here
  std::fstream stream{path, std::ios::binary | std::ios::in | std::ios::out};
  stream.seekg(static_cast<std::streamoff>(sizeof(std::uint32_t) * 8));
  std::uint32_t views_offset{0};
  std::array<char, sizeof(views_offset)> located{};
  stream.read(located.data(), located.size());
  std::memcpy(&views_offset, located.data(), sizeof(views_offset));

  stream.seekp(static_cast<std::streamoff>(views_offset));
  const std::array<char, 8> occupied{{1, 0, 0, 0, 0, 0, 0, 0}};
  stream.write(occupied.data(), occupied.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // A refused artifact denies rather than serving anybody a tree it guessed at
  EXPECT_FALSE(authentication.permits(
      at("/machine/x"), authentication.caller({.bearer = "table-secret"})));
  EXPECT_FALSE(authentication.permits(at("/anywhere"),
                                      authentication.caller({.bearer = ""})));
}

TEST(save_rejects_a_nameless_policy) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_NAMELESS"}}}};
  const auto path{test_path("oidc_nameless.bin")};
  try {
    save(policies, path, path, anywhere);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationPolicyNameError &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy requires a name of its own");
  }
}

TEST(save_rejects_a_nameless_key_policy) {
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NAMELESS"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("key_nameless.bin")};
  try {
    save(policies, path, path, anywhere);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationPolicyNameError &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy requires a name of its own");
  }
}

TEST(save_rejects_two_policies_sharing_a_name) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_keys{{"ONE_TEST_KEY_SAME_A"}};
  const std::array<std::string_view, 1> beta_keys{{"ONE_TEST_KEY_SAME_B"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "shared",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                alpha_keys}},
       {.paths = beta_paths,
        .name = "shared",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = beta_keys}}}};
  const auto path{test_path("name_shared.bin")};
  try {
    save(policies, path, path, anywhere);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationPolicyNameError &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy requires a name of its own");
  }
}

TEST(save_rejects_a_policy_taking_the_anonymous_name) {
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_RESERVED"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "public",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("name_reserved.bin")};
  try {
    save(policies, path, path, anywhere);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationPolicyNameError &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy requires a name of its own");
  }
}

TEST(save_writes_the_largest_table_a_configuration_can_declare) {
  constexpr std::size_t groups{4};
  constexpr auto per_group{
      sourcemeta::one::Authentication::Table::MAXIMUM_COMBINABLE_POLICIES};
  constexpr auto total{groups * per_group};
  std::vector<std::string> path_storage;
  std::vector<std::string> name_storage;
  std::vector<std::string> issuer_storage;
  std::vector<std::string> claims_storage;
  path_storage.reserve(total);
  name_storage.reserve(total);
  issuer_storage.reserve(total);
  claims_storage.reserve(total);
  for (std::size_t index{0}; index < total; index += 1) {
    path_storage.push_back("/p" + std::to_string(index));
    name_storage.push_back("p" + std::to_string(index));
    // The first group answers to the tokens these tests mint, so that a caller
    // reaches into the table rather than only past it. The rest name issuers of
    // their own, which is what keeps the groups apart and the table at its
    // largest
    issuer_storage.push_back(index < per_group
                                 ? "acme"
                                 : "https://idp.test/" +
                                       std::to_string(index / per_group));
    claims_storage.push_back(
        R"JSON({ "groups": { "essential": true, "values": [ "g)JSON" +
        std::to_string(index) + R"JSON(" ] } })JSON");
  }

  std::vector<std::string_view> path_views;
  path_views.reserve(total);
  for (const auto &value : path_storage) {
    path_views.push_back(value);
  }

  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  std::vector<sourcemeta::one::Authentication::Policy> policies;
  policies.reserve(total);
  for (std::size_t index{0}; index < total; index += 1) {
    policies.push_back(
        {.paths = std::span<const std::string_view>{&path_views[index], 1},
         .name = name_storage[index],
         .credential = sourcemeta::one::Authentication::Policy::Token{
             .issuer = issuer_storage[index],
             .audience = "client",
             .jwks_uri = "https://idp.test/jwks",
             .algorithms = algorithms,
             .claims = claims_storage[index]}});
  }

  // Each group contributes every combination over it, which is the most views a
  // configuration can ask for at the maximum number of policies
  const auto path{test_path("largest_table.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(), "public");
  // A name read back out of a table this size, rather than the entry that sits
  // first in it whatever was written after
  EXPECT_EQ(
      authentication
          .caller({.bearer = token_with(R"JSON({ "groups": [ "g0" ] })JSON")})
          .view(),
      "p0");
  EXPECT_EQ(
      authentication
          .caller({.bearer = token_with(R"JSON({ "groups": [ "g15" ] })JSON")})
          .view(),
      "p15");
  // And a combination, spelled from its members sorted rather than from the
  // order the token happened to carry them in
  EXPECT_EQ(authentication
                .caller({.bearer = token_with(
                             R"JSON({ "groups": [ "g1", "g0" ] })JSON")})
                .view(),
            "p0+p1");
  EXPECT_EQ(authentication
                .caller({.bearer = token_with(
                             R"JSON({ "groups": [ "g2", "g15" ] })JSON")})
                .view(),
            "p15+p2");
  // A token no policy answers to is placed nowhere, which is the anonymous
  // view rather than a name the table happens to carry
  EXPECT_EQ(
      authentication
          .caller({.bearer = token_with(R"JSON({ "groups": [ "gx" ] })JSON")})
          .view(),
      "public");
}

TEST(save_rejects_a_policy_named_as_a_combination_of_others) {
  setenv("ONE_TEST_KEY_COMBINATION", "combination-secret", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{
      {"ONE_TEST_KEY_COMBINATION"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  // The two token policies name one issuer, so they combine into a view spelled
  // by joining them, which is the name the third policy also carries
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms}},
       {.paths = beta_paths,
        .name = "beta",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms}},
       {.paths = machine_paths,
        .name = "alpha+beta",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const auto path{test_path("name_combination.bin")};
  try {
    save(policies, path, path, anywhere);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationViewNameCollisionError &error) {
    EXPECT_STREQ(
        error.what(),
        "An authentication policy name collides with a combination of others");
  }
}

TEST(save_accepts_a_separator_that_spells_no_other_combination) {
  setenv("ONE_TEST_KEY_LONE_SEPARATOR", "separator-secret", 1);
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{
      {"ONE_TEST_KEY_LONE_SEPARATOR"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = machine_paths,
        .name = "alpha+beta",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const auto path{test_path("name_separator.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = "separator-secret"}).view(),
            "alpha+beta");
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(), "public");
}

TEST(union_of_an_apikey_and_an_oidc_policy_admits_key_or_session) {
  setenv("ONE_TEST_KEY_SESSION_UNION", "union-key", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SESSION_UNION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "acme",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_SESSION_UNION",
            .session_secrets = SESSION_SECRETS}}}};
  const auto path{test_path("oidc_session_union.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};

  const auto key_permitted{authentication.permits(
      at("/both/x"), authentication.caller({.bearer = "union-key"}))};
  EXPECT_TRUE(key_permitted);

  const auto sealed{session_for("okta", SESSION_SECRETS, "")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  const auto session_permitted{authentication.permits(
      at("/both/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)}))};
  EXPECT_TRUE(session_permitted);

  EXPECT_FALSE(authentication.permits(at("/both/x"),
                                      authentication.caller({.bearer = ""})));
}

TEST(session_cookie_does_not_open_an_apikey_path) {
  setenv("ONE_TEST_KEY_NO_SESSION", "key-only", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_NO_SESSION"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("oidc_session_apikey.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};

  const auto sealed{session_for("okta", SESSION_SECRETS, "")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};
  EXPECT_FALSE(authentication.permits(
      at("/internal/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));
}

// A login is offered under the name a policy was declared with, and nowhere
// else. A name this instance does not serve is answered as missing, which is
// the same answer a typo gets
TEST(a_login_starts_only_under_a_declared_name) {
  setenv("ONE_TEST_NAMED", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_NAMED",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("named"), anywhere)},
      provider.fetcher()};

  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(
      authentication.login("elsewhere", INSTANCE_URL, REDIRECT_URI, false, "")
          .result,
      sourcemeta::one::Authentication::Outcome::Result::Missing);
  EXPECT_EQ(
      authentication.login("", INSTANCE_URL, REDIRECT_URI, false, "").result,
      sourcemeta::one::Authentication::Outcome::Result::Missing);
}

// What a provider says about itself is retrieved once and kept for as long as
// it said, so a second login costs nothing. Anybody may start one, so asking
// again each time would let a stranger drive traffic at the provider
TEST(what_a_provider_said_is_retrieved_once_and_reused) {
  setenv("ONE_TEST_OIDC_CACHE", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;

  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_CACHE",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_oidc_cache"), anywhere)},
      provider.fetcher()};

  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(*provider.discoveries, 1);

  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(*provider.discoveries, 1);
}

// RFC 6749 Section 2.3.1 has every server accept the client secret in an
// authorization header and discourages carrying it in the request body, so the
// header is used wherever the provider takes it. A provider that lists nothing
// is taken to accept it, which is what the specification assigns to silence
TEST(a_provider_naming_no_authentication_method_gets_the_header) {
  setenv("ONE_TEST_OIDC_AUTH_SILENT", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;

  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_AUTH_SILENT",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_oidc_auth_silent"), anywhere)},
      provider.fetcher()};

  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_TRUE(*provider.secret_in_header);
}

TEST(a_provider_naming_the_header_gets_the_header) {
  setenv("ONE_TEST_OIDC_AUTH_BASIC", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  provider.advertises =
      R"JSON("token_endpoint_auth_methods_supported": [ "client_secret_basic" ])JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_AUTH_BASIC",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_oidc_auth_basic"), anywhere)},
      provider.fetcher()};

  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_TRUE(*provider.secret_in_header);
}

// A provider that does not take the header leaves the body as the only way to
// authenticate, so the preference gives way rather than the login failing. A
// body is what logging and proxies keep, which is why it is the second choice
TEST(a_provider_refusing_the_header_gets_the_body_instead) {
  setenv("ONE_TEST_OIDC_AUTH_POST", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  provider.advertises =
      R"JSON("token_endpoint_auth_methods_supported": [ "client_secret_post" ])JSON";
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_AUTH_POST",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_oidc_auth_post"), anywhere)},
      provider.fetcher()};

  EXPECT_EQ(sign_in(authentication, provider, "okta", "client",
                    R"JSON({ "sub": "a1b2" })JSON")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_FALSE(*provider.secret_in_header);
}

TEST(a_login_against_an_unreachable_provider_cannot_start) {
  setenv("ONE_TEST_OIDC_UNREACHABLE", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;

  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_UNREACHABLE",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("one_test_oidc_unreachable"), anywhere)},
      provider.fetcher()};
  Provider silent{provider};
  silent.reachable = false;
  const sourcemeta::one::Authentication unreachable{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("unreachable"), anywhere)},
      silent.fetcher()};

  const auto outcome{
      unreachable.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(reported(outcome, "authorization endpoint"));

  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

TEST(a_login_without_a_client_secret_cannot_start) {
  unsetenv("ONE_TEST_SECRET_UNSET");
  const Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SECRET_UNSET",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("secret_unset"), anywhere)},
      provider.fetcher()};

  const auto outcome{
      authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(reported(outcome, "No client secret is set"));
  // Nothing about the refusal reaches the person
  EXPECT_TRUE(outcome.location.empty());
  EXPECT_TRUE(outcome.cookies.empty());
}

// A variable set to nothing is not a secret, so it is the same answer as one
// that was never set, which is what the control beside it shows
TEST(a_login_with_a_blank_client_secret_cannot_start) {
  setenv("ONE_TEST_SECRET_BLANK", "", 1);
  setenv("ONE_TEST_SECRET_SET", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SECRET_BLANK",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("secret_blank"), anywhere)},
      provider.fetcher()};
  const auto outcome{
      authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(reported(outcome, "No client secret is set"));

  const std::array<sourcemeta::one::Authentication::Policy, 1> configured{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SECRET_SET",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication working{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              configured, test_path("secret_set"), anywhere)},
      provider.fetcher()};
  EXPECT_EQ(working.login("okta", INSTANCE_URL, REDIRECT_URI, false, "").result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
}

// A policy naming no variable at all names no secret either
TEST(a_login_naming_no_secret_variable_cannot_start) {
  const Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("secret_unnamed"), anywhere)},
      provider.fetcher()};

  const auto outcome{
      authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(reported(outcome, "No client secret is set"));
}

TEST(a_machine_policy_starts_no_login) {
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_MACHINE_KEY"}};
  setenv("ONE_TEST_MACHINE_KEY", "machine-secret", 1);
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("machine_login"), anywhere)},
      Provider{}.fetcher()};

  EXPECT_EQ(
      authentication.login("machine", INSTANCE_URL, REDIRECT_URI, false, "")
          .result,
      sourcemeta::one::Authentication::Outcome::Result::Missing);
}

TEST(a_login_asking_for_nowhere_returns_to_what_the_policy_governs) {
  setenv("ONE_TEST_DEFAULT_PATH", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  const Provider provider;
  const std::array<std::string_view, 2> paths{{"/portal", "/second"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_DEFAULT_PATH",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("default_path"), anywhere)},
      provider.fetcher()};

  const auto completed{sign_in(authentication, provider, "okta", "client",
                               R"JSON({ "sub": "a1b2" })JSON")};
  EXPECT_EQ(completed.result,
            sourcemeta::one::Authentication::Outcome::Result::Redirect);
  EXPECT_EQ(completed.location, "/portal");
}

// An instance that could not read its artifact offers no login at all, which
// is the same answer it gives to every other question
TEST(a_login_through_a_broken_artifact_is_missing) {
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          std::filesystem::path{"/no/such/authentication.bin"}},
      {}};
  EXPECT_EQ(authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")
                .result,
            sourcemeta::one::Authentication::Outcome::Result::Missing);
}

// A session is sealed under the secret of the policy that established it, so a
// value one policy minted is nothing to another, even where both are declared
// here. That is what stops a caller choosing which policy a value is read as
TEST(a_session_is_bound_to_the_policy_whose_secret_sealed_it) {
  setenv("ONE_TEST_BIND_A", "confidential", 1);
  setenv("ONE_TEST_BIND_B", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_BIND_OTHER_SECRET", "another-secret", 1);
  Provider provider;
  const std::array<std::string_view, 1> alpha{{"/alpha"}};
  const std::array<std::string_view, 1> beta{{"/beta"}};
  const std::array<std::string_view, 1> other{{"ONE_TEST_BIND_OTHER_SECRET"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = provider.issuer,
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_BIND_A",
                .session_secrets = SESSION_SECRETS}},
       {.paths = beta,
        .name = "google",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_BIND_B",
            .session_secrets = other}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("bind_policies"), anywhere)},
      provider.fetcher()};

  const auto established{session_for("okta", SESSION_SECRETS, "jane")};
  const auto carried{"sourcemeta_one_session=" + established};

  // It opens what the policy that minted it governs
  EXPECT_TRUE(authentication.permits(
      at("/alpha/x"), authentication.caller({.cookies = fields(carried)})));
  // And nothing the other governs, which holds a secret of its own
  EXPECT_FALSE(authentication.permits(
      at("/beta/x"), authentication.caller({.cookies = fields(carried)})));
}

// Without a secret there is nothing to seal a login with, so it does not start
TEST(a_login_without_a_session_secret_cannot_start) {
  setenv("ONE_TEST_SEAL_NONE", "confidential", 1);
  unsetenv("ONE_TEST_SEAL_NONE_SECRET");
  Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<std::string_view, 1> absent{{"ONE_TEST_SEAL_NONE_SECRET"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_SEAL_NONE",
            .session_secrets = absent}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("seal_none"), anywhere)},
      provider.fetcher()};

  const auto outcome{
      authentication.login("okta", INSTANCE_URL, REDIRECT_URI, false, "")};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Unavailable);
  EXPECT_TRUE(reported(outcome, "No session secret is set"));
  EXPECT_TRUE(outcome.cookies.empty());
}

TEST(reference_within_the_same_oidc_scope_is_permitted) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // The two policies differ in name and in the environment variable holding
  // the secret, neither of which affects who can authenticate, so the scopes
  // stay equal
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://login.test",
                .client_id = "registry",
                .client_secret_variable = "ONE_TEST_OIDC_REF_SAME",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://login.test",
            .client_id = "registry",
            .client_secret_variable = "ONE_TEST_OIDC_REF_SAME_OTHER",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{test_path("oidc_ref_same.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_TRUE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_across_distinct_oidc_clients_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://login.test",
                .client_id = "registry",
                .client_secret_variable = "ONE_TEST_OIDC_REF_ALPHA",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://login.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_BETA",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{test_path("oidc_ref_distinct.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_across_swapped_oidc_identities_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // One policy's issuer is the other's client identifier and vice versa, so
  // the scopes share both strings yet denote different provider clients
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://login.test",
                .client_id = "registry",
                .client_secret_variable = "ONE_TEST_OIDC_REF_SWAP_ALPHA",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "registry",
            .client_id = "https://login.test",
            .client_secret_variable = "ONE_TEST_OIDC_REF_SWAP_BETA",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{test_path("oidc_ref_swapped.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_mixing_identities_across_oidc_policies_is_rejected) {
  const std::array<std::string_view, 1> source_paths{{"/source"}};
  const std::array<std::string_view, 1> target_paths{{"/target"}};
  // The referrer pairs an issuer and a client identifier that the referent
  // only carries through two different policies, so no single referent scope
  // matches and the reference must not slip through their union
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = source_paths,
        .name = "source",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://alpha.test",
                .client_id = "dashboard",
                .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_SOURCE",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = target_paths,
        .name = "target-one",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://alpha.test",
                .client_id = "registry",
                .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_ONE",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = target_paths,
        .name = "target-two",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://beta.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_MIX_TWO",
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{test_path("oidc_ref_mixed.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/source/one"), at("/target/two")));
}

TEST(reference_between_oidc_scopes_distinguishes_claims) {
  setenv("ONE_TEST_OIDC_REF_CLAIMS", "confidential", 1);
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<std::string_view, 1> gated_paths{{"/gated"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = open_paths,
        .name = "open",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://acme.test",
                .client_id = "dashboard",
                .client_secret_variable = "ONE_TEST_OIDC_REF_CLAIMS",
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = gated_paths,
        .name = "gated",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_CLAIMS",
            .claims = CLAIMS_ONE_GROUP,
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{test_path("oidc_ref_claims.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  // The same provider client admitting a narrower set of people is a different
  // audience, so neither direction reaches the other
  EXPECT_FALSE(gate.reference_permitted(at("/open/one"), at("/gated/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/gated/two"), at("/open/one")));
}

TEST(reference_between_oidc_scopes_distinguishes_email_domains) {
  setenv("ONE_TEST_OIDC_REF_DOMAINS", "confidential", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> alpha_domains{{"acme.test"}};
  const std::array<std::string_view, 1> beta_domains{{"other.test"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://acme.test",
                .client_id = "dashboard",
                .client_secret_variable = "ONE_TEST_OIDC_REF_DOMAINS",
                .email_domains = alpha_domains,
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_DOMAINS",
            .email_domains = beta_domains,
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{test_path("oidc_ref_domains.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
}

TEST(reference_between_oidc_scopes_ignores_how_rules_were_written) {
  setenv("ONE_TEST_OIDC_REF_SPELLING", "confidential", 1);
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  // The same two domains, written in a different order and a different case
  const std::array<std::string_view, 2> alpha_domains{
      {"acme.test", "Other.Test"}};
  const std::array<std::string_view, 2> beta_domains{
      {"OTHER.test", "ACME.TEST"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "alpha",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://acme.test",
                .client_id = "dashboard",
                .client_secret_variable = "ONE_TEST_OIDC_REF_SPELLING",
                .claims = CLAIMS_TWO_GROUPS,
                .email_domains = alpha_domains,
                .session_secrets = SESSION_SECRETS_UNUSED}},
       {.paths = beta_paths,
        .name = "beta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "dashboard",
            .client_secret_variable = "ONE_TEST_OIDC_REF_SPELLING",
            .claims = CLAIMS_TWO_GROUPS,
            .email_domains = beta_domains,
            .session_secrets = SESSION_SECRETS_UNUSED}}}};
  const auto path{test_path("oidc_ref_spelling.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  // A domain names a host, so its case says nothing about who is admitted,
  // and neither does the order the rules were written in
  EXPECT_TRUE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(admission_by_an_apikey_policy_identifies_the_principal) {
  setenv("ONE_TEST_KEY_PRINCIPAL", "principal-secret", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("principal_apikey.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto permitted{authentication.permits(
      at("/internal/foo"),
      authentication.caller({.bearer = "principal-secret"}))};
  EXPECT_TRUE(permitted);
}

TEST(admission_by_a_jwt_policy_identifies_the_principal) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("principal_jwt.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};
  const auto permitted{authentication.permits(
      at("/secure/foo"), authentication.caller({.bearer = SIGNED_TOKEN}))};
  EXPECT_TRUE(permitted);
}

TEST(principal_identifies_the_admitting_policy_among_several) {
  setenv("ONE_TEST_KEY_PRINCIPAL_MIXED", "principal-mixed", 1);
  const std::array<std::string_view, 1> paths{{"/both"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL_MIXED"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("principal_mixed.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{SIGNED_KEYS}}},
                   nullptr)};

  const auto apikey_permitted{authentication.permits(
      at("/both/x"), authentication.caller({.bearer = "principal-mixed"}))};
  EXPECT_TRUE(apikey_permitted);

  const auto jwt_permitted{authentication.permits(
      at("/both/x"), authentication.caller({.bearer = SIGNED_TOKEN}))};
  EXPECT_TRUE(jwt_permitted);
}

TEST(anonymous_and_denied_verdicts_carry_no_principal) {
  setenv("ONE_TEST_KEY_PRINCIPAL_NONE", "principal-none", 1);
  const std::array<std::string_view, 1> paths{{"/internal"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_PRINCIPAL_NONE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("principal_none.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};

  // An uncovered path admits an anonymous caller
  const auto anonymous_permitted{authentication.permits(
      at("/open/foo"), authentication.caller({.bearer = ""}))};
  EXPECT_TRUE(anonymous_permitted);

  // A denial identifies nobody
  const auto denied_permitted{authentication.permits(
      at("/internal/foo"), authentication.caller({.bearer = "wrong"}))};
  EXPECT_FALSE(denied_permitted);

  // A broken artifact denies with no principal either
  const sourcemeta::one::Authentication missing{
      sourcemeta::one::Authentication::Table{
          std::filesystem::path{"/no/such/authentication.bin"}},
      stub_fetcher({}, nullptr)};
  const auto missing_permitted{missing.permits(
      at("/internal/foo"), missing.caller({.bearer = "principal-none"}))};
  EXPECT_FALSE(missing_permitted);
}

TEST(reference_rules_treat_a_jwt_scope_conservatively) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_reference.bin")};
  save(policies, path, path, anywhere);

  // Reference checks read only the policy, so no key set transport is needed
  const sourcemeta::one::Authentication::Table gate{path};
  // A public schema may not reference one behind the token scope
  EXPECT_FALSE(gate.reference_permitted(at("/open/one"), at("/secure/two")));
  // The token scope may reference a public schema, and itself
  EXPECT_TRUE(gate.reference_permitted(at("/secure/one"), at("/open/two")));
  EXPECT_TRUE(gate.reference_permitted(at("/secure/one"), at("/secure/two")));
}

TEST(jwt_without_a_transport_denies_rather_than_crashes) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_no_transport.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, {}};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_policies_sharing_an_issuer_use_their_own_key_set) {
  const std::array<std::string_view, 1> primary_paths{{"/primary"}};
  const std::array<std::string_view, 1> secondary_paths{{"/secondary"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = primary_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/primary",
                .algorithms = algorithms}},
       {.paths = secondary_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/secondary",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_shared_issuer.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher(
          {{"https://idp.test/primary", std::string{SIGNED_KEYS}},
           {"https://idp.test/secondary", std::string{UNRELATED_KEYS}}},
          nullptr)};
  // The primary path is populated first, which under a per-issuer cache would
  // have leaked its key set to the secondary path
  EXPECT_TRUE(authentication.permits(
      at("/primary/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
  EXPECT_FALSE(authentication.permits(
      at("/secondary/x"), authentication.caller({.bearer = SIGNED_TOKEN})));
}

TEST(jwt_claims_admit_only_a_token_carrying_a_named_value) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_ONE_GROUP}}}};
  const auto path{test_path("jwt_claims_value.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "groups": [ "platform" ] })JSON")})));
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "groups": [ "support" ] })JSON")})));
  // A token the policy would otherwise admit, carrying no such claim at all
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = token_with("{}")})));
}

TEST(jwt_claims_admit_any_one_of_the_named_values) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_TWO_GROUPS}}}};
  const auto path{test_path("jwt_claims_alternatives.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "groups": [ "platform" ] })JSON")})));
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "groups": [ "oncall" ] })JSON")})));
  // Belonging to something else as well takes nothing away
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(
               R"JSON({ "groups": [ "support", "oncall" ] })JSON")})));
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "groups": [ "support" ] })JSON")})));
}

TEST(jwt_claims_require_every_rule_it_declares) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_GROUP_AND_DEPARTMENT}}}};
  const auto path{test_path("jwt_claims_cumulative.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(
               R"JSON({ "groups": [ "platform" ], "department": "engineering" })JSON")})));
  // Either rule alone leaves the other unsatisfied
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "groups": [ "platform" ] })JSON")})));
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer =
               token_with(R"JSON({ "department": "engineering" })JSON")})));
}

TEST(jwt_claims_read_a_scope_as_a_space_delimited_set) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_SCOPE}}}};
  const auto path{test_path("jwt_claims_scope.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "scope": "registry:read" })JSON")})));
  // The value is one of several granted, in any position
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(
               R"JSON({ "scope": "openid registry:read profile" })JSON")})));
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "scope": "openid" })JSON")})));
  // A granted scope that merely contains the required one as a prefix is a
  // different grant, and admitting it would hand over what nobody issued
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer =
               token_with(R"JSON({ "scope": "registry:readwrite" })JSON")})));
  // Scope values are case-sensitive by RFC 6749 Section 3.3
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "scope": "Registry:Read" })JSON")})));
}

TEST(jwt_claims_deny_an_ordinary_rule_that_names_no_value) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_GROUPS_NO_VALUES}}}};
  const auto path{test_path("jwt_claims_groups_empty.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // The same reading the scope rule gets, on the path that defers the
  // comparison rather than making it here
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "groups": [ "platform" ] })JSON")})));
}

TEST(jwt_claims_deny_a_scope_rule_that_names_no_value) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_SCOPE_NO_VALUES}}}};
  const auto path{test_path("jwt_claims_scope_empty.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // An allow list naming nothing admits nobody, rather than widening to
  // every token that carries any scope at all
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "scope": "registry:read" })JSON")})));
}

TEST(jwt_claims_deny_a_scope_rule_this_cannot_read) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_SCOPE_UNREADABLE}}}};
  const auto path{test_path("jwt_claims_scope_unreadable.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // A value that is not a scope token denies, since passing it over would
  // leave a rule that admits every token carrying any scope
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "scope": "registry:read" })JSON")})));
}

TEST(jwt_claims_scope_without_a_constraint_still_requires_a_scope) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_SCOPE_UNCONSTRAINED}}}};
  const auto path{test_path("jwt_claims_scope_open.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // Constraining no value asks only that a scope be carried, so any one does
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "scope": "anything" })JSON")})));
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = token_with("{}")})));
  // A scope that is not a space-delimited string grants nothing this can read
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "scope": [ "anything" ] })JSON")})));
}

TEST(jwt_claims_match_a_group_object_on_its_identifier_alone) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_ONE_GROUP}}}};
  const auto path{test_path("jwt_claims_group_object.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // The shape RFC 9068 Section 2.2.3.1 gives the claim by way of RFC 7643
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(
               R"JSON({ "groups": [ { "value": "platform", "display": "Platform" } ] })JSON")})));
  // A display name is neither unique nor stable, so admitting on one would let
  // whoever can rename a group grant access
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(
               R"JSON({ "groups": [ { "value": "g-1", "display": "platform" } ] })JSON")})));
}

TEST(jwt_claims_never_match_a_value_that_is_not_a_string) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_VERIFIED}}}};
  const auto path{test_path("jwt_claims_non_string.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "verified": true })JSON")})));
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "verified": 1 })JSON")})));
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "verified": "true" })JSON")})));
}

TEST(jwt_without_claims_admits_a_token_carrying_none) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_claims_absent.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_TRUE(authentication.permits(
      at("/secure/x"), authentication.caller({.bearer = token_with("{}")})));
}

TEST(jwt_claims_that_do_not_parse_deny_everything) {
  const std::array<std::string_view, 1> paths{{"/secure"}};
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms,
                .claims = CLAIMS_ONE_GROUP}},
       {.paths = open_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("jwt_claims_corrupt.bin")};
  save(policies, path, path, anywhere);

  // Overwrite the first byte of the serialised rules, leaving their length
  // intact so that they are read but no longer parse
  std::string buffer;
  {
    std::ifstream input{path, std::ios::binary};
    buffer.assign(std::istreambuf_iterator<char>{input},
                  std::istreambuf_iterator<char>{});
  }
  const auto opening{buffer.find(R"("groups")")};
  if (opening == std::string::npos || opening == 0) {
    throw std::runtime_error{"Could not locate the serialized claim rules"};
  }

  buffer[opening - 1] = '?';
  {
    std::ofstream output{path, std::ios::binary | std::ios::trunc};
    output.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  }

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  // Rules that cannot be read are not passed over, since doing so would drop
  // the restriction and admit everyone the policy was meant to narrow
  EXPECT_FALSE(authentication.permits(
      at("/secure/x"),
      authentication.caller(
          {.bearer = token_with(R"JSON({ "groups": [ "platform" ] })JSON")})));
  // The whole artifact denies, rather than only the policy that carried them
  EXPECT_FALSE(authentication.permits(
      at("/open/x"), authentication.caller({.bearer = token_with("{}")})));
}

TEST(reference_between_jwt_scopes_distinguishes_claims) {
  const std::array<std::string_view, 1> open_paths{{"/open"}};
  const std::array<std::string_view, 1> gated_paths{{"/gated"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = open_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms}},
       {.paths = gated_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_ONE_GROUP}}}};
  const auto path{test_path("jwt_claims_reference.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  // Two policies alike but for their rules admit different callers, so the
  // looser one may not reach what the stricter one guards
  EXPECT_FALSE(gate.reference_permitted(at("/open/one"), at("/gated/two")));
  // The reverse is refused too, exactly as a differing token type is. A scope
  // is one indivisible identity rather than a set compared piecewise, so the
  // cost is a build that has to say so, against disclosing a referent to
  // somebody the referrer never admitted
  EXPECT_FALSE(gate.reference_permitted(at("/gated/two"), at("/open/one")));
}

TEST(reference_between_jwt_scopes_ignores_the_order_rules_were_written_in) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms,
                .claims = CLAIMS_TWO_GROUPS}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_TWO_GROUPS}}}};
  const auto path{test_path("jwt_claims_reference_order.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  // The rules arrive canonical, so two policies admitting the same callers
  // carry identical bytes and count as one audience in either direction
  EXPECT_TRUE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_between_jwt_scopes_distinguishes_algorithms) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<std::string_view, 1> gamma_paths{{"/gamma"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> ecdsa{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = ecdsa}},
       {.paths = gamma_paths,
        .name = "policy-2",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto path{test_path("jwt_reference_algorithms.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  // Same issuer, audience, and key set but a different algorithm is a different
  // scope, so no token could satisfy the reference
  EXPECT_FALSE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  // An identical policy is the same scope
  EXPECT_TRUE(gate.reference_permitted(at("/alpha/one"), at("/gamma/two")));
}

TEST(reference_between_jwt_scopes_ignores_algorithm_order) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 2> one_order{
      {sourcemeta::core::JWSAlgorithm::ES256,
       sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::core::JWSAlgorithm, 2> other_order{
      {sourcemeta::core::JWSAlgorithm::RS256,
       sourcemeta::core::JWSAlgorithm::ES256}};
  // The allow-list decides admission by membership, so these two admit exactly
  // the same tokens and must be the same scope
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = one_order}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = other_order}}}};
  const auto path{test_path("jwt_reference_algorithm_order.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_TRUE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_between_jwt_scopes_ignores_token_type_spelling) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // A media type is matched case-insensitively and with the `application/`
  // prefix optional, so these two admit exactly the same tokens
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa,
                .token_type = "at+jwt"}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa,
            .token_type = "Application/AT+JWT"}}}};
  const auto path{test_path("jwt_reference_token_type_spelling.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_TRUE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_between_jwt_scopes_ignores_repeated_algorithms) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 3> repeated{
      {sourcemeta::core::JWSAlgorithm::RS256,
       sourcemeta::core::JWSAlgorithm::ES256,
       sourcemeta::core::JWSAlgorithm::RS256}};
  const std::array<sourcemeta::core::JWSAlgorithm, 2> once{
      {sourcemeta::core::JWSAlgorithm::RS256,
       sourcemeta::core::JWSAlgorithm::ES256}};
  // Naming an algorithm twice admits nothing a single mention does not
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = repeated}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = once}}}};
  const auto path{test_path("jwt_reference_repeated_algorithms.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_TRUE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_TRUE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(a_token_type_that_is_the_bare_media_prefix_still_names_a_type) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // Reducing the prefix on its own would leave nothing, and a policy naming no
  // type accepts every type, so a policy that names one must never become one
  // that does not
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa,
                .token_type = "application/"}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto path{test_path("jwt_token_type_bare_prefix.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(a_token_type_carrying_a_further_separator_keeps_its_prefix) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // What follows the prefix is only a subtype when it carries no separator of
  // its own, so these two name different types and are read as such
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa,
                .token_type = "application/one/two"}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa,
            .token_type = "one/two"}}}};
  const auto path{test_path("jwt_token_type_nested_separator.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_across_swapped_jwt_identities_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // One policy's issuer is the other's audience and vice versa, so the scopes
  // share both strings yet no token satisfies them both
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://login.test",
                .audience = "registry",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "registry",
            .audience = "https://login.test",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto path{test_path("jwt_reference_swapped.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

TEST(reference_mixing_identities_across_jwt_policies_is_rejected) {
  const std::array<std::string_view, 1> source_paths{{"/source"}};
  const std::array<std::string_view, 1> target_paths{{"/target"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // The referrer pairs an issuer and an audience that the referent only
  // carries through two different policies, so no single referent scope
  // matches and the reference must not slip through their union
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = source_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://alpha.test",
                .audience = "dashboard",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa}},
       {.paths = target_paths,
        .name = "policy-1",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://alpha.test",
                .audience = "registry",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = rsa}},
       {.paths = target_paths,
        .name = "policy-2",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://beta.test",
            .audience = "dashboard",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto path{test_path("jwt_reference_mixed.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/source/one"), at("/target/two")));
}

TEST(reference_across_swapped_jwt_key_set_locations_is_rejected) {
  const std::array<std::string_view, 1> alpha_paths{{"/alpha"}};
  const std::array<std::string_view, 1> beta_paths{{"/beta"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> rsa{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  // The key set location decides which keys sign an admitted token, so
  // trading it with the audience denotes a different scope as surely as
  // trading the issuer does
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = alpha_paths,
        .name = "policy-0",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "https://idp.test/jwks",
                .jwks_uri = "registry",
                .algorithms = rsa}},
       {.paths = beta_paths,
        .name = "policy-1",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "registry",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = rsa}}}};
  const auto path{test_path("jwt_reference_swapped_keys.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_FALSE(gate.reference_permitted(at("/alpha/one"), at("/beta/two")));
  EXPECT_FALSE(gate.reference_permitted(at("/beta/two"), at("/alpha/one")));
}

// A configured policy path that only differs cosmetically still has to gate the
// location it names. A spelling the matcher could not traverse would leave the
// target public while the configuration reads as though it were gated

TEST(a_policy_path_declared_canonically_gates_its_location) {
  setenv("ONE_TEST_KEY_CANONICAL", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_CANONICAL"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("policy_declared_canonically.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/private/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      at("/private/secret"),
      authentication.caller({.bearer = "spelling-secret"})));
  // A location the policy does not name stays public
  EXPECT_TRUE(authentication.permits(at("/public/string"),
                                     authentication.caller({.bearer = ""})));
}

TEST(a_policy_path_carrying_a_dot_segment_gates_its_location) {
  setenv("ONE_TEST_KEY_DOT", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"/./private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_DOT"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("policy_carrying_a_dot_segment.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/private/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      at("/private/secret"),
      authentication.caller({.bearer = "spelling-secret"})));
  // A location the policy does not name stays public
  EXPECT_TRUE(authentication.permits(at("/public/string"),
                                     authentication.caller({.bearer = ""})));
}

TEST(a_policy_path_that_climbs_back_into_itself_gates_its_location) {
  setenv("ONE_TEST_KEY_CLIMB", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"/private/../private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_CLIMB"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("policy_that_climbs_back_into_itself.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/private/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      at("/private/secret"),
      authentication.caller({.bearer = "spelling-secret"})));
  // A location the policy does not name stays public
  EXPECT_TRUE(authentication.permits(at("/public/string"),
                                     authentication.caller({.bearer = ""})));
}

TEST(a_policy_path_carrying_a_repeated_separator_gates_its_location) {
  setenv("ONE_TEST_KEY_SEPARATOR", "spelling-secret", 1);
  const std::array<std::string_view, 1> paths{{"//private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_SEPARATOR"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "policy",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("policy_carrying_a_repeated_separator.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/private/secret"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.permits(
      at("/private/secret"),
      authentication.caller({.bearer = "spelling-secret"})));
  // A location the policy does not name stays public
  EXPECT_TRUE(authentication.permits(at("/public/string"),
                                     authentication.caller({.bearer = ""})));
}

TEST(views_of_nothing_are_the_public_one_alone) {
  const auto views{sourcemeta::one::Authentication::Table::enumerate({})};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}}}));
}

TEST(views_name_a_static_key_policy_on_its_own) {
  const std::array<std::string_view, 1> paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "vault",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};

  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "vault", .policies = {0}}}));
}

TEST(views_name_an_interactive_policy_on_its_own) {
  const std::array<std::string_view, 1> paths{{"/console"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "desk",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "desk", .policies = {0}}}));
}

TEST(views_name_a_token_policy_on_its_own) {
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "machine", .policies = {0}}}));
}

TEST(views_combine_token_policies_that_name_one_issuer) {
  const std::array<std::string_view, 1> legal_paths{{"/legal"}};
  const std::array<std::string_view, 1> tech_paths{{"/tech"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = legal_paths,
        .name = "legal",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = tech_paths,
        .name = "tech",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  // A claim is a list and a rule is met by any of its values, so one token can
  // satisfy both
  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "legal", .policies = {0}},
                       {.name = "legal+tech", .policies = {0, 1}},
                       {.name = "tech", .policies = {1}}}));
}

TEST(views_never_combine_token_policies_across_issuers) {
  const std::array<std::string_view, 1> staff_paths{{"/internal"}};
  const std::array<std::string_view, 1> partner_paths{{"/partners"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = staff_paths,
        .name = "staff",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = partner_paths,
        .name = "partner",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.partner.com/realms/main"}}}};

  // A token carries one issuer and is verified against it before any rule is
  // read, so nobody can ever hold both
  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "partner", .policies = {1}},
                       {.name = "staff", .policies = {0}}}));
}

TEST(views_never_combine_token_policies_testing_one_claim_across_issuers) {
  const std::array<std::string_view, 1> legal_paths{{"/legal"}};
  const std::array<std::string_view, 1> partner_paths{{"/partners"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = legal_paths,
        .name = "legal",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff",
                .claims = R"({"department":{"values":["legal"]}})"}},
       {.paths = partner_paths,
        .name = "partner-legal",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.partner.com/realms/main",
            .claims = R"({"department":{"values":["legal"]}})"}}}};

  // The issuer is decisive and is read first, so testing the same claim for the
  // same value means nothing across them
  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "legal", .policies = {0}},
                       {.name = "partner-legal", .policies = {1}}}));
}

TEST(views_never_combine_interactive_policies_under_one_issuer) {
  const std::array<std::string_view, 1> first_paths{{"/one"}};
  const std::array<std::string_view, 1> second_paths{{"/two"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = first_paths,
        .name = "first",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = second_paths,
        .name = "second",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  // A browser holds one session naming one policy, so sharing an issuer buys an
  // interactive caller nothing
  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "first", .policies = {0}},
                       {.name = "second", .policies = {1}}}));
}

TEST(views_never_combine_static_key_policies) {
  const std::array<std::string_view, 1> first_paths{{"/one"}};
  const std::array<std::string_view, 1> second_paths{{"/two"}};
  const std::array<std::string_view, 1> first_keys{{"one-secret"}};
  const std::array<std::string_view, 1> second_keys{{"two-secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = first_paths,
        .name = "first",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                first_keys}},
       {.paths = second_paths,
        .name = "second",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = second_keys}}}};

  // A caller presents one key, so it satisfies one of these whatever it holds
  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "first", .policies = {0}},
                       {.name = "second", .policies = {1}}}));
}

TEST(views_spell_a_combination_the_same_however_it_was_declared) {
  const std::array<std::string_view, 1> tech_paths{{"/tech"}};
  const std::array<std::string_view, 1> legal_paths{{"/legal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = tech_paths,
        .name = "tech",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = legal_paths,
        .name = "legal",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "legal", .policies = {1}},
                       {.name = "legal+tech", .policies = {0, 1}},
                       {.name = "tech", .policies = {0}}}));
}

TEST(views_of_three_token_policies_under_one_issuer_are_every_combination) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = paths,
        .name = "a",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = paths,
        .name = "b",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = paths,
        .name = "c",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "a", .policies = {0}},
                       {.name = "a+b", .policies = {0, 1}},
                       {.name = "a+b+c", .policies = {0, 1, 2}},
                       {.name = "a+c", .policies = {0, 2}},
                       {.name = "b", .policies = {1}},
                       {.name = "b+c", .policies = {1, 2}},
                       {.name = "c", .policies = {2}}}));
}

TEST(views_mix_a_combining_group_with_policies_that_stand_alone) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 3> policies{
      {{.paths = paths,
        .name = "vault",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}},
       {.paths = paths,
        .name = "legal",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "https://idp.example.com/realms/staff"}},
       {.paths = paths,
        .name = "tech",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"}}}};

  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "legal", .policies = {1}},
                       {.name = "legal+tech", .policies = {1, 2}},
                       {.name = "tech", .policies = {2}},
                       {.name = "vault", .policies = {0}}}));
}

TEST(views_hold_the_public_one_even_when_every_path_is_governed) {
  const std::array<std::string_view, 1> paths{{"/"}};
  const std::array<std::string_view, 1> keys{{"secret"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "everything",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};

  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views, (std::vector<sourcemeta::one::Authentication::View>{
                       {.name = "public", .policies = {}},
                       {.name = "everything", .policies = {0}}}));
}

TEST(views_of_six_token_policies_under_one_issuer_are_every_combination) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, 6> policies{};
  const std::array<std::string_view, 6> names{{"a", "b", "c", "d", "e", "f"}};
  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).name = names.at(index);
    policies.at(index).credential =
        sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"};
  }

  // Two to the sixth, being every non-empty combination plus the anonymous one
  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views.size(), 64);
}

TEST(views_of_two_issuer_groups_are_a_sum_rather_than_a_product) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, 8> policies{};
  const std::array<std::string_view, 8> names{
      {"a", "b", "c", "d", "e", "f", "g", "h"}};
  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).name = names.at(index);
    policies.at(index).credential =
        sourcemeta::one::Authentication::Policy::Token{
            .issuer = index < 4 ? "https://idp.example.com/realms/staff"
                                : "https://idp.partner.com/realms/main"};
  }

  // Each group contributes its own combinations and nothing crosses between
  // them, so the total adds rather than multiplies
  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views.size(), 1 + 15 + 15);
}

TEST(views_of_the_largest_combinable_group_are_every_combination) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<
      sourcemeta::one::Authentication::Policy,
      sourcemeta::one::Authentication::Table::MAXIMUM_COMBINABLE_POLICIES>
      policies{};
  std::vector<std::string> names;
  names.reserve(policies.size());
  for (std::size_t index{0}; index < policies.size(); index++) {
    names.push_back("p" + std::to_string(index));
  }

  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).name = names.at(index);
    policies.at(index).credential =
        sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"};
  }

  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(
      views.size(),
      (std::size_t{1}
       << sourcemeta::one::Authentication::Table::MAXIMUM_COMBINABLE_POLICIES));
  EXPECT_EQ(views.at(0).name, "public");
}

TEST(views_refuse_a_group_whose_combinations_cannot_be_produced) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<
      sourcemeta::one::Authentication::Policy,
      sourcemeta::one::Authentication::Table::MAXIMUM_COMBINABLE_POLICIES + 1>
      policies{};
  std::vector<std::string> names;
  names.reserve(policies.size());
  for (std::size_t index{0}; index < policies.size(); index++) {
    names.push_back("p" + std::to_string(index));
  }

  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).name = names.at(index);
    policies.at(index).credential =
        sourcemeta::one::Authentication::Policy::Token{
            .issuer = "https://idp.example.com/realms/staff"};
  }

  try {
    const auto views{
        sourcemeta::one::Authentication::Table::enumerate(policies)};
    EXPECT_EQ(views.size(), 0);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationTooManyViewsError &error) {
    EXPECT_STREQ(error.what(),
                 "Too many authentication policies share an issuer");
    EXPECT_EQ(error.issuer(), "https://idp.example.com/realms/staff");
    EXPECT_EQ(
        error.count(),
        sourcemeta::one::Authentication::Table::MAXIMUM_COMBINABLE_POLICIES +
            1);
  }
}

TEST(views_of_many_policies_across_issuers_are_never_refused) {
  const std::array<std::string_view, 1> paths{{"/one"}};
  std::array<sourcemeta::one::Authentication::Policy, 40> policies{};
  std::vector<std::string> names;
  names.reserve(policies.size());
  for (std::size_t index{0}; index < policies.size(); index++) {
    names.push_back("p" + std::to_string(index));
  }

  // Well past the ceiling in total, and no group approaches it, so nothing is
  // refused. What a build makes of forty views is not decided here
  for (std::size_t index{0}; index < policies.size(); index++) {
    policies.at(index).paths = paths;
    policies.at(index).name = names.at(index);
    policies.at(index).credential =
        sourcemeta::one::Authentication::Policy::Token{.issuer =
                                                           names.at(index)};
  }

  const auto views{sourcemeta::one::Authentication::Table::enumerate(policies)};
  EXPECT_EQ(views.size(), 41);
}

TEST(a_presented_key_decides_over_a_session) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_PRECEDENCE_SECRET", "confidential", 1);
  setenv("ONE_TEST_PRECEDENCE_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> portal_paths{{"/portal"}};
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{
      {"ONE_TEST_PRECEDENCE_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = portal_paths,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_PRECEDENCE_SECRET",
                .session_secrets = SESSION_SECRETS}},
       {.paths = machine_paths,
        .name = "machine",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const auto path{test_path("precedence.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto sealed{session_for("okta", SESSION_SECRETS, "jane@acme.test")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  // Each alone opens what it governs, which is what makes the pair below a
  // choice between two live credentials rather than one working answer
  EXPECT_TRUE(authentication.permits(
      at("/portal/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));
  EXPECT_TRUE(authentication.permits(
      at("/machine/x"), authentication.caller({.bearer = "machine-secret"})));

  // Presented together, the request is read as the key it carried, so the
  // portal the session would have opened is refused
  EXPECT_FALSE(authentication.permits(
      at("/portal/x"), authentication.caller({.bearer = "machine-secret",
                                              .cookies = fields(cookies)})));
  const auto permitted{authentication.permits(
      at("/machine/x"), authentication.caller({.bearer = "machine-secret",
                                               .cookies = fields(cookies)}))};
  EXPECT_TRUE(permitted);
}

TEST(a_presented_key_that_opens_nothing_sets_a_session_aside) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_FALLBACK_SECRET", "confidential", 1);
  setenv("ONE_TEST_FALLBACK_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> portal_paths{{"/portal"}};
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{{"ONE_TEST_FALLBACK_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = portal_paths,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_FALLBACK_SECRET",
                .session_secrets = SESSION_SECRETS}},
       {.paths = machine_paths,
        .name = "machine",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const auto path{test_path("precedence_stale.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto sealed{session_for("okta", SESSION_SECRETS, "jane@acme.test")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  // A key that opens nothing is still a key that was presented, so the session
  // is set aside and nothing admits. The cost of the rule, and the reason it is
  // worth stating rather than leaving to be discovered
  EXPECT_FALSE(authentication.permits(
      at("/portal/x"), authentication.caller({.bearer = "retired-secret",
                                              .cookies = fields(cookies)})));

  // The same session presented on its own still opens it, so what changed is
  // what the request carried rather than whether the session is any good
  EXPECT_TRUE(authentication.permits(
      at("/portal/x"),
      authentication.caller({.bearer = "", .cookies = fields(cookies)})));
}

TEST(a_caller_presenting_nothing_is_served_the_anonymous_view) {
  setenv("ONE_TEST_VIEW_ANONYMOUS_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_VIEW_ANONYMOUS_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("view_anonymous.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(), "public");
  EXPECT_EQ(authentication.caller({.bearer = "retired-secret"}).view(),
            "public");
  EXPECT_EQ(authentication.caller({.bearer = "machine-secret"}).view(),
            "machine");
}

TEST(a_token_satisfying_two_policies_is_served_their_combined_view) {
  const std::array<std::string_view, 1> platform_paths{{"/platform"}};
  const std::array<std::string_view, 1> oncall_paths{{"/oncall"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = platform_paths,
        .name = "platform",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms,
                .claims = CLAIMS_ONE_GROUP}},
       {.paths = oncall_paths,
        .name = "oncall",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_ONCALL_GROUP}}}};
  const auto path{test_path("view_token_groups.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_EQ(authentication
                .caller({.bearer = token_with(
                             R"JSON({ "groups": [ "platform" ] })JSON")})
                .view(),
            "platform");
  EXPECT_EQ(authentication
                .caller({.bearer = token_with(
                             R"JSON({ "groups": [ "oncall" ] })JSON")})
                .view(),
            "oncall");
  // Spelled from the policies sorted rather than in the order they were
  // declared, so one combination has one name wherever it is reached from
  EXPECT_EQ(
      authentication
          .caller({.bearer = token_with(
                       R"JSON({ "groups": [ "oncall", "platform" ] })JSON")})
          .view(),
      "oncall+platform");
  EXPECT_EQ(authentication
                .caller({.bearer = token_with(
                             R"JSON({ "groups": [ "support" ] })JSON")})
                .view(),
            "public");
}

TEST(the_recorded_table_names_every_view_it_serves) {
  const std::array<std::string_view, 1> platform_paths{{"/platform"}};
  const std::array<std::string_view, 1> oncall_paths{{"/oncall"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = platform_paths,
        .name = "platform",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms}},
       {.paths = oncall_paths,
        .name = "oncall",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("recorded_table.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  // The anonymous view first, then the rest by name, which is the order a build
  // fans its actions out in
  const auto table{gate.views()};
  EXPECT_EQ(table.size(), std::size_t{4});
  EXPECT_EQ(table.at(0).name(), "public");
  EXPECT_EQ(table.at(0).policies(),
            sourcemeta::one::Authentication::PolicySet{0});
  EXPECT_EQ(table.at(1).name(), "oncall");
  EXPECT_EQ(table.at(1).policies(),
            sourcemeta::one::Authentication::PolicySet{0b10});
  EXPECT_EQ(table.at(2).name(), "oncall+platform");
  EXPECT_EQ(table.at(2).policies(),
            sourcemeta::one::Authentication::PolicySet{0b11});
  EXPECT_EQ(table.at(3).name(), "platform");
  EXPECT_EQ(table.at(3).policies(),
            sourcemeta::one::Authentication::PolicySet{0b01});
}

TEST(a_view_shows_what_it_governs_and_whatever_nobody_governs) {
  const std::array<std::string_view, 1> platform_paths{{"/platform"}};
  const std::array<std::string_view, 1> oncall_paths{{"/oncall"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = platform_paths,
        .name = "platform",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms}},
       {.paths = oncall_paths,
        .name = "oncall",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms}}}};
  const auto path{test_path("view_visibility.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  const auto anonymous{gate.view("public")};
  const auto oncall{gate.view("oncall")};
  const auto both{gate.view("oncall+platform")};
  const auto platform{gate.view("platform")};

  // What nobody governs is shown to everybody, the anonymous view included
  EXPECT_TRUE(gate.visible(at("/open/x"), anonymous));
  EXPECT_TRUE(gate.visible(at("/open/x"), oncall));
  EXPECT_TRUE(gate.visible(at("/open/x"), both));
  EXPECT_TRUE(gate.visible(at("/open/x"), platform));

  // And a governed location only to a view satisfying something governing it
  EXPECT_FALSE(gate.visible(at("/platform/x"), anonymous));
  EXPECT_FALSE(gate.visible(at("/platform/x"), oncall));
  EXPECT_TRUE(gate.visible(at("/platform/x"), both));
  EXPECT_TRUE(gate.visible(at("/platform/x"), platform));
  EXPECT_FALSE(gate.visible(at("/oncall/x"), anonymous));
  EXPECT_TRUE(gate.visible(at("/oncall/x"), oncall));
  EXPECT_TRUE(gate.visible(at("/oncall/x"), both));
  EXPECT_FALSE(gate.visible(at("/oncall/x"), platform));
}

TEST(an_instance_that_read_no_artifact_shows_nothing) {
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          std::filesystem::path{"/no/such/authentication.bin"}},
      stub_fetcher({}, nullptr)};
  // It admits nobody, so it must not answer that everything is public either,
  // which is what knowing nothing about who governs what would otherwise mean
  EXPECT_FALSE(authentication.permits(at("/anywhere"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.table().views().empty());
  EXPECT_FALSE(authentication.table().visible(
      at("/anywhere"), authentication.table().view("public")));
  EXPECT_FALSE(authentication.table().visible(
      at("/"), authentication.table().view("public")));
}

TEST(a_corrupt_artifact_shows_nothing) {
  const auto path{test_path("visible_corrupt.bin")};
  std::ofstream stream{path, std::ios::binary};
  const std::array<char, 4> garbage{{'N', 'O', 'P', 'E'}};
  stream.write(garbage.data(), garbage.size());
  stream.close();

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_FALSE(authentication.permits(at("/anywhere"),
                                      authentication.caller({.bearer = ""})));
  EXPECT_TRUE(authentication.table().views().empty());
  EXPECT_FALSE(authentication.table().visible(
      at("/anywhere"), authentication.table().view("public")));
}

TEST(a_name_no_view_holds_is_served_as_anonymous) {
  setenv("ONE_TEST_KEY_VIEW_INDEX", "index-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_KEY_VIEW_INDEX"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("visible_index.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication::Table gate{path};
  EXPECT_EQ(gate.views().size(), std::size_t{2});
  // A location nobody governs is shown under either view this declares
  EXPECT_TRUE(gate.visible(at("/open/x"), gate.view("public")));
  EXPECT_TRUE(gate.visible(at("/open/x"), gate.view("machine")));

  // A build stamps a view's name on the actions it fans out, so a policy
  // withdrawn since leaves a name this no longer holds. It is served what a
  // caller holding nothing is served, rather than what the name used to reach
  const auto withdrawn{gate.view("retired")};
  EXPECT_EQ(withdrawn.name(), "public");
  EXPECT_TRUE(gate.visible(at("/open/x"), withdrawn));
  EXPECT_FALSE(gate.visible(at("/machine/x"), withdrawn));
}

TEST(a_caller_presenting_nothing_belongs_to_no_policy) {
  setenv("ONE_TEST_CLASSIFY_ANONYMOUS_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{
      {"ONE_TEST_CLASSIFY_ANONYMOUS_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("classify_anonymous.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(),
            sourcemeta::one::VIEW_PUBLIC);
}

TEST(a_credential_opening_nothing_belongs_to_no_policy) {
  setenv("ONE_TEST_CLASSIFY_UNKNOWN_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> paths{{"/machine"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_CLASSIFY_UNKNOWN_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "machine",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("classify_unknown.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = "retired-secret"}).view(),
            sourcemeta::one::VIEW_PUBLIC);
}

TEST(a_key_places_its_caller_in_the_policy_it_opens) {
  setenv("ONE_TEST_CLASSIFY_FIRST_KEY", "first-secret", 1);
  setenv("ONE_TEST_CLASSIFY_SECOND_KEY", "second-secret", 1);
  const std::array<std::string_view, 1> first_paths{{"/first"}};
  const std::array<std::string_view, 1> second_paths{{"/second"}};
  const std::array<std::string_view, 1> first_keys{
      {"ONE_TEST_CLASSIFY_FIRST_KEY"}};
  const std::array<std::string_view, 1> second_keys{
      {"ONE_TEST_CLASSIFY_SECOND_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = first_paths,
        .name = "first",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                first_keys}},
       {.paths = second_paths,
        .name = "second",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = second_keys}}}};
  const auto path{test_path("classify_key.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  EXPECT_EQ(authentication.caller({.bearer = "first-secret"}).view(), "first");
  EXPECT_EQ(authentication.caller({.bearer = "second-secret"}).view(),
            "second");
}

TEST(a_key_is_placed_without_reference_to_any_path) {
  setenv("ONE_TEST_CLASSIFY_DEEP_KEY", "deep-secret", 1);
  const std::array<std::string_view, 1> paths{{"/deep/inside/somewhere"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_CLASSIFY_DEEP_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "deep",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};
  const auto path{test_path("classify_deep.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // Admitted under the policy where it governs, so the gate has read the key
  const auto governed{
      authentication.permits(at("/deep/inside/somewhere/x"),
                             authentication.caller({.bearer = "deep-secret"}))};
  EXPECT_TRUE(governed);

  // Admitted anonymously where no policy governs, which is a different answer
  // reached without reading the key at all
  const auto ungoverned{authentication.permits(
      at("/elsewhere"), authentication.caller({.bearer = "deep-secret"}))};
  EXPECT_TRUE(ungoverned);

  // The same answer for a caller carrying nothing, so being admitted there says
  // nothing about who is asking
  const auto anonymous{authentication.permits(
      at("/elsewhere"), authentication.caller({.bearer = ""}))};
  EXPECT_TRUE(anonymous);

  // The placement answers once for the caller, naming the policy the key opens
  // wherever they happen to be asking from
  EXPECT_EQ(authentication.caller({.bearer = "deep-secret"}).view(), "deep");
  EXPECT_EQ(authentication.caller({.bearer = ""}).view(),
            sourcemeta::one::VIEW_PUBLIC);
}

TEST(a_key_opening_two_policies_reaches_only_the_first_declared) {
  setenv("ONE_TEST_CLASSIFY_SHARED_EARLY", "shared-secret", 1);
  setenv("ONE_TEST_CLASSIFY_SHARED_LATE", "shared-secret", 1);
  const std::array<std::string_view, 1> early_paths{{"/early"}};
  const std::array<std::string_view, 1> late_paths{{"/late"}};
  const std::array<std::string_view, 1> early_keys{
      {"ONE_TEST_CLASSIFY_SHARED_EARLY"}};
  const std::array<std::string_view, 1> late_keys{
      {"ONE_TEST_CLASSIFY_SHARED_LATE"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = early_paths,
        .name = "early",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys =
                                                                early_keys}},
       {.paths = late_paths,
        .name = "late",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = late_keys}}}};
  const auto path{test_path("classify_shared.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  // Two variables holding one value is the form the configuration cannot see,
  // and it is the only shape where being admitted and being shown could ever
  // have parted. A caller is read as the first policy their key opens, and what
  // they reach is what that placement holds, so the second policy's area is not
  // theirs. It never was in any useful sense: the answer served there would
  // have been read from a view that does not hold it
  const auto early{authentication.permits(
      at("/early/x"), authentication.caller({.bearer = "shared-secret"}))};
  EXPECT_TRUE(early);
  const auto late{authentication.permits(
      at("/late/x"), authentication.caller({.bearer = "shared-secret"}))};
  EXPECT_FALSE(late);

  // What is reached and where it is read from are one answer, which is what
  // naming the placement here pins
  EXPECT_EQ(authentication.caller({.bearer = "shared-secret"}).view(), "early");
}

TEST(a_token_belongs_to_every_policy_of_its_issuer_that_it_satisfies) {
  const std::array<std::string_view, 1> platform_paths{{"/platform"}};
  const std::array<std::string_view, 1> oncall_paths{{"/oncall"}};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> algorithms{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = platform_paths,
        .name = "platform",
        .credential =
            sourcemeta::one::Authentication::Policy::Token{
                .issuer = "acme",
                .audience = "client",
                .jwks_uri = "https://idp.test/jwks",
                .algorithms = algorithms,
                .claims = CLAIMS_ONE_GROUP}},
       {.paths = oncall_paths,
        .name = "oncall",
        .credential = sourcemeta::one::Authentication::Policy::Token{
            .issuer = "acme",
            .audience = "client",
            .jwks_uri = "https://idp.test/jwks",
            .algorithms = algorithms,
            .claims = CLAIMS_ONCALL_GROUP}}}};
  const auto path{test_path("classify_token_groups.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path},
      stub_fetcher({{"https://idp.test/jwks", std::string{CLAIMS_KEYS}}},
                   nullptr)};
  EXPECT_EQ(authentication
                .caller({.bearer = token_with(
                             R"JSON({ "groups": [ "platform" ] })JSON")})
                .view(),
            "platform");
  EXPECT_EQ(authentication
                .caller({.bearer = token_with(
                             R"JSON({ "groups": [ "oncall" ] })JSON")})
                .view(),
            "oncall");
  // One token carrying both reaches both areas, so a placement naming either
  // alone would hide one of them
  EXPECT_EQ(
      authentication
          .caller({.bearer = token_with(
                       R"JSON({ "groups": [ "oncall", "platform" ] })JSON")})
          .view(),
      "oncall+platform");
  EXPECT_EQ(authentication
                .caller({.bearer = token_with(
                             R"JSON({ "groups": [ "support" ] })JSON")})
                .view(),
            sourcemeta::one::VIEW_PUBLIC);
}

TEST(a_session_places_its_caller_in_the_policy_that_established_it) {
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  setenv("ONE_TEST_CLASSIFY_SESSION_SECRET", "confidential", 1);
  setenv("ONE_TEST_CLASSIFY_SESSION_KEY", "machine-secret", 1);
  const std::array<std::string_view, 1> portal_paths{{"/portal"}};
  const std::array<std::string_view, 1> machine_paths{{"/machine"}};
  const std::array<std::string_view, 1> machine_keys{
      {"ONE_TEST_CLASSIFY_SESSION_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 2> policies{
      {{.paths = portal_paths,
        .name = "okta",
        .credential =
            sourcemeta::one::Authentication::Policy::Interactive{
                .issuer = "acme",
                .client_id = "client",
                .client_secret_variable = "ONE_TEST_CLASSIFY_SESSION_SECRET",
                .session_secrets = SESSION_SECRETS}},
       {.paths = machine_paths,
        .name = "machine",
        .credential = sourcemeta::one::Authentication::Policy::ApiKey{
            .keys = machine_keys}}}};
  const auto path{test_path("classify_session.bin")};
  save(policies, path, path, anywhere);

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{path}, stub_fetcher({}, nullptr)};
  const auto sealed{session_for("okta", SESSION_SECRETS, "jane@acme.test")};
  const std::string cookies{"sourcemeta_one_session=" + sealed};

  EXPECT_EQ(
      authentication.caller({.bearer = "", .cookies = fields(cookies)}).view(),
      "okta");
  EXPECT_EQ(authentication.caller({.bearer = "machine-secret"}).view(),
            "machine");
  // A request carrying a key is read as that key, so the session it also
  // carried places nobody, exactly as it admits nobody
  EXPECT_EQ(
      authentication
          .caller({.bearer = "machine-secret", .cookies = fields(cookies)})
          .view(),
      "machine");
  EXPECT_EQ(
      authentication
          .caller({.bearer = "retired-secret", .cookies = fields(cookies)})
          .view(),
      sourcemeta::one::VIEW_PUBLIC);
}

// A table compiled in this process reads every section from the bytes it holds
// rather than from a mapping it does not have, so it answers a credential the
// same way one read back from a file does
TEST(a_compiled_table_reads_a_policy_without_a_mapping) {
  setenv("ONE_TEST_COMPILED_KEY", "compiled-secret", 1);
  const std::array<std::string_view, 1> paths{{"/private"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_COMPILED_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "guard",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("compiled_only"), anywhere)},
      stub_fetcher({}, nullptr)};

  EXPECT_TRUE(authentication.permits(at("/open"), authentication.caller({})));
  EXPECT_FALSE(
      authentication.permits(at("/private"), authentication.caller({})));
  EXPECT_TRUE(authentication.permits(
      at("/private"), authentication.caller({.bearer = "compiled-secret"})));
  EXPECT_FALSE(authentication.permits(
      at("/private"), authentication.caller({.bearer = "wrong-secret"})));
  EXPECT_EQ(authentication.caller({.bearer = "compiled-secret"}).view(),
            "guard");
}

// A route nobody governs is reached by anybody, so an audience requirement on
// it narrows nothing. Refusing a caller there for presenting a token issued
// elsewhere would refuse them for holding a credential they did not need, at a
// route they could have reached by presenting nothing at all
TEST(an_ungoverned_route_ignores_the_audience_it_requires) {
  setenv("ONE_TEST_UNGOVERNED_ROUTE_KEY", "elsewhere-secret", 1);
  const std::array<std::string_view, 1> paths{{"/elsewhere"}};
  const std::array<std::string_view, 1> keys{{"ONE_TEST_UNGOVERNED_ROUTE_KEY"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "elsewhere",
        .credential =
            sourcemeta::one::Authentication::Policy::ApiKey{.keys = keys}}}};

  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("ungoverned_route"), anywhere)},
      stub_fetcher({}, nullptr)};

  const sourcemeta::one::Authentication::RouteTarget target{"/self/v1/mcp"};

  // Nobody governs it, so it opens whatever the caller brought
  EXPECT_TRUE(authentication.permits(target, authentication.caller({}),
                                     "https://example.com/self/v1/mcp"));
  EXPECT_TRUE(authentication.permits(
      target, authentication.caller({.bearer = "elsewhere-secret"}),
      "https://example.com/self/v1/mcp"));

  // And the same route without a requirement answers identically, which is
  // what shows the requirement is what did nothing rather than the route
  EXPECT_TRUE(authentication.permits(target, authentication.caller({})));

  // A governed path still answers to who is asking
  EXPECT_FALSE(
      authentication.permits(at("/elsewhere/x"), authentication.caller({})));
  EXPECT_TRUE(authentication.permits(
      at("/elsewhere/x"),
      authentication.caller({.bearer = "elsewhere-secret"})));
}

// The other direction of the same separation. A session is what somebody holds
// once they are signed in, and a transaction is what anybody may obtain without
// holding anything, so reading either as the other is what the purposes exist
// to stop. Both are sealed under one policy secret, so nothing but the purpose
// tells them apart
TEST(a_session_never_opens_as_a_transaction) {
  setenv("ONE_TEST_PURPOSE_BACK", "confidential", 1);
  setenv(SESSION_SECRET_VARIABLE, "session-secret", 1);
  Provider provider;
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = provider.issuer,
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_PURPOSE_BACK",
            .session_secrets = SESSION_SECRETS}}}};
  const sourcemeta::one::Authentication authentication{
      sourcemeta::one::Authentication::Table{
          sourcemeta::one::Authentication::Table::compile(
              policies, test_path("purpose_back"), anywhere)},
      provider.fetcher()};

  // A session this instance established, obtained the way anybody obtains one
  const auto established{session_for("okta", SESSION_SECRETS, "jane")};

  // Presented where a callback looks for the transaction it is completing. It
  // cannot open there, so the callback is refused before the provider is
  // consulted at all
  const auto carried{"sourcemeta_one_transaction=" + established};
  const std::array<std::string_view, 1> presented{{carried}};
  const auto outcome{authentication.callback(
      "okta", "https://registry.test", "https://registry.test/callback",
      {.state = "any-state", .code = "a-code"}, {.cookies = presented})};
  EXPECT_EQ(outcome.result,
            sourcemeta::one::Authentication::Outcome::Result::Invalid);

  // And the control: the same value read as what it is does open, so the
  // refusal above came from the purpose rather than from the value
  const auto as_a_session{"sourcemeta_one_session=" + established};
  EXPECT_TRUE(authentication.permits(
      at("/portal/x"),
      authentication.caller({.cookies = fields(as_a_session)})));
}

// A ceiling and a missing secret are refusals a caller earns the same way, and
// both used to answer with an error naming no policy at all
TEST(save_rejects_more_policies_than_a_set_can_name) {
  std::vector<std::vector<std::string_view>> paths;
  std::vector<std::vector<std::string_view>> keys;
  std::vector<std::string> names;
  constexpr auto total{sourcemeta::one::Authentication::MAXIMUM_POLICIES + 1};
  for (std::size_t index{0}; index < total; index += 1) {
    names.push_back("policy-" + std::to_string(index));
    paths.push_back({"/scope"});
    keys.push_back({"ONE_TEST_KEY_CEILING"});
  }

  std::vector<sourcemeta::one::Authentication::Policy> policies;
  for (std::size_t index{0}; index < total; index += 1) {
    policies.push_back(
        {.paths = paths[index],
         .name = names[index],
         .credential = sourcemeta::one::Authentication::Policy::ApiKey{
             .keys = keys[index]}});
  }

  const auto path{test_path("ceiling.bin")};
  try {
    save(policies, path, path, anywhere);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationTooManyPoliciesError &error) {
    EXPECT_STREQ(error.what(), "Too many authentication policies");
    EXPECT_EQ(error.count(), total);
  }
}

TEST(save_rejects_an_interactive_policy_without_a_session_secret) {
  const std::array<std::string_view, 1> paths{{"/portal"}};
  const std::array<sourcemeta::one::Authentication::Policy, 1> policies{
      {{.paths = paths,
        .name = "okta",
        .credential = sourcemeta::one::Authentication::Policy::Interactive{
            .issuer = "https://acme.test",
            .client_id = "client",
            .client_secret_variable = "ONE_TEST_OIDC_NO_SECRET"}}}};
  const auto path{test_path("no_session_secret.bin")};
  try {
    save(policies, path, path, anywhere);
    FAIL();
  } catch (const sourcemeta::one::AuthenticationMissingSecretError &error) {
    EXPECT_STREQ(error.what(),
                 "An interactive authentication policy requires a session "
                 "secret");
    EXPECT_EQ(error.name(), "okta");
  }
}
