#ifndef SOURCEMETA_ONE_AUTHENTICATION_DISCOVERY_H
#define SOURCEMETA_ONE_AUTHENTICATION_DISCOVERY_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

// The provider endpoints that a metadata document advertises, each present
// only when the document declares it as a string
struct DiscoveryDocument {
  std::optional<std::string> jwks_uri;
  std::optional<std::string> authorization_endpoint;
  std::optional<std::string> token_endpoint;
  std::optional<std::string> userinfo_endpoint;
};

// The well-known location of an issuer's metadata document
auto SOURCEMETA_ONE_AUTHENTICATION_EXPORT discovery_url(std::string_view issuer)
    -> std::string;

// Read the provider endpoints out of a metadata document, returning nothing
// for a body that is not a JSON object
auto SOURCEMETA_ONE_AUTHENTICATION_EXPORT discovery_parse(std::string_view body)
    -> std::optional<DiscoveryDocument>;

} // namespace sourcemeta::one

#endif
