# The shared collection is governed by two interactive policies, so the gate
# cannot choose a provider for the browser. Instead of a direct redirect it
# sends the browser to the page that offers the choice, carrying the covering
# policies so the picker offers only those.
GET {{base}}/shared/
Accept: text/html
HTTP 303
Location: /self/v1/auth/login?to=%2Fshared%2F&policy=keycloak&policy=other
Cache-Control: no-store
[Asserts]
header "Set-Cookie" not exists

# The picker page lists every interactive provider, in declaration order, by its
# configured title, each linking to its own login. It is served with the same
# browser hardening and no-store discipline as the other gate pages
GET {{base}}/self/v1/auth/login
Accept: text/html
HTTP 200
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept-Encoding
[Asserts]
xpath "string(//title)" == "Sign In"
xpath "string(//h1)" == "SSO Sandbox"
xpath "count(//a[@data-sourcemeta-ui-login])" == 2
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@data-sourcemeta-ui-login)" == "keycloak"
xpath "normalize-space((//a[@data-sourcemeta-ui-login])[1])" == "Corporate SSO"
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@href)" == "/self/v1/auth/login/keycloak"
xpath "string((//a[@data-sourcemeta-ui-login])[2]/@data-sourcemeta-ui-login)" == "other"
xpath "normalize-space((//a[@data-sourcemeta-ui-login])[2])" == "Partner Login"
xpath "string((//a[@data-sourcemeta-ui-login])[2]/@href)" == "/self/v1/auth/login/other"

# A path governed by a single interactive policy still redirects straight to
# that provider, never through the picker
GET {{base}}/private/
Accept: text/html
HTTP 303
Location: /self/v1/auth/login/keycloak?to=%2Fprivate%2F
Cache-Control: no-store

# A machine asking for JSON is denied, never sent to the picker
GET {{base}}/shared/doc.json
Accept: application/json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
denied: body
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
{{denied}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
