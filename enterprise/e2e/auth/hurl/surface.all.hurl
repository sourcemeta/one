# A policy on /self/v1/health locks the readiness probe at dispatch: denied
# without a credential, with the canonical 401 shape
GET {{base}}/self/v1/health
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

# The problem document validates against the error schema named by its Link
POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A wrong credential is denied identically
GET {{base}}/self/v1/health
Authorization: Bearer not-the-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
wrong_key_body: body
wrong_key_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/self/v1/api/schemas/evaluate{{wrong_key_schema}}
```
{{wrong_key_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A content key that opens /private does not open this surface
GET {{base}}/self/v1/health
Authorization: Bearer primary-secret-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
content_key_body: body
content_key_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/self/v1/api/schemas/evaluate{{content_key_schema}}
```
{{content_key_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The internal key does not cross over either
GET {{base}}/self/v1/health
Authorization: Bearer internal-secret-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
internal_key_body: body
internal_key_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/self/v1/api/schemas/evaluate{{internal_key_schema}}
```
{{internal_key_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The surface key admits, and the probe answers
GET {{base}}/self/v1/health
Authorization: Bearer surface-secret-key
HTTP 200
Cache-Control: no-store
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Content-Length" == "0"

# HEAD is gated exactly like GET, and per RFC 9110 carries no body
HEAD {{base}}/self/v1/health
HTTP 401
WWW-Authenticate: Bearer realm="registry"
[Asserts]
bytes count == 0

# HEAD admitted with the key, still with no body
HEAD {{base}}/self/v1/health
Authorization: Bearer surface-secret-key
HTTP 200
Cache-Control: no-store
[Asserts]
bytes count == 0

# A CORS preflight carries no credentials and is never gated
OPTIONS {{base}}/self/v1/health
Origin: http://example.com
Access-Control-Request-Method: GET
HTTP 204
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, HEAD, OPTIONS
Access-Control-Allow-Headers: Accept, Accept-Encoding
Access-Control-Max-Age: 3600
Cache-Control: no-store
Allow: GET, HEAD, OPTIONS

# The gate precedes method validation: an unsupported method denies without the
# key rather than revealing the method is wrong
POST {{base}}/self/v1/health
Content-Type: application/json
```
{}
```
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
method_denied_body: body
method_denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/self/v1/api/schemas/evaluate{{method_denied_schema}}
```
{{method_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# With the key the surface admits and the method check then answers 405
POST {{base}}/self/v1/health
Authorization: Bearer surface-secret-key
Content-Type: application/json
```
{}
```
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Allow: GET, HEAD, OPTIONS
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
method_allowed_body: body
method_allowed_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:method-not-allowed"
jsonpath "$.title" == "Method Not Allowed"
jsonpath "$.status" == 405
jsonpath "$.detail" == "This HTTP method is invalid for this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{method_allowed_schema}}
```
{{method_allowed_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The lock does not bleed past its segment boundary: the sibling machine API
# answers a public schema's health to anonymous callers
GET {{base}}/self/v1/api/schemas/health/public/string
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/health/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
public_health: body
public_health_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.score" exists
jsonpath "$.errors" exists

# The health response round-trips against its schema through evaluate
POST {{base}}/self/v1/api/schemas/evaluate{{public_health_schema}}
```
{{public_health}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The MCP endpoint stays public, and its envelope round-trips against the MCP
# response schema named by its Link
POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
mcp_body: body
mcp_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {}
}

POST {{base}}/self/v1/api/schemas/evaluate{{mcp_schema}}
```
{{mcp_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A self schema under /self/v1/schemas stays public
GET {{base}}/self/v1/schemas/api/error
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *

# The trailing-slash MCP route is a distinct route, equally public
POST {{base}}/self/v1/mcp/
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 7, "method": "ping" }
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
mcp_trailing_body: body
mcp_trailing_schema: header "Link" regex "<([^>]+)>"
{
  "jsonrpc": "2.0",
  "id": 7,
  "result": {}
}

POST {{base}}/self/v1/api/schemas/evaluate{{mcp_trailing_schema}}
```
{{mcp_trailing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The static asset surface is governed by no policy, so the bundled web UI
# assets stay public, with their immutable caching intact
GET {{base}}/self/v1/static/style.min.css
HTTP 200
Cache-Control: public, max-age=31536000, immutable
Content-Type: text/css
Vary: Accept-Encoding
[Asserts]
header "ETag" exists
header "Last-Modified" exists

# The catch-all schema route opts out of the surface gate: a public schema is
# served keyless, governed only by the content gate
GET {{base}}/public/string.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/public/string",
  "type": "string"
}

# The surface key is not a master key: it does not satisfy the content gate on
# an apiKey-scoped schema
GET {{base}}/private/secret.json
Authorization: Bearer surface-secret-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
not_master_body: body
not_master_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/self/v1/api/schemas/evaluate{{not_master_schema}}
```
{{not_master_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
