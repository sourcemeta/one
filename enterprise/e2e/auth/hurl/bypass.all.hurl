# None of these spellings of an apiKey path may bypass the gate. Each one that
# canonicalises onto the protected resource is denied exactly as the canonical
# path would be, and a spelling that resolves elsewhere never reaches it.

# Upper-cased prefix
GET {{base}}/PRIVATE/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Repeated leading slash
GET {{base}}//private/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Repeated interior slash
GET {{base}}/private//secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Single-dot segment
GET {{base}}/private/./secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Leading single-dot segment
GET {{base}}/./private/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Parent traversal from a public prefix into an apiKey prefix
GET {{base}}/public/../private/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Percent-encoded slash: not a path separator, so this is a single literal
# segment at the public root that names no resource. It answers a plain 404
# rather than a disguised entry into the apiKey scope
GET {{base}}/private%2Fsecret.json
HTTP 404
Content-Type: application/problem+json
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.status" == 404

# Parent traversal that lands back on a public prefix is still anonymous
GET {{base}}/private/../public/string.json
HTTP 200
Content-Type: application/schema+json
[Asserts]
jsonpath "$.type" == "string"

# Repeated leading slash combined with an upper-cased prefix. Each half of
# this spelling is covered above on its own, and the pair once reached the
# policy trie in a spelling the trie never holds, which left the resource open
GET {{base}}//PRIVATE/secret.json
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
upper_slash_body: body
upper_slash_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
```
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}
```

POST {{base}}/self/v1/api/schemas/evaluate{{upper_slash_schema}}
```
{{upper_slash_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Three leading slashes with an upper-cased prefix
GET {{base}}///PRIVATE/secret.json
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
triple_slash_body: body
triple_slash_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
```
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}
```

POST {{base}}/self/v1/api/schemas/evaluate{{triple_slash_schema}}
```
{{triple_slash_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Repeated leading slash, upper case, and a dot segment together
GET {{base}}//PRIVATE/./secret.json
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
upper_dot_body: body
upper_dot_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
```
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}
```

POST {{base}}/self/v1/api/schemas/evaluate{{upper_dot_schema}}
```
{{upper_dot_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An upper-cased extension names the same representation of the same resource
GET {{base}}//PRIVATE/SECRET.JSON
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
upper_extension_body: body
upper_extension_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
```
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}
```

POST {{base}}/self/v1/api/schemas/evaluate{{upper_extension_schema}}
```
{{upper_extension_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A JWT-gated prefix is reached through the same spelling, and denied the same
GET {{base}}//MACHINE/config.json
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
upper_jwt_body: body
upper_jwt_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
```
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}
```

POST {{base}}/self/v1/api/schemas/evaluate{{upper_jwt_schema}}
```
{{upper_jwt_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The same spelling admits the caller that holds the key, so the gate reads the
# location the policy governs rather than merely refusing anything unusual
GET {{base}}//PRIVATE/secret.json
[Options]
path-as-is: true
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
[Asserts]
header "ETag" exists
jsonpath "$.$schema" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.$id" == "http://localhost:8000/private/secret"
jsonpath "$.type" == "object"
jsonpath "$.required" count == 1
jsonpath "$.required[0]" == "token"
jsonpath "$.properties.token.type" == "string"

# The gated directory listing is denied through the same spelling
GET {{base}}//PRIVATE/
[Options]
path-as-is: true
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
upper_directory_body: body
upper_directory_schema: header "Link" regex "<([^>]+)>"
```
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}
```

POST {{base}}/self/v1/api/schemas/evaluate{{upper_directory_schema}}
```
{{upper_directory_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
