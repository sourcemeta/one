# A policy naming a team admits a member of it, and nobody else.
#
# Each of these files stands for a fresh browser: a deployment offers nowhere to
# end its own session, so signing in as a second account within one of them
# would be answered with the first account's sign-in rather than a form

GET {{base}}/self/v1/auth/login/platform
HTTP 303
Cache-Control: no-store
[Captures]
authorize_url: header "Location"
[Asserts]
header "Location" contains "scope=read%3Aorg"

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
Location: /team
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session" exists
cookie "sourcemeta_one_renewal" not exists

GET {{base}}/team/roster.json
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/team/roster",
  "type": "array",
  "items": {
    "$ref": "../public/string"
  }
}
