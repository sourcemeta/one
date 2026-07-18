# A browser navigating to a gated page is shown that path's login page in place:
# a 401 whose body is the login rather than the plain denial. A single
# interactive policy governs the private tree, so the page offers just that one
# provider, named by the title it falls back to.
GET {{base}}/private/
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept, Accept-Encoding
[Captures]
login_etag: header "ETag"
[Asserts]
header "Location" not exists
header "Set-Cookie" not exists
xpath "string(//title)" == "Sign In"
xpath "string(//h1)" == "SSO Sandbox"
xpath "string(//p[@class='text-secondary text-center small mb-4'])" == "Choose how you want to sign in"
xpath "count(//a[@data-sourcemeta-ui-login])" == 1
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@data-sourcemeta-ui-login)" == "keycloak"
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@href)" == "/self/v1/auth/login/keycloak"
xpath "normalize-space((//a[@data-sourcemeta-ui-login])[1])" == "keycloak"

# HEAD is a navigation too: the same 401, with no body
HEAD {{base}}/private/
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
header "Location" not exists

# An existing schema page under the policy is answered by the byte-identical
# login page, so a denial never reveals that the requested schema exists. The
# ETag is the content hash, so an equal ETag proves an equal body
GET {{base}}/private/secret
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
header "Location" not exists
header "ETag" == {{login_etag}}

# A path that resolves to no schema at all returns the same page, so a missing
# resource is indistinguishable from a real one under the same policy
GET {{base}}/private/does-not-exist
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
header "Location" not exists
header "ETag" == {{login_etag}}

# A nested path that does not exist resolves up through the nearest governing
# directory, still to the identical page
GET {{base}}/private/nope/deeper/still
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
header "Location" not exists
header "ETag" == {{login_etag}}

# The query string no longer changes the response: the page carries no return
# target, so the login endpoint recovers it from the referring page instead
GET {{base}}/private/?view=raw&page=2
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
header "Location" not exists
header "ETag" == {{login_etag}}

# A public page is served, never fronted by a login, so the page only guards
# what is actually gated
GET {{base}}/public/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
xpath "string(//title)" != "Sign In"

# A client asking for JSON is denied plainly, never shown the login page, so
# scripting the API still sees the honest 401
GET {{base}}/private/secret.json
Accept: application/json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
machine_denied: body
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
{{machine_denied}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# An explicit ".json" asks for the schema, not the page, so even a browser is
# denied plainly rather than shown the login
GET {{base}}/private/secret.json
Accept: text/html
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
browser_json_denied: body
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
{{browser_json_denied}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
