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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "http://localhost:8000/test/object"
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "http://localhost:8000/test/object"
jsonpath "$.result.structuredContent.results[0].title" == "My Object"
variable "text" jsonpath "$.results[0].title" == "My Object"
jsonpath "$.result.structuredContent.results[0].description" == "An Object Example"
variable "text" jsonpath "$.results[0].description" == "An Object Example"

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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "http://localhost:8000/test/object"
jsonpath "$.result.structuredContent.results[0].title" == "My Object"
jsonpath "$.result.structuredContent.results[0].description" == "An Object Example"
variable "text" jsonpath "$.results" count == 1
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "http://localhost:8000/test/object"
variable "text" jsonpath "$.results[0].title" == "My Object"
variable "text" jsonpath "$.results[0].description" == "An Object Example"

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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "http://localhost:8000/test/object"
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "http://localhost:8000/test/object"

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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "http://localhost:8000/test/full/01"
jsonpath "$.result.content[2].name" == "Full 01"
jsonpath "$.result.content[2].description" == "A schema with both a title and a description (01)"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "http://localhost:8000/test/full/02"
jsonpath "$.result.content[3].name" == "Full 02"
jsonpath "$.result.content[3].description" == "A schema with both a title and a description (02)"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "http://localhost:8000/test/full/03"
jsonpath "$.result.content[4].name" == "Full 03"
jsonpath "$.result.content[4].description" == "A schema with both a title and a description (03)"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "http://localhost:8000/test/full/04"
jsonpath "$.result.content[5].name" == "Full 04"
jsonpath "$.result.content[5].description" == "A schema with both a title and a description (04)"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.content[6].type" == "resource_link"
jsonpath "$.result.content[6].uri" == "http://localhost:8000/test/full/05"
jsonpath "$.result.content[6].name" == "Full 05"
jsonpath "$.result.content[6].description" == "A schema with both a title and a description (05)"
jsonpath "$.result.content[6].mimeType" == "application/schema+json"
jsonpath "$.result.content[7].type" == "resource_link"
jsonpath "$.result.content[7].uri" == "http://localhost:8000/test/description-only/01"
jsonpath "$.result.content[7].name" == "/test/description-only/01"
jsonpath "$.result.content[7].description" == "A schema with only a description (01)"
jsonpath "$.result.content[7].mimeType" == "application/schema+json"
jsonpath "$.result.content[8].type" == "resource_link"
jsonpath "$.result.content[8].uri" == "http://localhost:8000/test/description-only/02"
jsonpath "$.result.content[8].name" == "/test/description-only/02"
jsonpath "$.result.content[8].description" == "A schema with only a description (02)"
jsonpath "$.result.content[8].mimeType" == "application/schema+json"
jsonpath "$.result.content[9].type" == "resource_link"
jsonpath "$.result.content[9].uri" == "http://localhost:8000/test/description-only/03"
jsonpath "$.result.content[9].name" == "/test/description-only/03"
jsonpath "$.result.content[9].description" == "A schema with only a description (03)"
jsonpath "$.result.content[9].mimeType" == "application/schema+json"
jsonpath "$.result.content[10].type" == "resource_link"
jsonpath "$.result.content[10].uri" == "http://localhost:8000/test/description-only/04"
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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/object"
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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/description-only/01"
jsonpath "$.result.content[1].name" == "/test/description-only/01"
jsonpath "$.result.content[1].description" == "A schema with only a description (01)"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "http://localhost:8000/test/description-only/02"
jsonpath "$.result.content[2].name" == "/test/description-only/02"
jsonpath "$.result.content[2].description" == "A schema with only a description (02)"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 2
variable "text" jsonpath "$.results" count == 2
jsonpath "$.result.structuredContent.results[0].title" == ""
jsonpath "$.result.structuredContent.results[0].description" == "A schema with only a description (01)"

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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/title-only/01"
jsonpath "$.result.content[1].name" == "Title Only 01"
jsonpath "$.result.content[1].description" not exists
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "http://localhost:8000/test/title-only/02"
jsonpath "$.result.content[2].name" == "Title Only 02"
jsonpath "$.result.content[2].description" not exists
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 2
variable "text" jsonpath "$.results" count == 2
jsonpath "$.result.structuredContent.results[0].title" == "Title Only 01"
jsonpath "$.result.structuredContent.results[0].description" == ""

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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/bare/01"
jsonpath "$.result.content[1].name" == "/test/bare/01"
jsonpath "$.result.content[1].description" not exists
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "http://localhost:8000/test/bare/02"
jsonpath "$.result.content[2].name" == "/test/bare/02"
jsonpath "$.result.content[2].description" not exists
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 2
variable "text" jsonpath "$.results" count == 2
jsonpath "$.result.structuredContent.results[0].title" == ""
jsonpath "$.result.structuredContent.results[0].description" == ""

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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "http://localhost:8000/test/object"
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "http://localhost:8000/test/object"

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
jsonpath "$.error.message" == "Invalid params"

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
jsonpath "$.error.message" == "Invalid params"

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
jsonpath "$.error.message" == "Invalid params"

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
jsonpath "$.error.message" == "Invalid params"

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
jsonpath "$.error.message" == "Invalid params"

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
jsonpath "$.error.message" == "Invalid params"

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
jsonpath "$.error.message" == "Invalid params"

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
jsonpath "$.error.message" == "Invalid params"

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
jsonpath "$.result.content[1].uri" == "http://localhost:8000/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/test/object"
jsonpath "$.result.structuredContent.results[0].identifier" == "http://localhost:8000/test/object"
variable "text" jsonpath "$.results[0].path" == "/test/object"
variable "text" jsonpath "$.results[0].identifier" == "http://localhost:8000/test/object"

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
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
