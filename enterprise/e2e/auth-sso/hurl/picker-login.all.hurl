# Choosing the second provider on the picker completes a real login against its
# own realm and lands on the page that was denied, proving the picker's second
# option is a genuine, distinct issuer rather than a decoration.
GET {{base}}/self/v1/auth/login/other?to=%2Fshared%2F
HTTP 303
Cache-Control: no-store
[Captures]
authorize_url: header "Location" replace "http://keycloak:8080" "http://localhost:8080"
[Asserts]
header "Location" startsWith "http://keycloak:8080/realms/second/protocol/openid-connect/auth?"
header "Set-Cookie" startsWith "sourcemeta_one_transaction_other="

# The second realm renders its own login form
GET {{authorize_url}}
HTTP 200
[Captures]
login_action: xpath "string(//form[@id='kc-form-login']/@action)" replace "http://keycloak:8080" "http://localhost:8080"

# Submitting the credentials returns to the second policy's callback
POST {{login_action}}
[FormParams]
username: jane
password: jane-password
HTTP 302
[Captures]
callback_url: header "Location"
[Asserts]
header "Location" startsWith "http://localhost:8000/self/v1/auth/callback/other?"
header "Location" contains "code="

# The callback establishes the session and returns to the sealed page
GET {{callback_url}}
HTTP 303
Location: /shared/
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session_other" exists

# The session opens the shared collection that was denied before, served
# private so a shared cache cannot retain it
GET {{base}}/shared/doc.json
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/shared/doc",
  "type": "object"
}
