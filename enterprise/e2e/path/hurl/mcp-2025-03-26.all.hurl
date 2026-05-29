POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "protocolVersion": "2025-03-26",
    "capabilities": {},
    "clientInfo": { "name": "hurl", "version": "1.0.0" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.result.protocolVersion" == "2025-03-26"
jsonpath "$.result.capabilities.resources" isObject
jsonpath "$.result.capabilities.resources" isEmpty
jsonpath "$.result.capabilities.tools" isObject
jsonpath "$.result.capabilities.tools" isEmpty
jsonpath "$.result.serverInfo.name" == "sourcemeta-one-enterprise"
jsonpath "$.result.serverInfo.version" matches "^.+$"
jsonpath "$.result.instructions" matches "^.+$"
# Added in 2025-06-18 ("title" field on Implementation)
# https://modelcontextprotocol.io/specification/2025-06-18/changelog
jsonpath "$..title" not exists
# Added in 2025-11-25
# https://modelcontextprotocol.io/specification/2025-11-25/changelog
jsonpath "$..description" not exists
jsonpath "$..websiteUrl" not exists
jsonpath "$..icons" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 2, "method": "resources/templates/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 2
jsonpath "$.result.resourceTemplates" count == 1
jsonpath "$.result.resourceTemplates[0].uriTemplate" == "{{base}}/v1/catalog/{+path}{?bundle}"
jsonpath "$.result.resourceTemplates[0].name" == "JSON Schema"
jsonpath "$.result.resourceTemplates[0].description" == "A JSON Schema in this catalog, addressable by its canonical absolute URI under this server's origin. This instance is sovereign over its own URL namespace, schemas whose URIs lie outside this namespace will not be served here. Substitute `{+path}` with the schema's catalog path. Include `{?bundle}` (its presence alone triggers bundling, any value is ignored) to receive the schema with every external `$ref` inlined into a single self-contained document. The URI must not contain a fragment. Use `resources/list` to discover the available schemas rather than guessing paths"
jsonpath "$.result.resourceTemplates[0].mimeType" == "application/schema+json"
# Added in 2025-06-18 ("title" on ResourceTemplate)
# https://modelcontextprotocol.io/specification/2025-06-18/changelog
jsonpath "$..title" not exists
# Added in 2025-11-25 (SEP-973)
# https://github.com/modelcontextprotocol/modelcontextprotocol/issues/973
jsonpath "$..icons" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 3, "method": "resources/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 3
jsonpath "$.result.resources" count == 50
jsonpath "$.result.resources[0].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.resources[0].name" == "Example With Both"
jsonpath "$.result.resources[0].description" == "A schema that has both a title and a description"
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.resources[0].size" == 242
jsonpath "$.result.resources[0].annotations.priority" == 0.5
# Added in 2025-06-18 ("title" on Resource)
# https://modelcontextprotocol.io/specification/2025-06-18/changelog
jsonpath "$..title" not exists
# Added in 2025-11-25 (SEP-973)
# https://github.com/modelcontextprotocol/modelcontextprotocol/issues/973
jsonpath "$..icons" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 4,
  "method": "resources/read",
  "params": { "uri": "{{base}}/v1/catalog/self/v1/schemas/api/error" }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 4
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/error"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" isString

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 5, "method": "tools/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 5
jsonpath "$.result.tools" count == 11
jsonpath "$.result.tools[0].name" == "list_directory"
jsonpath "$.result.tools[0].description" == "List the contents of a directory in the catalog"
jsonpath "$.result.tools[0].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/list/rpc"
jsonpath "$.result.tools[0].inputSchema.type" == "object"
jsonpath "$.result.tools[0].annotations.title" == "List Directory"
jsonpath "$.result.tools[0].annotations.readOnlyHint" == true
jsonpath "$.result.tools[0].annotations.destructiveHint" == false
jsonpath "$.result.tools[0].annotations.idempotentHint" == true
jsonpath "$.result.tools[0].annotations.openWorldHint" == false
jsonpath "$.result.tools[1].name" == "get_schema_dependencies"
jsonpath "$.result.tools[1].description" == "List the schemas this schema directly or transitively references, the outgoing edges of the dependency graph"
jsonpath "$.result.tools[1].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/rpc"
jsonpath "$.result.tools[1].inputSchema.type" == "object"
jsonpath "$.result.tools[1].annotations.title" == "Get Schema Dependencies"
jsonpath "$.result.tools[1].annotations.readOnlyHint" == true
jsonpath "$.result.tools[1].annotations.destructiveHint" == false
jsonpath "$.result.tools[1].annotations.idempotentHint" == true
jsonpath "$.result.tools[1].annotations.openWorldHint" == false
jsonpath "$.result.tools[2].name" == "get_schema_dependents"
jsonpath "$.result.tools[2].annotations.title" == "Get Schema Dependents"
jsonpath "$.result.tools[2].annotations.readOnlyHint" == true
jsonpath "$.result.tools[2].annotations.destructiveHint" == false
jsonpath "$.result.tools[2].annotations.idempotentHint" == true
jsonpath "$.result.tools[2].annotations.openWorldHint" == false
jsonpath "$.result.tools[3].name" == "get_schema_health"
jsonpath "$.result.tools[3].annotations.title" == "Get Schema Health"
jsonpath "$.result.tools[3].annotations.readOnlyHint" == true
jsonpath "$.result.tools[3].annotations.destructiveHint" == false
jsonpath "$.result.tools[3].annotations.idempotentHint" == true
jsonpath "$.result.tools[3].annotations.openWorldHint" == false
jsonpath "$.result.tools[4].name" == "get_schema_locations"
jsonpath "$.result.tools[4].annotations.title" == "Get Schema Locations"
jsonpath "$.result.tools[4].annotations.readOnlyHint" == true
jsonpath "$.result.tools[4].annotations.destructiveHint" == false
jsonpath "$.result.tools[4].annotations.idempotentHint" == true
jsonpath "$.result.tools[4].annotations.openWorldHint" == false
jsonpath "$.result.tools[5].name" == "get_schema_positions"
jsonpath "$.result.tools[5].annotations.title" == "Get Schema Positions"
jsonpath "$.result.tools[5].annotations.readOnlyHint" == true
jsonpath "$.result.tools[5].annotations.destructiveHint" == false
jsonpath "$.result.tools[5].annotations.idempotentHint" == true
jsonpath "$.result.tools[5].annotations.openWorldHint" == false
jsonpath "$.result.tools[6].name" == "get_schema_stats"
jsonpath "$.result.tools[6].annotations.title" == "Get Schema Stats"
jsonpath "$.result.tools[6].annotations.readOnlyHint" == true
jsonpath "$.result.tools[6].annotations.destructiveHint" == false
jsonpath "$.result.tools[6].annotations.idempotentHint" == true
jsonpath "$.result.tools[6].annotations.openWorldHint" == false
jsonpath "$.result.tools[7].name" == "get_schema_metadata"
jsonpath "$.result.tools[7].annotations.title" == "Get Schema Metadata"
jsonpath "$.result.tools[7].annotations.readOnlyHint" == true
jsonpath "$.result.tools[7].annotations.destructiveHint" == false
jsonpath "$.result.tools[7].annotations.idempotentHint" == true
jsonpath "$.result.tools[7].annotations.openWorldHint" == false
jsonpath "$.result.tools[8].name" == "evaluate_schema"
jsonpath "$.result.tools[8].annotations.title" == "Evaluate Schema"
jsonpath "$.result.tools[8].annotations.readOnlyHint" == true
jsonpath "$.result.tools[8].annotations.destructiveHint" == false
jsonpath "$.result.tools[8].annotations.idempotentHint" == true
jsonpath "$.result.tools[8].annotations.openWorldHint" == false
jsonpath "$.result.tools[9].name" == "trace_schema_evaluation"
jsonpath "$.result.tools[9].annotations.title" == "Trace Schema Evaluation"
jsonpath "$.result.tools[9].annotations.readOnlyHint" == true
jsonpath "$.result.tools[9].annotations.destructiveHint" == false
jsonpath "$.result.tools[9].annotations.idempotentHint" == true
jsonpath "$.result.tools[9].annotations.openWorldHint" == false
jsonpath "$.result.tools[10].name" == "search_schemas"
jsonpath "$.result.tools[10].annotations.title" == "Search Schemas"
jsonpath "$.result.tools[10].annotations.readOnlyHint" == true
jsonpath "$.result.tools[10].annotations.destructiveHint" == false
jsonpath "$.result.tools[10].annotations.idempotentHint" == true
jsonpath "$.result.tools[10].annotations.openWorldHint" == false
# Added in 2025-06-18 ("outputSchema" on Tool)
# https://modelcontextprotocol.io/specification/2025-06-18/changelog
jsonpath "$..outputSchema" not exists
# Added in 2025-06-18 ("title" on Tool, distinct from `annotations.title`,
# which was already present in 2025-03-26's ToolAnnotations)
# https://modelcontextprotocol.io/specification/2025-06-18/changelog
jsonpath "$.result.tools[0].title" not exists
jsonpath "$.result.tools[1].title" not exists
jsonpath "$.result.tools[2].title" not exists
jsonpath "$.result.tools[3].title" not exists
jsonpath "$.result.tools[4].title" not exists
jsonpath "$.result.tools[5].title" not exists
jsonpath "$.result.tools[6].title" not exists
jsonpath "$.result.tools[7].title" not exists
jsonpath "$.result.tools[8].title" not exists
jsonpath "$.result.tools[9].title" not exists
jsonpath "$.result.tools[10].title" not exists
# Added in 2025-11-25 (SEP-973)
# https://github.com/modelcontextprotocol/modelcontextprotocol/issues/973
jsonpath "$..icons" not exists
# Added in 2025-11-25 (SEP-1686)
# https://github.com/modelcontextprotocol/modelcontextprotocol/issues/1686
jsonpath "$..execution" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
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
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 6
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" isString
# Added in 2025-06-18 (structured tool output)
# https://modelcontextprotocol.io/specification/2025-06-18/changelog
jsonpath "$.result.structuredContent" not exists
# Added in 2025-06-18 (resource_link content blocks)
# https://modelcontextprotocol.io/specification/2025-06-18/changelog
jsonpath "$..uri" not exists
jsonpath "$..mimeType" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 7,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "example" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 7
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 6
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" isString
jsonpath "$.result.content[1].type" == "text"
jsonpath "$.result.content[1].text" == "/v1/catalog/example/bare\n{{base}}/v1/catalog/example/bare"
jsonpath "$.result.content[2].type" == "text"
jsonpath "$.result.content[2].text" == "/v1/catalog/example/description-only\n{{base}}/v1/catalog/example/description-only\nA schema with only a description and no title"
jsonpath "$.result.content[3].type" == "text"
jsonpath "$.result.content[3].text" == "Example With Both\n{{base}}/v1/catalog/example/full\nA schema that has both a title and a description"
jsonpath "$.result.content[4].type" == "text"
jsonpath "$.result.content[4].text" == "Example With Title Only\n{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.content[5].type" == "text"
jsonpath "$.result.content[5].text" matches "^Z{65532}\\.\\.\\.\nhttp://localhost:8000/v1/catalog/example/zzz-truncate-target$"
# Added in 2025-06-18 (resource_link content blocks). Every entry in
# `content[]` must be a `text` block instead.
# https://modelcontextprotocol.io/specification/2025-06-18/changelog
jsonpath "$.result.content[0].uri" not exists
jsonpath "$.result.content[0].mimeType" not exists
jsonpath "$.result.content[1].uri" not exists
jsonpath "$.result.content[1].mimeType" not exists
jsonpath "$.result.content[2].uri" not exists
jsonpath "$.result.content[2].mimeType" not exists
jsonpath "$.result.content[3].uri" not exists
jsonpath "$.result.content[3].mimeType" not exists
jsonpath "$.result.content[4].uri" not exists
jsonpath "$.result.content[4].mimeType" not exists
jsonpath "$.result.content[5].uri" not exists
jsonpath "$.result.content[5].mimeType" not exists
# Added in 2025-06-18 (structured tool output)
# https://modelcontextprotocol.io/specification/2025-06-18/changelog
jsonpath "$.result.structuredContent" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-03-26
Content-Type: application/json
```
[ { "jsonrpc": "2.0", "id": 1, "method": "ping" } ]
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == null
jsonpath "$.error.code" == 6
jsonpath "$.error.message" == "Unsupported operation"
jsonpath "$.error.data" == "Batch operations are not supported in this protocol version yet"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
