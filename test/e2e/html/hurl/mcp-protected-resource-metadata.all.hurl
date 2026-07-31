# An instance that guards nothing has no protected resource to describe, so
# RFC 9728 metadata would name an authorization server no caller here would
# ever be sent to. Both editions answer the same way for such an instance:
# the Community edition because it has no authentication at all, the
# Enterprise edition because this instance declares no policy over the
# endpoint. Absence is reported as absence rather than as an empty document,
# since a client is forbidden from using metadata missing the fields it needs
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

# The refusal is the same whatever a caller presents, since nothing here is
# withheld from anybody in particular
GET {{base}}/.well-known/oauth-protected-resource/self/v1/mcp
Authorization: Bearer whatever
HTTP 404
Content-Type: application/problem+json
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

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
GET {{base}}/self/v1/schemas/mcp/protected-resource-metadata/response
HTTP 200
[Asserts]
jsonpath "$.title" == "Sourcemeta One MCP Protected Resource Metadata Response"
jsonpath "$.type" == "object"
jsonpath "$.required" includes "resource"
jsonpath "$.required" includes "authorization_servers"
jsonpath "$.required" includes "bearer_methods_supported"
jsonpath "$.additionalProperties" == false
