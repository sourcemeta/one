# The bar is written per view, so what it offers follows from who the page was
# written for rather than from anything decided while serving it. This instance
# gates one collection interactively and the rest with machine credentials, so
# it exercises all three answers: a way in, a way out, and neither.

# The anonymous view offers the way in, since somebody reading it has no
# session and this instance has somewhere to sign in to
GET {{base}}/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 1
xpath "string(//a[@data-sourcemeta-ui-signin]/@href)" == "/self/v1/auth/login"
xpath "normalize-space(//a[@data-sourcemeta-ui-signin])" == "Sign In"
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# A nested page in the same view says the same, since the bar is the page's
# rather than the path's
GET {{base}}/public/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//a[@data-sourcemeta-ui-signin])" == 1
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# So does a schema page
GET {{base}}/public/string
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//a[@data-sourcemeta-ui-signin])" == 1
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# And so does the page for a path this view does not hold, which is the one
# that matters most: a visitor who followed a link to something gated is told
# there is nothing there, and the way in is still on the page
GET {{base}}/private/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//a[@data-sourcemeta-ui-signin])" == 1
xpath "string(//a[@data-sourcemeta-ui-signin]/@href)" == "/self/v1/auth/login"
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# A view a machine credential reaches has no session behind it, so it is
# offered neither a way in nor a way out. Signing out of a key is not a thing,
# and the way in would be no use to whoever presented one
GET {{base}}/private/
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav[contains(@class, 'navbar')])" == 1
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# The same for a schema page under that key
GET {{base}}/private/secret
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0

# The login page carries no bar at all. A caller reading it is already where
# the bar would have sent them, and it names its providers instead
GET {{base}}/self/v1/auth/login
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//nav)" == 0
xpath "count(//a[@data-sourcemeta-ui-signin])" == 0
xpath "count(//button[@data-sourcemeta-ui-signout])" == 0
xpath "count(//a[@data-sourcemeta-ui-login])" == 1
