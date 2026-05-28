POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 200,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 200
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
  "id": 201,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 201
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
  "id": 202,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 202
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 11
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/self/v1/schemas/mcp/error"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.content[6].type" == "resource_link"
jsonpath "$.result.content[6].uri" == "{{base}}/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.content[6].mimeType" == "application/schema+json"
jsonpath "$.result.content[7].type" == "resource_link"
jsonpath "$.result.content[7].uri" == "{{base}}/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.content[7].mimeType" == "application/schema+json"
jsonpath "$.result.content[8].type" == "resource_link"
jsonpath "$.result.content[8].uri" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.content[8].mimeType" == "application/schema+json"
jsonpath "$.result.content[9].type" == "resource_link"
jsonpath "$.result.content[9].uri" == "{{base}}/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.content[9].mimeType" == "application/schema+json"
jsonpath "$.result.content[10].type" == "resource_link"
jsonpath "$.result.content[10].uri" == "{{base}}/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.content[10].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 9
jsonpath "$.result.structuredContent.results[0].from" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[0].to" == "{{base}}/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.structuredContent.results[0].at" == "/anyOf/0/$ref"
jsonpath "$.result.structuredContent.results[1].from" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[1].to" == "{{base}}/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.structuredContent.results[1].at" == "/anyOf/1/$ref"
jsonpath "$.result.structuredContent.results[2].from" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[2].to" == "{{base}}/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent.results[2].at" == "/anyOf/2/$ref"
jsonpath "$.result.structuredContent.results[3].from" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[3].to" == "{{base}}/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.structuredContent.results[3].at" == "/anyOf/3/$ref"
jsonpath "$.result.structuredContent.results[4].from" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[4].to" == "{{base}}/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.structuredContent.results[4].at" == "/anyOf/4/$ref"
jsonpath "$.result.structuredContent.results[5].from" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[5].to" == "{{base}}/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.structuredContent.results[5].at" == "/anyOf/5/$ref"
jsonpath "$.result.structuredContent.results[6].from" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[6].to" == "{{base}}/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.structuredContent.results[6].at" == "/anyOf/6/$ref"
jsonpath "$.result.structuredContent.results[7].from" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[7].to" == "{{base}}/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.structuredContent.results[7].at" == "/anyOf/7/$ref"
jsonpath "$.result.structuredContent.results[8].from" == "{{base}}/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent.results[8].to" == "{{base}}/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.structuredContent.results[8].at" == "/$defs/reserved/anyOf/3/properties/data/anyOf/0/$ref"
variable "text" jsonpath "$.results" count == 9
variable "text" jsonpath "$.results[0].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[0].to" == "{{base}}/self/v1/schemas/mcp/initialize/response"
variable "text" jsonpath "$.results[0].at" == "/anyOf/0/$ref"
variable "text" jsonpath "$.results[1].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[1].to" == "{{base}}/self/v1/schemas/mcp/ping/response"
variable "text" jsonpath "$.results[1].at" == "/anyOf/1/$ref"
variable "text" jsonpath "$.results[2].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[2].to" == "{{base}}/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[2].at" == "/anyOf/2/$ref"
variable "text" jsonpath "$.results[3].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[3].to" == "{{base}}/self/v1/schemas/mcp/resources/list/response"
variable "text" jsonpath "$.results[3].at" == "/anyOf/3/$ref"
variable "text" jsonpath "$.results[4].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[4].to" == "{{base}}/self/v1/schemas/mcp/resources/templates/list/response"
variable "text" jsonpath "$.results[4].at" == "/anyOf/4/$ref"
variable "text" jsonpath "$.results[5].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[5].to" == "{{base}}/self/v1/schemas/mcp/resources/read/response"
variable "text" jsonpath "$.results[5].at" == "/anyOf/5/$ref"
variable "text" jsonpath "$.results[6].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[6].to" == "{{base}}/self/v1/schemas/mcp/tools/list/response"
variable "text" jsonpath "$.results[6].at" == "/anyOf/6/$ref"
variable "text" jsonpath "$.results[7].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[7].to" == "{{base}}/self/v1/schemas/mcp/tools/call/response"
variable "text" jsonpath "$.results[7].at" == "/anyOf/7/$ref"
variable "text" jsonpath "$.results[8].from" == "{{base}}/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[8].to" == "{{base}}/self/v1/schemas/api/schemas/evaluate/response"
variable "text" jsonpath "$.results[8].at" == "/$defs/reserved/anyOf/3/properties/data/anyOf/0/$ref"

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
  "id": 203,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "self/v1/schemas/mcp/response" }
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
jsonpath "$.id" == 203
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
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 204,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
dependency_uri: jsonpath "$.result.content[1].uri"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 204
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 11
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/self/v1/schemas/mcp/error"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.content[6].type" == "resource_link"
jsonpath "$.result.content[6].uri" == "{{base}}/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.content[6].mimeType" == "application/schema+json"
jsonpath "$.result.content[7].type" == "resource_link"
jsonpath "$.result.content[7].uri" == "{{base}}/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.content[7].mimeType" == "application/schema+json"
jsonpath "$.result.content[8].type" == "resource_link"
jsonpath "$.result.content[8].uri" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.content[8].mimeType" == "application/schema+json"
jsonpath "$.result.content[9].type" == "resource_link"
jsonpath "$.result.content[9].uri" == "{{base}}/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.content[9].mimeType" == "application/schema+json"
jsonpath "$.result.content[10].type" == "resource_link"
jsonpath "$.result.content[10].uri" == "{{base}}/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.content[10].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 9
jsonpath "$.result.structuredContent.results[0].from" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[0].to" == "{{base}}/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.structuredContent.results[0].at" == "/anyOf/0/$ref"
jsonpath "$.result.structuredContent.results[2].to" == "{{base}}/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results" count == 9
variable "text" jsonpath "$.results[0].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[0].to" == "{{base}}/self/v1/schemas/mcp/initialize/response"
variable "text" jsonpath "$.results[0].at" == "/anyOf/0/$ref"
variable "text" jsonpath "$.results[1].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[1].to" == "{{base}}/self/v1/schemas/mcp/ping/response"
variable "text" jsonpath "$.results[1].at" == "/anyOf/1/$ref"
variable "text" jsonpath "$.results[2].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[2].to" == "{{base}}/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[2].at" == "/anyOf/2/$ref"
variable "text" jsonpath "$.results[3].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[3].to" == "{{base}}/self/v1/schemas/mcp/resources/list/response"
variable "text" jsonpath "$.results[3].at" == "/anyOf/3/$ref"
variable "text" jsonpath "$.results[4].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[4].to" == "{{base}}/self/v1/schemas/mcp/resources/templates/list/response"
variable "text" jsonpath "$.results[4].at" == "/anyOf/4/$ref"
variable "text" jsonpath "$.results[5].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[5].to" == "{{base}}/self/v1/schemas/mcp/resources/read/response"
variable "text" jsonpath "$.results[5].at" == "/anyOf/5/$ref"
variable "text" jsonpath "$.results[6].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[6].to" == "{{base}}/self/v1/schemas/mcp/tools/list/response"
variable "text" jsonpath "$.results[6].at" == "/anyOf/6/$ref"
variable "text" jsonpath "$.results[7].from" == "{{base}}/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[7].to" == "{{base}}/self/v1/schemas/mcp/tools/call/response"
variable "text" jsonpath "$.results[7].at" == "/anyOf/7/$ref"
variable "text" jsonpath "$.results[8].from" == "{{base}}/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[8].to" == "{{base}}/self/v1/schemas/api/schemas/evaluate/response"
variable "text" jsonpath "$.results[8].at" == "/$defs/reserved/anyOf/3/properties/data/anyOf/0/$ref"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/schemas/api/schemas/evaluate/response
HTTP 200
[Captures]
schema_text: body

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 205,
  "method": "resources/read",
  "params": {
    "uri": "{{dependency_uri}}"
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
jsonpath "$.id" == 205
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{dependency_uri}}"
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
  "id": 210,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 210
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
  "id": 211,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 211
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
  "id": 212,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 212
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
  "id": 213,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 213
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
  "id": 214,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 214
jsonpath "$.result.isError" == true
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
  "id": 215,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 215
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
  "id": 220,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 220
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/dependencies/rpc#/required"
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
  "id": 221,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 221
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
  "id": 222,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 222
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/schema/type"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/dependencies/rpc#/properties/schema/type"
jsonpath "$.error.data.errors[0].instanceLocation" == "/schema"
jsonpath "$.error.data.errors[0].error" == "The value was expected to be of type string but it was of type integer"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/dependencies/rpc#/properties"
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
  "id": 223,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 223
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/additionalProperties"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/dependencies/rpc#/additionalProperties"
jsonpath "$.error.data.errors[0].instanceLocation" == "/extra"
jsonpath "$.error.data.errors[0].error" == "The object value was not expected to define the property \"extra\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/additionalProperties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/dependencies/rpc#/additionalProperties"
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
  "id": 224,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies"
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
jsonpath "$.id" == 224
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/api/schemas/dependencies/rpc#/required"
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
  "id": 225,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == 225
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
  "id": "dependencies-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
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
jsonpath "$.id" == "dependencies-call-string-id"
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
