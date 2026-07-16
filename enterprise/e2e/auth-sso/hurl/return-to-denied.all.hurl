# A login whose return target is not a same-origin local path cannot be used to
# bounce the browser off-site. Here the target is a protocol-relative URL, the
# classic open-redirect payload, crafted directly onto the login as an attacker
# would. It is dropped rather than sealed, so the callback falls back to the
# instance root and the session never lands on the attacker's host.
GET {{base}}/self/v1/auth/login/keycloak?to=%2F%2Fevil.example.com
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

# The malicious target was never sealed, so the session lands at the root, never
# at the attacker's host
GET {{callback_url}}
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session_keycloak" exists
