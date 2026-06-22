# The locked static surface is denied without a credential, the error Link
# carrying the base path
GET {{base}}/registry/self/v1/static/style.min.css
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
static_denied_body: body
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

# The problem document validates against the error schema. The evaluate
# endpoint takes the schema as a base-path-relative argument
POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{static_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A wrong credential is denied identically
GET {{base}}/registry/self/v1/static/style.min.css
Authorization: Bearer not-the-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
[Captures]
static_wrong_body: body
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{static_wrong_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The content key does not open the static surface
GET {{base}}/registry/self/v1/static/style.min.css
Authorization: Bearer content-secret-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
[Captures]
static_content_body: body
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{static_content_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The static key admits, serving the bundled asset with its immutable caching
GET {{base}}/registry/self/v1/static/style.min.css
Authorization: Bearer static-secret-key
HTTP 200
Cache-Control: public, max-age=31536000, immutable
Content-Type: text/css
Vary: Accept-Encoding
[Asserts]
header "ETag" exists
header "Last-Modified" exists

# HEAD is gated exactly like GET
HEAD {{base}}/registry/self/v1/static/style.min.css
HTTP 401
WWW-Authenticate: Bearer realm="registry"

# HEAD admitted with the key
HEAD {{base}}/registry/self/v1/static/style.min.css
Authorization: Bearer static-secret-key
HTTP 200
Content-Type: text/css
Cache-Control: public, max-age=31536000, immutable

# A CORS preflight is never gated, even under the base path
OPTIONS {{base}}/registry/self/v1/static/style.min.css
Origin: http://example.com
Access-Control-Request-Method: GET
HTTP 204
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, HEAD, OPTIONS
Access-Control-Allow-Headers: Accept-Encoding, If-None-Match, If-Modified-Since
Access-Control-Max-Age: 3600
Cache-Control: no-store
Allow: GET, HEAD, OPTIONS

# A public schema is served to everyone, its $id composed against the base path
GET {{base}}/registry/public/string.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Vary: User-Agent, Accept-Encoding
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/registry/public/string",
  "type": "string"
}

# An apiKey schema is denied without a credential
GET {{base}}/registry/private/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
private_denied_body: body
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{private_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The static key does not open content: the gates are independent
GET {{base}}/registry/private/secret.json
Authorization: Bearer static-secret-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
[Captures]
private_static_body: body
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{private_static_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The content key admits, and the response is marked private under the base path
GET {{base}}/registry/private/secret.json
Authorization: Bearer content-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Vary: User-Agent, Accept-Encoding
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/registry/private/secret",
  "type": "object",
  "required": [ "token" ],
  "properties": {
    "token": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# A public sibling surface stays open: the readiness probe is reachable
GET {{base}}/registry/self/v1/health
HTTP 200
Cache-Control: no-store
[Asserts]
header "Content-Length" == "0"

# A self schema under /self/v1/schemas is public
GET {{base}}/registry/self/v1/schemas/api/error
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *

# The machine API is public and answers a public schema's health anonymously
GET {{base}}/registry/self/v1/api/schemas/health/public/string
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </registry/self/v1/schemas/api/schemas/health/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
public_health: body
[Asserts]
header "ETag" exists
jsonpath "$.score" exists
jsonpath "$.errors" exists

# The health response round-trips against its schema through evaluate
POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/health/response
```
{{public_health}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The MCP endpoint is public under the base path, and its envelope round-trips
# against the MCP response schema named by its Link
POST {{base}}/registry/self/v1/mcp
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </registry/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
mcp_body: body
[Asserts]
header "Vary" not exists
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {}
}

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/mcp/response
```
{{mcp_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
