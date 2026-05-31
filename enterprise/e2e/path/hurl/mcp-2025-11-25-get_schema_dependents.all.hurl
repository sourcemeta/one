POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1500,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependents",
    "arguments": { "schema": "{{base}}/v1/catalog/self/v1/schemas/mcp/error" }
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
jsonpath "$.id" == 1500
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 3
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
jsonpath "$.result.structuredContent.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results" count == 1
variable "text" jsonpath "$.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"

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
  "id": 1501,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependents",
    "arguments": { "schema": "{{base}}/v1/catalog/self/v1/schemas/mcp/error" }
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
dependent_uri: jsonpath "$.result.content[2].uri"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1501
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 3
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
jsonpath "$.result.structuredContent.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results" count == 1
variable "text" jsonpath "$.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/schemas/mcp/response
HTTP 200
[Captures]
schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1502,
  "method": "resources/read",
  "params": {
    "uri": "{{dependent_uri}}"
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
jsonpath "$.id" == 1502
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{dependent_uri}}"
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
  "id": 1599,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependents",
    "arguments": { "schema": "self/v1/schemas/mcp/error" }
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
jsonpath "$.id" == 1599
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
  "id": 1600,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependents",
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
jsonpath "$.id" == 1600
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
