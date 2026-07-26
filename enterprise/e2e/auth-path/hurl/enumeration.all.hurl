# Every surface that enumerates schemas has to filter against the caller's
# credential, and under a base path those surfaces once answered with the whole
# catalogue because the location they checked still carried the base path,
# which no policy is declared with. Each surface is asked anonymously and then
# with the key, so filtering is proven to distinguish the two rather than to
# refuse everything, and every response is asserted whole.

# The gated schema itself is denied anonymously
GET {{base}}/registry/private/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
[Captures]
denied_body: body
denied_schema: header "Link" regex "</registry([^>]+)>"
[Asserts]
header "ETag" not exists
header "Last-Modified" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/registry/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Search does not disclose it, and the empty result is the whole body
GET {{base}}/registry/self/v1/api/schemas/search?q=secret
HTTP 200
Cache-Control: public, max-age=60
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </registry/self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
empty_search: body
empty_search_schema: header "Link" regex "</registry([^>]+)>"
[Asserts]
jsonpath "$" count == 0

POST {{base}}/registry/self/v1/api/schemas/evaluate{{empty_search_schema}}
```
{{empty_search}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Nor does a query naming the gated collection itself
GET {{base}}/registry/self/v1/api/schemas/search?q=private
HTTP 200
Cache-Control: public, max-age=60
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </registry/self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Asserts]
jsonpath "$" count == 0

# The public schema is disclosed in full, so the filter is not refusing all
GET {{base}}/registry/self/v1/api/schemas/search?q=string
HTTP 200
Cache-Control: public, max-age=60
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </registry/self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
public_search: body
public_search_schema: header "Link" regex "</registry([^>]+)>"
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/registry/public/string"
jsonpath "$[0].identifier" == "http://localhost:8000/registry/public/string"
jsonpath "$[0].title" == ""
jsonpath "$[0].description" == ""
jsonpath "$[0].priority" == 50

POST {{base}}/registry/self/v1/api/schemas/evaluate{{public_search_schema}}
```
{{public_search}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# With the key the gated schema becomes visible through the same query, so the
# earlier empty answer was filtering rather than an absent index
GET {{base}}/registry/self/v1/api/schemas/search?q=secret
Authorization: Bearer content-secret-key
HTTP 200
Cache-Control: private, max-age=60
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </registry/self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
admitted_search: body
admitted_search_schema: header "Link" regex "</registry([^>]+)>"
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/registry/private/secret"
jsonpath "$[0].identifier" == "http://localhost:8000/registry/private/secret"
jsonpath "$[0].title" == ""
jsonpath "$[0].description" == ""
jsonpath "$[0].priority" == 50

POST {{base}}/registry/self/v1/api/schemas/evaluate{{admitted_search_schema}}
```
{{admitted_search}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

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
Link: </registry/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
anonymous_resources: body
anonymous_resources_schema: header "Link" regex "</registry([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.error" not exists
jsonpath "$.result.resources[*].uri" not includes "http://localhost:8000/registry/private/secret"
jsonpath "$.result.resources[*].uri" includes "http://localhost:8000/registry/public/string"

POST {{base}}/registry/self/v1/api/schemas/evaluate{{anonymous_resources_schema}}
```
{{anonymous_resources}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# With the key it appears, so the listing filters rather than truncates
POST {{base}}/registry/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
Authorization: Bearer content-secret-key
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
Link: </registry/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
admitted_resources: body
admitted_resources_schema: header "Link" regex "</registry([^>]+)>"
[Asserts]
jsonpath "$.result.resources[*].uri" includes "http://localhost:8000/registry/private/secret"
jsonpath "$.result.resources[*].uri" includes "http://localhost:8000/registry/public/string"

POST {{base}}/registry/self/v1/api/schemas/evaluate{{admitted_resources_schema}}
```
{{admitted_resources}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The MCP search tool filters on the same basis, asserted as a whole envelope
POST {{base}}/registry/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": { "name": "search_schemas", "arguments": { "q": "secret" } }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Link: </registry/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
anonymous_tool: body
anonymous_tool_schema: header "Link" regex "</registry([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.error" not exists
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.results" count == 0

POST {{base}}/registry/self/v1/api/schemas/evaluate{{anonymous_tool_schema}}
```
{{anonymous_tool}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
