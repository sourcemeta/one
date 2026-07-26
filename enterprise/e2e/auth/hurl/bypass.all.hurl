# None of these spellings of an apiKey path may bypass the gate. Each one that
# canonicalises onto the protected resource is denied exactly as the canonical
# path would be, and a spelling that resolves elsewhere never reaches it.

# Upper-cased prefix
GET {{base}}/PRIVATE/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Repeated leading slash
GET {{base}}//private/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Repeated interior slash
GET {{base}}/private//secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Single-dot segment
GET {{base}}/private/./secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Leading single-dot segment
GET {{base}}/./private/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Parent traversal from a public prefix into an apiKey prefix
GET {{base}}/public/../private/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Percent-encoded slash: not a path separator, so this is a single literal
# segment at the public root that names no resource. It answers a plain 404
# rather than a disguised entry into the apiKey scope
GET {{base}}/private%2Fsecret.json
HTTP 404
Content-Type: application/problem+json
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.status" == 404

# Parent traversal that lands back on a public prefix is still anonymous
GET {{base}}/private/../public/string.json
HTTP 200
Content-Type: application/schema+json
[Asserts]
jsonpath "$.type" == "string"

# Repeated leading slash combined with an upper-cased prefix. Each half of this
# spelling is covered above on its own, and the pair once slipped past the gate
# because the path reached the policy trie in a spelling the trie never holds
GET {{base}}//PRIVATE/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# The same pair against a directory rather than a schema
GET {{base}}//PRIVATE/
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Repeated leading slash, upper case, and a dot segment together
GET {{base}}//PRIVATE/./secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Three leading slashes with an upper-cased prefix
GET {{base}}///PRIVATE/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# An upper-cased extension names the same representation
GET {{base}}//PRIVATE/SECRET.JSON
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# A JWT-gated prefix is reached through the same spelling, and denied the same
GET {{base}}//MACHINE/config.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# The credential still admits the caller through a non-canonical spelling, so
# the gate is reading the same location rather than merely refusing oddities
GET {{base}}//PRIVATE/secret.json
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/schema+json
[Asserts]
jsonpath "$.type" == "object"
