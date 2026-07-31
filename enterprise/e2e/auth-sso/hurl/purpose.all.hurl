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
Access-Control-Expose-Headers: Link, ETag, WWW-Authenticate
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
transaction: cookie "sourcemeta_one_transaction"
[Asserts]
header "Set-Cookie" startsWith "sourcemeta_one_transaction="
cookie "sourcemeta_one_transaction" exists
cookie "sourcemeta_one_transaction[HttpOnly]" exists
cookie "sourcemeta_one_transaction[SameSite]" == "Lax"
cookie "sourcemeta_one_transaction[Max-Age]" == 600

# Presented as a session it opens nothing, and the denial is the one an
# empty-handed caller gets, so it discloses nothing about why it failed either
GET {{base}}/private/secret.json
Cookie: sourcemeta_one_session={{transaction}}
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
replayed_body: body
replayed_schema: header "Link" regex "<([^>]+)>"
[Asserts]
body == {{denied_body}}
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{replayed_schema}}
```
{{replayed_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Nor does it open any other path the same policy governs
GET {{base}}/archive/record.json
Cookie: sourcemeta_one_session={{transaction}}
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
archive_body: body
archive_schema: header "Link" regex "<([^>]+)>"
[Asserts]
body == {{denied_body}}
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{archive_schema}}
```
{{archive_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Nor the gated directory listing
GET {{base}}/private/
Cookie: sourcemeta_one_session={{transaction}}
Accept: application/json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
listing_body: body
listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
body == {{denied_body}}
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{listing_schema}}
```
{{listing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# It is still a valid transaction for the callback that minted it, so the two
# purposes are separated rather than the value being broken outright. Without
# a matching state the callback refuses it, which is the CSRF binding rather
# than the purpose check
GET {{base}}/self/v1/auth/callback/keycloak?state=not-the-sealed-state&code=x
Cookie: sourcemeta_one_transaction={{transaction}}
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
second_transaction: cookie "sourcemeta_one_transaction"

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
Cookie: sourcemeta_one_transaction={{second_transaction}}
HTTP 303
Location: /private
Cache-Control: no-store
[Captures]
session: cookie "sourcemeta_one_session"
[Asserts]
cookie "sourcemeta_one_session" exists

# The session it produced opens the gated schema, so the flow works
GET {{base}}/private/secret.json
Cookie: sourcemeta_one_session={{session}}
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/private/secret",
  "type": "object"
}

# But presented as a transaction it opens nothing, so the callback refuses it
# exactly as it refuses a caller carrying no transaction at all
GET {{base}}/self/v1/auth/callback/keycloak?state=whatever&code=x
Cookie: sourcemeta_one_transaction={{session}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
session_as_transaction_body: body
session_as_transaction_schema: header "Link" regex "<([^>]+)>"
[Asserts]
body == {{callback_body}}
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{session_as_transaction_schema}}
```
{{session_as_transaction_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
