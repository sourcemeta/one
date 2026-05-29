POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1300,
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
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1300
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 6
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.content[1].name" == "/v1/catalog/example/bare"
jsonpath "$.result.content[1].description" not exists
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.content[2].name" == "/v1/catalog/example/description-only"
jsonpath "$.result.content[2].description" == "A schema with only a description and no title"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.content[3].name" == "Example With Both"
jsonpath "$.result.content[3].description" == "A schema that has both a title and a description"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.content[4].name" == "Example With Title Only"
jsonpath "$.result.content[4].description" not exists
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.content[5].name" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.content[5].description" not exists
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.path" == "/v1/catalog/example"
variable "text" jsonpath "$.path" == "/v1/catalog/example"
jsonpath "$.result.structuredContent.url" == "{{base}}/v1/catalog/example"
variable "text" jsonpath "$.url" == "{{base}}/v1/catalog/example"
jsonpath "$.result.structuredContent.breadcrumb" count == 1
variable "text" jsonpath "$.breadcrumb" count == 1
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "example"
variable "text" jsonpath "$.breadcrumb[0].name" == "example"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/v1/catalog/example/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/v1/catalog/example/"
jsonpath "$.result.structuredContent.schemas" == 5
variable "text" jsonpath "$.schemas" == 5
jsonpath "$.result.structuredContent.health" == 0
variable "text" jsonpath "$.health" == 0
jsonpath "$.result.structuredContent.entries" count == 5
variable "text" jsonpath "$.entries" count == 5
jsonpath "$.result.structuredContent.entries[0].name" == "bare"
variable "text" jsonpath "$.entries[0].name" == "bare"
jsonpath "$.result.structuredContent.entries[0].type" == "schema"
variable "text" jsonpath "$.entries[0].type" == "schema"
jsonpath "$.result.structuredContent.entries[0].path" == "/v1/catalog/example/bare"
variable "text" jsonpath "$.entries[0].path" == "/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].identifier" == "{{base}}/v1/catalog/example/bare"
variable "text" jsonpath "$.entries[0].identifier" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].health" == 0
jsonpath "$.result.structuredContent.entries[0].priority" == 50
variable "text" jsonpath "$.entries[0].priority" == 50
variable "text" jsonpath "$.entries[0].health" == 0
jsonpath "$.result.structuredContent.entries[0].title" not exists
variable "text" jsonpath "$.entries[0].title" not exists
jsonpath "$.result.structuredContent.entries[0].description" not exists
variable "text" jsonpath "$.entries[0].description" not exists
jsonpath "$.result.structuredContent.entries[1].name" == "description-only"
variable "text" jsonpath "$.entries[1].name" == "description-only"
jsonpath "$.result.structuredContent.entries[1].type" == "schema"
variable "text" jsonpath "$.entries[1].type" == "schema"
jsonpath "$.result.structuredContent.entries[1].path" == "/v1/catalog/example/description-only"
variable "text" jsonpath "$.entries[1].path" == "/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.entries[1].identifier" == "{{base}}/v1/catalog/example/description-only"
variable "text" jsonpath "$.entries[1].identifier" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.entries[1].health" == 0
variable "text" jsonpath "$.entries[1].health" == 0
jsonpath "$.result.structuredContent.entries[1].title" not exists
variable "text" jsonpath "$.entries[1].title" not exists
jsonpath "$.result.structuredContent.entries[1].description" == "A schema with only a description and no title"
jsonpath "$.result.structuredContent.entries[1].priority" == 50
variable "text" jsonpath "$.entries[1].priority" == 50
variable "text" jsonpath "$.entries[1].description" == "A schema with only a description and no title"
jsonpath "$.result.structuredContent.entries[2].name" == "full"
variable "text" jsonpath "$.entries[2].name" == "full"
jsonpath "$.result.structuredContent.entries[2].type" == "schema"
variable "text" jsonpath "$.entries[2].type" == "schema"
jsonpath "$.result.structuredContent.entries[2].path" == "/v1/catalog/example/full"
variable "text" jsonpath "$.entries[2].path" == "/v1/catalog/example/full"
jsonpath "$.result.structuredContent.entries[2].identifier" == "{{base}}/v1/catalog/example/full"
variable "text" jsonpath "$.entries[2].identifier" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.structuredContent.entries[2].health" == 0
variable "text" jsonpath "$.entries[2].health" == 0
jsonpath "$.result.structuredContent.entries[2].title" == "Example With Both"
variable "text" jsonpath "$.entries[2].title" == "Example With Both"
jsonpath "$.result.structuredContent.entries[2].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.entries[2].priority" == 50
variable "text" jsonpath "$.entries[2].priority" == 50
variable "text" jsonpath "$.entries[2].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.entries[3].name" == "title-only"
variable "text" jsonpath "$.entries[3].name" == "title-only"
jsonpath "$.result.structuredContent.entries[3].type" == "schema"
variable "text" jsonpath "$.entries[3].type" == "schema"
jsonpath "$.result.structuredContent.entries[3].path" == "/v1/catalog/example/title-only"
variable "text" jsonpath "$.entries[3].path" == "/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.entries[3].identifier" == "{{base}}/v1/catalog/example/title-only"
variable "text" jsonpath "$.entries[3].identifier" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.entries[3].health" == 0
variable "text" jsonpath "$.entries[3].health" == 0
jsonpath "$.result.structuredContent.entries[3].title" == "Example With Title Only"
jsonpath "$.result.structuredContent.entries[3].priority" == 50
variable "text" jsonpath "$.entries[3].priority" == 50
variable "text" jsonpath "$.entries[3].title" == "Example With Title Only"
jsonpath "$.result.structuredContent.entries[3].description" not exists
variable "text" jsonpath "$.entries[3].description" not exists
jsonpath "$.result.structuredContent.entries[4].name" == "zzz-truncate-target"
variable "text" jsonpath "$.entries[4].name" == "zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].type" == "schema"
variable "text" jsonpath "$.entries[4].type" == "schema"
jsonpath "$.result.structuredContent.entries[4].path" == "/v1/catalog/example/zzz-truncate-target"
variable "text" jsonpath "$.entries[4].path" == "/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
variable "text" jsonpath "$.entries[4].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].health" == 0
variable "text" jsonpath "$.entries[4].health" == 0
jsonpath "$.result.structuredContent.entries[4].title" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.structuredContent.entries[4].priority" == 50
variable "text" jsonpath "$.entries[4].priority" == 50
variable "text" jsonpath "$.entries[4].title" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.structuredContent.entries[4].description" not exists
variable "text" jsonpath "$.entries[4].description" not exists

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
  "id": 1301,
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
text: jsonpath "$.result.content[0].text"
schema_uri: jsonpath "$.result.content[3].uri"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1301
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 6
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.content[1].name" == "/v1/catalog/example/bare"
jsonpath "$.result.content[1].description" not exists
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.content[2].name" == "/v1/catalog/example/description-only"
jsonpath "$.result.content[2].description" == "A schema with only a description and no title"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.content[3].name" == "Example With Both"
jsonpath "$.result.content[3].description" == "A schema that has both a title and a description"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.content[4].name" == "Example With Title Only"
jsonpath "$.result.content[4].description" not exists
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.content[5].name" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.content[5].description" not exists
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.path" == "/v1/catalog/example"
variable "text" jsonpath "$.path" == "/v1/catalog/example"
jsonpath "$.result.structuredContent.url" == "{{base}}/v1/catalog/example"
variable "text" jsonpath "$.url" == "{{base}}/v1/catalog/example"
jsonpath "$.result.structuredContent.breadcrumb" count == 1
variable "text" jsonpath "$.breadcrumb" count == 1
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "example"
variable "text" jsonpath "$.breadcrumb[0].name" == "example"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/v1/catalog/example/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/v1/catalog/example/"
jsonpath "$.result.structuredContent.schemas" == 5
variable "text" jsonpath "$.schemas" == 5
jsonpath "$.result.structuredContent.health" == 0
variable "text" jsonpath "$.health" == 0
jsonpath "$.result.structuredContent.entries" count == 5
variable "text" jsonpath "$.entries" count == 5
jsonpath "$.result.structuredContent.entries[0].name" == "bare"
variable "text" jsonpath "$.entries[0].name" == "bare"
jsonpath "$.result.structuredContent.entries[0].type" == "schema"
variable "text" jsonpath "$.entries[0].type" == "schema"
jsonpath "$.result.structuredContent.entries[0].path" == "/v1/catalog/example/bare"
variable "text" jsonpath "$.entries[0].path" == "/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].identifier" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].priority" == 50
variable "text" jsonpath "$.entries[0].priority" == 50
variable "text" jsonpath "$.entries[0].identifier" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].title" not exists
variable "text" jsonpath "$.entries[0].title" not exists
jsonpath "$.result.structuredContent.entries[0].description" not exists
variable "text" jsonpath "$.entries[0].description" not exists
jsonpath "$.result.structuredContent.entries[1].name" == "description-only"
variable "text" jsonpath "$.entries[1].name" == "description-only"
jsonpath "$.result.structuredContent.entries[1].type" == "schema"
variable "text" jsonpath "$.entries[1].type" == "schema"
jsonpath "$.result.structuredContent.entries[1].path" == "/v1/catalog/example/description-only"
variable "text" jsonpath "$.entries[1].path" == "/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.entries[1].identifier" == "{{base}}/v1/catalog/example/description-only"
variable "text" jsonpath "$.entries[1].identifier" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.entries[1].title" not exists
variable "text" jsonpath "$.entries[1].title" not exists
jsonpath "$.result.structuredContent.entries[1].description" == "A schema with only a description and no title"
jsonpath "$.result.structuredContent.entries[1].priority" == 50
variable "text" jsonpath "$.entries[1].priority" == 50
variable "text" jsonpath "$.entries[1].description" == "A schema with only a description and no title"
jsonpath "$.result.structuredContent.entries[2].name" == "full"
variable "text" jsonpath "$.entries[2].name" == "full"
jsonpath "$.result.structuredContent.entries[2].type" == "schema"
variable "text" jsonpath "$.entries[2].type" == "schema"
jsonpath "$.result.structuredContent.entries[2].path" == "/v1/catalog/example/full"
variable "text" jsonpath "$.entries[2].path" == "/v1/catalog/example/full"
jsonpath "$.result.structuredContent.entries[2].identifier" == "{{base}}/v1/catalog/example/full"
variable "text" jsonpath "$.entries[2].identifier" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.structuredContent.entries[2].title" == "Example With Both"
variable "text" jsonpath "$.entries[2].title" == "Example With Both"
jsonpath "$.result.structuredContent.entries[2].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.entries[2].priority" == 50
variable "text" jsonpath "$.entries[2].priority" == 50
variable "text" jsonpath "$.entries[2].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.entries[3].name" == "title-only"
variable "text" jsonpath "$.entries[3].name" == "title-only"
jsonpath "$.result.structuredContent.entries[3].type" == "schema"
variable "text" jsonpath "$.entries[3].type" == "schema"
jsonpath "$.result.structuredContent.entries[3].path" == "/v1/catalog/example/title-only"
variable "text" jsonpath "$.entries[3].path" == "/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.entries[3].identifier" == "{{base}}/v1/catalog/example/title-only"
variable "text" jsonpath "$.entries[3].identifier" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.entries[3].title" == "Example With Title Only"
jsonpath "$.result.structuredContent.entries[3].priority" == 50
variable "text" jsonpath "$.entries[3].priority" == 50
variable "text" jsonpath "$.entries[3].title" == "Example With Title Only"
jsonpath "$.result.structuredContent.entries[3].description" not exists
variable "text" jsonpath "$.entries[3].description" not exists
jsonpath "$.result.structuredContent.entries[4].name" == "zzz-truncate-target"
variable "text" jsonpath "$.entries[4].name" == "zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].type" == "schema"
variable "text" jsonpath "$.entries[4].type" == "schema"
jsonpath "$.result.structuredContent.entries[4].path" == "/v1/catalog/example/zzz-truncate-target"
variable "text" jsonpath "$.entries[4].path" == "/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].priority" == 50
variable "text" jsonpath "$.entries[4].priority" == 50
variable "text" jsonpath "$.entries[4].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].title" matches "^Z{65532}\\.\\.\\.$"
variable "text" jsonpath "$.entries[4].title" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.structuredContent.entries[4].description" not exists
variable "text" jsonpath "$.entries[4].description" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
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
  "id": 1302,
  "method": "resources/read",
  "params": {
    "uri": "{{schema_uri}}"
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
jsonpath "$.id" == 1302
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{schema_uri}}"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An LLM-supplied mixed-case directory name (e.g. "EXAMPLE", "Example")
# resolves to the same directory as the canonical lowercase form because
# the handler case-folds the input before looking it up

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1303,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "EXAMPLE" }
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
jsonpath "$.id" == 1303
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 6
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.content[1].name" == "/v1/catalog/example/bare"
jsonpath "$.result.content[1].description" not exists
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.content[2].name" == "/v1/catalog/example/description-only"
jsonpath "$.result.content[2].description" == "A schema with only a description and no title"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.content[3].name" == "Example With Both"
jsonpath "$.result.content[3].description" == "A schema that has both a title and a description"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.content[4].name" == "Example With Title Only"
jsonpath "$.result.content[4].description" not exists
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.content[5].name" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.content[5].description" not exists
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.path" == "/v1/catalog/example"
variable "text" jsonpath "$.path" == "/v1/catalog/example"
jsonpath "$.result.structuredContent.url" == "{{base}}/v1/catalog/example"
variable "text" jsonpath "$.url" == "{{base}}/v1/catalog/example"
jsonpath "$.result.structuredContent.breadcrumb" count == 1
variable "text" jsonpath "$.breadcrumb" count == 1
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "example"
variable "text" jsonpath "$.breadcrumb[0].name" == "example"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/v1/catalog/example/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/v1/catalog/example/"
jsonpath "$.result.structuredContent.schemas" == 5
variable "text" jsonpath "$.schemas" == 5
jsonpath "$.result.structuredContent.health" == 0
variable "text" jsonpath "$.health" == 0
jsonpath "$.result.structuredContent.entries" count == 5
variable "text" jsonpath "$.entries" count == 5
jsonpath "$.result.structuredContent.entries[0].name" == "bare"
variable "text" jsonpath "$.entries[0].name" == "bare"
jsonpath "$.result.structuredContent.entries[0].type" == "schema"
variable "text" jsonpath "$.entries[0].type" == "schema"
jsonpath "$.result.structuredContent.entries[0].path" == "/v1/catalog/example/bare"
variable "text" jsonpath "$.entries[0].path" == "/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].identifier" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].priority" == 50
variable "text" jsonpath "$.entries[0].priority" == 50
variable "text" jsonpath "$.entries[0].identifier" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].title" not exists
variable "text" jsonpath "$.entries[0].title" not exists
jsonpath "$.result.structuredContent.entries[0].description" not exists
variable "text" jsonpath "$.entries[0].description" not exists
jsonpath "$.result.structuredContent.entries[1].name" == "description-only"
variable "text" jsonpath "$.entries[1].name" == "description-only"
jsonpath "$.result.structuredContent.entries[1].type" == "schema"
variable "text" jsonpath "$.entries[1].type" == "schema"
jsonpath "$.result.structuredContent.entries[1].path" == "/v1/catalog/example/description-only"
variable "text" jsonpath "$.entries[1].path" == "/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.entries[1].identifier" == "{{base}}/v1/catalog/example/description-only"
variable "text" jsonpath "$.entries[1].identifier" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.entries[1].title" not exists
variable "text" jsonpath "$.entries[1].title" not exists
jsonpath "$.result.structuredContent.entries[1].description" == "A schema with only a description and no title"
jsonpath "$.result.structuredContent.entries[1].priority" == 50
variable "text" jsonpath "$.entries[1].priority" == 50
variable "text" jsonpath "$.entries[1].description" == "A schema with only a description and no title"
jsonpath "$.result.structuredContent.entries[2].name" == "full"
variable "text" jsonpath "$.entries[2].name" == "full"
jsonpath "$.result.structuredContent.entries[2].type" == "schema"
variable "text" jsonpath "$.entries[2].type" == "schema"
jsonpath "$.result.structuredContent.entries[2].path" == "/v1/catalog/example/full"
variable "text" jsonpath "$.entries[2].path" == "/v1/catalog/example/full"
jsonpath "$.result.structuredContent.entries[2].identifier" == "{{base}}/v1/catalog/example/full"
variable "text" jsonpath "$.entries[2].identifier" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.structuredContent.entries[2].title" == "Example With Both"
variable "text" jsonpath "$.entries[2].title" == "Example With Both"
jsonpath "$.result.structuredContent.entries[2].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.entries[2].priority" == 50
variable "text" jsonpath "$.entries[2].priority" == 50
variable "text" jsonpath "$.entries[2].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.entries[3].name" == "title-only"
variable "text" jsonpath "$.entries[3].name" == "title-only"
jsonpath "$.result.structuredContent.entries[3].type" == "schema"
variable "text" jsonpath "$.entries[3].type" == "schema"
jsonpath "$.result.structuredContent.entries[3].path" == "/v1/catalog/example/title-only"
variable "text" jsonpath "$.entries[3].path" == "/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.entries[3].identifier" == "{{base}}/v1/catalog/example/title-only"
variable "text" jsonpath "$.entries[3].identifier" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.entries[3].title" == "Example With Title Only"
jsonpath "$.result.structuredContent.entries[3].priority" == 50
variable "text" jsonpath "$.entries[3].priority" == 50
variable "text" jsonpath "$.entries[3].title" == "Example With Title Only"
jsonpath "$.result.structuredContent.entries[3].description" not exists
variable "text" jsonpath "$.entries[3].description" not exists
jsonpath "$.result.structuredContent.entries[4].name" == "zzz-truncate-target"
variable "text" jsonpath "$.entries[4].name" == "zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].type" == "schema"
variable "text" jsonpath "$.entries[4].type" == "schema"
jsonpath "$.result.structuredContent.entries[4].path" == "/v1/catalog/example/zzz-truncate-target"
variable "text" jsonpath "$.entries[4].path" == "/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].priority" == 50
variable "text" jsonpath "$.entries[4].priority" == 50
variable "text" jsonpath "$.entries[4].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].title" matches "^Z{65532}\\.\\.\\.$"
variable "text" jsonpath "$.entries[4].title" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.structuredContent.entries[4].description" not exists
variable "text" jsonpath "$.entries[4].description" not exists

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
  "id": 1304,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "Example" }
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
jsonpath "$.id" == 1304
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 6
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.content[1].name" == "/v1/catalog/example/bare"
jsonpath "$.result.content[1].description" not exists
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.content[2].name" == "/v1/catalog/example/description-only"
jsonpath "$.result.content[2].description" == "A schema with only a description and no title"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.content[3].name" == "Example With Both"
jsonpath "$.result.content[3].description" == "A schema that has both a title and a description"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.content[4].name" == "Example With Title Only"
jsonpath "$.result.content[4].description" not exists
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.content[5].name" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.content[5].description" not exists
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.path" == "/v1/catalog/example"
variable "text" jsonpath "$.path" == "/v1/catalog/example"
jsonpath "$.result.structuredContent.url" == "{{base}}/v1/catalog/example"
variable "text" jsonpath "$.url" == "{{base}}/v1/catalog/example"
jsonpath "$.result.structuredContent.breadcrumb" count == 1
variable "text" jsonpath "$.breadcrumb" count == 1
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "example"
variable "text" jsonpath "$.breadcrumb[0].name" == "example"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/v1/catalog/example/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/v1/catalog/example/"
jsonpath "$.result.structuredContent.schemas" == 5
variable "text" jsonpath "$.schemas" == 5
jsonpath "$.result.structuredContent.health" == 0
variable "text" jsonpath "$.health" == 0
jsonpath "$.result.structuredContent.entries" count == 5
variable "text" jsonpath "$.entries" count == 5
jsonpath "$.result.structuredContent.entries[0].name" == "bare"
variable "text" jsonpath "$.entries[0].name" == "bare"
jsonpath "$.result.structuredContent.entries[0].type" == "schema"
variable "text" jsonpath "$.entries[0].type" == "schema"
jsonpath "$.result.structuredContent.entries[0].path" == "/v1/catalog/example/bare"
variable "text" jsonpath "$.entries[0].path" == "/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].identifier" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].priority" == 50
variable "text" jsonpath "$.entries[0].priority" == 50
variable "text" jsonpath "$.entries[0].identifier" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.entries[0].title" not exists
variable "text" jsonpath "$.entries[0].title" not exists
jsonpath "$.result.structuredContent.entries[0].description" not exists
variable "text" jsonpath "$.entries[0].description" not exists
jsonpath "$.result.structuredContent.entries[1].name" == "description-only"
variable "text" jsonpath "$.entries[1].name" == "description-only"
jsonpath "$.result.structuredContent.entries[1].type" == "schema"
variable "text" jsonpath "$.entries[1].type" == "schema"
jsonpath "$.result.structuredContent.entries[1].path" == "/v1/catalog/example/description-only"
variable "text" jsonpath "$.entries[1].path" == "/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.entries[1].identifier" == "{{base}}/v1/catalog/example/description-only"
variable "text" jsonpath "$.entries[1].identifier" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.structuredContent.entries[1].title" not exists
variable "text" jsonpath "$.entries[1].title" not exists
jsonpath "$.result.structuredContent.entries[1].description" == "A schema with only a description and no title"
jsonpath "$.result.structuredContent.entries[1].priority" == 50
variable "text" jsonpath "$.entries[1].priority" == 50
variable "text" jsonpath "$.entries[1].description" == "A schema with only a description and no title"
jsonpath "$.result.structuredContent.entries[2].name" == "full"
variable "text" jsonpath "$.entries[2].name" == "full"
jsonpath "$.result.structuredContent.entries[2].type" == "schema"
variable "text" jsonpath "$.entries[2].type" == "schema"
jsonpath "$.result.structuredContent.entries[2].path" == "/v1/catalog/example/full"
variable "text" jsonpath "$.entries[2].path" == "/v1/catalog/example/full"
jsonpath "$.result.structuredContent.entries[2].identifier" == "{{base}}/v1/catalog/example/full"
variable "text" jsonpath "$.entries[2].identifier" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.structuredContent.entries[2].title" == "Example With Both"
variable "text" jsonpath "$.entries[2].title" == "Example With Both"
jsonpath "$.result.structuredContent.entries[2].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.entries[2].priority" == 50
variable "text" jsonpath "$.entries[2].priority" == 50
variable "text" jsonpath "$.entries[2].description" == "A schema that has both a title and a description"
jsonpath "$.result.structuredContent.entries[3].name" == "title-only"
variable "text" jsonpath "$.entries[3].name" == "title-only"
jsonpath "$.result.structuredContent.entries[3].type" == "schema"
variable "text" jsonpath "$.entries[3].type" == "schema"
jsonpath "$.result.structuredContent.entries[3].path" == "/v1/catalog/example/title-only"
variable "text" jsonpath "$.entries[3].path" == "/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.entries[3].identifier" == "{{base}}/v1/catalog/example/title-only"
variable "text" jsonpath "$.entries[3].identifier" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.structuredContent.entries[3].title" == "Example With Title Only"
jsonpath "$.result.structuredContent.entries[3].priority" == 50
variable "text" jsonpath "$.entries[3].priority" == 50
variable "text" jsonpath "$.entries[3].title" == "Example With Title Only"
jsonpath "$.result.structuredContent.entries[3].description" not exists
variable "text" jsonpath "$.entries[3].description" not exists
jsonpath "$.result.structuredContent.entries[4].name" == "zzz-truncate-target"
variable "text" jsonpath "$.entries[4].name" == "zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].type" == "schema"
variable "text" jsonpath "$.entries[4].type" == "schema"
jsonpath "$.result.structuredContent.entries[4].path" == "/v1/catalog/example/zzz-truncate-target"
variable "text" jsonpath "$.entries[4].path" == "/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].priority" == 50
variable "text" jsonpath "$.entries[4].priority" == 50
variable "text" jsonpath "$.entries[4].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.structuredContent.entries[4].title" matches "^Z{65532}\\.\\.\\.$"
variable "text" jsonpath "$.entries[4].title" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.structuredContent.entries[4].description" not exists
variable "text" jsonpath "$.entries[4].description" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
