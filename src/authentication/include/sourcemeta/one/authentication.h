#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <exception>   // std::exception
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::unique_ptr
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

// The enterprise edition verifies JSON Web Tokens against keys fetched from the
// issuer, so the test seam below accepts a substitute transport for those keys
#if defined(SOURCEMETA_ONE_ENTERPRISE)
#include <sourcemeta/core/jose.h> // sourcemeta::core::JWKSProvider
#else
// The community edition never pulls in the JOSE module, but a policy still
// names the signature algorithm type. A scoped enumeration with a fixed
// underlying type is a complete type once forward declared, which is all this
// needs
namespace sourcemeta::core {
enum class JWSAlgorithm : std::uint8_t;
}
#endif

namespace sourcemeta::one {

// Raised when an authentication policy is scoped to a path that is neither a
// declared collection or page (nor a namespace above one) nor a known route
class SOURCEMETA_ONE_AUTHENTICATION_EXPORT AuthenticationUnknownPathError
    : public std::exception {
public:
  AuthenticationUnknownPathError(std::filesystem::path path, std::string scope)
      : path_{std::move(path)}, scope_{std::move(scope)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "An authentication policy matches no known collection, page, or "
           "route";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto scope() const noexcept -> const std::string & {
    return this->scope_;
  }

private:
  std::filesystem::path path_;
  std::string scope_;
};

class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Authentication {
public:
  static constexpr std::size_t MAXIMUM_POLICIES{64};

  // How a presented credential is compared against a policy's stored keys.
  // Identity stores the key verbatim, every other algorithm stores it hashed
  enum class Algorithm : std::uint8_t { Identity = 0, Sha256 = 1 };

  // What a policy authenticates against. ApiKey compares a static credential
  // against a set of keys, JWT verifies a bearer token against an issuer
  enum class Type : std::uint8_t { ApiKey = 0, JWT = 1 };

  // A policy gates a set of path prefixes. A path covered by no policy is
  // public. An ApiKey policy admits a credential matching one of its keys under
  // its algorithm. A JWT policy admits a token signed by its issuer, carrying
  // its audience, and signed with one of its allowed algorithms. The issuer's
  // keys are discovered from the issuer unless an explicit jwks_uri is given
  struct Policy {
    std::span<const std::string_view> paths;
    std::span<const std::string_view> keys;
    Algorithm algorithm{Algorithm::Identity};
    Type type{Type::ApiKey};
    std::string_view issuer{};
    std::string_view audience{};
    std::string_view jwks_uri{};
    std::span<const sourcemeta::core::JWSAlgorithm> algorithms{};
  };

  struct Verdict {
    bool allowed;
  };

  static auto save(std::span<const Policy> policies,
                   const std::filesystem::path &configuration,
                   const std::filesystem::path &destination) -> void;

  explicit Authentication(const std::filesystem::path &path);

#if defined(SOURCEMETA_ONE_ENTERPRISE)
  // Test seam: construct with a substitute key set transport instead of the
  // default one that fetches over HTTP, so the JWT path is exercised offline
  Authentication(const std::filesystem::path &path,
                 sourcemeta::core::JWKSProvider::Fetcher fetcher);
#endif

  ~Authentication();

  // The artifact is memory-mapped and owned for the lifetime of the view
  Authentication(const Authentication &) = delete;
  Authentication(Authentication &&) = delete;
  auto operator=(const Authentication &) -> Authentication & = delete;
  auto operator=(Authentication &&) -> Authentication & = delete;

  // A non-empty base path is stripped off the input before matching, turning a
  // request URL path into a registry path
  [[nodiscard]] auto admits(std::string_view registry_path,
                            std::string_view credential,
                            std::string_view base_path = {}) const -> Verdict;

  // The configuration declaration indices of the policies that govern a path,
  // sorted ascending
  [[nodiscard]] auto governing(std::string_view registry_path,
                               std::string_view base_path = {}) const
      -> std::vector<std::size_t>;

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
