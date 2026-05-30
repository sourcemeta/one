POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1000,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": {
      "schema": "{{base}}/test/bare/01",
      "stringifiedInstance": "\"hello\""
    }
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
jsonpath "$.id" == 1000
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.valid" == true
variable "text" jsonpath "$.valid" == true
jsonpath "$.result.structuredContent.steps" count == 2
variable "text" jsonpath "$.steps" count == 2
jsonpath "$.result.structuredContent.steps[0].type" == "push"
variable "text" jsonpath "$.steps[0].type" == "push"
jsonpath "$.result.structuredContent.steps[0].name" == "AssertionTypeStrict"
variable "text" jsonpath "$.steps[0].name" == "AssertionTypeStrict"
jsonpath "$.result.structuredContent.steps[0].evaluatePath" == "/type"
variable "text" jsonpath "$.steps[0].evaluatePath" == "/type"
jsonpath "$.result.structuredContent.steps[0].instanceLocation" == ""
variable "text" jsonpath "$.steps[0].instanceLocation" == ""
jsonpath "$.result.structuredContent.steps[0].instancePositions" count == 4
jsonpath "$.result.structuredContent.steps[0].instancePositions[0]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[1]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[2]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[3]" == 7
variable "text" jsonpath "$.steps[0].instancePositions" count == 4
variable "text" jsonpath "$.steps[0].instancePositions[0]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[1]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[2]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[3]" == 7
jsonpath "$.result.structuredContent.steps[0].keywordLocation" == "{{base}}/test/bare/01#/type"
variable "text" jsonpath "$.steps[0].keywordLocation" == "{{base}}/test/bare/01#/type"
jsonpath "$.result.structuredContent.steps[0].vocabulary" == "https://json-schema.org/draft/2020-12/vocab/validation"
variable "text" jsonpath "$.steps[0].vocabulary" == "https://json-schema.org/draft/2020-12/vocab/validation"
jsonpath "$.result.structuredContent.steps[1].type" == "pass"
variable "text" jsonpath "$.steps[1].type" == "pass"
jsonpath "$.result.structuredContent.steps[1].name" == "AssertionTypeStrict"
variable "text" jsonpath "$.steps[1].name" == "AssertionTypeStrict"
jsonpath "$.result.structuredContent.steps[1].instancePositions" count == 4
jsonpath "$.result.structuredContent.steps[1].instancePositions[0]" == 1
jsonpath "$.result.structuredContent.steps[1].instancePositions[1]" == 1
jsonpath "$.result.structuredContent.steps[1].instancePositions[2]" == 1
jsonpath "$.result.structuredContent.steps[1].instancePositions[3]" == 7
variable "text" jsonpath "$.steps[1].instancePositions" count == 4
variable "text" jsonpath "$.steps[1].instancePositions[0]" == 1
variable "text" jsonpath "$.steps[1].instancePositions[1]" == 1
variable "text" jsonpath "$.steps[1].instancePositions[2]" == 1
variable "text" jsonpath "$.steps[1].instancePositions[3]" == 7

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
  "id": 1001,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": {
      "schema": "{{base}}/test/bare/01",
      "stringifiedInstance": "42"
    }
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
jsonpath "$.id" == 1001
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.valid" == false
variable "text" jsonpath "$.valid" == false
jsonpath "$.result.structuredContent.steps" count == 2
variable "text" jsonpath "$.steps" count == 2
jsonpath "$.result.structuredContent.steps[0].type" == "push"
variable "text" jsonpath "$.steps[0].type" == "push"
jsonpath "$.result.structuredContent.steps[0].name" == "AssertionTypeStrict"
variable "text" jsonpath "$.steps[0].name" == "AssertionTypeStrict"
jsonpath "$.result.structuredContent.steps[0].instancePositions" count == 4
jsonpath "$.result.structuredContent.steps[0].instancePositions[0]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[1]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[2]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[3]" == 2
variable "text" jsonpath "$.steps[0].instancePositions" count == 4
variable "text" jsonpath "$.steps[0].instancePositions[0]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[1]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[2]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[3]" == 2
jsonpath "$.result.structuredContent.steps[1].type" == "fail"
variable "text" jsonpath "$.steps[1].type" == "fail"
jsonpath "$.result.structuredContent.steps[1].name" == "AssertionTypeStrict"
variable "text" jsonpath "$.steps[1].name" == "AssertionTypeStrict"
jsonpath "$.result.structuredContent.steps[1].instancePositions" count == 4
jsonpath "$.result.structuredContent.steps[1].instancePositions[0]" == 1
jsonpath "$.result.structuredContent.steps[1].instancePositions[1]" == 1
jsonpath "$.result.structuredContent.steps[1].instancePositions[2]" == 1
jsonpath "$.result.structuredContent.steps[1].instancePositions[3]" == 2
variable "text" jsonpath "$.steps[1].instancePositions" count == 4
variable "text" jsonpath "$.steps[1].instancePositions[0]" == 1
variable "text" jsonpath "$.steps[1].instancePositions[1]" == 1
variable "text" jsonpath "$.steps[1].instancePositions[2]" == 1
variable "text" jsonpath "$.steps[1].instancePositions[3]" == 2

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
  "id": 1002,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": {
      "schema": "{{base}}/test/object",
      "stringifiedInstance": "{}"
    }
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
jsonpath "$.id" == 1002
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.valid" == true
variable "text" jsonpath "$.valid" == true
jsonpath "$.result.structuredContent.steps" count >= 1
variable "text" jsonpath "$.steps" count >= 1
jsonpath "$.result.structuredContent.steps[0].instancePositions" count == 4
jsonpath "$.result.structuredContent.steps[0].instancePositions[0]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[1]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[2]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[3]" == 2
variable "text" jsonpath "$.steps[0].instancePositions" count == 4
variable "text" jsonpath "$.steps[0].instancePositions[0]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[1]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[2]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[3]" == 2

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
  "id": 1003,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "test/bare/01", "stringifiedInstance": "\"hello\"" }
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
jsonpath "$.id" == 1003
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
  "id": 1010,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/does-not-exist", "stringifiedInstance": "\"hi\"" }
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
jsonpath "$.id" == 1010
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
  "id": 1011,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test", "stringifiedInstance": "\"x\"" }
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
jsonpath "$.id" == 1011
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
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1013,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "https://example.com/test/bare/01", "stringifiedInstance": "\"hi\"" }
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
jsonpath "$.id" == 1013
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
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1012,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/foo/../bare/01", "stringifiedInstance": "\"hi\"" }
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
jsonpath "$.id" == 1012
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.valid" == true
variable "text" jsonpath "$.valid" == true
jsonpath "$.result.structuredContent.steps" count == 2
variable "text" jsonpath "$.steps" count == 2

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
  "id": 1014,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/object#meta", "stringifiedInstance": "\"hi\"" }
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
jsonpath "$.id" == 1014
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
  "id": 1020,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "stringifiedInstance": "\"hello\"" }
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
jsonpath "$.id" == 1020
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/trace/rpc#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The object value was expected to only define properties \"schema\", and \"stringifiedInstance\""

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
  "id": 1021,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
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
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1021
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/trace/rpc#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The object value was expected to only define properties \"schema\", and \"stringifiedInstance\""

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
  "id": 1020,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
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
jsonpath "$.id" == 1020
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/trace/rpc#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The object value was expected to only define properties \"schema\", and \"stringifiedInstance\""

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
  "id": 1021,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/bare/01", "stringifiedInstance": "\"x\"", "extra": "foo" }
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
jsonpath "$.id" == 1021
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/trace/rpc#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The object value was expected to only define properties \"schema\", and \"stringifiedInstance\", but it also defines the property \"extra\""

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
  "id": 1024,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": 42, "stringifiedInstance": "\"x\"" }
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
jsonpath "$.id" == 1024
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/schema/type"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/trace/rpc#/properties/schema/type"
jsonpath "$.error.data.errors[0].instanceLocation" == "/schema"
jsonpath "$.error.data.errors[0].error" == "The value was expected to be of type string but it was of type integer"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/trace/rpc#/properties"
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
  "id": 1025,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
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
jsonpath "$.id" == 1025
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
  "id": "trace-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": {
      "schema": "{{base}}/test/bare/01",
      "stringifiedInstance": "\"ok\""
    }
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
jsonpath "$.id" == "trace-call-string-id"
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.valid" == true
variable "text" jsonpath "$.valid" == true
jsonpath "$.result.structuredContent.steps" count == 2
variable "text" jsonpath "$.steps" count == 2
jsonpath "$.result.structuredContent.steps[0].instancePositions" count == 4
jsonpath "$.result.structuredContent.steps[0].instancePositions[0]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[1]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[2]" == 1
jsonpath "$.result.structuredContent.steps[0].instancePositions[3]" == 4
variable "text" jsonpath "$.steps[0].instancePositions" count == 4
variable "text" jsonpath "$.steps[0].instancePositions[0]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[1]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[2]" == 1
variable "text" jsonpath "$.steps[0].instancePositions[3]" == 4
jsonpath "$.result.structuredContent.steps[1].instancePositions" count == 4
jsonpath "$.result.structuredContent.steps[1].instancePositions[0]" == 1
jsonpath "$.result.structuredContent.steps[1].instancePositions[1]" == 1
jsonpath "$.result.structuredContent.steps[1].instancePositions[2]" == 1
jsonpath "$.result.structuredContent.steps[1].instancePositions[3]" == 4
variable "text" jsonpath "$.steps[1].instancePositions" count == 4
variable "text" jsonpath "$.steps[1].instancePositions[0]" == 1
variable "text" jsonpath "$.steps[1].instancePositions[1]" == 1
variable "text" jsonpath "$.steps[1].instancePositions[2]" == 1
variable "text" jsonpath "$.steps[1].instancePositions[3]" == 4

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A malformed stringified instance is reported as a tool error rather than a
# JSON-RPC error.
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1090,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/bare/01", "stringifiedInstance": "{not valid json" }
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
jsonpath "$.id" == 1090
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "The instance is not valid JSON"
jsonpath "$.result.structuredContent" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An empty string is not valid JSON either.
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1091,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/bare/01", "stringifiedInstance": "" }
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
jsonpath "$.id" == 1091
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "The instance is not valid JSON"
jsonpath "$.result.structuredContent" not exists

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
  "id": 1092,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/bare/01", "stringifiedInstance": "\"hello\"" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
trace_baseline_text: jsonpath "$.result.content[0].text"

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1093,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/TEST/BARE/01", "stringifiedInstance": "\"hello\"" }
  }
}
```
HTTP 200
[Asserts]
jsonpath "$.id" == 1093
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{trace_baseline_text}}"

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1094,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/bare/01.JSON", "stringifiedInstance": "\"hello\"" }
  }
}
```
HTTP 200
[Asserts]
jsonpath "$.id" == 1094
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{trace_baseline_text}}"

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1095,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "HTTP://LOCALHOST:{{port}}/test/bare/01", "stringifiedInstance": "\"hello\"" }
  }
}
```
HTTP 200
[Asserts]
jsonpath "$.id" == 1095
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{trace_baseline_text}}"
