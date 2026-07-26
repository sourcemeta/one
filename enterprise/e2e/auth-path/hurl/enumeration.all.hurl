# Every surface that enumerates schemas has to filter against the caller's
# credential, and under a base path those surfaces once answered with the whole
# catalogue because the path they checked still carried the base path, which no
# policy is declared with. Each surface is asked anonymously and then with a
# credential, so filtering is proven to distinguish the two rather than to
# refuse everything.

# The gated schema itself is denied anonymously
GET {{base}}/registry/private/secret.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Search does not disclose it
GET {{base}}/registry/self/v1/api/schemas/search?q=secret
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
jsonpath "$" count == 0

# Nor does a query that would match it by name fragment
GET {{base}}/registry/self/v1/api/schemas/search?q=private
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 0

# The public schema is disclosed, so the filter is not simply refusing all
GET {{base}}/registry/self/v1/api/schemas/search?q=string
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/registry/public/string"
jsonpath "$[0].identifier" == "http://localhost:8000/registry/public/string"

# With the credential the gated schema becomes visible through the same query
GET {{base}}/registry/self/v1/api/schemas/search?q=secret
Authorization: Bearer content-secret-key
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/registry/private/secret"
jsonpath "$[0].identifier" == "http://localhost:8000/registry/private/secret"

# The MCP resource listing does not disclose the gated schema anonymously
POST {{base}}/registry/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "resources/list"
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.result.resources[*].uri" not includes "http://localhost:8000/registry/private/secret"
jsonpath "$.result.resources[*].uri" includes "http://localhost:8000/registry/public/string"

# With the credential it appears, so the listing filters rather than truncates
POST {{base}}/registry/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
Authorization: Bearer content-secret-key
```
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "resources/list"
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 2
jsonpath "$.result.resources[*].uri" includes "http://localhost:8000/registry/private/secret"
jsonpath "$.result.resources[*].uri" includes "http://localhost:8000/registry/public/string"

# The MCP search tool filters on the same basis
POST {{base}}/registry/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 3,
  "method": "tools/call",
  "params": { "name": "search_schemas", "arguments": { "q": "secret" } }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 3
jsonpath "$.result.structuredContent.results" count == 0
