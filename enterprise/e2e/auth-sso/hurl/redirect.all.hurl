# A browser navigating to a gated page is sent to begin a login, since a single
# interactive policy governs the private tree. The denial a machine sees becomes
# a redirect a person can follow.
GET {{base}}/private/
Accept: text/html
HTTP 303
Location: /self/v1/auth/login/keycloak?to=%2Fprivate%2F
Cache-Control: no-store
[Asserts]
header "Set-Cookie" not exists

# HEAD is a navigation too, and redirects identically with no body
HEAD {{base}}/private/
Accept: text/html
HTTP 303
Location: /self/v1/auth/login/keycloak?to=%2Fprivate%2F
Cache-Control: no-store

# The extensionless schema page redirects the same way, carrying its own path
GET {{base}}/private/secret
Accept: text/html
HTTP 303
Location: /self/v1/auth/login/keycloak?to=%2Fprivate%2Fsecret
Cache-Control: no-store

# The query string of the denied page is preserved in the return target, so a
# page whose state lives in its query comes back whole
GET {{base}}/private/?view=raw&page=2
Accept: text/html
HTTP 303
Location: /self/v1/auth/login/keycloak?to=%2Fprivate%2F%3Fview%3Draw%26page%3D2
Cache-Control: no-store

# A public page is served, never redirected, so the login only fronts what is
# actually gated
GET {{base}}/public/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
xpath "string(//title)" != "Unauthorized"

# A client asking for JSON is denied, never redirected, so scripting the API
# still sees the honest 401
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

# The data representation stays JSON even for a browser: an explicit ".json"
# asks for the schema, not the page, so it is denied rather than redirected
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
