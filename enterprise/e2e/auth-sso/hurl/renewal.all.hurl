# A session lasts an hour, and asking somebody to click their provider again
# every hour is the cost of that. A browser that has signed in before carries a
# marker, so a navigation that gets nowhere can ask the provider whether that
# sign-in still stands instead of asking the person. The provider answers
# without showing anything when it can, so the hour passes unnoticed.
#
# The marker is sealed by the policy that minted it, so only a browser this
# instance signed in holds one that opens. Anybody may write the name of a
# policy into a cookie, and doing so must reach nothing, since the answer
# differs by whether the path is there and gated

# Signing in is what earns a marker, so one is earned here rather than written
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
[Captures]
authorize_url: header "Location"

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
HTTP 303
Cache-Control: no-store
[Captures]
marker: cookie "sourcemeta_one_renewal"
[Asserts]
cookie "sourcemeta_one_session" exists
cookie "sourcemeta_one_renewal" exists
cookie "sourcemeta_one_renewal[HttpOnly]" exists

# Signing out takes the session and the marker with it, which leaves the marker
# above as the only one in hand and nothing else alongside it
POST {{base}}/self/v1/auth/logout
HTTP 303
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session[Max-Age]" == 0
cookie "sourcemeta_one_renewal[Max-Age]" == 0

# A browser navigation that gets nowhere while carrying the marker it was given
# is sent to its provider rather than answered. The page it was asking for
# travels along, so the renewal lands where the person already was
GET {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal={{marker}}
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" == "/self/v1/auth/login/keycloak?silent=1&to=%2Fprivate%2F"
header "Set-Cookie" not exists
header "WWW-Authenticate" not exists

# The name of a policy is not a marker. Anybody can write one, and honouring it
# would answer a stranger differently for a path that is there and gated than
# for one that was never here, which is the whole of what the gate hides
GET {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal=keycloak
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
xpath "string(//title)" == "Not Found"

# Nor is a marker something to be assembled out of one that opens. Every part
# of it is under the seal, so changing any of them leaves nothing that opens
GET {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal={{marker}}x
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
xpath "string(//title)" == "Not Found"

# The same navigation without the marker is told there is nothing there, which
# is what anybody who cannot see it is told. A stranger is never sent to a
# provider they may have no account with, and the provider never learns they
# were here
GET {{base}}/private/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
header "WWW-Authenticate" not exists
xpath "string(//title)" == "Not Found"
xpath "count(//a[@data-sourcemeta-ui-login])" == 0

# A value naming nothing this instance serves reaches nothing either
GET {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal=not-a-policy
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
xpath "string(//title)" == "Not Found"

# A client asking for JSON is answered plainly whatever it carries. Renewal is
# a browser navigation, and redirecting a script to an identity provider would
# turn an answer it can handle into a redirect chain it cannot
GET {{base}}/private/secret.json
Accept: application/json
Cookie: sourcemeta_one_renewal={{marker}}
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
denied_body: body
error_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Location" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
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

# A page that is really there is no dead end, so the marker changes nothing
GET {{base}}/public/
Accept: text/html
Cookie: sourcemeta_one_renewal={{marker}}
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
# answered as one, and the marker changes nothing about what a method reaches
POST {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal={{marker}}
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
[Asserts]
header "Location" not exists

# A silent attempt can fail well past the provider's answer: with no code at
# all, with a code the provider will not redeem, or with an identity that does
# not validate. None of that was asked for, so none of it is shown. Each one
# puts the browser back where it started and takes the marker with it, since an
# attempt that did not end in a session must not be made again on the next
# navigation, and every navigation after that.

# A silent attempt whose callback carries no code
GET {{base}}/self/v1/auth/login/keycloak?silent=1&to=/private/
HTTP 303
[Captures]
codeless_state: header "Location" regex "state=([^&]+)"

GET {{base}}/self/v1/auth/callback/keycloak?state={{codeless_state}}
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" == "/private/"
cookie "sourcemeta_one_renewal[Max-Age]" == 0
cookie "sourcemeta_one_transaction[Max-Age]" == 0

# A silent attempt whose code the provider refuses to redeem. Without the rule
# this would be the `502` an ordinary login gets, which nobody navigating to a
# page has any way to act on
GET {{base}}/self/v1/auth/login/keycloak?silent=1&to=/private/secret
HTTP 303
[Captures]
refused_state: header "Location" regex "state=([^&]+)"

GET {{base}}/self/v1/auth/callback/keycloak?state={{refused_state}}&code=a-code-nobody-issued
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" == "/private/secret"
header "Set-Cookie" not contains "sourcemeta_one_session="
cookie "sourcemeta_one_renewal[Max-Age]" == 0

# A provider that will not answer without asking the person, which is the
# ordinary end of a silent attempt rather than a fault (OpenID Connect Core 1.0
# Section 3.1.2.6). It reads exactly like the failures above
GET {{base}}/self/v1/auth/login/keycloak?silent=1&to=/private/
HTTP 303
[Captures]
required_state: header "Location" regex "state=([^&]+)"

GET {{base}}/self/v1/auth/callback/keycloak?state={{required_state}}&error=login_required
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" == "/private/"
cookie "sourcemeta_one_renewal[Max-Age]" == 0

# A fault the provider names, rather than a refusal to interact, is treated the
# same way. Only an operator reading the log needs to tell them apart, and the
# marker goes either way so the next navigation does not repeat the detour
GET {{base}}/self/v1/auth/login/keycloak?silent=1&to=/private/
HTTP 303
[Captures]
faulty_state: header "Location" regex "state=([^&]+)"

GET {{base}}/self/v1/auth/callback/keycloak?state={{faulty_state}}&error=server_error
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" == "/private/"
cookie "sourcemeta_one_renewal[Max-Age]" == 0

# The same failures on a login somebody started themselves are still reported
# to them, since they are watching and can act on what they are told
GET {{base}}/self/v1/auth/login/keycloak?to=/private/
HTTP 303
[Captures]
visible_state: header "Location" regex "state=([^&]+)"

GET {{base}}/self/v1/auth/callback/keycloak?state={{visible_state}}&code=a-code-nobody-issued
HTTP 500
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
visible_body: body
visible_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Location" not exists
{
  "type": "urn:sourcemeta:one:auth-incomplete",
  "title": "Internal Server Error",
  "status": 500,
  "detail": "The session could not be established"
}

POST {{base}}/self/v1/api/schemas/evaluate{{visible_schema}}
```
{{visible_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# And a decline on a login somebody started is still a decline, not a quiet
# redirect, so the one outcome a person caused is the one they are told about
GET {{base}}/self/v1/auth/login/keycloak?to=/private/
HTTP 303
[Captures]
declined_state: header "Location" regex "state=([^&]+)"

GET {{base}}/self/v1/auth/callback/keycloak?state={{declined_state}}&error=access_denied
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
[Captures]
declined_body: body
[Asserts]
header "Location" not exists
{
  "type": "urn:sourcemeta:one:auth-login-declined",
  "title": "Forbidden",
  "status": 403,
  "detail": "The identity provider declined the login"
}

POST {{base}}/self/v1/api/schemas/evaluate{{visible_schema}}
```
{{declined_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
