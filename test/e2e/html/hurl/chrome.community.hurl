# Why the bar can never grow a session control here: this edition signs nobody
# in. Declaring a policy is refused when the configuration is read, and every
# endpoint a control would have pointed at answers that it is not this
# edition's to offer. The bar has nothing to put in front of a caller because
# there is nothing behind it.

# Ending a session, which is what a sign-out control submits to
POST {{base}}/self/v1/auth/logout
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
logout_body: body
logout_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:enterprise-required",
  "title": "Forbidden",
  "status": 403,
  "detail": "This feature is only available in the Enterprise edition"
}

POST {{base}}/self/v1/api/schemas/evaluate{{logout_schema}}
```
{{logout_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Starting one, which is what the login page would have linked to
GET {{base}}/self/v1/auth/login/anything
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
{
  "type": "urn:sourcemeta:one:enterprise-required",
  "title": "Forbidden",
  "status": 403,
  "detail": "This feature is only available in the Enterprise edition"
}

# And completing one
GET {{base}}/self/v1/auth/callback/anything?code=a-code&state=a-state
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:enterprise-required",
  "title": "Forbidden",
  "status": 403,
  "detail": "This feature is only available in the Enterprise edition"
}

# And there is nowhere to be sent to sign in. This edition signs nobody in, so
# the endpoint is refused outright rather than answering with an empty list,
# which is what an Enterprise instance declaring no interactive policy does
GET {{base}}/self/v1/auth/login
Accept: text/html
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
signin_body: body
signin_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:enterprise-required",
  "title": "Forbidden",
  "status": 403,
  "detail": "This feature is only available in the Enterprise edition"
}

POST {{base}}/self/v1/api/schemas/evaluate{{signin_schema}}
```
{{signin_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Asking for data rather than a page is refused the same way, so the edition
# rather than the representation is what decides
GET {{base}}/self/v1/auth/login
Accept: application/json
HTTP 403
Cache-Control: no-store
Content-Type: application/problem+json
[Asserts]
jsonpath "$.status" == 403
jsonpath "$.type" == "urn:sourcemeta:one:enterprise-required"
