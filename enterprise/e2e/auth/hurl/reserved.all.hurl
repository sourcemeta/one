# The reports collection is locked, so an anonymous caller is denied and the
# existence of any schema under it stays hidden behind the canonical 401
GET {{base}}/reports/quarterly.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
locked_body: body
locked_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The denial round-trips against the error schema named by its Link
POST {{base}}/self/v1/api/schemas/evaluate{{locked_schema}}
```
{{locked_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A public window is pre-scoped over a reserved namespace inside the locked
# collection that holds no schema yet. The window opens for anonymous callers, so
# a missing path inside it answers a plain 404 rather than the collection's 401
GET {{base}}/reports/draft/placeholder.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
reserved_body: body
reserved_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.title" == "Not Found"
jsonpath "$.status" == 404
jsonpath "$.detail" == "There is nothing at this URL"

# The 404 problem document round-trips against its output schema
POST {{base}}/self/v1/api/schemas/evaluate{{reserved_schema}}
```
{{reserved_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Presenting the collection key changes nothing inside the reserved window: it is
# genuinely empty, so the answer stays an identical 404
GET {{base}}/reports/draft/placeholder.json
Authorization: Bearer primary-secret-key
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
reserved_keyed_body: body
reserved_keyed_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.title" == "Not Found"
jsonpath "$.status" == 404
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{reserved_keyed_schema}}
```
{{reserved_keyed_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# HEAD inside the open reserved window is a 404 like GET, and per RFC 9110 it
# carries no body
HEAD {{base}}/reports/draft/placeholder.json
HTTP 404
Cache-Control: no-store
[Asserts]
bytes count == 0
