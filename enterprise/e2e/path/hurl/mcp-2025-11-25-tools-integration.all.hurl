POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1200,
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
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
example_path: jsonpath "$.result.structuredContent.entries[1].path"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1200
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/v1/catalog/"
variable "text" jsonpath "$.path" == "/v1/catalog/"
jsonpath "$.result.structuredContent.entries[1].name" == "example"
variable "text" jsonpath "$.entries[1].name" == "example"
variable "example_path" == "/v1/catalog/example/"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1201,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "example" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
full_path: jsonpath "$.result.structuredContent.entries[2].path"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1201
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/v1/catalog/example"
variable "text" jsonpath "$.path" == "/v1/catalog/example"
jsonpath "$.result.structuredContent.entries[2].name" == "full"
variable "text" jsonpath "$.entries[2].name" == "full"
jsonpath "$.result.structuredContent.entries[2].type" == "schema"
variable "text" jsonpath "$.entries[2].type" == "schema"
variable "full_path" == "/v1/catalog/example/full"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1202,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
    "arguments": { "schema": "{{base}}{{full_path}}" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1202
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.identifier" == "{{base}}/v1/catalog/example/full"
variable "text" jsonpath "$.identifier" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.structuredContent.path" == "/v1/catalog/example/full"
variable "text" jsonpath "$.path" == "/v1/catalog/example/full"
jsonpath "$.result.structuredContent.title" == "Example With Both"
variable "text" jsonpath "$.title" == "Example With Both"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1210,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "Both" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
hit_path: jsonpath "$.result.structuredContent.results[0].path"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1210
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
variable "hit_path" == "/v1/catalog/example/full"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1211,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
    "arguments": { "schema": "{{base}}{{hit_path}}" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1211
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.identifier" == "{{base}}/v1/catalog/example/full"
variable "text" jsonpath "$.identifier" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.structuredContent.title" == "Example With Both"
variable "text" jsonpath "$.title" == "Example With Both"
jsonpath "$.result.structuredContent.description" == "A schema that has both a title and a description"
variable "text" jsonpath "$.description" == "A schema that has both a title and a description"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1220,
  "method": "tools/call",
  "params": {
    "name": "evaluate_schema",
    "arguments": { "schema": "{{base}}/v1/catalog/example/bare", "stringifiedInstance": "42" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
error_location: jsonpath "$.result.structuredContent.errors[0].absoluteKeywordLocation"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1220
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.valid" == false
variable "text" jsonpath "$.valid" == false
variable "error_location" == "{{base}}/v1/catalog/example/bare#/type"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1230,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": { "schema": "{{base}}/v1/catalog/example/bare", "stringifiedInstance": "42" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1230
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.valid" == false
variable "text" jsonpath "$.valid" == false
jsonpath "$.result.structuredContent.steps[1].type" == "fail"
variable "text" jsonpath "$.steps[1].type" == "fail"
jsonpath "$.result.structuredContent.steps[1].keywordLocation" == "{{error_location}}"
variable "text" jsonpath "$.steps[1].keywordLocation" == "{{error_location}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A 70KB title in the source schema gets truncated to 65535 bytes
# ending in "..." through get_schema_metadata, mirroring the search index

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "metadata-truncated",
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
    "arguments": { "schema": "{{base}}/v1/catalog/example/zzz-truncate-target" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "metadata-truncated"
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.path" == "/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.title" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.structuredContent.priority" == 50
jsonpath "$.result.structuredContent.health" == 0
jsonpath "$.result.structuredContent.dependencies" == 0
jsonpath "$.result.structuredContent.alert" == null
variable "text" jsonpath "$.identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
variable "text" jsonpath "$.path" == "/v1/catalog/example/zzz-truncate-target"
variable "text" jsonpath "$.title" matches "^Z{65532}\\.\\.\\.$"
variable "text" jsonpath "$.priority" == 50
variable "text" jsonpath "$.health" == 0
variable "text" jsonpath "$.dependencies" == 0
variable "text" jsonpath "$.alert" == null

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
