# A session lasts an hour, and asking somebody to click their provider again
# every hour is the cost of that. A browser that has signed in before carries a
# marker naming the policy it used, so a denial can ask the provider whether
# that sign-in still stands instead of asking the person. The provider answers
# without showing anything when it can, so the hour passes unnoticed.
#
# The marker is not a credential. It names a policy and nothing else, and
# whoever holds it can only start a login they were free to start anyway.

# A denied browser navigation carrying the marker is sent to its provider
# rather than shown the sign-in page. The page it was denied travels along, so
# the renewal lands where the person already was
GET {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal=keycloak
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" == "/self/v1/auth/login/keycloak?silent=1&to=%2Fprivate%2F"
header "Set-Cookie" not exists
header "WWW-Authenticate" not exists

# The same navigation without the marker is the sign-in page, unchanged. A
# stranger is never sent to a provider they may have no account with, and the
# provider never learns they were here
GET {{base}}/private/
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
header "Location" not exists
xpath "string(//title)" == "Sign In"
xpath "count(//a[@data-sourcemeta-ui-login])" == 1

# A marker naming a policy that gates somewhere else is ignored. Honouring it
# would send somebody to a provider whose answer cannot admit them here, which
# would deny them again and start the whole thing over
GET {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal=cleartext
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
header "Location" not exists
xpath "string(//title)" == "Sign In"

# A marker naming nothing this instance serves is ignored the same way
GET {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal=not-a-policy
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
xpath "string(//title)" == "Sign In"

# A client asking for JSON is answered plainly whatever it carries. Renewal is
# a browser navigation, and redirecting a script to an identity provider would
# turn a denial it can handle into a redirect chain it cannot
GET {{base}}/private/secret.json
Accept: application/json
Cookie: sourcemeta_one_renewal=keycloak
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
denied_body: body
error_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Location" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{denied_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A public page is not a denial, so the marker changes nothing about it
GET {{base}}/public/
Accept: text/html
Cookie: sourcemeta_one_renewal=keycloak
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists

# The login endpoint asked for a silent attempt tells the provider not to
# interact, which is what lets the answer arrive without the person seeing
# anything. OpenID Connect Core 1.0 Section 3.1.2.1 requires `none` to appear
# alone, so it is the whole of the parameter
GET {{base}}/self/v1/auth/login/keycloak?silent=1
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" startsWith "https://keycloak:8443/realms/main/protocol/openid-connect/auth?"
header "Location" contains "prompt=none"
header "Location" contains "code_challenge_method=S256"
header "Location" contains "state="
header "Location" contains "nonce="
cookie "sourcemeta_one_transaction" exists
cookie "sourcemeta_one_transaction[HttpOnly]" exists

# A login somebody started themselves never says that, since a provider told
# not to interact cannot ask them who they are, and they came here to be asked
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" startsWith "https://keycloak:8443/realms/main/protocol/openid-connect/auth?"
header "Location" not contains "prompt"
cookie "sourcemeta_one_transaction" exists

# A silent attempt carries the page to return to through the sealed
# transaction, exactly as an ordinary login does
GET {{base}}/self/v1/auth/login/keycloak?silent=1&to=/private/secret
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" contains "prompt=none"
cookie "sourcemeta_one_transaction" exists

# The renewal redirect is a navigation, so a method that cannot navigate is
# answered as one, and the marker does not change which methods a gate accepts
POST {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal=keycloak
HTTP 401
Cache-Control: no-store
[Asserts]
header "Location" not exists
