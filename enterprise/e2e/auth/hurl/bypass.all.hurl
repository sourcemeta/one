# None of these spellings of an apiKey path may bypass the gate. Each must be
# denied exactly as the canonical path would be, without a credential.

# Upper-cased prefix
GET {{base}}/PRIVATE/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Repeated leading slash
GET {{base}}//private/secret.json
HTTP 401
Content-Type: application/problem+json

# Repeated interior slash
GET {{base}}/private//secret.json
HTTP 401
Content-Type: application/problem+json

# Single-dot segment
GET {{base}}/private/./secret.json
HTTP 401
Content-Type: application/problem+json

# Leading single-dot segment
GET {{base}}/./private/secret.json
HTTP 401
Content-Type: application/problem+json

# Parent traversal from a public prefix into an apiKey prefix
GET {{base}}/public/../private/secret.json
HTTP 401
Content-Type: application/problem+json

# Percent-encoded slash
GET {{base}}/private%2Fsecret.json
HTTP 401
Content-Type: application/problem+json

# Parent traversal that lands back on a public prefix is still anonymous
GET {{base}}/private/../public/string.json
HTTP 200
Content-Type: application/schema+json
[Asserts]
jsonpath "$.type" == "string"
