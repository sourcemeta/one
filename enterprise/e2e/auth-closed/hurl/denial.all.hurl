# This registry is locked in its entirety behind a single apiKey policy at the
# root. Nothing is public: content, listings, the JSON API, search, evaluate,
# the MCP endpoint, the static assets and even the bundled self schemas are all
# closed without a credential. This file probes every surface. What each one
# answers turns on what is being withheld: a registry path is answered as one
# that is not there, while a route this software publishes is refused outright,
# since that route exists on every instance and refusing it gives nothing away.
# Either way the body is the canonical problem document, leaking nothing.

# A schema is not there, and what is answered carries no hint of it
GET {{base}}/catalog/example.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# It round-trips against its error schema. Evaluate is itself private, so the
# round-trip is the one place a credential is presented. Every other answer of
# this shape in this file is byte-identical to the body validated here
POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
Authorization: Bearer full-secret-key
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A schema that carries a title and description answers identically: none of
# that authored content leaks
GET {{base}}/catalog/classified.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# A nested schema answers the same
GET {{base}}/catalog/nested/deep.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# Existence is not probeable: a path that does not exist answers identically to
# one that does, since the gate precedes the existence check
GET {{base}}/catalog/ghost.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# A collection that was never declared answers the same way, so the set of
# collections cannot be discovered by guessing either
GET {{base}}/made-up/anything.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# A cased spelling of a real path normalises to the same locked identity
GET {{base}}/CATALOG/EXAMPLE.JSON
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# The bundle variant (served on the Deno user agent) is gated like everything
# else, so the cross-reference bundle cannot exfiltrate contents either
GET {{base}}/catalog/example.json
User-Agent: Deno/1.0
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# A browser navigating anywhere in a registry gated whole is told there is
# nothing there, the root included. The page names no provider, since choosing
# one is what the login page is for, but the bar carries the way to it. On an
# instance where nothing at all is open that link is the only thing a visitor
# can act on, and it is what makes this answer somewhere to start rather than
# a dead end
GET {{base}}/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Captures]
missing_etag: header "ETag"
[Asserts]
header "Location" not exists
header "Set-Cookie" not exists
header "WWW-Authenticate" not exists
xpath "string(//title)" == "Not Found"
xpath "count(//a[@data-sourcemeta-ui-login])" == 0
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 1
xpath "string(//a[@data-sourcemeta-ui-signin]/@href)" == "/self/v1/auth/login"
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# A nested directory is answered by the byte-identical page, so the whole gated
# tree reads the same from one path to the next
GET {{base}}/catalog/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
header "Set-Cookie" not exists
header "ETag" == {{missing_etag}}

# The same directories requested as JSON answer alike, the root included
GET {{base}}/
Accept: application/json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

GET {{base}}/catalog/
Accept: application/json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# The machine listing is a published route, so it is refused rather than
# reported missing, at every level and including the root
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
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
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
WWW-Authenticate: Bearer realm="registry", resource_metadata="{{base}}/.well-known/oauth-protected-resource/self/v1/mcp"
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
HTTP 404
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

# The challenge is exposed to a browser, not just sent. A cross-origin client
# that cannot read `WWW-Authenticate` sees only that it was refused, never what
# would satisfy the refusal, so the header is named in the exposed list
POST {{base}}/self/v1/mcp
Origin: {{base}}
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {} }
```
HTTP 401
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry", resource_metadata="{{base}}/.well-known/oauth-protected-resource/self/v1/mcp"
Access-Control-Allow-Origin: *
[Asserts]
header "Access-Control-Expose-Headers" contains "WWW-Authenticate"

# The preflight admits the header a credential travels in, on a gated instance
# exactly as on an open one
OPTIONS {{base}}/self/v1/mcp
Origin: {{base}}
Access-Control-Request-Method: POST
Access-Control-Request-Headers: authorization
HTTP 204
Cache-Control: no-store
Access-Control-Allow-Origin: {{base}}
Access-Control-Allow-Methods: POST, OPTIONS
[Asserts]
header "Access-Control-Allow-Headers" == "Authorization, Content-Type, MCP-Protocol-Version"
bytes count == 0
