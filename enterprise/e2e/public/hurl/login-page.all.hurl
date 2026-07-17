# A full instance still exposes the login route, but with no interactive
# provider configured there is no page to offer, so it is not found rather than
# an empty page. A machine and a browser are told the same thing
GET {{base}}/self/v1/auth/login
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
not_found: body
error_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{not_found}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
