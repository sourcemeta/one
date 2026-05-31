POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
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
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.results[0].title" == "My Object"
variable "text" jsonpath "$.results[0].title" == "My Object"
jsonpath "$.result.structuredContent.results[0].description" == "An Object Example"
variable "text" jsonpath "$.results[0].description" == "An Object Example"
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 67
variable "text" jsonpath "$.results[0].priority" == 50
variable "text" jsonpath "$.results[0].health" == 67

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
  "id": 1106,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "object", "limit": 1 }
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
resource_uri: jsonpath "$.result.content[1].uri"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1106
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.results[0].title" == "My Object"
jsonpath "$.result.structuredContent.results[0].description" == "An Object Example"
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 67
variable "text" jsonpath "$.results" count == 1
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.results[0].title" == "My Object"
variable "text" jsonpath "$.results[0].description" == "An Object Example"
variable "text" jsonpath "$.results[0].priority" == 50
variable "text" jsonpath "$.results[0].health" == 67

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/test/object
HTTP 200
[Captures]
schema_text: body

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1107,
  "method": "resources/read",
  "params": {
    "uri": "{{resource_uri}}"
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
jsonpath "$.id" == 1107
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{resource_uri}}"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

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
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 67
variable "text" jsonpath "$.results[0].priority" == 50
variable "text" jsonpath "$.results[0].health" == 67

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
jsonpath "$.result.content" count == 11
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/test/full/01"
jsonpath "$.result.content[2].name" == "Full 01"
jsonpath "$.result.content[2].description" == "A schema with both a title and a description (01)"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/test/full/02"
jsonpath "$.result.content[3].name" == "Full 02"
jsonpath "$.result.content[3].description" == "A schema with both a title and a description (02)"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/test/full/03"
jsonpath "$.result.content[4].name" == "Full 03"
jsonpath "$.result.content[4].description" == "A schema with both a title and a description (03)"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/test/full/04"
jsonpath "$.result.content[5].name" == "Full 04"
jsonpath "$.result.content[5].description" == "A schema with both a title and a description (04)"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.content[6].type" == "resource_link"
jsonpath "$.result.content[6].uri" == "{{base}}/test/full/05"
jsonpath "$.result.content[6].name" == "Full 05"
jsonpath "$.result.content[6].description" == "A schema with both a title and a description (05)"
jsonpath "$.result.content[6].mimeType" == "application/schema+json"
jsonpath "$.result.content[7].type" == "resource_link"
jsonpath "$.result.content[7].uri" == "{{base}}/test/description-only/01"
jsonpath "$.result.content[7].name" == "/test/description-only/01"
jsonpath "$.result.content[7].description" == "A schema with only a description (01)"
jsonpath "$.result.content[7].mimeType" == "application/schema+json"
jsonpath "$.result.content[8].type" == "resource_link"
jsonpath "$.result.content[8].uri" == "{{base}}/test/description-only/02"
jsonpath "$.result.content[8].name" == "/test/description-only/02"
jsonpath "$.result.content[8].description" == "A schema with only a description (02)"
jsonpath "$.result.content[8].mimeType" == "application/schema+json"
jsonpath "$.result.content[9].type" == "resource_link"
jsonpath "$.result.content[9].uri" == "{{base}}/test/description-only/03"
jsonpath "$.result.content[9].name" == "/test/description-only/03"
jsonpath "$.result.content[9].description" == "A schema with only a description (03)"
jsonpath "$.result.content[9].mimeType" == "application/schema+json"
jsonpath "$.result.content[10].type" == "resource_link"
jsonpath "$.result.content[10].uri" == "{{base}}/test/description-only/04"
jsonpath "$.result.content[10].name" == "/test/description-only/04"
jsonpath "$.result.content[10].description" == "A schema with only a description (04)"
jsonpath "$.result.content[10].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 10
variable "text" jsonpath "$.results" count == 10

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
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1

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
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.results" exists
jsonpath "$.result.structuredContent.results" count == 0
variable "text" jsonpath "$.results" exists
variable "text" jsonpath "$.results" count == 0

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
  "id": 1110,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "description-only", "limit": 2 }
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
jsonpath "$.id" == 1110
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 3
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/description-only/01"
jsonpath "$.result.content[1].name" == "/test/description-only/01"
jsonpath "$.result.content[1].description" == "A schema with only a description (01)"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/test/description-only/02"
jsonpath "$.result.content[2].name" == "/test/description-only/02"
jsonpath "$.result.content[2].description" == "A schema with only a description (02)"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 2
variable "text" jsonpath "$.results" count == 2
jsonpath "$.result.structuredContent.results[0].title" == ""
jsonpath "$.result.structuredContent.results[0].description" == "A schema with only a description (01)"
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 0

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
  "id": 1111,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "title-only", "limit": 2 }
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
jsonpath "$.id" == 1111
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 3
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/title-only/01"
jsonpath "$.result.content[1].name" == "Title Only 01"
jsonpath "$.result.content[1].description" not exists
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/test/title-only/02"
jsonpath "$.result.content[2].name" == "Title Only 02"
jsonpath "$.result.content[2].description" not exists
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 2
variable "text" jsonpath "$.results" count == 2
jsonpath "$.result.structuredContent.results[0].title" == "Title Only 01"
jsonpath "$.result.structuredContent.results[0].description" == ""
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 0

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
  "id": 1112,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "bare", "limit": 2 }
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
jsonpath "$.id" == 1112
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 3
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/bare/01"
jsonpath "$.result.content[1].name" == "/test/bare/01"
jsonpath "$.result.content[1].description" not exists
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/test/bare/02"
jsonpath "$.result.content[2].name" == "/test/bare/02"
jsonpath "$.result.content[2].description" not exists
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 2
variable "text" jsonpath "$.results" count == 2
jsonpath "$.result.structuredContent.results[0].title" == ""
jsonpath "$.result.structuredContent.results[0].description" == ""
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 0

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
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 67
variable "text" jsonpath "$.results[0].priority" == 50
variable "text" jsonpath "$.results[0].health" == 67

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
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The value was expected to be an object that defines the property \"q\""

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
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/limit/minimum"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/limit/minimum"
jsonpath "$.error.data.errors[0].instanceLocation" == "/limit"
jsonpath "$.error.data.errors[0].error" == "The integer value 0 was expected to be greater than or equal to the integer 1"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

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
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/limit/maximum"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/limit/maximum"
jsonpath "$.error.data.errors[0].instanceLocation" == "/limit"
jsonpath "$.error.data.errors[0].error" == "The integer value 101 was expected to be less than or equal to the integer 100"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

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
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/limit/type"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/limit/type"
jsonpath "$.error.data.errors[0].instanceLocation" == "/limit"
jsonpath "$.error.data.errors[0].error" == "The value was expected to be of type integer, or string but it was of type number"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A string-encoded limit (e.g. "5") is accepted as a workaround for MCP
# clients that stringify non-string tool arguments, equivalent to limit: 5

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
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
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1124
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 6
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/test/full/01"
jsonpath "$.result.content[2].name" == "Full 01"
jsonpath "$.result.content[2].description" == "A schema with both a title and a description (01)"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/test/full/02"
jsonpath "$.result.content[3].name" == "Full 02"
jsonpath "$.result.content[3].description" == "A schema with both a title and a description (02)"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/test/full/03"
jsonpath "$.result.content[4].name" == "Full 03"
jsonpath "$.result.content[4].description" == "A schema with both a title and a description (03)"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/test/full/04"
jsonpath "$.result.content[5].name" == "Full 04"
jsonpath "$.result.content[5].description" == "A schema with both a title and a description (04)"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 5
variable "text" jsonpath "$.results" count == 5

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
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/scope/minItems"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/scope/minItems"
jsonpath "$.error.data.errors[0].instanceLocation" == "/scope"
jsonpath "$.error.data.errors[0].error" == "The array value was expected to contain at least 1 item but it contained 0 items"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

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
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/scope/uniqueItems"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/scope/uniqueItems"
jsonpath "$.error.data.errors[0].instanceLocation" == "/scope"
jsonpath "$.error.data.errors[0].error" == "The array value contained the following duplicate item: \"path\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

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
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 3
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/scope/items/enum"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/scope/items/enum"
jsonpath "$.error.data.errors[0].instanceLocation" == "/scope/0"
jsonpath "$.error.data.errors[0].error" == "The string value \"bogus\" was expected to equal one of the following values: \"description\", \"path\", and \"title\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties/scope/items"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/scope/items"
jsonpath "$.error.data.errors[1].instanceLocation" == "/scope"
jsonpath "$.error.data.errors[1].error" == "Every item in the array value was expected to validate against the given subschema"
jsonpath "$.error.data.errors[2].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[2].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[2].instanceLocation" == ""
jsonpath "$.error.data.errors[2].error" == "The object value was expected to validate against the 3 defined properties subschemas"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A JSON-encoded string scope (e.g. "[\"path\"]") is accepted as a
# workaround for MCP clients that stringify non-string tool arguments,
# equivalent to scope: ["path"]

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1130,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "object", "scope": "[\"path\"]" }
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
jsonpath "$.id" == 1130
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 67
variable "text" jsonpath "$.results[0].priority" == 50
variable "text" jsonpath "$.results[0].health" == 67

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A string scope that does not parse as JSON is rejected by the handler
# with a tool error

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1131,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "scope": "bogus" }
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
jsonpath "$.id" == 1131
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "The scope is not valid JSON"
jsonpath "$.result.structuredContent" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A string scope that parses as JSON but is not an array is rejected by
# the handler with a tool error

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1133,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "scope": "\"path\"" }
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
jsonpath "$.id" == 1133
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "The scope must be a JSON-encoded array of strings"
jsonpath "$.result.structuredContent" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A string scope that parses as an empty JSON array is rejected by the
# handler with a tool error, mirroring the array form's minItems: 1

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1135,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "scope": "[]" }
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
jsonpath "$.id" == 1135
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "The scope must contain at least one item"
jsonpath "$.result.structuredContent" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A string scope that parses as a JSON array of unknown tokens is
# rejected by the handler with a tool error

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1134,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "scope": "[\"bogus\"]" }
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
jsonpath "$.id" == 1134
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "The scope items must be \"path\", \"title\", or \"description\""
jsonpath "$.result.structuredContent" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A string limit outside the allowed range is rejected at the schema
# level by the regex pattern

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1132,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "test", "limit": "101" }
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
jsonpath "$.id" == 1132
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/limit/pattern"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/limit/pattern"
jsonpath "$.error.data.errors[0].instanceLocation" == "/limit"
jsonpath "$.error.data.errors[0].error" == "The string value \"101\" was expected to match the regular expression \"^([1-9]|[1-9][0-9]|100)$\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

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
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/additionalProperties"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/additionalProperties"
jsonpath "$.error.data.errors[0].instanceLocation" == "/extra"
jsonpath "$.error.data.errors[0].error" == "The object value was not expected to define the property \"extra\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/additionalProperties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/additionalProperties"
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

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
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
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 67
variable "text" jsonpath "$.results[0].priority" == 50
variable "text" jsonpath "$.results[0].health" == 67

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An empty query is rejected.
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1140,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "" }
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
jsonpath "$.id" == 1140
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/q/minLength"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/q/minLength"
jsonpath "$.error.data.errors[0].instanceLocation" == "/q"
jsonpath "$.error.data.errors[0].error" == "The string value \"\" was expected to consist of at least 1 character but it consisted of 0 characters"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A whitespace-only query is rejected.
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1141,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "   " }
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
jsonpath "$.id" == 1141
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/q/pattern"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/q/pattern"
jsonpath "$.error.data.errors[0].instanceLocation" == "/q"
jsonpath "$.error.data.errors[0].error" == "The string value \"   \" was expected to match the regular expression \"\\S\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Tabs, newlines, and other whitespace characters are also rejected.
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1142,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "\t\n\r " }
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
jsonpath "$.id" == 1142
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/q/pattern"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties/q/pattern"
jsonpath "$.error.data.errors[0].instanceLocation" == "/q"
jsonpath "$.error.data.errors[0].error" == "The string value \"\t\n\r \" was expected to match the regular expression \"\\S\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
