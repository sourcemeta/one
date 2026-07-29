# The console sits behind both a machine apiKey policy and a human OIDC policy,
# so an anonymous browser is denied until it logs in
GET {{base}}/console/dashboard.json
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
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

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{anonymous_denied}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The machine key admits the very same console the human session will
GET {{base}}/console/dashboard.json
Authorization: Bearer console-secret-key
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Vary: User-Agent, Accept-Encoding
[Asserts]
header "ETag" matches /^"[0-9a-f]{64}"$/
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/console/dashboard",
  "type": "object",
  "required": [ "widgets" ],
  "properties": {
    "widgets": {
      "type": "array",
      "items": {
        "type": "string"
      }
    }
  },
  "additionalProperties": false
}

# A browser navigating to the console page is shown its login in place, a 401
# whose body is the login page rather than a redirect, while the machine key
# above keeps working: one path, two audiences
GET {{base}}/console/
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
header "Location" not exists
header "Set-Cookie" not exists
xpath "string(//title)" == "Sign In"
xpath "count(//a[@data-sourcemeta-ui-login])" == 1
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@href)" == "/self/v1/auth/login/keycloak"

# Starting a login redirects to the real provider with the transaction cookie
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

# The callback establishes the session and clears the transaction. This login
# named no target, so it lands on what the policy governs
GET {{callback_url}}
HTTP 303
Location: /console
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session" exists
cookie "sourcemeta_one_session[Max-Age]" == 3600
cookie "sourcemeta_one_session[HttpOnly]" exists
cookie "sourcemeta_one_session[SameSite]" == "Lax"
cookie "sourcemeta_one_transaction[Max-Age]" == 0

# The human session now opens the console without any machine key
GET {{base}}/console/dashboard.json
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Vary: User-Agent, Accept-Encoding
[Asserts]
header "ETag" matches /^"[0-9a-f]{64}"$/
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/console/dashboard",
  "type": "object",
  "required": [ "widgets" ],
  "properties": {
    "widgets": {
      "type": "array",
      "items": {
        "type": "string"
      }
    }
  },
  "additionalProperties": false
}

# Logging out clears the session
POST {{base}}/self/v1/auth/logout
HTTP 303
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 2
cookie "sourcemeta_one_session[Max-Age]" == 0
cookie "sourcemeta_one_transaction[Max-Age]" == 0
header "Location" startsWith "https://keycloak:8443/realms/main/protocol/openid-connect/logout?"

# The browser is gated again, the denial byte-identical to the anonymous one
GET {{base}}/console/dashboard.json
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
gated_again: body
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
{{gated_again}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# But the machine key still opens the very same console the human just left
GET {{base}}/console/dashboard.json
Authorization: Bearer console-secret-key
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "ETag" matches /^"[0-9a-f]{64}"$/
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/console/dashboard",
  "type": "object",
  "required": [ "widgets" ],
  "properties": {
    "widgets": {
      "type": "array",
      "items": {
        "type": "string"
      }
    }
  },
  "additionalProperties": false
}
