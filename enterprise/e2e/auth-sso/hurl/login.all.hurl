# The public catalog is served to everyone, no login required
GET {{base}}/public/string.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/public/string",
  "type": "string"
}

# The private catalog denies an anonymous caller, and the denial validates
# against its error schema
GET {{base}}/private/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
anonymous_denied: body
error_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The archive catalog is governed by that same policy, so it denies identically
GET {{base}}/archive/record.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
archive_denied: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{archive_denied}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{anonymous_denied}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Starting a login discovers the real provider endpoint and redirects there,
# carrying the transaction cookie and the authorization request parameters
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
# with an authorization code
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

# The callback redeems the code, establishes the session, and clears the
# transaction. This login named no target, so it lands on what the policy
# governs rather than the instance root. The policy governs two collections and
# lands on the one it declares first, which is not the one that sorts first
GET {{callback_url}}
HTTP 303
Location: /private
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session_keycloak" exists
cookie "sourcemeta_one_session_keycloak[Max-Age]" == 3600
cookie "sourcemeta_one_session_keycloak[HttpOnly]" exists
cookie "sourcemeta_one_session_keycloak[SameSite]" == "Lax"
cookie "sourcemeta_one_transaction_keycloak[Max-Age]" == 0

# The session now opens the private catalog that was denied before, served
# private so a shared cache cannot retain it
GET {{base}}/private/secret.json
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/private/secret",
  "type": "object"
}

# The one session opens every collection the policy governs, not only the one
# the login landed on
GET {{base}}/archive/record.json
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
  "$id": "{{base}}/archive/record",
  "type": "array"
}

# Logging out clears the session
GET {{base}}/self/v1/auth/logout
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
header "Set-Cookie" startsWith "sourcemeta_one_session_keycloak="

# And the private catalog is gated again, the denial byte-identical to the first
GET {{base}}/private/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
final_denied: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{final_denied}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
