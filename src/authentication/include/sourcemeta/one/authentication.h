#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <sourcemeta/one/authentication_error.h>
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
#include <string>      // std::string, std::move
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

  /// Where a resource lives within an instance, in the single spelling every
  /// part of the system agrees on: no base path, no leading or trailing
  /// separator, no empty or relative segments, and lowercase throughout. The
  /// instance root is the empty value.
  ///
  /// An extension is kept, because it names a representation of a resource
  /// rather than a different resource, and a policy may gate one
  /// representation on its own. The artifact tree stores a resource once, so
  /// locating an artifact drops the extension while the gate keeps it.
  ///
  /// Policies, artifact locations, and directory listings are all keyed by
  /// this, so the spelling has to be decided in exactly one place. That is
  /// what this type is for: a value can only come from `parse`, which means a
  /// caller cannot hand a path to one part of the system that another part
  /// would read differently.
  class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Path {
  public:
    /// Canonicalise a request path or a URL against an instance, returning
    /// nothing when the input names somewhere outside it
    [[nodiscard]] static auto parse(const std::string_view input,
                                    const std::string_view instance_url,
                                    const std::string_view base_path)
        -> std::optional<Path>;

    /// Canonicalise input that is already relative to the instance root, such
    /// as a configured policy path or a path the indexer composed. It carries
    /// no base path to remove and names nowhere outside the instance, so
    /// unlike `parse` it always yields a value
    [[nodiscard]] static auto relative(const std::string_view input) -> Path;

    [[nodiscard]] auto value() const noexcept -> std::string_view {
      return this->value_;
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
      return this->value_.empty();
    }

    [[nodiscard]] auto operator==(const Path &other) const noexcept -> bool {
      return this->value_ == other.value_;
    }

  private:
    Path() = default;
    explicit Path(std::string value) : value_{std::move(value)} {}
    std::string value_;
  };

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

  // Whether what a request presented admits it to a path. The path arrives
  // canonical, so every caller asks about the same location rather than about
  // whichever spelling it happened to receive
  [[nodiscard]] auto admits(const Path &path,
                            const Credentials &credentials) const -> Verdict;

  // Whether what a request presented admits it to an explicit route, which the
  // router matched on the request target literally rather than on the location
  // that target resolves to. The spelling is taken as matched, so a target
  // reaching past a governed prefix is still governed by it, and a non-empty
  // base path is removed first
  [[nodiscard]] auto admits_route(std::string_view target,
                                  std::string_view base_path,
                                  const Credentials &credentials) const
      -> Verdict;

  // The configuration declaration indices of the policies that govern a path,
  // sorted ascending
  [[nodiscard]] auto governing(const Path &path) const
      -> std::vector<std::size_t>;

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

  [[nodiscard]] auto reference_permitted(const Path &referrer,
                                         const Path &referent) const -> bool;

private:
  // The implementation differs by edition and owns the memory-mapped artifact,
  // so it is hidden behind a pointer to keep the binary format out of the
  // shared interface
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace sourcemeta::one

#endif
