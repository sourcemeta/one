# A login started with no explicit target but a same-origin referring page
# returns to that page once the session is established. When the login is served
# in place, the referrer is exactly the denied page, so the return target needs
# no query at all: the endpoint recovers it from the Referer. The referring
# page's query string survives the round trip intact.
GET {{base}}/self/v1/auth/login/keycloak
Referer: {{base}}/private/secret?view=raw
HTTP 303
Cache-Control: no-store
[Captures]
authorize_url: header "Location" replace "http://keycloak:8080" "http://localhost:8080"
[Asserts]
header "Set-Cookie" startsWith "sourcemeta_one_transaction_keycloak="

# The provider renders its login form
GET {{authorize_url}}
HTTP 200
[Captures]
login_action: xpath "string(//form[@id='kc-form-login']/@action)" replace "http://keycloak:8080" "http://localhost:8080"

# Submitting the user's credentials returns to the callback with a code
POST {{login_action}}
[FormParams]
username: jane
password: jane-password
HTTP 302
[Captures]
callback_url: header "Location"
[Asserts]
header "Location" contains "code="

# The callback establishes the session and returns to the referring page, its
# query string preserved, proving the Referer drove the return target
GET {{callback_url}}
HTTP 303
Location: /private/secret?view=raw
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session_keycloak" exists
cookie "sourcemeta_one_transaction_keycloak[Max-Age]" == 0
