# An unauthorized caller gets an identical 401 for a protected path whether or
# not the schema exists, because the gate denies before the existence check.
# The JSON surface answers with an RFC 9457 problem document and the HTML
# surface with the index-time 401 page. Existence is only revealed to a caller
# the policy admits.

# JSON, existing protected schema, no credential: the full problem document,
# round-tripped through its own output schema
GET {{base}}/private/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# JSON, nonexistent path under the same protected prefix, no credential: the
# byte-identical problem document, so probing cannot distinguish a real schema
# from a guess
GET {{base}}/private/ghost.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
ghost_body: body
ghost_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/self/v1/api/schemas/evaluate{{ghost_schema}}
```
{{ghost_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# JSON, with the admitting credential, existing schema: the schema itself
GET {{base}}/private/secret.json
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/private/secret",
  "type": "object",
  "required": [ "token" ],
  "properties": {
    "token": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# JSON, with the credential, nonexistent: existence is now revealed as a genuine
# 404 problem document, round-tripped through its output schema
GET {{base}}/private/ghost.json
Authorization: Bearer primary-secret-key
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
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

# HTML, existing protected schema, no credential: the 401 page, carrying the
# same bearer challenge and no-store discipline as the JSON envelope
GET {{base}}/private/secret
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept, Accept-Encoding
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This page requires authentication"
xpath "string(//p[@class='lead'])" == "Present a valid credential to access it"

# HTML, nonexistent path under the protected prefix, no credential: the
# byte-identical 401 page, so the HTML surface leaks no existence either
GET {{base}}/private/ghost
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept, Accept-Encoding
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This page requires authentication"
xpath "string(//p[@class='lead'])" == "Present a valid credential to access it"

# HTML, a credential the policy does not admit: the byte-identical 401 page, so
# the response never reveals that a credential was presented at all
GET {{base}}/private/secret
Accept: text/html
Authorization: Bearer not-the-key
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept, Accept-Encoding
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This page requires authentication"
xpath "string(//p[@class='lead'])" == "Present a valid credential to access it"

# HTML, with the admitting credential: the real schema page, never the 401 page
GET {{base}}/private/secret
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
[Asserts]
header "Content-Type" contains "text/html"
xpath "string(//title)" != "Unauthorized"

# HTML, a schema nested under the protected prefix: it inherits the gate, so an
# anonymous caller gets the 401 page and the admitting credential the schema page
GET {{base}}/private/open/shared
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept, Accept-Encoding
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This page requires authentication"
xpath "string(//p[@class='lead'])" == "Present a valid credential to access it"

GET {{base}}/private/open/shared
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
[Asserts]
header "Content-Type" contains "text/html"
xpath "string(//title)" != "Unauthorized"
