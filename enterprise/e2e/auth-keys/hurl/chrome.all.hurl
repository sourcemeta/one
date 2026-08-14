# A registry gated by machine credentials alone. Something here is private, so
# this is not an open instance, and yet nobody signs in: an apiKey policy
# admits a program holding a key rather than a person at a provider.
#
# That is what this sandbox is for. The bar offers a way in only where there is
# somewhere to go, and here there is not, so an instance with policies looks
# exactly like an instance with none. Offering one would link to a page that
# holds nothing, and it would invite a person to do something no policy here
# lets them do.

# The anonymous view, which is what a visitor is served
GET {{base}}/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0
xpath "count(//form[@action='/self/v1/auth/logout'])" == 0

# The open collection, reachable by anybody
GET {{base}}/open/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# The page for the gated collection, which this view does not hold. Nothing
# here hints that a key would open it, the bar included
GET {{base}}/vault/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# And the view the key opens, which is a view with no session behind it, so it
# is offered no way out any more than the anonymous one is offered a way in
GET {{base}}/vault/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

GET {{base}}/vault/secret
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# The login page holds nothing to offer, since no policy here signs anybody in,
# and it says so rather than going missing. The bar offering no way in and this
# page naming none are the same fact told twice
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
xpath "count(//a[@data-sourcemeta-ui-login])" == 0
xpath "normalize-space(//p)" == "There is no way to sign in to this instance"

# And the data underneath it says the same, which is what the page is drawn from
GET {{base}}/self/v1/auth/login
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/auth/login/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
signin_body: body
signin_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.providers" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{signin_schema}}
```
{{signin_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
