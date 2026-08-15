# The MCP endpoint is a protected resource in its own right, so a token has to
# name it and not merely name something wider that happens to contain it. This
# instance declares both shapes: `machine` governs everything and calls itself
# the registry, while `mcp-tokens` governs the endpoint and calls itself the
# endpoint. A token minted for the registry therefore opens the catalog but
# not this endpoint, which is what stops a token issued for one service
# reaching another that shares its audience.
# https://modelcontextprotocol.io/specification/2025-11-25/basic/authorization

# The provider mints a token for the registry, which is what every existing
# machine consumer of this instance carries
POST https://keycloak:8443/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-service
client_secret: ci-service-secret
HTTP 200
[Captures]
registry_token: jsonpath "$.access_token"
[Asserts]
jsonpath "$.token_type" == "Bearer"

# It opens the catalog, since that is what it was issued for
GET {{base}}/catalog/example.json
Authorization: Bearer {{registry_token}}
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/catalog/example",
  "type": "string"
}

# It does not open the MCP endpoint, because it never names it. The refusal is
# the ordinary one, so nothing about it says which audience was missing
POST {{base}}/self/v1/mcp
Authorization: Bearer {{registry_token}}
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry", resource_metadata="{{base}}/.well-known/oauth-protected-resource/self/v1/mcp"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
registry_denied_body: body
error_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
Authorization: Bearer full-secret-key
```
{{registry_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A token minted for the endpoint opens it, which is what a client following
# the metadata document ends up holding
POST https://keycloak:8443/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-mcp
client_secret: ci-mcp-secret
HTTP 200
[Captures]
endpoint_token: jsonpath "$.access_token"

POST {{base}}/self/v1/mcp
Authorization: Bearer {{endpoint_token}}
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 2, "method": "ping" }
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
endpoint_body: body
mcp_schema: header "Link" regex "<([^>]+)>"
{
  "jsonrpc": "2.0",
  "id": 2,
  "result": {}
}

POST {{base}}/self/v1/api/schemas/evaluate{{mcp_schema}}
Authorization: Bearer full-secret-key
```
{{endpoint_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# That token names only the endpoint, so it does not open the catalog either.
# Naming the endpoint is not a way of naming everything
GET {{base}}/catalog/example.json
Authorization: Bearer {{endpoint_token}}
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
[Captures]
endpoint_elsewhere_body: body
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
Authorization: Bearer full-secret-key
```
{{endpoint_elsewhere_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# One token can carry both audiences, which is the migration for a deployment
# whose single token used to reach everything. It opens both
POST https://keycloak:8443/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-both
client_secret: ci-both-secret
HTTP 200
[Captures]
both_token: jsonpath "$.access_token"

POST {{base}}/self/v1/mcp
Authorization: Bearer {{both_token}}
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 3, "method": "ping" }
```
HTTP 200
Content-Type: application/json
[Captures]
both_body: body
{
  "jsonrpc": "2.0",
  "id": 3,
  "result": {}
}

POST {{base}}/self/v1/api/schemas/evaluate{{mcp_schema}}
Authorization: Bearer full-secret-key
```
{{both_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/catalog/example.json
Authorization: Bearer {{both_token}}
HTTP 200
Content-Type: application/schema+json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/catalog/example",
  "type": "string"
}

# The requirement is about tokens, so a credential that carries no audience is
# unaffected. The key scoped to this endpoint still opens it
POST {{base}}/self/v1/mcp
Authorization: Bearer mcp-secret-key
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 4, "method": "ping" }
```
HTTP 200
Content-Type: application/json
[Captures]
key_body: body
{
  "jsonrpc": "2.0",
  "id": 4,
  "result": {}
}

POST {{base}}/self/v1/api/schemas/evaluate{{mcp_schema}}
Authorization: Bearer full-secret-key
```
{{key_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
