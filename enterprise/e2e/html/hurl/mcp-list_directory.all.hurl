# Missing arguments object

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 100,
  "method": "tools/call",
  "params": {
    "name": "list_directory"
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
jsonpath "$.id" == 100
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Root listing

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 101,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
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
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 101
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.path" == "/"
variable "text" jsonpath "$.path" == "/"
jsonpath "$.result.structuredContent.url" == "{{base}}"
variable "text" jsonpath "$.url" == "{{base}}"
jsonpath "$.result.structuredContent.breadcrumb" count == 0
variable "text" jsonpath "$.breadcrumb" count == 0
jsonpath "$.result.structuredContent.schemas" == 113
variable "text" jsonpath "$.schemas" == 113
jsonpath "$.result.structuredContent.entries" count == 2
variable "text" jsonpath "$.entries" count == 2
jsonpath "$.result.structuredContent.entries[0].name" == "self"
variable "text" jsonpath "$.entries[0].name" == "self"
jsonpath "$.result.structuredContent.entries[0].title" == "Self"
variable "text" jsonpath "$.entries[0].title" == "Self"
jsonpath "$.result.structuredContent.entries[0].description" == "The schemas that define the current version of this instance"
variable "text" jsonpath "$.entries[0].description" == "The schemas that define the current version of this instance"
jsonpath "$.result.structuredContent.entries[0].type" == "directory"
variable "text" jsonpath "$.entries[0].type" == "directory"
jsonpath "$.result.structuredContent.entries[0].health" == 100
variable "text" jsonpath "$.entries[0].health" == 100
jsonpath "$.result.structuredContent.entries[0].schemas" == 45
variable "text" jsonpath "$.entries[0].schemas" == 45
jsonpath "$.result.structuredContent.entries[0].path" == "/self/"
variable "text" jsonpath "$.entries[0].path" == "/self/"
jsonpath "$.result.structuredContent.entries[1].name" == "test"
variable "text" jsonpath "$.entries[1].name" == "test"
jsonpath "$.result.structuredContent.entries[1].type" == "directory"
variable "text" jsonpath "$.entries[1].type" == "directory"
jsonpath "$.result.structuredContent.entries[1].health" == 15
variable "text" jsonpath "$.entries[1].health" == 15
jsonpath "$.result.structuredContent.entries[1].schemas" == 68
variable "text" jsonpath "$.entries[1].schemas" == 68
jsonpath "$.result.structuredContent.entries[1].path" == "/test/"
variable "text" jsonpath "$.entries[1].path" == "/test/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Explicit empty string path

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 102,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "" }
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
jsonpath "$.id" == 102
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/"
variable "text" jsonpath "$.path" == "/"
jsonpath "$.result.structuredContent.url" == "{{base}}"
variable "text" jsonpath "$.url" == "{{base}}"
jsonpath "$.result.structuredContent.breadcrumb" count == 0
variable "text" jsonpath "$.breadcrumb" count == 0
jsonpath "$.result.structuredContent.schemas" == 113
variable "text" jsonpath "$.schemas" == 113
jsonpath "$.result.structuredContent.entries" count == 2
variable "text" jsonpath "$.entries" count == 2

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Subdirectory listing

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 103,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test" }
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
jsonpath "$.id" == 103
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/test"
variable "text" jsonpath "$.path" == "/test"
jsonpath "$.result.structuredContent.url" == "{{base}}/test"
variable "text" jsonpath "$.url" == "{{base}}/test"
jsonpath "$.result.structuredContent.breadcrumb" count == 1
variable "text" jsonpath "$.breadcrumb" count == 1
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "test"
variable "text" jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/test/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.result.structuredContent.schemas" == 68
variable "text" jsonpath "$.schemas" == 68
jsonpath "$.result.structuredContent.entries" count == 6
variable "text" jsonpath "$.entries" count == 6

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Deep subdirectory listing

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 104,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test/bare" }
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
jsonpath "$.id" == 104
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/test/bare"
variable "text" jsonpath "$.path" == "/test/bare"
jsonpath "$.result.structuredContent.url" == "{{base}}/test/bare"
variable "text" jsonpath "$.url" == "{{base}}/test/bare"
jsonpath "$.result.structuredContent.breadcrumb" count == 2
variable "text" jsonpath "$.breadcrumb" count == 2
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "test"
variable "text" jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/test/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.result.structuredContent.breadcrumb[1].name" == "bare"
variable "text" jsonpath "$.breadcrumb[1].name" == "bare"
jsonpath "$.result.structuredContent.breadcrumb[1].path" == "/test/bare/"
variable "text" jsonpath "$.breadcrumb[1].path" == "/test/bare/"
jsonpath "$.result.structuredContent.schemas" == 50
variable "text" jsonpath "$.schemas" == 50
jsonpath "$.result.structuredContent.entries" count == 50
variable "text" jsonpath "$.entries" count == 50

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Subdirectory listing with title and description

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 105,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "self" }
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
jsonpath "$.id" == 105
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/self"
variable "text" jsonpath "$.path" == "/self"
jsonpath "$.result.structuredContent.url" == "{{base}}/self"
variable "text" jsonpath "$.url" == "{{base}}/self"
jsonpath "$.result.structuredContent.title" == "Self"
variable "text" jsonpath "$.title" == "Self"
jsonpath "$.result.structuredContent.description" == "The schemas that define the current version of this instance"
variable "text" jsonpath "$.description" == "The schemas that define the current version of this instance"
jsonpath "$.result.structuredContent.breadcrumb" count == 1
variable "text" jsonpath "$.breadcrumb" count == 1
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "self"
variable "text" jsonpath "$.breadcrumb[0].name" == "self"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/self/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/self/"
jsonpath "$.result.structuredContent.schemas" == 45
variable "text" jsonpath "$.schemas" == 45

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Non-existent path

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 110,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "doesnotexist" }
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
jsonpath "$.id" == 110
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Directory not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Path points at a schema, not a directory

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 111,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test/object" }
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
jsonpath "$.id" == 111
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Directory not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Absolute path

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 112,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "/test" }
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
jsonpath "$.id" == 112
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Bare ".." segment

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 113,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": ".." }
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
jsonpath "$.id" == 113
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Mid-path ".." segment

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 114,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test/.." }
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
jsonpath "$.id" == 114
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Bare "." segment

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 115,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "." }
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
jsonpath "$.id" == 115
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Mid-path "." segment

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 116,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test/." }
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
jsonpath "$.id" == 116
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Percent-encoded ".." treated as literal segment name

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 117,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "..%2F..%2Fetc" }
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
jsonpath "$.id" == 117
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Directory not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Backslash in path

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 118,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test\\bare" }
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
jsonpath "$.id" == 118
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Directory not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Path is an integer

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 119,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": 42 }
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
jsonpath "$.id" == 119
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Path is an array

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 120,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": [] }
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
jsonpath "$.id" == 120
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
  "id": 121,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test", "extra": "foo" }
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
jsonpath "$.id" == 121
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
  "id": 122,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
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
jsonpath "$.id" == 122
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Arguments is a string

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 123,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": "test"
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
jsonpath "$.id" == 123
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
  "id": "list-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "list_directory",
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
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "list-call-string-id"
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/"
variable "text" jsonpath "$.path" == "/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
