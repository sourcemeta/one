# A login transaction and a session are both sealed values this instance mints
# for the same policy, and the cookie name that tells them apart travels
# outside the signature where whoever presents it chooses the name. So the two
# are signed under different keys, and a value minted for one purpose is not a
# value for the other. Anyone can obtain a transaction, because starting a
# login requires nothing, which is what once made presenting one as a session
# an unauthenticated way into every gated path.

# The gated schema is denied to a caller carrying nothing
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

# Starting a login hands anyone a freshly sealed transaction, with no
# credential and no exchange with the provider
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
Cache-Control: no-store
[Captures]
transaction: cookie "sourcemeta_one_transaction_keycloak"
[Asserts]
header "Set-Cookie" startsWith "sourcemeta_one_transaction_keycloak="
cookie "sourcemeta_one_transaction_keycloak" exists
cookie "sourcemeta_one_transaction_keycloak[HttpOnly]" exists
cookie "sourcemeta_one_transaction_keycloak[SameSite]" == "Lax"
cookie "sourcemeta_one_transaction_keycloak[Max-Age]" == 600

# Presented as a session it opens nothing, and the denial is the one an
# empty-handed caller gets, so it discloses nothing about why it failed either
GET {{base}}/private/secret.json
Cookie: sourcemeta_one_session_keycloak={{transaction}}
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
body == {{denied_body}}

# Nor does it open any other path the same policy governs
GET {{base}}/archive/record.json
Cookie: sourcemeta_one_session_keycloak={{transaction}}
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
[Asserts]
body == {{denied_body}}

# Nor the gated directory listing
GET {{base}}/private/
Cookie: sourcemeta_one_session_keycloak={{transaction}}
Accept: application/json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
[Asserts]
body == {{denied_body}}

# It is still a valid transaction for the callback that minted it, so the two
# purposes are separated rather than the value being broken outright. Without
# a matching state the callback refuses it, which is the CSRF binding rather
# than the purpose check
GET {{base}}/self/v1/auth/callback/keycloak?state=not-the-sealed-state&code=x
Cookie: sourcemeta_one_transaction_keycloak={{transaction}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
callback_body: body
callback_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{callback_schema}}
```
{{callback_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# And a session cookie is not a transaction either, so the separation holds in
# both directions. A real session is obtained by logging in
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
[Captures]
authorize_url: header "Location"
second_transaction: cookie "sourcemeta_one_transaction_keycloak"

GET {{authorize_url}}
HTTP 200
[Captures]
login_action: xpath "string(//form[@id='kc-form-login']/@action)"

POST {{login_action}}
[FormParams]
username: jane
password: jane-password
HTTP 302
[Captures]
callback_url: header "Location"

GET {{callback_url}}
Cookie: sourcemeta_one_transaction_keycloak={{second_transaction}}
HTTP 303
Location: /private
Cache-Control: no-store
[Captures]
session: cookie "sourcemeta_one_session_keycloak"
[Asserts]
cookie "sourcemeta_one_session_keycloak" exists

# The session it produced opens the gated schema, so the flow works
GET {{base}}/private/secret.json
Cookie: sourcemeta_one_session_keycloak={{session}}
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
[Asserts]
jsonpath "$.type" == "object"

# But presented as a transaction it opens nothing, so the callback refuses it
# exactly as it refuses a caller carrying no transaction at all
GET {{base}}/self/v1/auth/callback/keycloak?state=whatever&code=x
Cookie: sourcemeta_one_transaction_keycloak={{session}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
[Asserts]
body == {{callback_body}}
