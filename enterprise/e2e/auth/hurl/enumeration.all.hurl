# INVARIANT, load-bearing for the nesting idiom. A protected directory's own
# listing is never enumerable without a credential. The supported way to hide a
# sensitive name, title or description is to put the policy on a deliberately
# anonymous boundary container and nest the subject inside it. That only works
# because an outsider cannot enumerate the boundary: if this property ever
# regresses, every nested name and description behind it leaks. These cases pin
# it from every enumeration surface.

# The machine listing of a protected directory is not there without a
# credential, and what is answered is the canonical problem document, carrying
# nothing about the children it hides
GET {{base}}/self/v1/api/list/private
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Existence under the boundary is not probeable: a nested path that does not
# exist answers identically to the boundary itself, so nested names cannot be
# discovered by guessing. The gate runs ahead of the existence check
GET {{base}}/self/v1/api/list/private/ghost
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
probe_body: body
probe_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{probe_schema}}
```
{{probe_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The human rendering of the same directory is gated identically, serving the
# page for a path that is not there rather than any listing of its contents
GET {{base}}/private/
Accept: text/html
HTTP 404
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(//title)" == "Not Found"
xpath "string(//h2[@class='fw-bold'])" == "Oops! What you are looking for is not here"

# Content negotiation is not a way around the gate: asking the directory page
# for JSON answers the same way
GET {{base}}/private/
Accept: application/json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
negotiated_body: body
negotiated_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
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
HTTP 404
[Asserts]
bytes count == 0

# With the credential the gate is the only thing that was hiding the interior:
# the listing enumerates, revealing the nested names and the per-child metadata
# (identifier, sizes, dialect, health) withheld until now. Served private
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
jsonpath "$.private" == true
jsonpath "$.entries[0].name" == "open"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/private/open/"
jsonpath "$.entries[0].private" == true
jsonpath "$.entries[1].name" == "secret"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].path" == "/private/secret"
jsonpath "$.entries[1].private" == true
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

# The same answer, byte for byte, and only now does it mean anything. To an
# anonymous caller it said nothing at all, since every path under the boundary
# said it. To an admitted caller it says this one path is really absent, which
# is what makes absence a disclosure reserved for whoever can enumerate
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
body == {{probe_body}}
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
