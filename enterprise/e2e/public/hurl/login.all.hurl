# An instance nobody can sign in to still answers where signing in would
# happen, with an empty list rather than with nothing. The page and the data
# are the same statement, so a custom interface and a browser are told the same
# thing about the same instance.
#
# This sandbox declares no policy at all, which is the shape every public
# instance has.

# What a custom interface reads
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

# And what a browser is shown, which says the same thing in words rather than
# leaving a space where the ways in would be
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

# The bar offers no way here either, so nothing sends a person to this page in
# the first place. It answers honestly to whoever types its URL
GET {{base}}/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# A method the endpoint does not serve is refused before anything is read
POST {{base}}/self/v1/auth/login
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
refused_body: body
refused_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Allow" == "GET, HEAD, OPTIONS"
{
  "type": "urn:sourcemeta:one:method-not-allowed",
  "title": "Method Not Allowed",
  "status": 405,
  "detail": "This HTTP method is invalid for this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{refused_schema}}
```
{{refused_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
