# Resolve the resource template list once. Used below in the two-valid-request
# batch case to fully assert the inner template descriptor without duplicating
# the canonical resourceTemplates assertion across hurl files
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 0, "method": "resources/templates/list" }
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.result.resourceTemplates" count == 1
jsonpath "$.result.resourceTemplates[0].uriTemplate" == "{{base}}/v1/catalog/{+path}{?bundle}"

# Batch of two valid request methods. JSON-RPC 2.0 §6 says the server must
# return responses as an array in any order. We preserve input order so clients
# do not have to id-match
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "id": 1, "method": "ping" },
  { "jsonrpc": "2.0", "id": 2, "method": "resources/templates/list" }
]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 2
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 1
jsonpath "$[0].result" isObject
jsonpath "$[0].result" isEmpty
jsonpath "$[0].error" not exists
jsonpath "$[1].jsonrpc" == "2.0"
jsonpath "$[1].id" == 2
jsonpath "$[1].error" not exists
jsonpath "$[1].result.resourceTemplates" count == 1
jsonpath "$[1].result.resourceTemplates[0].uriTemplate" == "{{base}}/v1/catalog/{+path}{?bundle}"
jsonpath "$[1].result.resourceTemplates[0].name" == "JSON Schema"
jsonpath "$[1].result.resourceTemplates[0].mimeType" == "application/schema+json"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch mixing a valid request with a structurally invalid sub-element. The
# valid one dispatches normally. The invalid one returns its own Invalid
# Request envelope. Neither failure can poison the other
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "id": 3, "method": "ping" },
  {}
]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 2
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 3
jsonpath "$[0].result" isObject
jsonpath "$[0].result" isEmpty
jsonpath "$[0].error" not exists
jsonpath "$[1].jsonrpc" == "2.0"
jsonpath "$[1].id" == null
jsonpath "$[1].result" not exists
jsonpath "$[1].error.code" == -32600
jsonpath "$[1].error.message" == "Invalid Request"
jsonpath "$[1].error.data" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch containing only notifications. JSON-RPC §6: server must not return an
# empty array, it must "return nothing at all". We use the same 202 Accepted
# semantics the single-notification path uses
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "method": "notifications/initialized" },
  { "jsonrpc": "2.0", "method": "notifications/cancelled", "params": { "requestId": 42 } }
]
```
HTTP 202
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

# Empty array body. JSON-RPC §6 explicitly says this must produce a single
# Response object, not an empty array
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" isObject
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == null
jsonpath "$.result" not exists
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"
jsonpath "$.error.data" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch with one request only. The response must still be an array, matching
# the shape of the request. Batch-aware clients distinguish "single response"
# from "1-element array"
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[ { "jsonrpc": "2.0", "id": 4, "method": "ping" } ]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 1
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 4
jsonpath "$[0].result" isObject
jsonpath "$[0].result" isEmpty
jsonpath "$[0].error" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch with one request and one notification. The notification produces no
# response entry, but the body is still a 1-element array because at least
# one response exists
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "id": 5, "method": "ping" },
  { "jsonrpc": "2.0", "method": "notifications/cancelled", "params": { "requestId": 99 } }
]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 1
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 5
jsonpath "$[0].result" isObject
jsonpath "$[0].result" isEmpty
jsonpath "$[0].error" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch where one sub-element is a successful tools/call. MCP 2025-03-26 tool
# responses do not carry `structuredContent` (that was added in 2025-06-18)
# and `tools/list` on this revision does not advertise `outputSchema` either,
# so the per-tool `outputSchema`-against-`structuredContent` round-trip from
# the 2025-11-25 single-call tests has no analogue here. The Link round-trip
# below still validates the whole batch body against the wrapped
# `mcp/response` schema, which keeps the contract on the response shape
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "id": 50, "method": "ping" },
  { "jsonrpc": "2.0", "id": 51, "method": "tools/call", "params": { "name": "list_directory", "arguments": { "path": "self/v1/schemas/mcp/notifications" } } }
]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 2
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 50
jsonpath "$[0].result" isObject
jsonpath "$[0].result" isEmpty
jsonpath "$[0].error" not exists
jsonpath "$[1].jsonrpc" == "2.0"
jsonpath "$[1].id" == 51
jsonpath "$[1].error" not exists
jsonpath "$[1].result.isError" == false
jsonpath "$[1].result.content" count == 3
jsonpath "$[1].result.content[0].type" == "text"
jsonpath "$[1].result.content[1].type" == "text"
jsonpath "$[1].result.content[1].text" == "Sourcemeta One MCP Cancelled Notification\n{{base}}/v1/catalog/self/v1/schemas/mcp/notifications/cancelled\nCancellation hint from a client for an in-flight request. Sourcemeta One accepts and discards these"
jsonpath "$[1].result.content[2].type" == "text"
jsonpath "$[1].result.content[2].text" == "Sourcemeta One MCP Initialized Notification\n{{base}}/v1/catalog/self/v1/schemas/mcp/notifications/initialized\nClient signal that the handshake is complete and normal operation may begin"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch where a tool call references an unknown tool. The error envelope is
# returned for that sub-element while the rest of the batch proceeds
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "id": 6, "method": "ping" },
  { "jsonrpc": "2.0", "id": 7, "method": "tools/call", "params": { "name": "no_such_tool" } }
]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 2
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 6
jsonpath "$[0].result" isObject
jsonpath "$[0].result" isEmpty
jsonpath "$[0].error" not exists
jsonpath "$[1].jsonrpc" == "2.0"
jsonpath "$[1].id" == 7
jsonpath "$[1].result" not exists
jsonpath "$[1].error.code" == -32602
jsonpath "$[1].error.message" == "Invalid tool name"
jsonpath "$[1].error.data" == "Use `tools/list` to discover the available tools"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch elements with identical ids. The spec does not forbid id collisions,
# clients are responsible for picking unique ones. We must still process both
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "id": 8, "method": "ping" },
  { "jsonrpc": "2.0", "id": 8, "method": "ping" }
]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 2
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 8
jsonpath "$[0].result" isObject
jsonpath "$[0].result" isEmpty
jsonpath "$[0].error" not exists
jsonpath "$[1].jsonrpc" == "2.0"
jsonpath "$[1].id" == 8
jsonpath "$[1].result" isObject
jsonpath "$[1].result" isEmpty
jsonpath "$[1].error" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch of literal scalars. Per JSON-RPC §6 each non-conforming element
# yields its own Invalid Request envelope. The response array has one entry
# per sub-element (id null because non-objects have no id to echo)
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[ 1, "foo", null ]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 3
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == null
jsonpath "$[0].result" not exists
jsonpath "$[0].error.code" == -32600
jsonpath "$[0].error.message" == "Invalid Request"
jsonpath "$[0].error.data" not exists
jsonpath "$[1].jsonrpc" == "2.0"
jsonpath "$[1].id" == null
jsonpath "$[1].result" not exists
jsonpath "$[1].error.code" == -32600
jsonpath "$[1].error.message" == "Invalid Request"
jsonpath "$[1].error.data" not exists
jsonpath "$[2].jsonrpc" == "2.0"
jsonpath "$[2].id" == null
jsonpath "$[2].result" not exists
jsonpath "$[2].error.code" == -32600
jsonpath "$[2].error.message" == "Invalid Request"
jsonpath "$[2].error.data" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch on 2025-06-18 is rejected with a single Invalid Request envelope
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
[ { "jsonrpc": "2.0", "id": 1, "method": "ping" } ]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-06-18
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" isObject
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == null
jsonpath "$.result" not exists
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"
jsonpath "$.error.data" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Same single-envelope rejection on 2025-11-25
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
[ { "jsonrpc": "2.0", "id": 1, "method": "ping" } ]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" isObject
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == null
jsonpath "$.result" not exists
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"
jsonpath "$.error.data" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Batch with one valid request and one sub-element with a wrong jsonrpc
# version. The wrong-version one comes back as Invalid Request, the other
# dispatches normally
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "id": 9, "method": "ping" },
  { "jsonrpc": "1.0", "id": 10, "method": "ping" }
]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$" count == 2
jsonpath "$[0].jsonrpc" == "2.0"
jsonpath "$[0].id" == 9
jsonpath "$[0].result" isObject
jsonpath "$[0].result" isEmpty
jsonpath "$[0].error" not exists
jsonpath "$[1].jsonrpc" == "2.0"
jsonpath "$[1].id" == 10
jsonpath "$[1].result" not exists
jsonpath "$[1].error.code" == -32600
jsonpath "$[1].error.message" == "Invalid Request"
jsonpath "$[1].error.data" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
