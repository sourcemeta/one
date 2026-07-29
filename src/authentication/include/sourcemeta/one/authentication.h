#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <sourcemeta/one/authentication_error.h>

#include <sourcemeta/core/jose.h>

#include <chrono>      // std::chrono::sys_seconds
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <functional>  // std::function
#include <memory>      // std::unique_ptr
#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

namespace sourcemeta::one {

// Everything a request presented for authentication: the bearer value from
// the authorization header and every cookie field it carried, either of which
// may admit the caller under a covering policy. The cookie fields are kept as
// they arrived rather than joined, since a request may carry more than one and
// what a cookie means is decided by whoever reads it
struct Credentials {
  std::string_view bearer{};
  std::span<const std::string_view> cookies{};
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

  // What a sealed value is for. A value is only ever opened for the purpose it
  // was sealed under, because the two derive different keys from the policy's
  // secret, so one kind of value cannot be presented as the other
  enum class Purpose : std::uint8_t { Session = 0, Transaction = 1 };

  // A browser holds one session, whichever policy established it, and the
  // policy travels inside the sealed value rather than in the name. One name
  // means logging out has a single thing to end, and means a caller cannot
  // choose which policy a value is read as by choosing what to call it
  static constexpr std::string_view SESSION_COOKIE{"sourcemeta_one_session"};

  // A login transaction follows the same shape for the short window between
  // the login redirect and the callback
  static constexpr std::string_view TRANSACTION_COOKIE{
      "sourcemeta_one_transaction"};

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
    // The `typ` header a presented token must carry, empty to accept any
    std::string_view token_type{};
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

  // Whether a path is one this instance could gate. A policy scoped to
  // anything else would gate nothing at all, so the answer comes from whoever
  // knows what this instance serves
  using PathGuard = std::function<bool(std::string_view)>;

  // Write the policies to the artifact the gate reads, refusing any policy
  // scoped to a path the guard does not recognise
  static auto save(std::span<const Policy> policies,
                   const std::filesystem::path &configuration,
                   const std::filesystem::path &destination,
                   const PathGuard &gateable) -> void;

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
    // The first registry path the policy governs
    std::string_view default_path{};
  };

  // The interactive policy declared under the given name, if any
  [[nodiscard]] auto interactive(std::string_view name) const
      -> std::optional<InteractivePolicy>;

  // Where a provider says its endpoints are. The values are copies, so they
  // stay usable across a refresh of what the provider last said
  struct ProviderEndpoints {
    std::string authorization{};
    std::string token{};
    std::string jwks_uri{};
    // Absent from a provider that does not offer to end its own session
    std::string end_session{};
  };

  // What the named interactive policy's provider says about itself, retrieved
  // once and refreshed on the freshness its own response advertises rather
  // than on every request. Nothing is returned when the policy is unknown,
  // the provider cannot be reached, or what it returned is not a description
  // this instance can act on
  [[nodiscard]] auto endpoints(std::string_view policy) const
      -> std::optional<ProviderEndpoints>;

  // The client secret the named interactive policy authenticates to its
  // provider with, if the policy is known and its secret is configured in the
  // environment. Every secret this system holds is read here rather than by
  // whoever needs it, so no caller is in a position to read one differently
  [[nodiscard]] auto client_secret(std::string_view policy) const
      -> std::optional<std::string>;

  // Recover the payload of a session value, whichever interactive policy
  // minted it, returning nothing when no policy accepts it. A value is only
  // accepted under the policy its payload names, so a caller learns which
  // policy established the session rather than choosing one to try
  [[nodiscard]] auto open_session(std::string_view value) const
      -> std::optional<std::string>;

  // Sealing is an edition-dependent capability. Where an instance does not
  // offer it, nothing seals and no value opens, so a caller that treats an
  // absent seal as unusable and an unopened value as a denial reaches the same
  // outcome under either edition

  // Seal a payload for one purpose under the named interactive policy's
  // session secret, producing a value that the gate and this instance's
  // replicas accept until the expiry. Nothing is produced when the policy is
  // unknown or its session secret is not configured in the environment
  [[nodiscard]] auto seal(std::string_view policy, Purpose purpose,
                          std::string_view payload,
                          std::chrono::sys_seconds expiry) const
      -> std::optional<std::string>;

  // Recover the payload of a value sealed for that purpose under the named
  // policy by this instance or one of its replicas, returning nothing for a
  // value that does not verify, was sealed for another purpose, or has expired
  [[nodiscard]] auto open(std::string_view policy, Purpose purpose,
                          std::string_view value) const
      -> std::optional<std::string>;

  // Bind a payload and an expiry under a key derived from the secret and the
  // purpose, producing a value that is safe to transport as a cookie. Only a
  // holder of the secret can produce or alter such a value, though anyone can
  // read its contents
  [[nodiscard]] static auto
  seal_value(std::string_view payload, Purpose purpose, std::string_view secret,
             std::chrono::sys_seconds issued, std::chrono::sys_seconds expiry)
      -> std::string;

  // Recover the payload of a sealed value, returning nothing for a value that
  // was not produced for that purpose under one of the given secrets, was
  // altered in any way, or has expired. Accepting several secrets lets a newly
  // introduced secret coexist with the one it replaces until every value
  // sealed under the old secret has expired
  [[nodiscard]] static auto
  open_value(std::string_view value, Purpose purpose,
             std::span<const std::string_view> secrets,
             std::chrono::sys_seconds now) -> std::optional<std::string>;

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
