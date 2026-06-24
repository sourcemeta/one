# A single path gated by two policies of different algorithms: an identity policy
# and a sha256 policy. The matcher unions them, so either key type opens the
# path, each compared under its own policy's algorithm.

# Anonymous access is denied, the canonical problem document round-tripping
# through the error schema
GET {{base}}/mixed/thing.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
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

# The identity key, stored verbatim, admits and serves the schema marked private
GET {{base}}/mixed/thing.json
Authorization: Bearer mixed-plain-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/mixed/thing",
  "type": "integer"
}

# The sha256 key, sent in the clear, admits too: the server hashes it and matches
# the pre-hashed value. The identity policy is checked first and fails, so this
# proves the union does not bail out at the first non-matching policy
GET {{base}}/mixed/thing.json
Authorization: Bearer mixed-hashed-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$['$id']" == "{{base}}/mixed/thing"
jsonpath "$.type" == "integer"

# Presenting the stored hash itself never authenticates: the server hashes the
# credential before comparing, so the stored value is not a usable key
GET {{base}}/mixed/thing.json
Authorization: Bearer 2508e2ba355946ba6065f01eb183424aaea10ddaf7bbff90e38f3b702e89e23d
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
hash_body: body
hash_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{hash_schema}}
```
{{hash_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A key that matches neither policy is denied
GET {{base}}/mixed/thing.json
Authorization: Bearer not-a-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
wrong_body: body
wrong_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{wrong_schema}}
```
{{wrong_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A key from another policy does not cross over into this path
GET {{base}}/mixed/thing.json
Authorization: Bearer primary-secret-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
crosspolicy_body: body
crosspolicy_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{crosspolicy_schema}}
```
{{crosspolicy_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The machine listing is denied anonymously and enumerates with either key,
# carrying both governing policy names sorted
GET {{base}}/self/v1/api/list/mixed
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
listing_denied_body: body
listing_denied_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{listing_denied_schema}}
```
{{listing_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/mixed
Authorization: Bearer mixed-hashed-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
listing_body: body
listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.policies" count == 2
jsonpath "$.policies[0]" == "mixed-hashed"
jsonpath "$.policies[1]" == "mixed-plain"
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "thing"
jsonpath "$.entries[0].path" == "/mixed/thing"
jsonpath "$.entries[0].policies" count == 2
jsonpath "$.entries[0].policies[0]" == "mixed-hashed"
jsonpath "$.entries[0].policies[1]" == "mixed-plain"

POST {{base}}/self/v1/api/schemas/evaluate{{listing_schema}}
```
{{listing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
