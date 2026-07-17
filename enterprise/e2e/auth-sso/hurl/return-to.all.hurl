# A login started with a return target sends the browser back to that exact
# page once the session is established, rather than to the instance root. This
# is the far end of the gate redirect: the gate hands the login a "to", the
# login seals it, and the callback honours it. The target carries a query
# string, which survives the round trip intact.
GET {{base}}/self/v1/auth/login/keycloak?to=%2Fprivate%2Fsecret%3Fview%3Draw
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

# The callback establishes the session and returns to the sealed page, its
# query string preserved
GET {{callback_url}}
HTTP 303
Location: /private/secret?view=raw
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session_keycloak" exists
cookie "sourcemeta_one_transaction_keycloak[Max-Age]" == 0
