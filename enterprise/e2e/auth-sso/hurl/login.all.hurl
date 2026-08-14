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

# An anonymous reader is offered the way in, on an open page and on the page
# for something this view does not hold alike
GET {{base}}/public/
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//a[@data-sourcemeta-ui-signin])" == 1
xpath "string(//a[@data-sourcemeta-ui-signin]/@href)" == "/self/v1/auth/login"
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

GET {{base}}/private/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//a[@data-sourcemeta-ui-signin])" == 1
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

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
header "ETag" exists
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
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept-Encoding
[Asserts]
header "Access-Control-Allow-Origin" not exists
xpath "count(//a[@data-sourcemeta-ui-login])" == 10

# Nothing else is served there
POST {{base}}/self/v1/auth/login
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Allow: GET, HEAD, OPTIONS
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
method_body: body
method_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:method-not-allowed",
  "title": "Method Not Allowed",
  "status": 405,
  "detail": "This HTTP method is invalid for this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{method_schema}}
```
{{method_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

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

# The root listing under the session marks what the session opens as private.
# The lock is drawn from a policy governing an entry, whichever kind of
# credential satisfies it, so an interactive session marks the same entries a
# key would. The public collection stays a plain folder beside them
GET {{base}}/
Accept: text/html
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 2
xpath "count(//tr[.//a[normalize-space(.) = 'private']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'archive']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'public']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'self']]//i[contains(@class, 'bi-folder-fill')])" == 1
# What this session does not open is absent rather than locked, so the mark
# never stands for something being withheld
xpath "count(//tr[.//a[normalize-space(.) = 'team']])" == 0
xpath "count(//tr[.//a[normalize-space(.) = 'corp']])" == 0

# And the data it is drawn from names the policy on each of them
GET {{base}}/self/v1/api/list
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
session_listing: body
session_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/"
jsonpath "$.private" == false
jsonpath "$.entries" count == 4
jsonpath "$.entries[0].name" == "archive"
jsonpath "$.entries[0].private" == true
jsonpath "$.entries[1].name" == "private"
jsonpath "$.entries[1].private" == true
jsonpath "$.entries[2].name" == "public"
jsonpath "$.entries[2].private" == false
jsonpath "$.entries[3].name" == "self"
jsonpath "$.entries[3].private" == false

POST {{base}}/self/v1/api/schemas/evaluate{{session_schema}}
```
{{session_listing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The bar in the view the session resolves to offers the way out. Every page
# written for that view carries it, the listing and a schema page alike
GET {{base}}/private/
Accept: text/html
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 1
xpath "string(//form[button/@data-sourcemeta-ui-signout]/@action)" == "/self/v1/auth/logout"

GET {{base}}/private/secret
Accept: text/html
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 1

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
