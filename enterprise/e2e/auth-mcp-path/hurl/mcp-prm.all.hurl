# RFC 9728 forms the metadata location by inserting the well-known string
# between the host and the path, which places it above any configured base
# path. An instance served under one therefore publishes the document
# somewhere derivation cannot reach, and only the `WWW-Authenticate` header
# leads a client there. That makes the header the whole discovery mechanism
# here, and the URL it names has to be exactly where the document is.
#
# It once was not. The challenge was composed as origin plus base path plus
# well-known path, but the instance URL already carries the base path, so it
# advertised the base path twice and named a URL that resolves nowhere. Every
# check until then had been against a root deployment, where the two spellings
# happen to coincide. This sandbox exists so that they cannot diverge again.
# https://datatracker.ietf.org/doc/html/rfc9728

# The endpoint is gated, so an anonymous client is refused and told where a
# token comes from. The base path appears once
POST {{base}}/registry/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry", resource_metadata="{{base}}/registry/.well-known/oauth-protected-resource/self/v1/mcp"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
denied_body: body
error_schema: header "Link" regex "</registry([^>]+)>"
[Asserts]
header "WWW-Authenticate" not contains "/registry/registry/"
header "Access-Control-Expose-Headers" contains "WWW-Authenticate"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/registry/self/v1/api/schemas/evaluate{{error_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The URL that challenge names is where the document actually is, which is the
# property the doubled base path broke
GET {{base}}/registry/.well-known/oauth-protected-resource/self/v1/mcp
HTTP 200
Content-Type: application/json
Cache-Control: public, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
Link: </registry/self/v1/schemas/mcp/prm/response>; rel="describedby"
[Captures]
document_body: body
document_schema: header "Link" regex "</registry([^>]+)>"
{
  "resource": "{{base}}/registry/self/v1/mcp",
  "authorization_servers": [ "https://accounts.example.com" ],
  "bearer_methods_supported": [ "header" ]
}

POST {{base}}/registry/self/v1/api/schemas/evaluate{{document_schema}}
```
{{document_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The resource identifier the document declares is the endpoint that refused
# the client, so a token requested for it is one this instance accepts
GET {{base}}/registry/.well-known/oauth-protected-resource/self/v1/mcp
HTTP 200
[Asserts]
jsonpath "$.resource" == "{{base}}/registry/self/v1/mcp"
jsonpath "$.resource" not contains "/registry/registry/"

# Where the specification would look, above the base path, there is nothing.
# That is the limitation the header exists to cover rather than an oversight
GET {{base}}/.well-known/oauth-protected-resource/registry/self/v1/mcp
HTTP 404

# The trailing-slash spelling is served under the base path too
GET {{base}}/registry/.well-known/oauth-protected-resource/self/v1/mcp/
HTTP 200
Content-Type: application/json
[Captures]
slash_body: body
{
  "resource": "{{base}}/registry/self/v1/mcp",
  "authorization_servers": [ "https://accounts.example.com" ],
  "bearer_methods_supported": [ "header" ]
}

POST {{base}}/registry/self/v1/api/schemas/evaluate{{document_schema}}
```
{{slash_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Catalog content is public here, so the gate covers the MCP endpoint alone and
# the challenge above is not something every path carries
GET {{base}}/registry/catalog/example
HTTP 200
Content-Type: application/schema+json
[Asserts]
header "WWW-Authenticate" not exists
jsonpath "$['$schema']" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$['$id']" == "{{base}}/registry/catalog/example"
jsonpath "$.title" == "Example"
jsonpath "$.description" == "A schema served publicly while the MCP endpoint is gated"
jsonpath "$.type" == "string"
