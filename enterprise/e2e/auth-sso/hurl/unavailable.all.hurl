# The login page names every policy governing the path it fronts, so which
# policies exist is published rather than secret. How the deployment is doing is
# not. A caller who has authenticated to nothing must not be able to ask a login
# endpoint whether an operator left a secret unset, whether the identity
# provider is answering, or which of the two went wrong, so every reason a login
# cannot start answers the same way.
#
# The sandbox gates one collection with two policies that are each missing
# exactly one secret, and gates another with a provider this instance refuses to
# talk to. Those are three different causes reached through three different
# checks, so an identical response to all three is the property under test.

# A policy whose client secret is unset. The login refuses before it goes
# anywhere near the provider
GET {{base}}/self/v1/auth/login/no-client-secret
HTTP 500
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
uniform_body: body
uniform_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
header "WWW-Authenticate" not exists
{
  "type": "urn:sourcemeta:one:auth-unavailable",
  "title": "Internal Server Error",
  "status": 500,
  "detail": "This login cannot be started"
}

POST {{base}}/self/v1/api/schemas/evaluate{{uniform_schema}}
```
{{uniform_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A policy whose session secret is unset. This one has a usable client secret
# and a provider that answers, so it gets as far as sealing the transaction and
# fails there, several checks further in than the one above
GET {{base}}/self/v1/auth/login/no-session-secret
HTTP 500
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
sealing_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
header "WWW-Authenticate" not exists
body == {{uniform_body}}
{
  "type": "urn:sourcemeta:one:auth-unavailable",
  "title": "Internal Server Error",
  "status": 500,
  "detail": "This login cannot be started"
}

POST {{base}}/self/v1/api/schemas/evaluate{{uniform_schema}}
```
{{sealing_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A provider this instance will not talk to, refused on what its own discovery
# document says rather than on anything configured here. A third cause, still
# byte-identical
GET {{base}}/self/v1/auth/login/cleartext
HTTP 500
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
provider_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{uniform_body}}
{
  "type": "urn:sourcemeta:one:auth-unavailable",
  "title": "Internal Server Error",
  "status": 500,
  "detail": "This login cannot be started"
}

POST {{base}}/self/v1/api/schemas/evaluate{{uniform_schema}}
```
{{provider_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A policy name that does not exist stays a plain 404, which is the one
# distinction worth keeping: the response for a name the login page never
# offered says nothing about this deployment, and answering a published login
# link with 404 would be a worse lie than withholding the cause
GET {{base}}/self/v1/auth/login/not-a-policy
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
unknown_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{uniform_schema}}
```
{{unknown_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A working policy still redirects, so the uniform refusal above is about the
# broken policies and not about the endpoint having stopped working
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" startsWith "https://keycloak:8443/realms/main/protocol/openid-connect/auth?"
cookie "sourcemeta_one_transaction" exists

# The collection those two broken policies gate is still gated. A login that
# cannot start must not leave the path it governs open
GET {{base}}/unavailable/thing.json
Accept: application/json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
denied_body: body
[Asserts]
header "ETag" not exists
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{uniform_schema}}
```
{{denied_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A browser reaching that collection is shown the login page naming both broken
# policies, which is what makes their existence public and the refusal above
# the only thing being withheld
GET {{base}}/unavailable/
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
header "Location" not exists
header "Set-Cookie" not exists
xpath "string(//title)" == "Sign In"
xpath "string(//h1)" == "SSO Sandbox"
xpath "count(//a[@data-sourcemeta-ui-login])" == 3
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@data-sourcemeta-ui-login)" == "no-client-secret"
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@href)" == "/self/v1/auth/login/no-client-secret"
xpath "normalize-space((//a[@data-sourcemeta-ui-login])[1])" == "no-client-secret"
xpath "string((//a[@data-sourcemeta-ui-login])[2]/@data-sourcemeta-ui-login)" == "no-session-secret"
xpath "string((//a[@data-sourcemeta-ui-login])[2]/@href)" == "/self/v1/auth/login/no-session-secret"
xpath "normalize-space((//a[@data-sourcemeta-ui-login])[2])" == "no-session-secret"
xpath "string((//a[@data-sourcemeta-ui-login])[3]/@data-sourcemeta-ui-login)" == "short-session-secret"
xpath "string((//a[@data-sourcemeta-ui-login])[3]/@href)" == "/self/v1/auth/login/short-session-secret"
xpath "normalize-space((//a[@data-sourcemeta-ui-login])[3])" == "short-session-secret"

# HEAD and GET agree, so the refusal is not a property of having a body
HEAD {{base}}/self/v1/auth/login/no-client-secret
HTTP 500
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists

# The refusal is reached on the login's own terms rather than through the
# route's method handling, so an unsupported method still answers as one
POST {{base}}/self/v1/auth/login/no-client-secret
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Allow: GET, HEAD, OPTIONS
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
method_body: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:method-not-allowed",
  "title": "Method Not Allowed",
  "status": 405,
  "detail": "This HTTP method is invalid for this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{uniform_schema}}
```
{{method_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# OPTIONS is answered before any policy is looked at, so a broken policy and a
# working one advertise the same methods
OPTIONS {{base}}/self/v1/auth/login/no-client-secret
HTTP 204
Cache-Control: no-store
Allow: GET, HEAD, OPTIONS
[Asserts]
header "Set-Cookie" not exists

# A query string cannot talk the endpoint past the refusal, so the return
# target a working login honours buys nothing on a broken one
GET {{base}}/self/v1/auth/login/no-client-secret?to=/unavailable/thing
HTTP 500
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
targeted_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{uniform_body}}
{
  "type": "urn:sourcemeta:one:auth-unavailable",
  "title": "Internal Server Error",
  "status": 500,
  "detail": "This login cannot be started"
}

POST {{base}}/self/v1/api/schemas/evaluate{{uniform_schema}}
```
{{targeted_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A policy whose session secret is set but too short to sign with cannot mint a
# cookie either, and says no more about itself than the others. A secret short
# enough to be found offline from a single captured cookie would let anybody
# holding one mint sessions of their own, so it is refused where it is read
GET {{base}}/self/v1/auth/login/short-session-secret
HTTP 500
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
short_secret_body: body
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
body == {{uniform_body}}
{
  "type": "urn:sourcemeta:one:auth-unavailable",
  "title": "Internal Server Error",
  "status": 500,
  "detail": "This login cannot be started"
}

POST {{base}}/self/v1/api/schemas/evaluate{{uniform_schema}}
```
{{short_secret_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A policy naming two secrets signs with the first and starts a login normally,
# so naming more than one costs nothing when none of them has been retired yet
GET {{base}}/self/v1/auth/login/rotated
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" startsWith "https://keycloak:8443/realms/main/protocol/openid-connect/auth?"
cookie "sourcemeta_one_transaction" exists
cookie "sourcemeta_one_transaction[HttpOnly]" exists
cookie "sourcemeta_one_transaction[SameSite]" == "Lax"
