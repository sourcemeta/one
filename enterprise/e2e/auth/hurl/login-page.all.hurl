# With a single interactive provider the login page still renders, offering that
# one provider by the title it falls back to, its own name. It is served with
# the same browser hardening and no-store discipline as the other gate pages
GET {{base}}/self/v1/auth/login
Accept: text/html
HTTP 200
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept-Encoding
[Asserts]
xpath "string(//title)" == "Sign In"
xpath "string(//h1)" == "Authentication Sandbox"
xpath "count(//a[@data-sourcemeta-ui-login])" == 1
xpath "normalize-space((//a[@data-sourcemeta-ui-login])[1])" == "keycloak"
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@href)" == "/self/v1/auth/login/keycloak"
