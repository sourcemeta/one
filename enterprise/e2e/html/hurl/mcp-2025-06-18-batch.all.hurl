# MCP 2025-06-18 removed support for JSON-RPC batching. See the changelog at
# https://modelcontextprotocol.io/specification/2025-06-18/changelog. Any array
# body on this protocol version is rejected with a single `-32600 Invalid
# Request` envelope, regardless of whether the batch contents look valid

# Single-element batch
POST {{base}}/self/v1/mcp
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
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-06-18
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
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

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
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

# Multi-element batch with otherwise-valid requests
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-06-18
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
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-06-18
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
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

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
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

# Empty array body
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
[]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-06-18
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
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

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
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

# Batch of only notifications. On 2025-03-26 we would 202 Accept these. On
# 2025-06-18 the version rejects the batch wrapper itself, so the contents
# do not matter and the response is still the Invalid Request envelope
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
[
  { "jsonrpc": "2.0", "method": "notifications/initialized" },
  { "jsonrpc": "2.0", "method": "notifications/cancelled", "params": { "requestId": 1 } }
]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-06-18
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
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

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
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

# Batch with garbage sub-elements
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
[ 1, "foo", null ]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-06-18
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
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

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
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
