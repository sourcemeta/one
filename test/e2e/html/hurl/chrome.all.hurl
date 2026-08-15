# Nothing in this instance is gated, so nothing in its bar is about being
# gated. The Community edition cannot declare a policy at all, which is
# refused when the configuration is read rather than here, so this is the
# whole of what its chrome ever looks like. An Enterprise instance declaring
# no policy is served the same bar, which is the contract the two editions
# hold to each other.

GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0
xpath "count(//form[@action='/self/v1/auth/logout'])" == 0
xpath "count(//a[@data-sourcemeta-ui-login])" == 0

# A directory page
GET {{base}}/test/doc
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# A schema page
GET {{base}}/test/doc/string-1
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# And the page for a path that is not there, which is the one place a bar
# offering a way in would be most tempting to put
GET {{base}}/nothing-is-here
Accept: text/html
HTTP 404
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0
