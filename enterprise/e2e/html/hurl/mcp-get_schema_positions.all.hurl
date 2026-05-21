POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 600,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "{{base}}/test/bare/01" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 600
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent['']" count == 4
variable "text" jsonpath "$['']" count == 4
jsonpath "$.result.structuredContent['/$schema']" count == 4
variable "text" jsonpath "$['/$schema']" count == 4
jsonpath "$.result.structuredContent['/$id']" count == 4
variable "text" jsonpath "$['/$id']" count == 4
jsonpath "$.result.structuredContent['/type']" count == 4
variable "text" jsonpath "$['/type']" count == 4

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 601,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "{{base}}/test/object" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 601
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent['/title']" count == 4
variable "text" jsonpath "$['/title']" count == 4
jsonpath "$.result.structuredContent['/description']" count == 4
variable "text" jsonpath "$['/description']" count == 4
jsonpath "$.result.structuredContent['/properties']" count == 4
variable "text" jsonpath "$['/properties']" count == 4
jsonpath "$.result.structuredContent['/properties/foo']" count == 4
variable "text" jsonpath "$['/properties/foo']" count == 4
jsonpath "$.result.structuredContent['/properties/foo/type']" count == 4
variable "text" jsonpath "$['/properties/foo/type']" count == 4
jsonpath "$.result.structuredContent['/properties/INVALID_PROPERTY_1']" count == 4
variable "text" jsonpath "$['/properties/INVALID_PROPERTY_1']" count == 4
jsonpath "$.result.structuredContent['/examples']" count == 4
variable "text" jsonpath "$['/examples']" count == 4
jsonpath "$.result.structuredContent['/examples/0']" count == 4
variable "text" jsonpath "$['/examples/0']" count == 4

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 602,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "test/object" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 602
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.result" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 610,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "{{base}}/test/does-not-exist" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 610
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Schema not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 611,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "{{base}}/test" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 611
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Schema not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 612,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "https://example.com/test/object" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 612
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].text" == "Schema not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 613,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "{{base}}/test/foo/../object" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 613
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" exists
variable "text" jsonpath "$" exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 614,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "{{base}}/../etc/passwd" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 614
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].text" == "Schema not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 620,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": {}
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 620
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 621,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 621
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.result" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 622,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": 42 }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 622
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 623,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "{{base}}/test/object", "extra": "foo" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 623
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 624,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 624
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 625,
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": null
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 625
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "positions-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "get_schema_positions",
    "arguments": { "schema": "{{base}}/test/object" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "positions-call-string-id"
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" exists
variable "text" jsonpath "$" exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
