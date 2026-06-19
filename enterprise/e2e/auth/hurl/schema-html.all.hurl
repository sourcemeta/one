# The HTML rendering of a schema is gated identically to its JSON form: the
# same registry path drives the decision regardless of the negotiated type.

# Public schema HTML is anonymous
GET {{base}}/public/string
Accept: text/html
HTTP 200
[Asserts]
header "Content-Type" contains "text/html"

# apiKey schema HTML is denied without a credential, rendering the 401 page
GET {{base}}/private/secret
Accept: text/html
HTTP 401
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This page requires authentication"

# apiKey schema HTML is served with the credential
GET {{base}}/private/secret
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
[Asserts]
header "Content-Type" contains "text/html"

# The public carve-out under the apiKey prefix renders anonymously
GET {{base}}/private/open/shared
Accept: text/html
HTTP 200
[Asserts]
header "Content-Type" contains "text/html"

# A browser request (HTML preferred) for an apiKey schema renders the 401 page
GET {{base}}/private/secret
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
HTTP 401
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
xpath "string(//title)" == "Unauthorized"
