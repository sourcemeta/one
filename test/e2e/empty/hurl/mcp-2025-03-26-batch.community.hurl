# Community batch: a request that the stub can answer (initialize) and one
# that it cannot (tools/call) round-trip together. The tool call returns the
# stub's "Enterprise edition required" envelope. The initialize succeeds. The
# batch as a whole completes and the response shape is an array
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  {
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-03-26",
      "capabilities": {},
      "clientInfo": { "name": "hurl", "version": "1.0.0" }
    }
  },
  { "jsonrpc": "2.0", "id": 2, "method": "tools/call", "params": { "name": "x" } }
]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 2
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 1
jsonpath "$[0].error" not exists
jsonpath "$[0].result.protocolVersion" == "2025-03-26"
jsonpath "$[0].result.capabilities.resources.subscribe" == false
jsonpath "$[0].result.capabilities.resources.listChanged" == false
jsonpath "$[0].result.serverInfo.name" == "sourcemeta-one"
jsonpath "$[0].result.serverInfo.version" == "1.0.0"
jsonpath "$[0].result.serverInfo.title" == "Sourcemeta One"
jsonpath "$[0].result.serverInfo.description" == "A JSON Schema registry"
jsonpath "$[0].result.serverInfo.websiteUrl" == "https://example.com"
jsonpath "$[1].jsonrpc" == "2.0"
jsonpath "$[1].id" == 2
jsonpath "$[1].result" not exists
jsonpath "$[1].error.code" == -32000
jsonpath "$[1].error.message" == "Enterprise edition required"
jsonpath "$[1].error.data" == "MCP support is only available in the Enterprise edition of Sourcemeta One"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Batch on 2025-11-25 community is still rejected with a single Invalid
# Request envelope because batches were removed from that revision
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
[ { "jsonrpc": "2.0", "id": 1, "method": "initialize", "params": { "protocolVersion": "2025-11-25", "capabilities": {}, "clientInfo": { "name": "h", "version": "1" } } } ]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" isObject
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == null
jsonpath "$.result" not exists
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"
jsonpath "$.error.data" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Batch of only notifications on community: 202 Accepted with no body
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "method": "notifications/initialized" },
  { "jsonrpc": "2.0", "method": "notifications/cancelled", "params": { "requestId": 1 } }
]
```
HTTP 202
Access-Control-Allow-Origin: http://localhost:8000
[Asserts]
bytes count == 0

# Empty array on 2025-03-26 community: single Invalid Request envelope
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" isObject
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == null
jsonpath "$.result" not exists
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"
jsonpath "$.error.data" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
