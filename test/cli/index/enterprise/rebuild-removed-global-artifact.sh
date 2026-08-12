#!/bin/sh

# The build state records that a build wrote a file, not that the file is still
# there, and an incremental build trusts it. So an artifact removed since, by a
# stray deletion or a sync that went wrong, is never produced again: indexing
# reports success and leaves the instance without it.
#
# The authentication artifact is the one where that hurts most. The gate reads
# it on every request and a missing one denies every path, so the instance is
# down while the build that was supposed to repair it exits zero. Routing is the
# same shape.
#
# Each artifact is removed on its own, so that recreating it cannot be a side
# effect of one of the others being gone. Removing the configuration anchor or
# the version would be noticed at startup, which is why neither is the case
# under test here.

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com",
  "authentication": [
    {
      "type": "apiKey",
      "name": "alpha",
      "paths": [ "/schemas" ],
      "algorithm": "identity",
      "keys": [ { "environmentVariable": "ONE_KEY_ALPHA" } ]
    }
  ],
  "contents": {
    "schemas": { "baseUri": "https://example.com/", "path": "./schemas" }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a"
}
EOF

cat << 'EOF' > "$TMP/expected.txt"
./authentication.bin
./configuration.json
./explorer
./explorer/alpha
./explorer/alpha/%
./explorer/alpha/%/401.metapack
./explorer/alpha/%/404.metapack
./explorer/alpha/%/directory-html.metapack
./explorer/alpha/%/directory.metapack
./explorer/alpha/%/login-html.metapack
./explorer/alpha/%/mcp.metapack
./explorer/alpha/%/search.metapack
./explorer/alpha/schemas
./explorer/alpha/schemas/%
./explorer/alpha/schemas/%/directory-html.metapack
./explorer/alpha/schemas/%/directory.metapack
./explorer/alpha/schemas/%/login-html.metapack
./explorer/alpha/schemas/a
./explorer/alpha/schemas/a/%
./explorer/alpha/schemas/a/%/dependents.metapack
./explorer/alpha/schemas/a/%/schema-html.metapack
./explorer/alpha/schemas/a/%/schema.metapack
./explorer/alpha/self
./explorer/alpha/self/%
./explorer/alpha/self/%/directory-html.metapack
./explorer/alpha/self/%/directory.metapack
./explorer/alpha/self/%/login-html.metapack
./explorer/alpha/self/v1
./explorer/alpha/self/v1/%
./explorer/alpha/self/v1/%/directory-html.metapack
./explorer/alpha/self/v1/%/directory.metapack
./explorer/alpha/self/v1/%/login-html.metapack
./explorer/alpha/self/v1/schemas
./explorer/alpha/self/v1/schemas/%
./explorer/alpha/self/v1/schemas/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/%/directory.metapack
./explorer/alpha/self/v1/schemas/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api
./explorer/alpha/self/v1/schemas/api/%
./explorer/alpha/self/v1/schemas/api/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/error
./explorer/alpha/self/v1/schemas/api/error/%
./explorer/alpha/self/v1/schemas/api/error/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/error/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/error/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/list
./explorer/alpha/self/v1/schemas/api/list/%
./explorer/alpha/self/v1/schemas/api/list/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/list/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/list/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/list/response
./explorer/alpha/self/v1/schemas/api/list/response/%
./explorer/alpha/self/v1/schemas/api/list/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/list/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/list/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas
./explorer/alpha/self/v1/schemas/api/schemas/%
./explorer/alpha/self/v1/schemas/api/schemas/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependencies
./explorer/alpha/self/v1/schemas/api/schemas/dependencies/%
./explorer/alpha/self/v1/schemas/api/schemas/dependencies/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependencies/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependencies/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependencies/response
./explorer/alpha/self/v1/schemas/api/schemas/dependencies/response/%
./explorer/alpha/self/v1/schemas/api/schemas/dependencies/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependencies/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependencies/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependents
./explorer/alpha/self/v1/schemas/api/schemas/dependents/%
./explorer/alpha/self/v1/schemas/api/schemas/dependents/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependents/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependents/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependents/response
./explorer/alpha/self/v1/schemas/api/schemas/dependents/response/%
./explorer/alpha/self/v1/schemas/api/schemas/dependents/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependents/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/dependents/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/evaluate
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/%
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/request
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/request/%
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/response
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/response/%
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/evaluate/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/health
./explorer/alpha/self/v1/schemas/api/schemas/health/%
./explorer/alpha/self/v1/schemas/api/schemas/health/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/health/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/health/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/health/response
./explorer/alpha/self/v1/schemas/api/schemas/health/response/%
./explorer/alpha/self/v1/schemas/api/schemas/health/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/health/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/health/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/locations
./explorer/alpha/self/v1/schemas/api/schemas/locations/%
./explorer/alpha/self/v1/schemas/api/schemas/locations/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/locations/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/locations/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/locations/response
./explorer/alpha/self/v1/schemas/api/schemas/locations/response/%
./explorer/alpha/self/v1/schemas/api/schemas/locations/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/locations/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/locations/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/metadata
./explorer/alpha/self/v1/schemas/api/schemas/metadata/%
./explorer/alpha/self/v1/schemas/api/schemas/metadata/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/metadata/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/metadata/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/metadata/response
./explorer/alpha/self/v1/schemas/api/schemas/metadata/response/%
./explorer/alpha/self/v1/schemas/api/schemas/metadata/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/metadata/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/metadata/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/output-error
./explorer/alpha/self/v1/schemas/api/schemas/output-error/%
./explorer/alpha/self/v1/schemas/api/schemas/output-error/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/output-error/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/output-error/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/position
./explorer/alpha/self/v1/schemas/api/schemas/position/%
./explorer/alpha/self/v1/schemas/api/schemas/position/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/position/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/position/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/positions
./explorer/alpha/self/v1/schemas/api/schemas/positions/%
./explorer/alpha/self/v1/schemas/api/schemas/positions/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/positions/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/positions/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/positions/response
./explorer/alpha/self/v1/schemas/api/schemas/positions/response/%
./explorer/alpha/self/v1/schemas/api/schemas/positions/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/positions/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/positions/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/rdf
./explorer/alpha/self/v1/schemas/api/schemas/rdf/%
./explorer/alpha/self/v1/schemas/api/schemas/rdf/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/rdf/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/rdf/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/rdf/request
./explorer/alpha/self/v1/schemas/api/schemas/rdf/request/%
./explorer/alpha/self/v1/schemas/api/schemas/rdf/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/rdf/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/rdf/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/rdf/response
./explorer/alpha/self/v1/schemas/api/schemas/rdf/response/%
./explorer/alpha/self/v1/schemas/api/schemas/rdf/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/rdf/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/rdf/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/search
./explorer/alpha/self/v1/schemas/api/schemas/search/%
./explorer/alpha/self/v1/schemas/api/schemas/search/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/search/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/search/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/search/response
./explorer/alpha/self/v1/schemas/api/schemas/search/response/%
./explorer/alpha/self/v1/schemas/api/schemas/search/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/search/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/search/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/stats
./explorer/alpha/self/v1/schemas/api/schemas/stats/%
./explorer/alpha/self/v1/schemas/api/schemas/stats/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/stats/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/stats/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/stats/response
./explorer/alpha/self/v1/schemas/api/schemas/stats/response/%
./explorer/alpha/self/v1/schemas/api/schemas/stats/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/stats/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/stats/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/trace
./explorer/alpha/self/v1/schemas/api/schemas/trace/%
./explorer/alpha/self/v1/schemas/api/schemas/trace/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/trace/%/directory.metapack
./explorer/alpha/self/v1/schemas/api/schemas/trace/%/login-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/trace/request
./explorer/alpha/self/v1/schemas/api/schemas/trace/request/%
./explorer/alpha/self/v1/schemas/api/schemas/trace/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/trace/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/trace/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/api/schemas/trace/response
./explorer/alpha/self/v1/schemas/api/schemas/trace/response/%
./explorer/alpha/self/v1/schemas/api/schemas/trace/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/api/schemas/trace/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/api/schemas/trace/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp
./explorer/alpha/self/v1/schemas/mcp/%
./explorer/alpha/self/v1/schemas/mcp/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/error
./explorer/alpha/self/v1/schemas/mcp/error/%
./explorer/alpha/self/v1/schemas/mcp/error/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/error/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/error/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/initialize
./explorer/alpha/self/v1/schemas/mcp/initialize/%
./explorer/alpha/self/v1/schemas/mcp/initialize/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/initialize/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/initialize/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/initialize/request
./explorer/alpha/self/v1/schemas/mcp/initialize/request/%
./explorer/alpha/self/v1/schemas/mcp/initialize/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/initialize/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/initialize/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/initialize/response
./explorer/alpha/self/v1/schemas/mcp/initialize/response/%
./explorer/alpha/self/v1/schemas/mcp/initialize/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/initialize/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/initialize/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/notifications
./explorer/alpha/self/v1/schemas/mcp/notifications/%
./explorer/alpha/self/v1/schemas/mcp/notifications/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/notifications/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/notifications/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/notifications/cancelled
./explorer/alpha/self/v1/schemas/mcp/notifications/cancelled/%
./explorer/alpha/self/v1/schemas/mcp/notifications/cancelled/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/notifications/cancelled/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/notifications/cancelled/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/notifications/initialized
./explorer/alpha/self/v1/schemas/mcp/notifications/initialized/%
./explorer/alpha/self/v1/schemas/mcp/notifications/initialized/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/notifications/initialized/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/notifications/initialized/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/ping
./explorer/alpha/self/v1/schemas/mcp/ping/%
./explorer/alpha/self/v1/schemas/mcp/ping/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/ping/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/ping/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/ping/request
./explorer/alpha/self/v1/schemas/mcp/ping/request/%
./explorer/alpha/self/v1/schemas/mcp/ping/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/ping/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/ping/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/ping/response
./explorer/alpha/self/v1/schemas/mcp/ping/response/%
./explorer/alpha/self/v1/schemas/mcp/ping/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/ping/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/ping/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/prm
./explorer/alpha/self/v1/schemas/mcp/prm/%
./explorer/alpha/self/v1/schemas/mcp/prm/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/prm/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/prm/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/prm/response
./explorer/alpha/self/v1/schemas/mcp/prm/response/%
./explorer/alpha/self/v1/schemas/mcp/prm/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/prm/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/prm/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/request
./explorer/alpha/self/v1/schemas/mcp/request/%
./explorer/alpha/self/v1/schemas/mcp/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/resources
./explorer/alpha/self/v1/schemas/mcp/resources/%
./explorer/alpha/self/v1/schemas/mcp/resources/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/list
./explorer/alpha/self/v1/schemas/mcp/resources/list/%
./explorer/alpha/self/v1/schemas/mcp/resources/list/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/list/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/list/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/list/request
./explorer/alpha/self/v1/schemas/mcp/resources/list/request/%
./explorer/alpha/self/v1/schemas/mcp/resources/list/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/list/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/list/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/list/response
./explorer/alpha/self/v1/schemas/mcp/resources/list/response/%
./explorer/alpha/self/v1/schemas/mcp/resources/list/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/list/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/list/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/read
./explorer/alpha/self/v1/schemas/mcp/resources/read/%
./explorer/alpha/self/v1/schemas/mcp/resources/read/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/read/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/read/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/read/request
./explorer/alpha/self/v1/schemas/mcp/resources/read/request/%
./explorer/alpha/self/v1/schemas/mcp/resources/read/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/read/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/read/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/read/response
./explorer/alpha/self/v1/schemas/mcp/resources/read/response/%
./explorer/alpha/self/v1/schemas/mcp/resources/read/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/read/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/read/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates
./explorer/alpha/self/v1/schemas/mcp/resources/templates/%
./explorer/alpha/self/v1/schemas/mcp/resources/templates/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/%
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/request
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/request/%
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/response
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/response/%
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/resources/templates/list/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/response
./explorer/alpha/self/v1/schemas/mcp/response/%
./explorer/alpha/self/v1/schemas/mcp/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools
./explorer/alpha/self/v1/schemas/mcp/tools/%
./explorer/alpha/self/v1/schemas/mcp/tools/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call
./explorer/alpha/self/v1/schemas/mcp/tools/call/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/evaluate-schema/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-dependents/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-health/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-locations/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-metadata/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-positions/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/get-schema-stats/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/instance-to-rdf/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/list-directory/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/search-schemas/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/call/trace-schema-evaluation/response/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/list
./explorer/alpha/self/v1/schemas/mcp/tools/list/%
./explorer/alpha/self/v1/schemas/mcp/tools/list/%/directory-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/list/%/directory.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/list/%/login-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/list/request
./explorer/alpha/self/v1/schemas/mcp/tools/list/request/%
./explorer/alpha/self/v1/schemas/mcp/tools/list/request/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/list/request/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/list/request/%/schema.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/list/response
./explorer/alpha/self/v1/schemas/mcp/tools/list/response/%
./explorer/alpha/self/v1/schemas/mcp/tools/list/response/%/dependents.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/list/response/%/schema-html.metapack
./explorer/alpha/self/v1/schemas/mcp/tools/list/response/%/schema.metapack
./explorer/public
./explorer/public/%
./explorer/public/%/401.metapack
./explorer/public/%/404.metapack
./explorer/public/%/directory-html.metapack
./explorer/public/%/directory.metapack
./explorer/public/%/login-html.metapack
./explorer/public/%/mcp.metapack
./explorer/public/%/search.metapack
./explorer/public/schemas
./explorer/public/schemas/%
./explorer/public/schemas/%/directory-html.metapack
./explorer/public/schemas/%/directory.metapack
./explorer/public/schemas/%/login-html.metapack
./explorer/public/schemas/a
./explorer/public/schemas/a/%
./explorer/public/schemas/a/%/dependents.metapack
./explorer/public/schemas/a/%/schema-html.metapack
./explorer/public/schemas/a/%/schema.metapack
./routes.bin
./schemas
./schemas/schemas
./schemas/schemas/a
./schemas/schemas/a/%
./schemas/schemas/a/%/blaze-exhaustive.metapack
./schemas/schemas/a/%/blaze-fast.metapack
./schemas/schemas/a/%/bundle.metapack
./schemas/schemas/a/%/dependencies.metapack
./schemas/schemas/a/%/editor.metapack
./schemas/schemas/a/%/health.metapack
./schemas/schemas/a/%/locations.metapack
./schemas/schemas/a/%/positions.metapack
./schemas/schemas/a/%/schema.metapack
./schemas/schemas/a/%/stats.metapack
./state.bin
./version.json
EOF

"$1" "$TMP/one.json" "$TMP/output"

rm "$TMP/output/authentication.bin"
"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 \
  \( -path './schemas/self' -o -path './explorer/public/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

diff "$TMP/manifest.txt" "$TMP/expected.txt"

rm "$TMP/output/routes.bin"
"$1" "$TMP/one.json" "$TMP/output"

cd "$TMP/output"
find . -mindepth 1 \
  \( -path './schemas/self' -o -path './explorer/public/self' \) -prune \
  -o -print \
  | LC_ALL=C sort > "$TMP/manifest.txt"
cd - > /dev/null

diff "$TMP/manifest.txt" "$TMP/expected.txt"
