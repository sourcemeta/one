# The whole registry is locked, and a third key now opens it: an interactive
# human session, alongside the apiKey and the JWT. This proves the one endpoint
# admits all three, and that the interactive login itself is reachable through
# the root gate that covers every path, the login and callback endpoints
# included, because those two vouch for themselves.

# The catalog denies an anonymous browser, like everything else here
GET {{base}}/catalog/example.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
anonymous_denied: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
Authorization: Bearer full-secret-key
```
{{anonymous_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The login endpoint is covered by the root gate, yet stays reachable and
# redirects to the provider, carrying the transaction cookie
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
Cache-Control: no-store
[Captures]
authorize_url: header "Location" replace "http://keycloak:8080" "http://localhost:8080"
[Asserts]
header "Location" startsWith "http://keycloak:8080/realms/main/protocol/openid-connect/auth?"
header "Location" contains "response_type=code"
header "Location" contains "client_id=registry"
header "Location" contains "scope=openid"
header "Location" contains "code_challenge_method=S256"
header "Location" contains "state="
header "Location" contains "nonce="
header "Set-Cookie" startsWith "sourcemeta_one_transaction_keycloak="

# The provider renders its login form
GET {{authorize_url}}
HTTP 200
[Captures]
login_action: xpath "string(//form[@id='kc-form-login']/@action)" replace "http://keycloak:8080" "http://localhost:8080"

# Submitting the user's credentials sends the browser back to the callback
POST {{login_action}}
[FormParams]
username: jane
password: jane-password
HTTP 302
[Captures]
callback_url: header "Location"
[Asserts]
header "Location" startsWith "http://localhost:8000/self/v1/auth/callback/keycloak?"
header "Location" contains "code="
header "Location" contains "state="

# The callback, also covered by the root gate yet reachable, establishes the
# session and clears the transaction
GET {{callback_url}}
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session_keycloak" exists
cookie "sourcemeta_one_session_keycloak[Max-Age]" == 3600
cookie "sourcemeta_one_session_keycloak[HttpOnly]" exists
cookie "sourcemeta_one_session_keycloak[SameSite]" == "Lax"
cookie "sourcemeta_one_transaction_keycloak[Max-Age]" == 0

# The human session now opens the locked catalog, no machine credential in sight
GET {{base}}/catalog/example.json
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/catalog/example",
  "type": "string"
}

# The apiKey opens the very same resource, so machine and human coexist on one
# fully-locked path
GET {{base}}/catalog/example.json
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/catalog/example",
  "type": "string"
}

# Logout is not exempt, but the live session passes the gate, so a logged-in
# browser reaches it and its session is cleared
GET {{base}}/self/v1/auth/logout
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
header "Set-Cookie" startsWith "sourcemeta_one_session_keycloak="

# With the session gone the catalog is locked to the browser again, the denial
# byte-identical to the anonymous one at the top
GET {{base}}/catalog/example.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
locked_again: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
Authorization: Bearer full-secret-key
```
{{locked_again}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
