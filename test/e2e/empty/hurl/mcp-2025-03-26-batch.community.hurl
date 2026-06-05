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
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 2
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 1
jsonpath "$[0].error" not exists
jsonpath "$[0].result.protocolVersion" == "2025-03-26"
jsonpath "$[0].result.capabilities" isObject
jsonpath "$[0].result.capabilities.resources" isObject
jsonpath "$[0].result.capabilities.resources" isEmpty
jsonpath "$[0].result.serverInfo.name" == "sourcemeta-one"
jsonpath "$[0].result.serverInfo.version" matches "^.+$"
jsonpath "$[0].result.serverInfo.title" not exists
jsonpath "$[0].result.serverInfo.description" not exists
jsonpath "$[0].result.serverInfo.websiteUrl" not exists
jsonpath "$[0].result.instructions" == "This is an instance of Sourcemeta One named \"Sourcemeta\" (The next-generation JSON Schema platform). Sourcemeta One is a JSON Schema registry. It serves a catalog of JSON Schemas organized as a tree of directories. Every schema is exposed as an MCP resource via the `JSON Schema` resource template (see `resources/templates/list`) and is addressable by its canonical absolute URI. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Use `tools/list` to discover the operations this catalog exposes. Because this server is a JSON Schema registry, the schemas defining each tool's input parameters are themselves resources in this registry. To inspect a tool's full parameter shape, call `resources/read` on the `$ref` URL in that tool's `inputSchema`. Learn more at https://one.sourcemeta.com"
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
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
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
MCP-Protocol-Version: 2025-11-25
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
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
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
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
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
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
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
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
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
