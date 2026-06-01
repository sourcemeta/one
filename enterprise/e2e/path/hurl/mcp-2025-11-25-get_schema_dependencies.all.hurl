# Resolve the tool's `outputSchema.$ref` once via `tools/list`. Every
# successful `tools/call` below validates its `structuredContent` against
# this schema so the advertised output shape and the actual one stay in lockstep.
#
# TODO: Drop the `structured_content` body-regex captures in favour of a plain
# `jsonpath "$.result.structuredContent"` once hurl can inline a captured JSON
# object as a request body. Tracked upstream at
# https://github.com/Orange-OpenSource/hurl/issues/2152
POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "tools/list"
}
```
HTTP 200
[Captures]
output_schema_path: jsonpath "$.result.tools[?@.name=='get_schema_dependencies'].outputSchema['$ref']" regex "https?://[^/]+/v1/catalog(.+)"
[Asserts]
jsonpath "$.result.tools[?@.name=='get_schema_dependencies'].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/get-schema-dependencies/response"

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1400,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "{{base}}/v1/catalog/self/v1/schemas/mcp/response" }
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
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1400
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 11
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.content[6].type" == "resource_link"
jsonpath "$.result.content[6].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.content[6].mimeType" == "application/schema+json"
jsonpath "$.result.content[7].type" == "resource_link"
jsonpath "$.result.content[7].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.content[7].mimeType" == "application/schema+json"
jsonpath "$.result.content[8].type" == "resource_link"
jsonpath "$.result.content[8].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.content[8].mimeType" == "application/schema+json"
jsonpath "$.result.content[9].type" == "resource_link"
jsonpath "$.result.content[9].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.content[9].mimeType" == "application/schema+json"
jsonpath "$.result.content[10].type" == "resource_link"
jsonpath "$.result.content[10].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.content[10].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 9
jsonpath "$.result.structuredContent.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.structuredContent.results[0].at" == "/anyOf/0/$ref"
jsonpath "$.result.structuredContent.results[1].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[1].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.structuredContent.results[1].at" == "/anyOf/1/$ref"
jsonpath "$.result.structuredContent.results[2].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[2].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent.results[2].at" == "/anyOf/2/$ref"
jsonpath "$.result.structuredContent.results[3].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[3].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.structuredContent.results[3].at" == "/anyOf/3/$ref"
jsonpath "$.result.structuredContent.results[4].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[4].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.structuredContent.results[4].at" == "/anyOf/4/$ref"
jsonpath "$.result.structuredContent.results[5].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[5].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.structuredContent.results[5].at" == "/anyOf/5/$ref"
jsonpath "$.result.structuredContent.results[6].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[6].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.structuredContent.results[6].at" == "/anyOf/6/$ref"
jsonpath "$.result.structuredContent.results[7].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[7].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.structuredContent.results[7].at" == "/anyOf/7/$ref"
jsonpath "$.result.structuredContent.results[8].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent.results[8].to" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.structuredContent.results[8].at" == "/$defs/standard/anyOf/4/properties/data/$ref"
variable "text" jsonpath "$.results" count == 9
variable "text" jsonpath "$.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
variable "text" jsonpath "$.results[0].at" == "/anyOf/0/$ref"
variable "text" jsonpath "$.results[1].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[1].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
variable "text" jsonpath "$.results[1].at" == "/anyOf/1/$ref"
variable "text" jsonpath "$.results[2].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[2].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[2].at" == "/anyOf/2/$ref"
variable "text" jsonpath "$.results[3].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[3].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
variable "text" jsonpath "$.results[3].at" == "/anyOf/3/$ref"
variable "text" jsonpath "$.results[4].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[4].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
variable "text" jsonpath "$.results[4].at" == "/anyOf/4/$ref"
variable "text" jsonpath "$.results[5].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[5].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
variable "text" jsonpath "$.results[5].at" == "/anyOf/5/$ref"
variable "text" jsonpath "$.results[6].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[6].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
variable "text" jsonpath "$.results[6].at" == "/anyOf/6/$ref"
variable "text" jsonpath "$.results[7].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[7].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
variable "text" jsonpath "$.results[7].at" == "/anyOf/7/$ref"
variable "text" jsonpath "$.results[8].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[8].to" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
variable "text" jsonpath "$.results[8].at" == "/$defs/standard/anyOf/4/properties/data/$ref"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 1401,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "{{base}}/v1/catalog/self/v1/schemas/mcp/response" }
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
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
dependency_uri: jsonpath "$.result.content[1].uri"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1401
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 11
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.content[6].type" == "resource_link"
jsonpath "$.result.content[6].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.content[6].mimeType" == "application/schema+json"
jsonpath "$.result.content[7].type" == "resource_link"
jsonpath "$.result.content[7].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.content[7].mimeType" == "application/schema+json"
jsonpath "$.result.content[8].type" == "resource_link"
jsonpath "$.result.content[8].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.content[8].mimeType" == "application/schema+json"
jsonpath "$.result.content[9].type" == "resource_link"
jsonpath "$.result.content[9].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.content[9].mimeType" == "application/schema+json"
jsonpath "$.result.content[10].type" == "resource_link"
jsonpath "$.result.content[10].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.content[10].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 9
jsonpath "$.result.structuredContent.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.structuredContent.results[0].at" == "/anyOf/0/$ref"
jsonpath "$.result.structuredContent.results[1].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[1].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.structuredContent.results[1].at" == "/anyOf/1/$ref"
jsonpath "$.result.structuredContent.results[2].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[2].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent.results[2].at" == "/anyOf/2/$ref"
jsonpath "$.result.structuredContent.results[3].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[3].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.structuredContent.results[3].at" == "/anyOf/3/$ref"
jsonpath "$.result.structuredContent.results[4].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[4].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.structuredContent.results[4].at" == "/anyOf/4/$ref"
jsonpath "$.result.structuredContent.results[5].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[5].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.structuredContent.results[5].at" == "/anyOf/5/$ref"
jsonpath "$.result.structuredContent.results[6].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[6].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.structuredContent.results[6].at" == "/anyOf/6/$ref"
jsonpath "$.result.structuredContent.results[7].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[7].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.structuredContent.results[7].at" == "/anyOf/7/$ref"
jsonpath "$.result.structuredContent.results[8].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent.results[8].to" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.structuredContent.results[8].at" == "/$defs/standard/anyOf/4/properties/data/$ref"
variable "text" jsonpath "$.results" count == 9
variable "text" jsonpath "$.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
variable "text" jsonpath "$.results[0].at" == "/anyOf/0/$ref"
variable "text" jsonpath "$.results[1].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[1].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
variable "text" jsonpath "$.results[1].at" == "/anyOf/1/$ref"
variable "text" jsonpath "$.results[2].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[2].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[2].at" == "/anyOf/2/$ref"
variable "text" jsonpath "$.results[3].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[3].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
variable "text" jsonpath "$.results[3].at" == "/anyOf/3/$ref"
variable "text" jsonpath "$.results[4].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[4].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
variable "text" jsonpath "$.results[4].at" == "/anyOf/4/$ref"
variable "text" jsonpath "$.results[5].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[5].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
variable "text" jsonpath "$.results[5].at" == "/anyOf/5/$ref"
variable "text" jsonpath "$.results[6].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[6].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
variable "text" jsonpath "$.results[6].at" == "/anyOf/6/$ref"
variable "text" jsonpath "$.results[7].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[7].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
variable "text" jsonpath "$.results[7].at" == "/anyOf/7/$ref"
variable "text" jsonpath "$.results[8].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[8].to" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
variable "text" jsonpath "$.results[8].at" == "/$defs/standard/anyOf/4/properties/data/$ref"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response
HTTP 200
[Captures]
schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1402,
  "method": "resources/read",
  "params": {
    "uri": "{{dependency_uri}}"
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
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1402
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{dependency_uri}}"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

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
  "id": 1499,
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
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1499
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid tool input schema URI"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment or query parameters"
jsonpath "$.result" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Schema URI with a fragment is rejected
POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1500,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "{{base}}/v1/catalog/test/object#meta" }
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
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1500
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid tool input schema URI"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment or query parameters"
jsonpath "$.result" not exists

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
  "id": 1600,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "{{base}}/v1/catalog/example/full" }
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
dependencies_path_baseline_text: jsonpath "$.result.content[0].text"
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1600
jsonpath "$.result.isError" == false

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 1601,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "HTTP://LOCALHOST:{{port}}/v1/catalog/EXAMPLE/FULL.JSON" }
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
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1601
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{dependencies_path_baseline_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 1602,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "{{base}}/v1/catalog/.json" }
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
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1602
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].text" == "Schema not found"

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
  "id": 1603,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "{{base}}/v1/catalog/example/full?bundle=1" }
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
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1603
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid tool input schema URI"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment or query parameters"
jsonpath "$.result" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
