# A request carries a bearer value and cookie fields, and one caller can hold
# both a machine key and a browser session. What was presented decides: a
# request carrying a bearer value is read as that credential and its session is
# not consulted, because attaching one is deliberate while a session is sent
# whether or not anybody meant it.
#
# The console is governed by a static-key policy and an interactive one at the
# same time, which is what makes both credentials meaningful at one path.

# Before any login, so the jar holds no session. The key alone opens the console
GET {{base}}/console/dashboard.json
Authorization: Bearer console-secret-key
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/console/dashboard",
  "type": "object",
  "required": [ "widgets" ],
  "properties": {
    "widgets": {
      "type": "array",
      "items": {
        "type": "string"
      }
    }
  },
  "additionalProperties": false
}

# And a key that opens somewhere else does not open this
GET {{base}}/console/dashboard.json
Authorization: Bearer primary-secret-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Signing in at the real provider, after which the jar carries the session
GET {{base}}/self/v1/auth/login/keycloak
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
Location: /console
[Asserts]
cookie "sourcemeta_one_session" exists

# The session alone opens the console, carrying no key at all
GET {{base}}/console/dashboard.json
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$['$id']" == "{{base}}/console/dashboard"

# A key this path accepts, alongside the session. Presenting one does not take
# anything away when it opens the path itself
GET {{base}}/console/dashboard.json
Authorization: Bearer console-secret-key
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$['$id']" == "{{base}}/console/dashboard"

# A key this path refuses, alongside a session it accepts. The request is read
# as the key it presented, so the session is never consulted and the console it
# would have opened is refused. This is the whole of the rule, and its cost
GET {{base}}/console/dashboard.json
Authorization: Bearer primary-secret-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# A key that opens nothing anywhere reads the same way, so what matters is that
# something was presented rather than that it named another policy
GET {{base}}/console/dashboard.json
Authorization: Bearer not-a-key-anywhere
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# An empty bearer is nothing presented, so the session decides as it would have
GET {{base}}/console/dashboard.json
Authorization: Bearer
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$['$id']" == "{{base}}/console/dashboard"

# So one caller still reaches both areas, by presenting the key where it opens
# something and nothing where the session does. What changed is that a single
# request now names one credential rather than two
GET {{base}}/reports/quarterly.json
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/reports/quarterly",
  "type": "array",
  "items": {
    "$ref": "../public/string"
  }
}

GET {{base}}/console/dashboard.json
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$['$id']" == "{{base}}/console/dashboard"

# And neither credential reaches what neither governs
GET {{base}}/internal/config.json
Authorization: Bearer primary-secret-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
