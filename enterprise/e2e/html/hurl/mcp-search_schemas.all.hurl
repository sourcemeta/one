# Single result

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1100,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "object" }
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
jsonpath "$.id" == 1100
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent" count == 1
variable "text" jsonpath "$" count == 1
jsonpath "$.result.structuredContent[0].path" == "/test/object"
jsonpath "$.result.structuredContent[0].identifier" == "http://localhost:8000/test/object"
variable "text" jsonpath "$[0].path" == "/test/object"
variable "text" jsonpath "$[0].identifier" == "http://localhost:8000/test/object"
jsonpath "$.result.structuredContent[0].title" == "My Object"
variable "text" jsonpath "$[0].title" == "My Object"
jsonpath "$.result.structuredContent[0].description" == "An Object Example"
variable "text" jsonpath "$[0].description" == "An Object Example"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Case insensitive query

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1101,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "OBJECT" }
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
jsonpath "$.id" == 1101
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" count == 1
variable "text" jsonpath "$" count == 1
jsonpath "$.result.structuredContent[0].path" == "/test/object"
jsonpath "$.result.structuredContent[0].identifier" == "http://localhost:8000/test/object"
variable "text" jsonpath "$[0].path" == "/test/object"
variable "text" jsonpath "$[0].identifier" == "http://localhost:8000/test/object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Default limit

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1102,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test" }
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
jsonpath "$.id" == 1102
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" count == 10
variable "text" jsonpath "$" count == 10

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Explicit limit

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1103,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "limit": 1 }
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
jsonpath "$.id" == 1103
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" count == 1
variable "text" jsonpath "$" count == 1

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# No matches

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1104,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "xyznonexistent" }
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
jsonpath "$.id" == 1104
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" count == 0
variable "text" jsonpath "$" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Explicit scope

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1105,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "object", "scope": [ "path" ] }
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
jsonpath "$.id" == 1105
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" count == 1
variable "text" jsonpath "$" count == 1
jsonpath "$.result.structuredContent[0].path" == "/test/object"
jsonpath "$.result.structuredContent[0].identifier" == "http://localhost:8000/test/object"
variable "text" jsonpath "$[0].path" == "/test/object"
variable "text" jsonpath "$[0].identifier" == "http://localhost:8000/test/object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Missing q argument

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1120,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
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
jsonpath "$.id" == 1120
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Limit below minimum

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1121,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "limit": 0 }
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
jsonpath "$.id" == 1121
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Limit above maximum

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1122,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "limit": 101 }
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
jsonpath "$.id" == 1122
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Limit is a float

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1123,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "limit": 1.5 }
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
jsonpath "$.id" == 1123
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Limit is a string

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1124,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "limit": "5" }
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
jsonpath "$.id" == 1124
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Empty scope

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1125,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "scope": [] }
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
jsonpath "$.id" == 1125
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Duplicate scope entries

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1126,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "scope": [ "path", "path" ] }
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
jsonpath "$.id" == 1126
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Invalid scope value

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1127,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "scope": [ "bogus" ] }
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
jsonpath "$.id" == 1127
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Extra property

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1128,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "extra": "foo" }
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
jsonpath "$.id" == 1128
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Arguments is null

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1129,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
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
jsonpath "$.id" == 1129
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# String id

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "search-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "object" }
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
jsonpath "$.id" == "search-call-string-id"
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" count == 1
variable "text" jsonpath "$" count == 1
jsonpath "$.result.structuredContent[0].path" == "/test/object"
jsonpath "$.result.structuredContent[0].identifier" == "http://localhost:8000/test/object"
variable "text" jsonpath "$[0].path" == "/test/object"
variable "text" jsonpath "$[0].identifier" == "http://localhost:8000/test/object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
