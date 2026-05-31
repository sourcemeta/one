# Resolve the tool's `outputSchema.$ref` once via `tools/list`. Every
# successful `tools/call` below validates its `structuredContent` against
# this schema so the advertised output shape and the actual one stay in lockstep
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
output_schema_path: jsonpath "$.result.tools[10].outputSchema['$ref']" regex "https?://[^/]+/v1/catalog(.+)"
[Asserts]
jsonpath "$.result.tools[10].name" == "search_schemas"
jsonpath "$.result.tools[10].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/rpc/response"

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1200,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "example", "limit": 3 }
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
jsonpath "$.id" == 1200
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 4
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.content[1].name" == "Example With Both"
jsonpath "$.result.content[1].description" == "A schema that has both a title and a description"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.content[2].name" == "/v1/catalog/example/description-only"
jsonpath "$.result.content[2].description" == "A schema with only a description and no title"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.content[3].name" == "Example With Title Only"
jsonpath "$.result.content[3].description" not exists
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 3
jsonpath "$.result.structuredContent.results[0].path" == "/v1/catalog/example/full"
jsonpath "$.result.structuredContent.results[0].identifier" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.structuredContent.results[0].title" == "Example With Both"
jsonpath "$.result.structuredContent.results[0].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 0
jsonpath "$.result.structuredContent.results[1].path" == "/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.results[1].identifier" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.results[1].title" == ""
jsonpath "$.result.structuredContent.results[1].description" == "A schema with only a description and no title"
jsonpath "$.result.structuredContent.results[1].priority" == 50
jsonpath "$.result.structuredContent.results[1].health" == 0
jsonpath "$.result.structuredContent.results[2].path" == "/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.results[2].identifier" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.results[2].title" == "Example With Title Only"
jsonpath "$.result.structuredContent.results[2].description" == ""
jsonpath "$.result.structuredContent.results[2].priority" == 50
jsonpath "$.result.structuredContent.results[2].health" == 0
variable "text" jsonpath "$.results" count == 3
variable "text" jsonpath "$.results[0].path" == "/v1/catalog/example/full"
variable "text" jsonpath "$.results[0].identifier" == "{{base}}/v1/catalog/example/full"
variable "text" jsonpath "$.results[0].title" == "Example With Both"
variable "text" jsonpath "$.results[0].description" == "A schema that has both a title and a description"
variable "text" jsonpath "$.results[0].priority" == 50
variable "text" jsonpath "$.results[0].health" == 0
variable "text" jsonpath "$.results[1].path" == "/v1/catalog/example/description-only"
variable "text" jsonpath "$.results[1].identifier" == "{{base}}/v1/catalog/example/description-only"
variable "text" jsonpath "$.results[1].title" == ""
variable "text" jsonpath "$.results[1].description" == "A schema with only a description and no title"
variable "text" jsonpath "$.results[1].priority" == 50
variable "text" jsonpath "$.results[1].health" == 0
variable "text" jsonpath "$.results[2].path" == "/v1/catalog/example/title-only"
variable "text" jsonpath "$.results[2].identifier" == "{{base}}/v1/catalog/example/title-only"
variable "text" jsonpath "$.results[2].title" == "Example With Title Only"
variable "text" jsonpath "$.results[2].description" == ""
variable "text" jsonpath "$.results[2].priority" == 50
variable "text" jsonpath "$.results[2].health" == 0

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
  "id": 1201,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "Both", "limit": 1 }
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
resource_uri: jsonpath "$.result.content[1].uri"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1201
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.content[1].name" == "Example With Both"
jsonpath "$.result.content[1].description" == "A schema that has both a title and a description"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/v1/catalog/example/full"
jsonpath "$.result.structuredContent.results[0].identifier" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.structuredContent.results[0].title" == "Example With Both"
jsonpath "$.result.structuredContent.results[0].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 0
variable "text" jsonpath "$.results" count == 1
variable "text" jsonpath "$.results[0].path" == "/v1/catalog/example/full"
variable "text" jsonpath "$.results[0].identifier" == "{{base}}/v1/catalog/example/full"
variable "text" jsonpath "$.results[0].title" == "Example With Both"
variable "text" jsonpath "$.results[0].description" == "A schema that has both a title and a description"
variable "text" jsonpath "$.results[0].priority" == 50
variable "text" jsonpath "$.results[0].health" == 0

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

GET {{base}}/v1/catalog/example/full
HTTP 200
[Captures]
schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1202,
  "method": "resources/read",
  "params": {
    "uri": "{{resource_uri}}"
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
jsonpath "$.id" == 1202
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{resource_uri}}"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A whitespace-only query is rejected.
POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1203,
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
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1203
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/q/pattern"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/rpc/request#/properties/q/pattern"
jsonpath "$.error.data.errors[0].instanceLocation" == "/q"
jsonpath "$.error.data.errors[0].error" == "The string value \"   \" was expected to match the regular expression \"\\S\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/rpc/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the 3 defined properties subschemas"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A 70KB schema title is truncated in the search index to 65535 bytes
# ending in "..." rather than dropping the schema from the index
# entirely

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1204,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": { "q": "truncate-target" }
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
jsonpath "$.id" == 1204
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.content[1].name" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
variable "text" jsonpath "$.results" count == 1
jsonpath "$.result.structuredContent.results[0].path" == "/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.results[0].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.results[0].title" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.structuredContent.results[0].description" == ""
jsonpath "$.result.structuredContent.results[0].priority" == 50
jsonpath "$.result.structuredContent.results[0].health" == 0
variable "text" jsonpath "$.results[0].path" == "/v1/catalog/example/zzz-truncate-target"
variable "text" jsonpath "$.results[0].title" matches "^Z{65532}\\.\\.\\.$"

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
