POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 400,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 400
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.score" == 67
variable "text" jsonpath "$.score" == 67
jsonpath "$.result.structuredContent.errors" count == 1
variable "text" jsonpath "$.errors" count == 1
jsonpath "$.result.structuredContent.errors[0].name" == "custom/all_properties_camelcase"
variable "text" jsonpath "$.errors[0].name" == "custom/all_properties_camelcase"
jsonpath "$.result.structuredContent.errors[0].message" == "Ensure camelCase properties"
variable "text" jsonpath "$.errors[0].message" == "Ensure camelCase properties"
jsonpath "$.result.structuredContent.errors[0].custom" == true
variable "text" jsonpath "$.errors[0].custom" == true
jsonpath "$.result.structuredContent.errors[0].pointers" count == 1
variable "text" jsonpath "$.errors[0].pointers" count == 1
jsonpath "$.result.structuredContent.errors[0].pointers[0]" == "/properties/INVALID_PROPERTY_1"
variable "text" jsonpath "$.errors[0].pointers[0]" == "/properties/INVALID_PROPERTY_1"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 401,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 401
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.score" == 0
variable "text" jsonpath "$.score" == 0
jsonpath "$.result.structuredContent.errors" count == 3
variable "text" jsonpath "$.errors" count == 3

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 402,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 402
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment"
jsonpath "$.result" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 410,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 410
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
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
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 411,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 411
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
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 412,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 412
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
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 413,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 413
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.score" == 67
variable "text" jsonpath "$.score" == 67

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 414,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 414
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

# Schema URI with a fragment is rejected
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 415,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
    "arguments": { "schema": "{{base}}/test/object#meta" }
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
jsonpath "$.id" == 415
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment"
jsonpath "$.result" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 420,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 420
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/health/rpc#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The value was expected to be an object that defines the property \"schema\""

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 421,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 421
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment"
jsonpath "$.result" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 422,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 422
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/schema/type"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/health/rpc#/properties/schema/type"
jsonpath "$.error.data.errors[0].instanceLocation" == "/schema"
jsonpath "$.error.data.errors[0].error" == "The value was expected to be of type string but it was of type integer"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/health/rpc#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the defined properties subschemas"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 423,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 423
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/additionalProperties"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/health/rpc#/additionalProperties"
jsonpath "$.error.data.errors[0].instanceLocation" == "/extra"
jsonpath "$.error.data.errors[0].error" == "The object value was not expected to define the property \"extra\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/additionalProperties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/health/rpc#/additionalProperties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was not expected to define additional properties"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 424,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health"
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
jsonpath "$.id" == 424
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/health/rpc#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The value was expected to be an object that defines the property \"schema\""

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 425,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == 425
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
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "health-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
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
jsonpath "$.id" == "health-call-string-id"
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.score" == 67
variable "text" jsonpath "$.score" == 67

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 970,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
    "arguments": { "schema": "{{base}}/test/object" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
health_baseline_text: jsonpath "$.result.content[0].text"

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 971,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
    "arguments": { "schema": "{{base}}/TEST/OBJECT" }
  }
}
```
HTTP 200
[Asserts]
jsonpath "$.id" == 971
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{health_baseline_text}}"

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 972,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
    "arguments": { "schema": "{{base}}/test/object.JSON" }
  }
}
```
HTTP 200
[Asserts]
jsonpath "$.id" == 972
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{health_baseline_text}}"

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 973,
  "method": "tools/call",
  "params": {
    "name": "get_schema_health",
    "arguments": { "schema": "HTTP://LOCALHOST:{{port}}/test/object" }
  }
}
```
HTTP 200
[Asserts]
jsonpath "$.id" == 973
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{health_baseline_text}}"
