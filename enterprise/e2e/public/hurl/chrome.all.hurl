# A registry nothing gates grows no session control at all. There is nowhere to
# sign in to and nothing to sign out of, so the bar carries only what an open
# instance has always carried.

GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0
xpath "count(//form[@action='/self/v1/auth/logout'])" == 0

# Every page carries the same bar, so a nested one says the same
GET {{base}}/jsonschema/2020-12
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# Including the page for a path that is not there
GET {{base}}/nothing-is-here
Accept: text/html
HTTP 404
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# The login page is reachable and says there is nothing to offer, since nothing
# signs anybody in here. The bar offering no way in and the page naming none are
# the same fact, and the page is covered in full alongside its data elsewhere
GET {{base}}/self/v1/auth/login
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(/html/head/title)" == "Sign In"
xpath "count(//a[@data-sourcemeta-ui-login])" == 0

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
