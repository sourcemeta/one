# A marker that opens is still only good where the policy that sealed it
# governs. Somebody signed in for one collection carries a real marker, and it
# has to reach nothing at another, or signing in anywhere would report what is
# gated everywhere, which is the whole of what the gate hides

GET {{base}}/self/v1/auth/login/team
HTTP 303
[Captures]
authorize_url: header "Location"

GET {{authorize_url}}
HTTP 200
[Captures]
login_action: xpath "string(//form[@id='kc-form-login']/@action)"

POST {{login_action}}
[FormParams]
username: jane
password: jane-password
HTTP 302
[Captures]
callback_url: header "Location"

GET {{callback_url}}
HTTP 303
Cache-Control: no-store
[Captures]
marker: cookie "sourcemeta_one_renewal"
[Asserts]
cookie "sourcemeta_one_renewal" exists

# Signing out leaves the marker above as the only thing in hand, so what
# follows is decided by it rather than by a session sitting alongside it
POST {{base}}/self/v1/auth/logout
HTTP 303
Cache-Control: no-store
[Asserts]
cookie "sourcemeta_one_session[Max-Age]" == 0
cookie "sourcemeta_one_renewal[Max-Age]" == 0

# Where it was minted it is honoured, which is what makes the refusal below
# about where it is presented rather than about the marker
GET {{base}}/team/
Accept: text/html
Cookie: sourcemeta_one_renewal={{marker}}
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" == "/self/v1/auth/login/team?silent=1&to=%2Fteam%2F"

# Somewhere else it is nothing, and a collection another policy gates reads
# exactly like a collection that was never here
GET {{base}}/private/
Accept: text/html
Cookie: sourcemeta_one_renewal={{marker}}
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
xpath "string(//title)" == "Not Found"

GET {{base}}/made-up/
Accept: text/html
Cookie: sourcemeta_one_renewal={{marker}}
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
xpath "string(//title)" == "Not Found"
