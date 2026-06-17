# The canonical denial is an RFC 9457 problem document with a Bearer challenge
GET {{base}}/private/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The Bearer scheme token is matched case-sensitively and exactly
GET {{base}}/private/secret.json
Authorization: Bearer PRIMARY-SECRET-KEY
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

GET {{base}}/private/secret.json
Authorization: Bearer primary-secret-key-extra
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# The Bearer scheme name itself is case-insensitive per RFC 6750
GET {{base}}/private/secret.json
Authorization: bearer primary-secret-key
HTTP 200
Content-Type: application/schema+json
[Asserts]
jsonpath "$['$id']" == "{{base}}/private/secret"

GET {{base}}/private/secret.json
Authorization: BEARER primary-secret-key
HTTP 200
Content-Type: application/schema+json

# Surrounding whitespace around the token is ignored
GET {{base}}/private/secret.json
Authorization: Bearer   primary-secret-key
HTTP 200
Content-Type: application/schema+json

# A non-Bearer scheme is not accepted
GET {{base}}/private/secret.json
Authorization: Basic primary-secret-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# A bare token with no scheme is not accepted
GET {{base}}/private/secret.json
Authorization: primary-secret-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# The Bearer scheme with no token is not accepted
GET {{base}}/private/secret.json
Authorization: Bearer
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# A denial must not disclose whether the resource exists: a missing schema
# under an apiKey prefix is denied identically to an existing one
GET {{base}}/private/does-not-exist.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# Only once authenticated is the absence of the resource revealed
GET {{base}}/private/does-not-exist.json
Authorization: Bearer primary-secret-key
HTTP 404

# A missing schema under a public prefix is a plain 404, no credential needed
GET {{base}}/public/does-not-exist.json
HTTP 404
