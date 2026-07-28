# Ending a session here leaves the provider's own session alone, so signing in
# again would not ask who you are and the next person at the same browser would
# arrive as whoever used it last. So logout sends the browser on to the
# provider to finish the job, carrying the identity token as proof of whose
# session it is asking to end, which is also what spares the person a
# confirmation page there.
#
# Both cookies are expired before any of that is decided, and whether or not
# the request carried them, so no outcome below can leave a session behind.

# A caller carrying nothing is still logged out, and still told where to go
POST {{base}}/self/v1/auth/logout
HTTP 303
Cache-Control: no-store
Location: /
[Asserts]
header "Set-Cookie" count == 2
cookie "sourcemeta_one_session[Max-Age]" == 0
cookie "sourcemeta_one_session[HttpOnly]" exists
cookie "sourcemeta_one_session[Path]" == "/"
cookie "sourcemeta_one_transaction[Max-Age]" == 0
cookie "sourcemeta_one_transaction[HttpOnly]" exists
cookie "sourcemeta_one_transaction[Path]" == "/"

# A value that is not a session this instance minted is cleared just the same,
# and discloses nothing by being treated differently
POST {{base}}/self/v1/auth/logout
Cookie: sourcemeta_one_session=not-a-session
HTTP 303
Cache-Control: no-store
Location: /
[Asserts]
header "Set-Cookie" count == 2
cookie "sourcemeta_one_session[Max-Age]" == 0
cookie "sourcemeta_one_transaction[Max-Age]" == 0

# A real login, so there is a genuine session to end
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
[Captures]
authorize_url: header "Location"
transaction: cookie "sourcemeta_one_transaction"

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
Cookie: sourcemeta_one_transaction={{transaction}}
HTTP 303
Location: /private
Cache-Control: no-store
[Captures]
session: cookie "sourcemeta_one_session"
[Asserts]
cookie "sourcemeta_one_session" exists

# The session opens the gated schema, so it is a real one
GET {{base}}/private/secret.json
Cookie: sourcemeta_one_session={{session}}
HTTP 200
Content-Type: application/schema+json
[Asserts]
jsonpath "$.type" == "object"

# Logging out expires both cookies and sends the browser to the provider to end
# its own session, naming this instance as where to come back to
POST {{base}}/self/v1/auth/logout
Cookie: sourcemeta_one_session={{session}}
HTTP 303
Cache-Control: no-store
[Captures]
provider_logout: header "Location"
[Asserts]
header "Set-Cookie" count == 2
cookie "sourcemeta_one_session[Max-Age]" == 0
cookie "sourcemeta_one_transaction[Max-Age]" == 0
header "Location" startsWith "https://keycloak:8443/realms/main/protocol/openid-connect/logout?"
header "Location" contains "id_token_hint="
header "Location" contains "post_logout_redirect_uri=http%3A%2F%2Flocalhost%3A8000"

# The provider honours it and sends the browser back where we asked, which is
# what registering the post-logout URI buys. No confirmation page appears,
# because the identity token said whose session this was
GET {{provider_logout}}
HTTP 302
[Asserts]
header "Location" == "http://localhost:8000"

# A sealed session carries its own validity and this instance keeps no record
# of one, so logging out takes it from the browser rather than revoking it.
# Somebody who copied the value beforehand can still present it until it
# expires. That is the standing cost of a stateless session, and it is asserted
# here rather than left to be discovered
GET {{base}}/private/secret.json
Cookie: sourcemeta_one_session={{session}}
HTTP 200
Content-Type: application/schema+json
[Asserts]
jsonpath "$.type" == "object"

# What logging out does end is the browser's copy, so a caller carrying only
# what the browser now holds is denied
GET {{base}}/private/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# And signing in again asks who you are rather than letting the next person in
# as the last one, which is the whole reason for the round trip above
GET {{base}}/self/v1/auth/login/keycloak
HTTP 303
[Captures]
second_authorize: header "Location"

GET {{second_authorize}}
HTTP 200
[Asserts]
xpath "count(//form[@id='kc-form-login'])" == 1
