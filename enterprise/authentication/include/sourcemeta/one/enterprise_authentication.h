#ifndef SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_H_
#define SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_H_

#ifndef SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/enterprise_authentication_export.h>
#endif

#include <string_view> // std::string_view

namespace sourcemeta::one {

// Evaluate a non-public authentication policy against a caller credential.
// Policy types beyond public are an enterprise feature, so their matching
// logic lives here and is compiled out of the community edition. Returns
// whether the credential satisfies the policy
auto SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_EXPORT
authentication_admits_enterprise(std::string_view credential) -> bool;

} // namespace sourcemeta::one

#endif
