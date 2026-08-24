# A policy naming a domain is answered against the addresses an account holds,
# since the address on the account itself is the public one and is frequently
# unset

GET {{base}}/self/v1/auth/login/corp
HTTP 303
Cache-Control: no-store
[Captures]
authorize_url: header "Location"
[Asserts]
header "Location" contains "scope=user%3Aemail"

GET {{authorize_url}}
HTTP 302
[Captures]
provider_url: header "Location"

GET {{provider_url}}
HTTP 200
[Captures]
login_action: xpath "string(//form[@id='kc-form-login']/@action)"

POST {{login_action}}
[FormParams]
username: octocat
password: octocat-password
HTTP 302
[Captures]
callback_url: header "Location"

GET {{callback_url}}
HTTP 303
Location: /corp
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session" exists
cookie "sourcemeta_one_renewal" not exists

GET {{base}}/corp/policy.json
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/corp/policy",
  "type": "object",
  "properties": {
    "name": {
      "$ref": "../public/string"
    }
  }
}
