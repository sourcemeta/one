#ifndef SOURCEMETA_ONE_SHARED_H
#define SOURCEMETA_ONE_SHARED_H

// This file is meant to contain logic shared
// between the indexer and the server

#include <sourcemeta/one/shared_encoding.h>
#include <sourcemeta/one/shared_metapack.h>
#include <sourcemeta/one/shared_version.h>

namespace sourcemeta::one {

constexpr auto HANDLER_SELF_V1_API_LIST = 1;
constexpr auto HANDLER_SELF_V1_API_LIST_PATH = 2;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_DEPENDENCIES = 3;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_DEPENDENTS = 4;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_HEALTH = 5;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_LOCATIONS = 6;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_POSITIONS = 7;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_STATS = 8;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_METADATA = 9;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_EVALUATE = 10;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_TRACE = 11;
constexpr auto HANDLER_SELF_V1_API_SCHEMAS_SEARCH = 12;
constexpr auto HANDLER_SELF_V1_API_DEFAULT = 13;
constexpr auto HANDLER_SELF_STATIC = 14;

} // namespace sourcemeta::one

#endif
