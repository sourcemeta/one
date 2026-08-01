# This edition implements the metadata, but this instance declares no policy
# over the MCP endpoint that could honestly name an authorization server, so
# there is nothing to publish. Absence is reported as absence rather than as an
# empty document, since a client is forbidden from using metadata missing the
# fields it needs
# https://datatracker.ietf.org/doc/html/rfc9728

GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
absent_body: body
error_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{absent_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A credential changes nothing, since what is missing is a policy rather than
# permission to see it
GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
Authorization: Bearer whatever
HTTP 404
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
credentialed_absent_body: body
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{credentialed_absent_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

HEAD {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
HTTP 404
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

# The schema describing the document is served whether or not this instance
# has a document to describe, since it is part of the catalog either way
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

# This instance gates nothing, so the endpoint answers rather than denies, and
# it names no metadata because there is none to name
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
  "result": {}
}

POST {{base}}/self/v1/api/schemas/evaluate{{mcp_schema}}
```
{{ping_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp/
HTTP 404
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
slash_absent_body: body
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{slash_absent_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
