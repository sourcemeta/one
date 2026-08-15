# The endpoint answers what a custom interface reads, which is the same
# information the page renders. Nobody signs in here, so the list is empty and
# the page says so, rather than the endpoint going missing and leaving a caller
# unable to tell "nobody can sign in" from "wrong URL".

GET {{base}}/self/v1/auth/login
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/auth/login/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
login_body: body
login_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Set-Cookie" not exists
jsonpath "$.title" == "Sourcemeta"
jsonpath "$.providers" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{login_schema}}
```
{{login_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The page is rendered from that artifact alone, so the two say the same thing
GET {{base}}/self/v1/auth/login
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
xpath "string(/html/head/title)" == "Sign In"
xpath "string(//h1)" == "Sourcemeta"
xpath "count(//a[@data-sourcemeta-ui-login])" == 0
xpath "normalize-space(//p)" == "There is no way to sign in to this instance"

# The response describes itself, so a caller validates it knowing nothing else
# about this instance
GET {{base}}/self/v1/schemas/api/auth/login/response
HTTP 200
Content-Type: application/schema+json
[Asserts]
jsonpath "$.title" == "Sourcemeta One Login API Response"

# A method the endpoint does not serve is refused before anything is read
POST {{base}}/self/v1/auth/login
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
header "Allow" == "GET, HEAD, OPTIONS"
jsonpath "$.status" == 405
