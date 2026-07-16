# This registry is locked in its entirety at the root, yet the two endpoints a
# browser must reach before it holds any session, login and callback, stay
# reachable: nothing could present a credential to reach the very login it needs,
# so those two opt out of the content gate and answer for themselves. Neither is
# configured here (there is no interactive policy), so each returns its own
# handler's answer rather than the root's 401, which is the proof that the gate
# let them through. The exemption is exactly that narrow: logout, which is only
# ever useful to a caller whose session already passes the gate, is not exempt
# and rides the gate like any other route.

# The login endpoint is reachable and, finding no such interactive policy,
# answers with its own not-found rather than the root's authentication denial
GET {{base}}/self/v1/auth/login/keycloak
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
login_body: body
login_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "WWW-Authenticate" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# The denial round-trips against its error schema. Evaluate is itself private, so
# the round-trip is the one place a credential is presented
POST {{base}}/self/v1/api/schemas/evaluate{{login_schema}}
Authorization: Bearer full-secret-key
```
{{login_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The callback is reachable too, and with no transaction cookie to vouch for the
# response it fails its own validation instead of the root's authentication gate
GET {{base}}/self/v1/auth/callback/keycloak?code=the-code&state=the-state
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
callback_body: body
[Asserts]
header "Set-Cookie" not exists
header "WWW-Authenticate" not exists
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{login_schema}}
Authorization: Bearer full-secret-key
```
{{callback_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

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

POST {{base}}/self/v1/api/schemas/evaluate{{login_schema}}
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
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{login_schema}}
Authorization: Bearer full-secret-key
```
{{health_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
