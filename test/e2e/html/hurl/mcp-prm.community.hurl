# This edition has no authentication, so it has no protected resource to
# describe and answers like every other route that only the Enterprise edition
# implements. A client reads any non-success as no metadata, which is the
# honest answer here either way
# https://datatracker.ietf.org/doc/html/rfc9728

GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
absent_body: body
error_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:enterprise-required",
  "title": "Forbidden",
  "status": 403,
  "detail": "This feature is only available in the Enterprise edition"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{absent_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The refusal is the same whatever a caller presents, since nothing here is
# withheld from anybody in particular
GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
Authorization: Bearer whatever
HTTP 403
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
credentialed_absent_body: body
{
  "type": "urn:sourcemeta:one:enterprise-required",
  "title": "Forbidden",
  "status": 403,
  "detail": "This feature is only available in the Enterprise edition"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{credentialed_absent_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

HEAD {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
HTTP 403
Content-Type: application/problem+json
[Asserts]
bytes count == 0

# The route exists even where the document does not, so a cross-origin client
# learns the answer rather than a network error
OPTIONS {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
Origin: http://example.com
Access-Control-Request-Method: GET
HTTP 204
Cache-Control: no-store
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, HEAD, OPTIONS
[Asserts]
bytes count == 0

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
```
{{method_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The schema describing the document is part of the catalog in both editions,
# even where no document is ever produced from it
GET {{base}}/self/v1/schemas/mcp/prm/response
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
[Asserts]
jsonpath "$['$schema']" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.title" == "Sourcemeta One MCP Protected Resource Metadata Response"
jsonpath "$.type" == "object"
jsonpath "$.required" includes "resource"
jsonpath "$.required" includes "authorization_servers"
jsonpath "$.required" includes "bearer_methods_supported"
jsonpath "$.additionalProperties" == false
jsonpath "$.properties.resource.format" == "uri"
jsonpath "$.properties.authorization_servers.items.format" == "uri"
jsonpath "$.properties.bearer_methods_supported.const" includes "header"

# There is no Model Context Protocol server in this edition at all, which is
# the deeper reason there is no metadata describing one. Nothing is denied
# here, so no challenge names a document either
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
HTTP 200
Content-Type: application/json
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
ping_body: body
mcp_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "WWW-Authenticate" not exists
{
  "jsonrpc": "2.0",
  "id": 1,
  "error": {
    "code": -32000,
    "message": "Enterprise edition required",
    "data": "MCP support is only available in the Enterprise edition of Sourcemeta One"
  }
}

POST {{base}}/self/v1/api/schemas/evaluate{{mcp_schema}}
```
{{ping_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp/
HTTP 403
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
slash_absent_body: body
{
  "type": "urn:sourcemeta:one:enterprise-required",
  "title": "Forbidden",
  "status": 403,
  "detail": "This feature is only available in the Enterprise edition"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{slash_absent_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
