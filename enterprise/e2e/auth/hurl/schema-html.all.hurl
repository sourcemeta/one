# The HTML rendering of a schema is gated identically to its JSON form: the
# same registry path drives the decision regardless of the negotiated type.

# Public schema HTML is anonymous
GET {{base}}/public/string
Accept: text/html
HTTP 200
[Asserts]
header "Content-Type" contains "text/html"

# apiKey schema HTML is absent without a credential, rendering the page for a
# path that is not there
GET {{base}}/private/secret
Accept: text/html
HTTP 404
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(//title)" == "Not Found"
xpath "string(//h2[@class='fw-bold'])" == "Oops! What you are looking for is not here"

# apiKey schema HTML is served with the credential, marked private so shared
# caches do not store it
GET {{base}}/private/secret
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
[Asserts]
header "Content-Type" contains "text/html"

# A schema nested under the apiKey prefix inherits the gate, rendering that
# same page without a credential and the schema page with it
GET {{base}}/private/open/shared
Accept: text/html
HTTP 404
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(//title)" == "Not Found"
xpath "string(//h2[@class='fw-bold'])" == "Oops! What you are looking for is not here"

GET {{base}}/private/open/shared
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
[Asserts]
header "Content-Type" contains "text/html"
xpath "string(//title)" != "Not Found"

# A browser request (HTML preferred) for an apiKey schema renders that page
GET {{base}}/private/secret
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
HTTP 404
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(//title)" == "Not Found"
