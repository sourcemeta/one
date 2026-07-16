# This registry is locked in its entirety at the root, so the root gate covers
# every path, the interactive authentication endpoints included. Login and
# callback nonetheless answer for themselves rather than the root's 401: a
# browser could never present a credential to reach the very login it needs, so
# those two opt out of the content gate and guard themselves instead. The
# exemption is exactly that narrow: logout, which is only ever useful to a caller
# whose session already passes the gate, is not exempt and rides the gate like
# any other route, as does every non-authentication surface.

# The callback is reachable, and running first, before any login has set a
# transaction cookie, it has nothing to vouch for the response, so it fails its
# own validation instead of the root's authentication gate
GET {{base}}/self/v1/auth/callback/keycloak?code=the-code&state=the-state
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
callback_body: body
error_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "WWW-Authenticate" not exists
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
Authorization: Bearer full-secret-key
```
{{callback_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The login endpoint is reachable through the root gate and initiates the flow,
# redirecting to the provider with a transaction cookie, rather than denying 401
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" startsWith "http://keycloak:8080/realms/main/protocol/openid-connect/auth?"
header "Location" contains "client_id=registry"
header "Set-Cookie" startsWith "sourcemeta_one_transaction_keycloak="
header "WWW-Authenticate" not exists

# Logout is not exempt: a session that could log out would itself pass the gate,
# so anonymous here the route denies like any other, leaking no session cookie
GET {{base}}/self/v1/auth/logout
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
logout_body: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
Authorization: Bearer full-secret-key
```
{{logout_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The exemption is narrow in the other direction too: a neighbouring internal
# route that is not an authentication entry point stays behind the root gate
GET {{base}}/self/v1/health
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
health_body: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
Authorization: Bearer full-secret-key
```
{{health_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
