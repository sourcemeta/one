# This registry is locked in its entirety behind a single apiKey policy at the
# root. Nothing is public: content, listings, the JSON API, search, evaluate, the
# MCP endpoint, the static assets and even the bundled self schemas all deny
# without a credential. This file probes every surface and asserts the denial is
# the canonical problem document, leaking nothing about what it hides.

# A schema denies, and the denial carries no hint of the schema
GET {{base}}/catalog/example.json
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

# The denial round-trips against its error schema. Evaluate is itself private, so
# the round-trip is the one place a credential is presented. All other denials in
# this file are byte-identical to the body validated here
POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
Authorization: Bearer full-secret-key
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A schema that carries a title and description denies identically: none of that
# authored content leaks
GET {{base}}/catalog/classified.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# A nested schema denies
GET {{base}}/catalog/nested/deep.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# Existence is not probeable: a path that does not exist denies identically to
# one that does, since the gate precedes the existence check
GET {{base}}/catalog/ghost.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# A collection that was never declared denies the same way, so the set of
# collections cannot be discovered by guessing either
GET {{base}}/made-up/anything.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# A cased spelling of a real path normalises to the same locked identity
GET {{base}}/CATALOG/EXAMPLE.JSON
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The bundle variant (served on the Deno user agent) is gated like everything
# else, so the cross-reference bundle cannot exfiltrate contents either
GET {{base}}/catalog/example.json
User-Agent: Deno/1.0
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# A browser navigating to a gated page is sent to begin a login rather than
# shown a denial, since a single interactive policy governs the whole registry
GET {{base}}/
Accept: text/html
HTTP 303
Location: /self/v1/auth/login/keycloak?to=%2F
Cache-Control: no-store
[Asserts]
header "Set-Cookie" not exists

GET {{base}}/catalog/
Accept: text/html
HTTP 303
Location: /self/v1/auth/login/keycloak?to=%2Fcatalog%2F
Cache-Control: no-store
[Asserts]
header "Set-Cookie" not exists

# The same directories requested as JSON deny identically, the root included
GET {{base}}/
Accept: application/json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

GET {{base}}/catalog/
Accept: application/json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The machine listing denies at every level, including the root
GET {{base}}/self/v1/api/list
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

GET {{base}}/self/v1/api/list/catalog
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The per-schema API surfaces deny
GET {{base}}/self/v1/api/schemas/health/catalog/example
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# Search denies, so the index cannot be queried for protected titles
GET {{base}}/self/v1/api/schemas/search?q=a
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# Evaluate denies anonymously, so it cannot be used as an oracle over private
# schemas
POST {{base}}/self/v1/api/schemas/evaluate/catalog/example
Content-Type: application/json
```
"hello"
```
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# Even the bundled self schemas, normally always public, are private here because
# the root policy covers them and there is no public carve-out
GET {{base}}/self/v1/schemas/api/error
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# Static assets bypass the content gate, so the root surface policy is their only
# protection: the bundled web UI is locked too
GET {{base}}/self/v1/static/style.min.css
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The MCP endpoint denies without a credential, so an anonymous client cannot
# even initialise a session
POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {} }
```
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# HEAD is gated exactly like GET and per RFC 9110 carries no body
HEAD {{base}}/catalog/example.json
HTTP 401
WWW-Authenticate: Bearer realm="registry"
[Asserts]
bytes count == 0

# The CORS preflight carries no credential and is exempt from the gate
OPTIONS {{base}}/catalog/example.json
Origin: http://example.com
Access-Control-Request-Method: GET
HTTP 204
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, HEAD, OPTIONS
Cache-Control: no-store
