#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <sourcemeta/one/authentication_error.h>
#include <sourcemeta/one/configuration.h>

#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/uritemplate.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <memory>      // std::unique_ptr
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace sourcemeta::one {

class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Authentication {
public:
  static constexpr std::size_t MAXIMUM_POLICIES{64};

  // How a presented credential is compared against a policy's stored keys.
  // Identity stores the key verbatim, every other algorithm stores it hashed
  enum class Algorithm : std::uint8_t { Identity = 0, Sha256 = 1 };

  enum class Type : std::uint8_t { ApiKey = 0, JWT = 1 };

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
