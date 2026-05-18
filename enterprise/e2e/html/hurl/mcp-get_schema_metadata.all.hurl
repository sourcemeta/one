# Bare schema

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 800,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 800
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.bytes" == 130
variable "text" jsonpath "$.bytes" == 130
jsonpath "$.result.structuredContent.bytesBundled" == 130
variable "text" jsonpath "$.bytesBundled" == 130
jsonpath "$.result.structuredContent.identifier" == "{{base}}/test/bare/01"
variable "text" jsonpath "$.identifier" == "{{base}}/test/bare/01"
jsonpath "$.result.structuredContent.path" == "/test/bare/01"
variable "text" jsonpath "$.path" == "/test/bare/01"
jsonpath "$.result.structuredContent.dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.examples" count == 0
variable "text" jsonpath "$.examples" count == 0
jsonpath "$.result.structuredContent.health" == 0
variable "text" jsonpath "$.health" == 0
jsonpath "$.result.structuredContent.dependencies" == 0
variable "text" jsonpath "$.dependencies" == 0
jsonpath "$.result.structuredContent.alert" == null
variable "text" jsonpath "$.alert" == null
jsonpath "$.result.structuredContent.breadcrumb" count == 3
variable "text" jsonpath "$.breadcrumb" count == 3
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "test"
variable "text" jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/test/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.result.structuredContent.breadcrumb[1].name" == "bare"
variable "text" jsonpath "$.breadcrumb[1].name" == "bare"
jsonpath "$.result.structuredContent.breadcrumb[1].path" == "/test/bare/"
variable "text" jsonpath "$.breadcrumb[1].path" == "/test/bare/"
jsonpath "$.result.structuredContent.breadcrumb[2].name" == "01"
variable "text" jsonpath "$.breadcrumb[2].name" == "01"
jsonpath "$.result.structuredContent.breadcrumb[2].path" == "/test/bare/01"
variable "text" jsonpath "$.breadcrumb[2].path" == "/test/bare/01"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Object schema

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 801,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 801
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.bytes" == 322
variable "text" jsonpath "$.bytes" == 322
jsonpath "$.result.structuredContent.bytesBundled" == 322
variable "text" jsonpath "$.bytesBundled" == 322
jsonpath "$.result.structuredContent.identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.path" == "/test/object"
variable "text" jsonpath "$.path" == "/test/object"
jsonpath "$.result.structuredContent.dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.title" == "My Object"
variable "text" jsonpath "$.title" == "My Object"
jsonpath "$.result.structuredContent.description" == "An Object Example"
variable "text" jsonpath "$.description" == "An Object Example"
jsonpath "$.result.structuredContent.examples" count == 1
variable "text" jsonpath "$.examples" count == 1
jsonpath "$.result.structuredContent.health" == 67
variable "text" jsonpath "$.health" == 67
jsonpath "$.result.structuredContent.dependencies" == 0
variable "text" jsonpath "$.dependencies" == 0
jsonpath "$.result.structuredContent.alert" == null
variable "text" jsonpath "$.alert" == null
jsonpath "$.result.structuredContent.breadcrumb" count == 2
variable "text" jsonpath "$.breadcrumb" count == 2
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "test"
variable "text" jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.result.structuredContent.breadcrumb[1].name" == "object"
variable "text" jsonpath "$.breadcrumb[1].name" == "object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Schema with non-zero dependencies count

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 802,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
    "arguments": { "schema": "{{base}}/self/v1/schemas/mcp/response" }
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
jsonpath "$.id" == 802
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.identifier" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.title" == "Sourcemeta One MCP Response"
jsonpath "$.result.structuredContent.dependencies" == 8
variable "text" jsonpath "$.title" == "Sourcemeta One MCP Response"
variable "text" jsonpath "$.dependencies" == 8

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Relative path

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 803,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 803
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.title" == "My Object"
variable "text" jsonpath "$.title" == "My Object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Schema URI not in catalog

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 810,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 810
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

# URI points at a directory

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 811,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 811
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

# URI with different host

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 812,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 812
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].text" == "Schema not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# URI with ".." canonicalising into catalog

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 813,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 813
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.path" == "/test/object"
variable "text" jsonpath "$.path" == "/test/object"
jsonpath "$.result.structuredContent.title" == "My Object"
variable "text" jsonpath "$.title" == "My Object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# URI with ".." escaping registry root

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 814,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 814
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].text" == "Schema not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Missing schema argument

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 820,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 820
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Empty schema string

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 821,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 821
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].text" == "Schema not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Schema is an integer

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 822,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 822
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
  "id": 823,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 823
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Missing arguments

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 824,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata"
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
jsonpath "$.id" == 824
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
  "id": 825,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == 825
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
  "id": "metadata-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
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
jsonpath "$.id" == "metadata-call-string-id"
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.title" == "My Object"
variable "text" jsonpath "$.title" == "My Object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
