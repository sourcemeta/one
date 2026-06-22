# Under a base path the gate must strip the base prefix and canonicalise the
# rest (case, percent-encoding, dot-segments) before matching, and the two must
# compose without opening a bypass. Every served response asserts the full
# schema body at its canonical, base-path id, and every denial asserts the full
# problem document and round-trips it through the error schema, addressed
# base-path-relative.

# An uppercased protected path lowercases to the protected resource and is gated
GET {{base}}/registry/PRIVATE/SECRET.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
cased_body: body
[Asserts]
header "ETag" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{cased_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# With the key it serves the protected resource at its canonical, base-path id
GET {{base}}/registry/PRIVATE/SECRET.json
Authorization: Bearer content-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "ETag" exists
header "Last-Modified" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/registry/private/secret",
  "type": "object",
  "required": [ "token" ],
  "properties": {
    "token": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# An uppercased public path serves anonymously at its canonical, base-path id
GET {{base}}/registry/PUBLIC/STRING.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "ETag" exists
header "Last-Modified" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/registry/public/string",
  "type": "string"
}

# A percent-encoded protected path decodes and is gated
GET {{base}}/registry/%70rivate/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
encoded_body: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{encoded_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Traversal out of the public prefix into the protected one resolves to the
# protected resource and is gated, not admitted as public
GET {{base}}/registry/public/../private/secret.json
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
traversal_body: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{traversal_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# With the key the same traversal serves the protected resource, proving it
# reached /private/secret
GET {{base}}/registry/public/../private/secret.json
Authorization: Bearer content-secret-key
[Options]
path-as-is: true
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
  "$id": "{{base}}/registry/private/secret",
  "type": "object",
  "required": [ "token" ],
  "properties": {
    "token": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# Traversal from the protected prefix into the public one is served anonymously
GET {{base}}/registry/private/../public/string.json
[Options]
path-as-is: true
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/registry/public/string",
  "type": "string"
}

# Traversal that climbs above the base path lands outside the registry
# namespace and finds nothing, rather than reaching a sibling of the base
GET {{base}}/registry/../private/secret.json
[Options]
path-as-is: true
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
escape_body: body
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{escape_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Traversal inside the locked static surface cannot escape its prefix: the
# surface gate matches the literal /self/v1/static and denies without the key
GET {{base}}/registry/self/v1/static/../../api/schemas/health/public/string
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
static_body: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{static_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
