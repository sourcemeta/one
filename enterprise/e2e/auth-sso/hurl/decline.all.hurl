# The provider answers a login by sending the browser back here, either with an
# authorization code or with a refusal. That makes this the one URL a stranger
# can aim somebody else's browser at, so nothing the provider is said to have
# answered is acted on until the callback has proven it belongs to a login this
# instance started. The sealed transaction names the state the provider will
# echo, and only a request echoing it gets past, which is what RFC 6749 Section
# 4.1.2.1 asks of a client. A refusal is held to the same proof as a code,
# because otherwise a page belonging to somebody else could make a person's
# login fail on their behalf.

# A login mints the transaction and names the state, which is everything the
# callback is checked against
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
Cache-Control: no-store
[Captures]
transaction: cookie "sourcemeta_one_transaction"
state: header "Location" regex "[?&]state=([A-Za-z0-9_-]+)"
[Asserts]
cookie "sourcemeta_one_transaction" exists
cookie "sourcemeta_one_transaction[HttpOnly]" exists
cookie "sourcemeta_one_transaction[SameSite]" == "Lax"
cookie "sourcemeta_one_transaction[Max-Age]" == 600

# A code presented against a state the transaction does not name gets no
# further than the gate, and the refusal describes nothing about what was wrong
GET {{base}}/self/v1/auth/callback/keycloak?code=a-code-nobody-issued&state=a-state-nobody-minted
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
refused_body: body
error_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
header "ETag" not exists
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{refused_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The very same request against the state the transaction does name reaches the
# provider, which refuses the invented code. Nothing but the state differs
# between this and the one before it, so the state is the whole of what stands
# between a stranger's callback and a token exchange
GET {{base}}/self/v1/auth/callback/keycloak?code=a-code-nobody-issued&state={{state}}
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 502
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
exchange_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
{
  "type": "urn:sourcemeta:one:auth-exchange-failed",
  "title": "Bad Gateway",
  "status": 502,
  "detail": "The authorization code could not be redeemed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{exchange_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A refusal carried by a proven callback is honoured, and answered as the
# provider's decision rather than as a denial of a resource, so somebody who
# declined at the provider is told what happened
GET {{base}}/self/v1/auth/callback/keycloak?error=access_denied&state={{state}}
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
declined_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
header "ETag" not exists
{
  "type": "urn:sourcemeta:one:auth-login-declined",
  "title": "Forbidden",
  "status": 403,
  "detail": "The identity provider declined the login"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{declined_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The same refusal against a state the transaction does not name is not
# honoured, and is refused exactly as an unproven code is, so a page belonging
# to somebody else cannot end a login in progress
GET {{base}}/self/v1/auth/callback/keycloak?error=access_denied&state=a-state-nobody-minted
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
unproven_decline_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{refused_body}}
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{unproven_decline_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# An answer claiming a code and a refusal at once contradicts itself, and the
# refusal decides, so nothing is exchanged on the strength of a code the same
# answer says was not granted
GET {{base}}/self/v1/auth/callback/keycloak?code=a-code-nobody-issued&error=access_denied&state={{state}}
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
contradictory_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{declined_body}}
{
  "type": "urn:sourcemeta:one:auth-login-declined",
  "title": "Forbidden",
  "status": 403,
  "detail": "The identity provider declined the login"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{contradictory_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A refusal names itself with an error code, so one naming nothing is not a
# decision to report as the provider's. It is no grant either, and a code
# arriving beside it is left unredeemed rather than read as the answer
GET {{base}}/self/v1/auth/callback/keycloak?code=a-code-nobody-issued&error=&state={{state}}
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
nameless_refusal_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{refused_body}}
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{nameless_refusal_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A proven callback carrying neither a code nor a refusal is malformed, and is
# answered in the terms an unproven one is, so a stranger learns the same
# either way
GET {{base}}/self/v1/auth/callback/keycloak?state={{state}}
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
neither_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{refused_body}}
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{neither_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# An empty state names nothing, including the transaction the browser is
# carrying, rather than reading as the absence of a requirement
GET {{base}}/self/v1/auth/callback/keycloak?error=access_denied&state=
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
empty_state_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{refused_body}}
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{empty_state_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The state is compared as the provider would echo it rather than loosely, so a
# value that merely starts with the one the transaction names is not it
GET {{base}}/self/v1/auth/callback/keycloak?error=access_denied&state={{state}}%20
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
extended_state_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{refused_body}}
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{extended_state_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A transaction is sealed under the policy that minted it, so another policy's
# callback opens nothing from it, even carrying the state it does name. Which
# policy a callback belongs to is therefore not something the caller chooses by
# aiming at a different URL
GET {{base}}/self/v1/auth/callback/cleartext?error=access_denied&state={{state}}
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
foreign_policy_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{refused_body}}
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{foreign_policy_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Every refusal above left the transaction where it was, which is what lets it
# still be honoured here after all of them. Clearing it on a refusal would hand
# anybody who can aim a browser at this URL a way to cancel a login in progress
GET {{base}}/self/v1/auth/callback/keycloak?error=access_denied&state={{state}}
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
surviving_body: body
[Asserts]
header "Set-Cookie" not exists
body == {{declined_body}}
{
  "type": "urn:sourcemeta:one:auth-login-declined",
  "title": "Forbidden",
  "status": 403,
  "detail": "The identity provider declined the login"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{surviving_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
