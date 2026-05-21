POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1000,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/bare/01", "instance": "hello" }
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
jsonpath "$.result.structuredContent.steps[0].keywordLocation" == "{{base}}/test/bare/01#/type"
variable "text" jsonpath "$.steps[0].keywordLocation" == "{{base}}/test/bare/01#/type"
jsonpath "$.result.structuredContent.steps[0].vocabulary" == "https://json-schema.org/draft/2020-12/vocab/validation"
variable "text" jsonpath "$.steps[0].vocabulary" == "https://json-schema.org/draft/2020-12/vocab/validation"
jsonpath "$.result.structuredContent.steps[1].type" == "pass"
variable "text" jsonpath "$.steps[1].type" == "pass"
jsonpath "$.result.structuredContent.steps[1].name" == "AssertionTypeStrict"
variable "text" jsonpath "$.steps[1].name" == "AssertionTypeStrict"

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
  "id": 1001,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/bare/01", "instance": 42 }
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
jsonpath "$.result.structuredContent.steps[1].type" == "fail"
variable "text" jsonpath "$.steps[1].type" == "fail"
jsonpath "$.result.structuredContent.steps[1].name" == "AssertionTypeStrict"
variable "text" jsonpath "$.steps[1].name" == "AssertionTypeStrict"

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
  "id": 1002,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/object", "instance": {} }
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
  "id": 1003,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "test/bare/01", "instance": "hello" }
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
jsonpath "$.id" == 1003
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.valid" == true
jsonpath "$.result.structuredContent.steps" count == 2
variable "text" jsonpath "$.valid" == true
variable "text" jsonpath "$.steps" count == 2

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
  "id": 1010,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/does-not-exist", "instance": "hi" }
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
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1011,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test", "instance": "x" }
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
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1013,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "https://example.com/test/bare/01", "instance": "hi" }
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
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1012,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/foo/../bare/01", "instance": "hi" }
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

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1020,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "instance": "hello" }
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
  "id": 1022,
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
jsonpath "$.id" == 1022
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
  "id": 1023,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/bare/01", "instance": "x", "extra": "foo" }
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
jsonpath "$.id" == 1023
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
  "id": 1024,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": 42, "instance": "x" }
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
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "trace-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/test/bare/01", "instance": "ok" }
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

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
