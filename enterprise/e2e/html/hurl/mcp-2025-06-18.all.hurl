POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "protocolVersion": "2025-06-18",
    "capabilities": {},
    "clientInfo": { "name": "hurl", "version": "1.0.0" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.result.protocolVersion" matches "^\\d{4}-\\d{2}-\\d{2}$"
jsonpath "$.result.capabilities.resources" isObject
jsonpath "$.result.capabilities.resources" isEmpty
jsonpath "$.result.capabilities.tools" isObject
jsonpath "$.result.capabilities.tools" isEmpty
jsonpath "$.result.serverInfo.name" == "sourcemeta-one-enterprise"
jsonpath "$.result.serverInfo.title" == "Sourcemeta One Enterprise"
jsonpath "$.result.serverInfo.version" matches "^.+$"
jsonpath "$.result.instructions" matches "^.+$"
# Added in 2025-11-25
# https://modelcontextprotocol.io/specification/2025-11-25/changelog
jsonpath "$.result.serverInfo.description" not exists
jsonpath "$.result.serverInfo.websiteUrl" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 2, "method": "resources/templates/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 2
jsonpath "$.result.resourceTemplates" count == 1
jsonpath "$.result.resourceTemplates[0].uriTemplate" == "{{base}}/{+path}{?bundle}"
jsonpath "$.result.resourceTemplates[0].name" == "JSON Schema"
jsonpath "$.result.resourceTemplates[0].description" == "A JSON Schema in this catalog, addressable by its canonical absolute URI under this server's origin. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Substitute `{+path}` with the schema's catalog path. Include `{?bundle}` (its presence alone triggers bundling, any value is ignored) to receive the schema with every external `$ref` inlined into a single self-contained document. The URI must not contain a fragment. Use `resources/list` to discover the available schemas rather than guessing paths"
jsonpath "$.result.resourceTemplates[0].mimeType" == "application/schema+json"
# Added in 2025-11-25 (SEP-973)
# https://github.com/modelcontextprotocol/modelcontextprotocol/issues/973
jsonpath "$..icons" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 3, "method": "resources/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 3
jsonpath "$.result.resources" count == 50
jsonpath "$.result.resources[0].uri" == "{{base}}/twins/foo"
jsonpath "$.result.resources[0].name" == "Twin Foo Schema"
jsonpath "$.result.resources[0].description" == "A schema whose base name collides with a sibling directory"
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.resources[0].size" == 263
jsonpath "$.result.resources[0].annotations.priority" == 0.5
# Added in 2025-11-25 (SEP-973)
# https://github.com/modelcontextprotocol/modelcontextprotocol/issues/973
jsonpath "$..icons" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 4,
  "method": "resources/read",
  "params": { "uri": "{{base}}/self/v1/schemas/api/error" }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 4
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/self/v1/schemas/api/error"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" isString

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 5, "method": "tools/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 5
jsonpath "$.result.tools" count == 11
jsonpath "$.result.tools[0].name" == "list_directory"
jsonpath "$.result.tools[0].description" == "List the contents of a directory in the catalog"
jsonpath "$.result.tools[0].inputSchema['$ref']" == "{{base}}/self/v1/schemas/api/list/rpc/request"
jsonpath "$.result.tools[0].inputSchema.type" == "object"
jsonpath "$.result.tools[0].outputSchema['$ref']" == "{{base}}/self/v1/schemas/api/list/rpc/response"
jsonpath "$.result.tools[0].outputSchema.type" == "object"
jsonpath "$.result.tools[0].annotations.title" == "List Directory"
jsonpath "$.result.tools[0].annotations.readOnlyHint" == true
jsonpath "$.result.tools[0].annotations.destructiveHint" == false
jsonpath "$.result.tools[0].annotations.idempotentHint" == true
jsonpath "$.result.tools[0].annotations.openWorldHint" == false
# Added in 2025-11-25 (SEP-973)
# https://github.com/modelcontextprotocol/modelcontextprotocol/issues/973
jsonpath "$..icons" not exists
# Added in 2025-11-25 (SEP-1686)
# https://github.com/modelcontextprotocol/modelcontextprotocol/issues/1686
jsonpath "$..execution" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 6,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {}
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 6
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.path" == "/"
jsonpath "$.result.structuredContent.url" == "{{base}}"
jsonpath "$.result.structuredContent.entries" count >= 1

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 60,
  "method": "tools/call",
  "params": {
    "name": "list_directory"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 60
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.path" == "/"
variable "text" jsonpath "$.path" == "/"
jsonpath "$.result.structuredContent.url" == "{{base}}"
variable "text" jsonpath "$.url" == "{{base}}"
jsonpath "$.result.structuredContent.breadcrumb" count == 0
variable "text" jsonpath "$.breadcrumb" count == 0
jsonpath "$.result.structuredContent.schemas" == 126
variable "text" jsonpath "$.schemas" == 126
jsonpath "$.result.structuredContent.entries" count == 3
variable "text" jsonpath "$.entries" count == 3
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
jsonpath "$.result.structuredContent.entries[0].schemas" == 56
variable "text" jsonpath "$.entries[0].schemas" == 56
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
jsonpath "$.result.structuredContent.entries[2].name" == "twins"
variable "text" jsonpath "$.entries[2].name" == "twins"
jsonpath "$.result.structuredContent.entries[2].title" == "Twins"
variable "text" jsonpath "$.entries[2].title" == "Twins"
jsonpath "$.result.structuredContent.entries[2].description" == "Schemas whose base name collides with a sibling directory"
variable "text" jsonpath "$.entries[2].description" == "Schemas whose base name collides with a sibling directory"
jsonpath "$.result.structuredContent.entries[2].type" == "directory"
variable "text" jsonpath "$.entries[2].type" == "directory"
jsonpath "$.result.structuredContent.entries[2].health" == 100
variable "text" jsonpath "$.entries[2].health" == 100
jsonpath "$.result.structuredContent.entries[2].schemas" == 2
variable "text" jsonpath "$.entries[2].schemas" == 2
jsonpath "$.result.structuredContent.entries[2].path" == "/twins/"
variable "text" jsonpath "$.entries[2].path" == "/twins/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-06-18
Content-Type: application/json
```
[ { "jsonrpc": "2.0", "id": 1, "method": "ping" } ]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == null
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"
jsonpath "$.error.data" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
