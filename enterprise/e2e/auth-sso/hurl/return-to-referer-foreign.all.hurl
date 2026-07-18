# A cross-origin referring page is never honoured as a return target, so the
# login cannot be turned into an open redirect by a crafted Referer. The login
# falls back to what the policy governs instead. This runs its own login from a
# fresh session so the provider presents its form.
GET {{base}}/self/v1/auth/login/keycloak
Referer: https://evil.example.com/private/secret
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
[Asserts]
header "Location" contains "code="

# The session lands on the policy's default page, never the foreign origin
GET {{callback_url}}
HTTP 303
Location: /private
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session_keycloak" exists
