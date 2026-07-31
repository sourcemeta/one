# A client that arrives holding nothing cannot be told to bring a credential
# without also being told where one comes from. RFC 9728 publishes that at a
# well-known location derived from the resource identifier, and this instance
# declares a `jwt` policy over the MCP endpoint whose audience is that
# identifier, so there is an authorization server it can honestly name.
# https://datatracker.ietf.org/doc/html/rfc9728

# The document is readable with no credential, on an instance where everything
# else demands one. That is the whole point rather than an oversight: gating it
# would leave it reachable only by somebody who no longer needs to ask
GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/protected-resource-metadata/response>; rel="describedby"
[Captures]
document_body: body
document_schema: header "Link" regex "<([^>]+)>"
{
  "resource": "{{base}}/self/v1/mcp",
  "authorization_servers": [ "https://keycloak:8443/realms/main" ],
  "bearer_methods_supported": [ "header" ]
}

POST {{base}}/self/v1/api/schemas/evaluate{{document_schema}}
Authorization: Bearer full-secret-key
```
{{document_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The instance is otherwise closed, so the contrast is what proves the
# exemption rather than a missing policy
GET {{base}}/catalog/example.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
denied_body: body
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
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Presenting a credential changes nothing, since the document describes the
# resource rather than the caller
GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
Authorization: Bearer full-secret-key
HTTP 200
Content-Type: application/json
{
  "resource": "{{base}}/self/v1/mcp",
  "authorization_servers": [ "https://keycloak:8443/realms/main" ],
  "bearer_methods_supported": [ "header" ]
}

# A credential that belongs to no policy is not a reason to withhold it either
GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
Authorization: Bearer not-any-key
HTTP 200
Content-Type: application/json
{
  "resource": "{{base}}/self/v1/mcp",
  "authorization_servers": [ "https://keycloak:8443/realms/main" ],
  "bearer_methods_supported": [ "header" ]
}

# Per RFC 9110 a HEAD carries the same headers and no body
HEAD {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/mcp/protected-resource-metadata/response>; rel="describedby"
[Asserts]
bytes count == 0

# A browser-based client fetches this cross-origin, so the preflight has to
# admit it
OPTIONS {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
Origin: http://example.com
Access-Control-Request-Method: GET
HTTP 204
Cache-Control: no-store
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, HEAD, OPTIONS
[Asserts]
bytes count == 0

# The document is read, never written
POST {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Allow: GET, HEAD, OPTIONS
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
method_body: body
{
  "type": "urn:sourcemeta:one:method-not-allowed",
  "title": "Method Not Allowed",
  "status": 405,
  "detail": "This HTTP method is invalid for this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
Authorization: Bearer full-secret-key
```
{{method_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The endpoint the document describes still refuses an anonymous caller, so
# publishing where a token comes from did not hand one out
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
HTTP 401
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.status" == 401
