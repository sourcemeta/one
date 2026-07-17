# INVARIANT, load-bearing for the nesting idiom. A protected directory's own
# listing is never enumerable without a credential. The supported way to hide a
# sensitive name, title or description is to put the policy on a deliberately
# anonymous boundary container and nest the subject inside it. That only works
# because an outsider cannot enumerate the boundary: if this property ever
# regresses, every nested name and description behind it leaks. These cases pin
# it from every enumeration surface.

# The machine listing of a protected directory denies without a credential, and
# the denial is the canonical problem document carrying nothing about the
# children it hides
GET {{base}}/self/v1/api/list/private
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Existence under the boundary is not probeable: a nested path that does not
# exist denies identically to the boundary itself, so nested names cannot be
# discovered by guessing. The gate runs ahead of the existence check
GET {{base}}/self/v1/api/list/private/ghost
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
probe_body: body
probe_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{probe_schema}}
```
{{probe_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The human rendering of the same directory is gated identically, serving the
# 401 page rather than any listing of its contents
GET {{base}}/private/
Accept: text/html
HTTP 401
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This page requires authentication"

# Content negotiation is not a way around the gate: asking the directory page for
# JSON denies the same way
GET {{base}}/private/
Accept: application/json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
negotiated_body: body
negotiated_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{negotiated_schema}}
```
{{negotiated_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# HEAD on the listing is gated exactly like GET, and per RFC 9110 carries no body
HEAD {{base}}/self/v1/api/list/private
HTTP 401
WWW-Authenticate: Bearer realm="registry"
[Asserts]
bytes count == 0

# With the credential the gate is the only thing that was hiding the interior:
# the listing enumerates, revealing the nested names and the per-child metadata
# (identifier, sizes, dialect, health) that the denial withheld. Served private
GET {{base}}/self/v1/api/list/private
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
listing_body: body
listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.entries" count == 2
jsonpath "$.policies" count == 1
jsonpath "$.policies[0].name" == "reports"
jsonpath "$.policies[0].title" == "reports"
jsonpath "$.policies[0].type" == "apiKey"
jsonpath "$.entries[0].name" == "open"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/private/open/"
jsonpath "$.entries[0].policies" count == 1
jsonpath "$.entries[0].policies[0].name" == "reports"
jsonpath "$.entries[0].policies[0].title" == "reports"
jsonpath "$.entries[0].policies[0].type" == "apiKey"
jsonpath "$.entries[1].name" == "secret"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].path" == "/private/secret"
jsonpath "$.entries[1].policies" count == 1
jsonpath "$.entries[1].policies[0].name" == "reports"
jsonpath "$.entries[1].policies[0].title" == "reports"
jsonpath "$.entries[1].policies[0].type" == "apiKey"
jsonpath "$.entries[1].identifier" == "{{base}}/private/secret"
jsonpath "$.entries[1].health" exists
jsonpath "$.entries[1].bytes" exists
jsonpath "$.entries[1].dialect" exists

POST {{base}}/self/v1/api/schemas/evaluate{{listing_schema}}
```
{{listing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Existence is revealed only after authentication: the nested path that denied
# anonymously now resolves to a genuine 404, so absence is disclosed only to a
# caller already admitted to the boundary
GET {{base}}/self/v1/api/list/private/ghost
Authorization: Bearer primary-secret-key
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
missing_body: body
missing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.title" == "Not Found"
jsonpath "$.status" == 404
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{missing_schema}}
```
{{missing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
