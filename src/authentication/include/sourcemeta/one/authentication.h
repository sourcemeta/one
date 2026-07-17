#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <sourcemeta/one/authentication_discovery.h>
#include <sourcemeta/one/authentication_error.h>
#include <sourcemeta/one/authentication_oidc.h>
#include <sourcemeta/one/authentication_session.h>
#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/uritemplate.h>

#include <chrono>      // std::chrono::sys_seconds
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::unique_ptr
#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace sourcemeta::one {

// Everything a request presented for authentication: the bearer value from
// the authorization header and the raw request cookie header, either of
// which may admit the caller under a covering policy
struct Credentials {
  std::string_view bearer{};
  std::string_view cookies{};
};

class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Authentication {
public:
  static constexpr std::size_t MAXIMUM_POLICIES{64};

  // How a presented credential is compared against a policy's stored keys.
  // Identity stores the key verbatim, every other algorithm stores it hashed
  enum class Algorithm : std::uint8_t { Identity = 0, Sha256 = 1 };

  enum class Type : std::uint8_t { ApiKey = 0, JWT = 1, OIDC = 2 };

  // A policy gates a set of path prefixes. A path covered by no policy is
  // public
  struct Policy {
    std::span<const std::string_view> paths{};
    std::span<const std::string_view> keys{};
    Algorithm algorithm{Algorithm::Identity};
    Type type{Type::ApiKey};
    std::string_view issuer{};
    std::string_view audience{};
    std::string_view jwks_uri{};
    std::span<const sourcemeta::core::JWSAlgorithm> algorithms{};
    std::string_view client_id{};
    // The environment variable name holding the client secret
    std::string_view client_secret_variable{};
    // The policy name, which interactive policies carry so their session
    // cookies can be recognised at the gate
    std::string_view name{};
    // The environment variable name holding the secret that signs this
    // policy's session and transaction cookies
    std::string_view session_secret_variable{};
  };

  // The identity of an admitted caller: the type of credential it presented
  // and the declaration index of the policy that admitted it
  struct Principal {
    Type type{Type::ApiKey};
    std::size_t policy{0};
  };

  struct Verdict {
    bool allowed;
    // Present only when a policy admitted the caller. An anonymous caller on
    // a public path and a denied caller both carry none
    std::optional<Principal> principal;
  };

  static auto save(std::span<const Policy> policies,
                   const std::filesystem::path &configuration,
                   const std::filesystem::path &destination) -> void;

  static auto save(const Configuration &configuration,
                   const sourcemeta::core::URITemplateRouterView &routes,
                   const std::filesystem::path &destination) -> void;

  Authentication(const std::filesystem::path &path,
                 sourcemeta::core::JWKSProvider::Fetcher fetcher);

  ~Authentication();

  // The artifact is memory-mapped and owned for the lifetime of the view
  Authentication(const Authentication &) = delete;
  Authentication(Authentication &&) = delete;
  auto operator=(const Authentication &) -> Authentication & = delete;
  auto operator=(Authentication &&) -> Authentication & = delete;

  // A non-empty base path is stripped off the input before matching, turning a
  // request URL path into a registry path. The credential is a presented
  // bearer value and the cookies are the raw request cookie header, either of
  // which may admit the caller under a covering policy
  [[nodiscard]] auto admits(std::string_view registry_path,
                            std::string_view credential,
                            std::string_view cookies = {},
                            std::string_view base_path = {}) const -> Verdict;

  // The configuration declaration indices of the policies that govern a path,
  // sorted ascending
  [[nodiscard]] auto governing(std::string_view registry_path,
                               std::string_view base_path = {}) const
      -> std::vector<std::size_t>;

  // The names of the interactive policies that govern a path, in declaration
  // order. An unauthenticated browser that a policy would otherwise deny can be
  // sent to begin a login when exactly one such policy covers the path. The
  // views point into the artifact and outlive the call
  [[nodiscard]] auto
  interactive_challenges(std::string_view registry_path,
                         std::string_view base_path = {}) const
      -> std::vector<std::string_view>;

  // What an interactive policy declares about its provider client. The views
  // point into the artifact and remain valid for the lifetime of this
  // instance
  struct InteractivePolicy {
    std::string_view issuer{};
    std::string_view client_id{};
    // The environment variable name holding the client secret
    std::string_view client_secret_variable{};
    // The first registry path the policy governs
    std::string_view default_path{};
  };

  // The interactive policy declared under the given name, if any
  [[nodiscard]] auto interactive(std::string_view name) const
      -> std::optional<InteractivePolicy>;

  // Seal a payload under the named interactive policy's session secret,
  // producing a value that the gate and this instance's replicas accept until
  // the expiry. Nothing is produced when the policy is unknown or its session
  // secret is not configured in the environment
  [[nodiscard]] auto seal(std::string_view policy, std::string_view payload,
                          std::chrono::sys_seconds expiry) const
      -> std::optional<std::string>;

  // Recover the payload of a value sealed under the named policy by this
  // instance or one of its replicas, returning nothing for a value that does
  // not verify or has expired
  [[nodiscard]] auto open(std::string_view policy, std::string_view value) const
      -> std::optional<std::string>;

  [[nodiscard]] auto reference_permitted(std::string_view referrer_path,
                                         std::string_view referent_path) const
      -> bool;

private:
  // The implementation differs by edition and owns the memory-mapped artifact,
  // so it is hidden behind a pointer to keep the binary format out of the
  // shared interface
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace sourcemeta::one

#endif
