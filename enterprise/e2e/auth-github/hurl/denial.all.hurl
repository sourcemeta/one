# An account the deployment authenticated, whom the policy does not admit.
#
# This account belongs to another organisation entirely, so the deployment
# vouches for it and the policy still refuses. That is the end of the road
# rather than something to try again, so no session is minted and the catalog
# stays where it was
GET {{base}}/self/v1/auth/login/engineering
HTTP 303
[Captures]
authorize_url: header "Location"

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
username: hubot
password: hubot-password
HTTP 302
[Captures]
callback_url: header "Location"

GET {{callback_url}}
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:auth-not-admitted",
  "title": "Forbidden",
  "status": 403,
  "detail": "This account is not admitted here"
}

GET {{base}}/private/secret.json
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

# A callback carrying no transaction belongs to no login this instance started,
# and one carrying a state nothing echoed is the same. Both are refused before
# anything the deployment said is honoured
GET {{base}}/self/v1/auth/callback/engineering?code=invented&state=invented
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
invalid_body: body
invalid_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{invalid_schema}}
```
{{invalid_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A name no policy answers to reveals nothing beyond that
GET {{base}}/self/v1/auth/login/invented
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

# A login that cannot be completed only strands the person at the deployment,
# so a policy whose client secret is unset is refused before the browser is
# sent anywhere, and every reason a login cannot start reads alike
GET {{base}}/self/v1/auth/login/no-client-secret
HTTP 500
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
unavailable_body: body
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:auth-unavailable",
  "title": "Internal Server Error",
  "status": 500,
  "detail": "This login cannot be started"
}

POST {{base}}/self/v1/api/schemas/evaluate{{invalid_schema}}
```
{{unavailable_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/unavailable/thing.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}
