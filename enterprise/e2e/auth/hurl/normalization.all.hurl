# Authorization binds to the canonical resource, so the content gate must
# reduce every spelling of a URL (case, percent-encoding, dot-segments,
# extension) to the same registry identity before consulting policy. These
# cases prove no spelling grants access the canonical path would deny, and no
# spelling denies access the canonical path would grant. Every served response
# asserts the full schema body at its canonical id, and every denial asserts the
# full problem document and round-trips it through the error schema.

# ---------------------------------------------------------------------------
# Casing: the gate lowercases, so case cannot bypass or misroute.
# ---------------------------------------------------------------------------

# An uppercased public path serves the public schema at its canonical id
GET {{base}}/PUBLIC/STRING.json
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
  "$id": "{{base}}/public/string",
  "type": "string"
}

# A mixed-case public path is identical
GET {{base}}/Public/String.json
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
  "$id": "{{base}}/public/string",
  "type": "string"
}

# An uppercased apiKey path is still gated: no key is denied, with the full
# problem document, which round-trips through the error schema
GET {{base}}/PRIVATE/SECRET.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
cased_denied_body: body
cased_denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{cased_denied_schema}}
```
{{cased_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The same uppercased apiKey path with the key serves the schema, marked private
GET {{base}}/PRIVATE/SECRET.json
Authorization: Bearer primary-secret-key
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

# An uppercased extension is content negotiation, stripped case-insensitively,
# so it is gated identically
GET {{base}}/private/secret.JSON
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
ext_denied_body: body
ext_denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{ext_denied_schema}}
```
{{ext_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/private/secret.JSON
Authorization: Bearer primary-secret-key
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

# Case does not cross policies: an uppercased internal path needs the internal
# key, not the primary one
GET {{base}}/INTERNAL/CONFIG.json
Authorization: Bearer primary-secret-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
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

GET {{base}}/INTERNAL/CONFIG.json
Authorization: Bearer internal-secret-key
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
  "$id": "{{base}}/internal/config",
  "type": "object",
  "additionalProperties": false
}

# ---------------------------------------------------------------------------
# Percent-encoding: the gate decodes once, so encoding cannot bypass.
# ---------------------------------------------------------------------------

# An encoded public path decodes to the public resource
GET {{base}}/%70ublic/string.json
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
  "$id": "{{base}}/public/string",
  "type": "string"
}

# An encoded apiKey path decodes to the protected resource and is gated
GET {{base}}/%70rivate/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
encoded_denied_body: body
encoded_denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{encoded_denied_schema}}
```
{{encoded_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/%70rivate/secret.json
Authorization: Bearer primary-secret-key
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

# An encoded slash is not a path separator: the request resolves to no servable
# resource and is denied regardless of credential, never serving
GET {{base}}/private%2fsecret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
slash_denied_body: body
slash_denied_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{slash_denied_schema}}
```
{{slash_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/private%2fsecret.json
Authorization: Bearer primary-secret-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# A double-encoded dot is decoded only once, so %252e becomes the literal
# segment %2e rather than a traversal: it stays under the public prefix and
# finds nothing, never reaching the protected sibling
GET {{base}}/public/%252e%252e/private/secret.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
double_encoded_body: body
double_encoded_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{double_encoded_schema}}
```
{{double_encoded_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# ---------------------------------------------------------------------------
# Dot-segments: the gate resolves traversal, so .. cannot escape a prefix.
# ---------------------------------------------------------------------------

# Traversal out of a public prefix into a protected one resolves to the
# protected resource and is gated, not admitted as public
GET {{base}}/public/../private/secret.json
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
traversal_denied_body: body
traversal_denied_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{traversal_denied_schema}}
```
{{traversal_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# With the key the same traversal serves the protected resource at its canonical
# id, proving it really reached /private/secret
GET {{base}}/public/../private/secret.json
Authorization: Bearer primary-secret-key
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

# The public carve-out under the apiKey prefix cannot be a springboard:
# traversal out of /private/open lands back in the protected /private
GET {{base}}/private/open/../secret.json
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
carveout_denied_body: body
carveout_denied_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{carveout_denied_schema}}
```
{{carveout_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Traversal from a protected prefix into a public one resolves to the public
# resource and is served anonymously at its canonical id
GET {{base}}/private/../public/string.json
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
  "$id": "{{base}}/public/string",
  "type": "string"
}

# A single-dot segment is also resolved away before matching
GET {{base}}/private/./secret.json
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
singledot_body: body
singledot_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{singledot_schema}}
```
{{singledot_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Single-encoded dot-segments decode to traversal and resolve the same way
GET {{base}}/public/%2e%2e/private/secret.json
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
encodeddot_body: body
encodeddot_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{encodeddot_schema}}
```
{{encodeddot_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# ---------------------------------------------------------------------------
# Extension as content negotiation: every representation shares one gate.
# ---------------------------------------------------------------------------

# The extensionless resource is gated
GET {{base}}/private/secret
Accept: application/json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
noext_body: body
noext_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{noext_schema}}
```
{{noext_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The JSON representation is gated identically, and admitted by the key
GET {{base}}/private/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
json_body: body
json_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{json_schema}}
```
{{json_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/private/secret.json
Authorization: Bearer primary-secret-key
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

# ---------------------------------------------------------------------------
# The two gates share one policy: a cased or encoded spelling of a locked
# endpoint misses its literal route, falls to the catch-all, and the content
# gate canonicalises it back so the same policy still denies it.
# ---------------------------------------------------------------------------

# The locked health endpoint, reached exactly, is denied at the surface gate
GET {{base}}/self/v1/health
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
surface_body: body
surface_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{surface_schema}}
```
{{surface_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An uppercased spelling misses the literal health route, falls to the catch-all
# and is denied by the same policy via the content gate: byte-identical, no
# bypass between the gates
GET {{base}}/self/v1/HEALTH
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
cased_endpoint_body: body
cased_endpoint_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{cased_endpoint_schema}}
```
{{cased_endpoint_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A percent-encoded spelling behaves the same
GET {{base}}/self/v1/%68ealth
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
encoded_endpoint_body: body
encoded_endpoint_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{encoded_endpoint_schema}}
```
{{encoded_endpoint_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
