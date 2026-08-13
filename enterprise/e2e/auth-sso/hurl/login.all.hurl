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

# The instance holds one login page, at a URL of its own, public and identical
# for everybody. It names every interactive policy declared, whichever path
# each one governs, since somebody signing in has not reached a path yet
GET {{base}}/self/v1/auth/login
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept-Encoding
[Asserts]
header "Set-Cookie" not exists
header "Access-Control-Allow-Origin" not exists
header "ETag" matches /^"[0-9a-f]{64}"$/
xpath "string(//title)" == "Sign In"
xpath "string(//h1)" == "SSO Sandbox"
xpath "string(//p[@class='text-secondary text-center small mb-4'])" == "Choose how you want to sign in"
xpath "count(//a[@data-sourcemeta-ui-login])" == 10
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@data-sourcemeta-ui-login)" == "keycloak"
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@href)" == "/self/v1/auth/login/keycloak"
xpath "normalize-space((//a[@data-sourcemeta-ui-login])[1])" == "keycloak"
xpath "string((//a[@data-sourcemeta-ui-login])[10]/@data-sourcemeta-ui-login)" == "rotated"

# It is the same page whoever asks for it, so a shared cache may hold it
GET {{base}}/self/v1/auth/login
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//a[@data-sourcemeta-ui-login])" == 10

# Nothing else is served there
POST {{base}}/self/v1/auth/login
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Allow: GET, HEAD, OPTIONS
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:method-not-allowed",
  "title": "Method Not Allowed",
  "status": 405,
  "detail": "This HTTP method is invalid for this URL"
}

# The private catalog is not there for an anonymous caller, and what it is told
# validates against its error schema
GET {{base}}/private/secret.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
anonymous_denied: body
error_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# The archive catalog is governed by that same policy, so it answers alike
GET {{base}}/archive/record.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
archive_denied: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
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
authorize_url: header "Location"
[Asserts]
header "Location" startsWith "https://keycloak:8443/realms/main/protocol/openid-connect/auth?"
header "Location" contains "response_type=code"
header "Location" contains "client_id=registry"
header "Location" contains "scope=openid"
header "Location" contains "code_challenge_method=S256"
header "Location" contains "state="
header "Location" contains "nonce="
header "Set-Cookie" startsWith "sourcemeta_one_transaction="

# The provider renders its login form
GET {{authorize_url}}
HTTP 200
[Captures]
login_action: xpath "string(//form[@id='kc-form-login']/@action)"

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
cookie "sourcemeta_one_session" exists
cookie "sourcemeta_one_session[Max-Age]" == 3600
cookie "sourcemeta_one_session[HttpOnly]" exists
cookie "sourcemeta_one_session[SameSite]" == "Lax"
cookie "sourcemeta_one_transaction[Max-Age]" == 0

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
  "type": "object",
  "properties": {
    "label": {
      "$ref": "../public/string"
    }
  }
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
  "type": "array",
  "items": {
    "$ref": "../private/secret"
  }
}

# Logging out clears the session and sends the browser on to the provider so
# its own session ends too, which logout.all.hurl covers in full
POST {{base}}/self/v1/auth/logout
HTTP 303
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 3
cookie "sourcemeta_one_session[Max-Age]" == 0
cookie "sourcemeta_one_transaction[Max-Age]" == 0
cookie "sourcemeta_one_renewal[Max-Age]" == 0
header "Location" startsWith "https://keycloak:8443/realms/main/protocol/openid-connect/logout?"

# And the private catalog is gone again, byte-identical to the first time
GET {{base}}/private/secret.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
final_denied: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
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
