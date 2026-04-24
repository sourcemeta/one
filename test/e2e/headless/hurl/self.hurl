# Health endpoint
GET {{base}}/self/v1/health
HTTP 200

# Root listing includes self directory
GET {{base}}/self/v1/api/list
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.entries[?(@.name == 'self')].type" nth 0 == "directory"

# Self schemas are browsable
GET {{base}}/self/v1/api/list/self
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.entries" count >= 1

# Configuration schema is reachable
GET {{base}}/self/v1/schemas/configuration/configuration.json
HTTP 200
Content-Type: application/schema+json

# API error schema is reachable
GET {{base}}/self/v1/schemas/api/error.json
HTTP 200
Content-Type: application/schema+json

# Metadata for a self schema works
GET {{base}}/self/v1/api/schemas/metadata/self/v1/schemas/configuration/configuration
Accept: application/json
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.path" == "/self/v1/schemas/configuration/configuration"
jsonpath "$.title" == "The Sourcemeta One configuration file format"
