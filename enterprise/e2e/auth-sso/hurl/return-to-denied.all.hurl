# A return target that is not a same-origin local path cannot be used to bounce
# the browser off-site. Each of the classic open-redirect payloads is crafted
# directly onto the login as an attacker would, travels through the full
# validation path, and is dropped rather than sealed, so the session lands on
# what the policy governs, never on the attacker's host.

# Protocol-relative. The first login authenticates against the form.
GET {{base}}/self/v1/auth/login/keycloak?to=%2F%2Fevil.example.com
HTTP 303
Cache-Control: no-store
[Captures]
authorize_url: header "Location" replace "http://keycloak:8080" "http://localhost:8080"
[Asserts]
header "Set-Cookie" startsWith "sourcemeta_one_transaction_keycloak="

GET {{authorize_url}}
HTTP 200
[Captures]
login_action: xpath "string(//form[@id='kc-form-login']/@action)" replace "http://keycloak:8080" "http://localhost:8080"

POST {{login_action}}
[FormParams]
username: jane
password: jane-password
HTTP 302
[Captures]
callback_url: header "Location"

GET {{callback_url}}
HTTP 303
Location: /private
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session_keycloak" exists

# Backslash-escaped, which a browser resolves to a protocol-relative URL. The
# provider now recognises the session, so the authorize returns straight to the
# callback without a form.
GET {{base}}/self/v1/auth/login/keycloak?to=%2F%5Cevil.example.com
HTTP 303
Cache-Control: no-store
[Captures]
authorize_backslash: header "Location" replace "http://keycloak:8080" "http://localhost:8080"

GET {{authorize_backslash}}
HTTP 302
[Captures]
callback_backslash: header "Location"

GET {{callback_backslash}}
HTTP 303
Location: /private
Cache-Control: no-store

# An absolute URL with a scheme, which is not a rooted path at all
GET {{base}}/self/v1/auth/login/keycloak?to=https%3A%2F%2Fevil.example.com
HTTP 303
Cache-Control: no-store
[Captures]
authorize_scheme: header "Location" replace "http://keycloak:8080" "http://localhost:8080"

GET {{authorize_scheme}}
HTTP 302
[Captures]
callback_scheme: header "Location"

GET {{callback_scheme}}
HTTP 303
Location: /private
Cache-Control: no-store
