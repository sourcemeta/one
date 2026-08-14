# Five policies gate this instance and only one of them signs a person in. What
# the endpoint names is that one, because a policy admitting a program has
# nowhere to send somebody who carries nothing, and offering it would be naming
# a way in that leads nowhere.
#
# The endpoint is reachable without a credential, since somebody who has not
# signed in yet is exactly who needs to read it.

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
header "Vary" == "Accept, Accept-Encoding"
header "WWW-Authenticate" not exists
jsonpath "$.title" == "Fully Private Sandbox"
jsonpath "$.providers" count == 1
jsonpath "$.providers[0].name" == "keycloak"
jsonpath "$.providers[0].path" == "/self/v1/auth/login/keycloak"
jsonpath "$.providers[*].name" not contains "registry"
jsonpath "$.providers[*].name" not contains "mcp"
jsonpath "$.providers[*].name" not contains "mcp-tokens"
jsonpath "$.providers[*].name" not contains "machine"

POST {{base}}/self/v1/api/schemas/evaluate{{signin_schema}}
```
{{signin_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The page says the same, on an instance where every other URL is closed to a
# caller carrying nothing
GET {{base}}/self/v1/auth/login
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(//h1)" == "Fully Private Sandbox"
xpath "count(//a[@data-sourcemeta-ui-login])" == 1
xpath "string((//a[@data-sourcemeta-ui-login])[1]/@href)" == "/self/v1/auth/login/keycloak"

# Presenting a machine credential changes nothing, since what is offered is a
# property of the instance rather than of who is asking
GET {{base}}/self/v1/auth/login
Accept: application/json
Authorization: Bearer full-secret-key
HTTP 200
[Asserts]
jsonpath "$.providers" count == 1
jsonpath "$.providers[0].name" == "keycloak"
