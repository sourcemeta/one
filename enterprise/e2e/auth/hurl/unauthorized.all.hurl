# An unauthorized caller gets an identical 401 for a protected path whether or
# not the schema exists, because the gate denies before the existence check.
# The JSON surface answers with a Problem Details envelope and the HTML surface
# with the index-time 401 page. Existence is only ever revealed to a caller the
# policy admits.

# JSON, existing protected schema, no credential: 401 Problem Details
GET {{base}}/private/secret.json
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

# JSON, nonexistent path under the same protected prefix, no credential: the
# identical 401, so probing cannot distinguish a real schema from a guess
GET {{base}}/private/ghost.json
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

# JSON, deep nonexistent path, no credential: still the identical 401
GET {{base}}/private/a/b/c.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# The existence distinction appears only once the caller is admitted: the real
# schema is served, and a guess is a genuine 404
GET {{base}}/private/secret.json
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/schema+json

GET {{base}}/private/ghost.json
Authorization: Bearer primary-secret-key
HTTP 404
Content-Type: application/problem+json

# HTML, existing protected schema, no credential: the 401 page, carrying the
# same bearer challenge and no-store discipline as the JSON envelope
GET {{base}}/private/secret
Accept: text/html
HTTP 401
Content-Type: text/html; charset=utf-8
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Referrer-Policy: strict-origin-when-cross-origin
X-Frame-Options: DENY
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This part of the registry requires authentication"
xpath "string(//p[@class='lead'])" == "Present a valid credential to access it"

# HTML, nonexistent path under the protected prefix, no credential: the
# byte-identical 401 page, so the HTML surface leaks no existence either
GET {{base}}/private/ghost
Accept: text/html
HTTP 401
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This part of the registry requires authentication"

# HTML, a credential the policy does not admit: still the 401 page
GET {{base}}/private/secret
Accept: text/html
Authorization: Bearer not-the-key
HTTP 401
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
xpath "string(//title)" == "Unauthorized"

# HTML, with the admitting credential: the real schema page, never the 401 page
GET {{base}}/private/secret
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
[Asserts]
header "Content-Type" contains "text/html"
xpath "string(//title)" != "Unauthorized"

# HTML, the public carve-out under the protected prefix: anonymous, never the
# 401 page
GET {{base}}/private/open/shared
Accept: text/html
HTTP 200
[Asserts]
header "Content-Type" contains "text/html"
xpath "string(//title)" != "Unauthorized"
