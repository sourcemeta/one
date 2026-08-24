# A policy naming accounts admits the accounts it names and no other, whatever
# else the deployment vouches for about them

GET {{base}}/self/v1/auth/login/personal
HTTP 303
Cache-Control: no-store
[Captures]
authorize_url: header "Location"
[Asserts]
header "Location" not contains "scope="

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
username: mona
password: mona-password
HTTP 302
[Captures]
callback_url: header "Location"

GET {{callback_url}}
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
not_admitted: body
admission_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:auth-not-admitted",
  "title": "Forbidden",
  "status": 403,
  "detail": "This account is not admitted here"
}

POST {{base}}/self/v1/api/schemas/evaluate{{admission_schema}}
```
{{not_admitted}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/desk/ticket.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}
