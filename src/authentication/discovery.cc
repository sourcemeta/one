#include <sourcemeta/one/authentication_discovery.h>

#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one {

// Provider metadata only arises from token-based and interactive
// authentication, which are enterprise features, so this edition never
// locates a metadata document and never reads one
auto discovery_url(const std::string_view) -> std::string { return {}; }

auto discovery_parse(const std::string_view)
    -> std::optional<DiscoveryDocument> {
  return std::nullopt;
}

} // namespace sourcemeta::one
