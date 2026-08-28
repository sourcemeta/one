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

# A policy signing people in through a deployment is an interactive policy like
# any other, so it grows the way in and appears on the login page
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

GET {{base}}/self/v1/auth/login
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/auth/login/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
login_body: body
login_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Set-Cookie" not exists
jsonpath "$.title" == "GitHub Sandbox"
jsonpath "$.providers" count == 5
jsonpath "$.providers[0].name" == "engineering"
jsonpath "$.providers[0].title" == "GitHub"
jsonpath "$.providers[0].path" == "/self/v1/auth/login/engineering"
jsonpath "$.providers[1].name" == "platform"
jsonpath "$.providers[2].name" == "personal"
jsonpath "$.providers[3].name" == "corp"
jsonpath "$.providers[4].name" == "no-client-secret"

POST {{base}}/self/v1/api/schemas/evaluate{{login_schema}}
```
{{login_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The private catalog is not there for an anonymous caller
GET {{base}}/private/secret.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
error_schema: header "Link" regex "<([^>]+)>"
anonymous_denied: body
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
{{anonymous_denied}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A deployment publishes nothing to discover, so where a login begins is
# composed from the origin the policy names rather than fetched. It carries no
# nonce, since nothing in this flow issues a token to bind one to, and it asks
# for exactly the scope the policy's one rule needs
GET {{base}}/self/v1/auth/login/engineering
HTTP 303
Cache-Control: no-store
[Captures]
authorize_url: header "Location"
[Asserts]
header "Location" startsWith "https://github:9443/login/oauth/authorize?"
header "Location" contains "response_type=code"
header "Location" contains "client_id=registry"
header "Location" contains "scope=read%3Aorg"
header "Location" contains "code_challenge_method=S256"
header "Location" contains "state="
header "Location" not contains "nonce="
header "Location" not contains "prompt="
header "Set-Cookie" startsWith "sourcemeta_one_transaction="
cookie "sourcemeta_one_session" not exists
cookie "sourcemeta_one_renewal" not exists
cookie "sourcemeta_one_transaction[Max-Age]" == 600
cookie "sourcemeta_one_transaction[HttpOnly]" exists
cookie "sourcemeta_one_transaction[SameSite]" == "Lax"

# The deployment serves the authorization endpoint under the two paths GitHub
# serves it at, and takes the browser on to where it authenticates people
GET {{authorize_url}}
HTTP 302
[Captures]
provider_url: header "Location"
[Asserts]
header "Location" startsWith "https://github:9443/realms/main/protocol/openid-connect/auth?"

GET {{provider_url}}
HTTP 200
[Captures]
login_action: xpath "string(//form[@id='kc-form-login']/@action)"

# Submitting the account's credentials sends the browser back to the callback
# with an authorization code
POST {{login_action}}
[FormParams]
username: octocat
password: octocat-password
HTTP 302
[Captures]
callback_url: header "Location"
[Asserts]
header "Location" startsWith "http://localhost:8000/self/v1/auth/callback/engineering?"
header "Location" contains "code="
header "Location" contains "state="

# The callback redeems the code, asks the deployment who the token was issued
# for and what they belong to, and establishes the session.
#
# Two cookies rather than three: the session and the spent transaction. A
# deployment cannot be asked whether a sign-in still stands without showing the
# person its own pages, so nothing here earns a browser a silent renewal
GET {{callback_url}}
HTTP 303
Location: /private
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 2
cookie "sourcemeta_one_session" exists
cookie "sourcemeta_one_session[Max-Age]" == 3600
cookie "sourcemeta_one_session[HttpOnly]" exists
cookie "sourcemeta_one_session[SameSite]" == "Lax"
cookie "sourcemeta_one_transaction[Max-Age]" == 0
cookie "sourcemeta_one_renewal" not exists

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

# The root listing under the session marks what the session opens as private,
# and what it does not open is absent rather than locked
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
xpath "count(//tr[.//a[normalize-space(.) = 'team']])" == 0
xpath "count(//tr[.//a[normalize-space(.) = 'corp']])" == 0
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 1

# A deployment offers nowhere to end its own session, so signing out clears
# what this instance minted and leaves the browser here rather than sending it
# on. The renewal cookie is expired alongside the rest, though no login through
# a deployment ever set one
POST {{base}}/self/v1/auth/logout
HTTP 303
Cache-Control: no-store
Location: /
[Asserts]
header "Set-Cookie" count == 3
cookie "sourcemeta_one_session[Max-Age]" == 0
cookie "sourcemeta_one_transaction[Max-Age]" == 0
cookie "sourcemeta_one_renewal[Max-Age]" == 0

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
