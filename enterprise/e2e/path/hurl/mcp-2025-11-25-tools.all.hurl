POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 50,
  "method": "tools/list"
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
jsonpath "$.id" == 50
jsonpath "$.result.tools" count == 11
jsonpath "$.result.tools[0].name" == "list_directory"
jsonpath "$.result.tools[0].description" == "List the contents of a directory in the catalog"
jsonpath "$.result.tools[0].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/list/rpc"
jsonpath "$.result.tools[0].inputSchema.type" == "object"
jsonpath "$.result.tools[0].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/list/response"
jsonpath "$.result.tools[0].outputSchema.type" == "object"
jsonpath "$.result.tools[0].annotations.title" == "List Directory"
jsonpath "$.result.tools[0].annotations.readOnlyHint" == true
jsonpath "$.result.tools[0].annotations.destructiveHint" == false
jsonpath "$.result.tools[0].annotations.idempotentHint" == true
jsonpath "$.result.tools[0].annotations.openWorldHint" == false
jsonpath "$.result.tools[1].name" == "get_schema_dependencies"
jsonpath "$.result.tools[1].description" == "List the schemas this schema directly or transitively references, the outgoing edges of the dependency graph"
jsonpath "$.result.tools[1].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/rpc"
jsonpath "$.result.tools[1].inputSchema.type" == "object"
jsonpath "$.result.tools[1].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/response"
jsonpath "$.result.tools[1].outputSchema.type" == "object"
jsonpath "$.result.tools[1].annotations.title" == "Get Schema Dependencies"
jsonpath "$.result.tools[1].annotations.readOnlyHint" == true
jsonpath "$.result.tools[1].annotations.destructiveHint" == false
jsonpath "$.result.tools[1].annotations.idempotentHint" == true
jsonpath "$.result.tools[1].annotations.openWorldHint" == false
jsonpath "$.result.tools[2].name" == "get_schema_dependents"
jsonpath "$.result.tools[2].description" == "List the schemas that directly or transitively reference this schema, the incoming edges of the dependency graph"
jsonpath "$.result.tools[2].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependents/rpc"
jsonpath "$.result.tools[2].inputSchema.type" == "object"
jsonpath "$.result.tools[2].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependents/response"
jsonpath "$.result.tools[2].outputSchema.type" == "object"
jsonpath "$.result.tools[2].annotations.title" == "Get Schema Dependents"
jsonpath "$.result.tools[2].annotations.readOnlyHint" == true
jsonpath "$.result.tools[2].annotations.destructiveHint" == false
jsonpath "$.result.tools[2].annotations.idempotentHint" == true
jsonpath "$.result.tools[2].annotations.openWorldHint" == false
jsonpath "$.result.tools[3].name" == "get_schema_health"
jsonpath "$.result.tools[3].description" == "Return the lint health score (0 to 100) and any lint findings for a schema in the catalog"
jsonpath "$.result.tools[3].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/health/rpc"
jsonpath "$.result.tools[3].inputSchema.type" == "object"
jsonpath "$.result.tools[3].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/health/response"
jsonpath "$.result.tools[3].outputSchema.type" == "object"
jsonpath "$.result.tools[3].annotations.title" == "Get Schema Health"
jsonpath "$.result.tools[3].annotations.readOnlyHint" == true
jsonpath "$.result.tools[3].annotations.destructiveHint" == false
jsonpath "$.result.tools[3].annotations.idempotentHint" == true
jsonpath "$.result.tools[3].annotations.openWorldHint" == false
jsonpath "$.result.tools[4].name" == "get_schema_locations"
jsonpath "$.result.tools[4].description" == "Return every URI exposed by a schema, including subschemas and anchors, with their parent and base URI relationships. The response carries per-pointer metadata and grows with the schema's pointer count, so substantial payloads are expected for large schemas"
jsonpath "$.result.tools[4].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/locations/rpc"
jsonpath "$.result.tools[4].inputSchema.type" == "object"
jsonpath "$.result.tools[4].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/locations/response"
jsonpath "$.result.tools[4].outputSchema.type" == "object"
jsonpath "$.result.tools[4].annotations.title" == "Get Schema Locations"
jsonpath "$.result.tools[4].annotations.readOnlyHint" == true
jsonpath "$.result.tools[4].annotations.destructiveHint" == false
jsonpath "$.result.tools[4].annotations.idempotentHint" == true
jsonpath "$.result.tools[4].annotations.openWorldHint" == false
jsonpath "$.result.tools[5].name" == "get_schema_positions"
jsonpath "$.result.tools[5].description" == "Return the source line and column position of every JSON Pointer inside a schema"
jsonpath "$.result.tools[5].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/positions/rpc"
jsonpath "$.result.tools[5].inputSchema.type" == "object"
jsonpath "$.result.tools[5].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/positions/response"
jsonpath "$.result.tools[5].outputSchema.type" == "object"
jsonpath "$.result.tools[5].annotations.title" == "Get Schema Positions"
jsonpath "$.result.tools[5].annotations.readOnlyHint" == true
jsonpath "$.result.tools[5].annotations.destructiveHint" == false
jsonpath "$.result.tools[5].annotations.idempotentHint" == true
jsonpath "$.result.tools[5].annotations.openWorldHint" == false
jsonpath "$.result.tools[6].name" == "get_schema_stats"
jsonpath "$.result.tools[6].description" == "Return per-vocabulary keyword usage statistics for a schema in the catalog, identifying which JSON Schema keywords the schema relies on and how often each appears"
jsonpath "$.result.tools[6].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/stats/rpc"
jsonpath "$.result.tools[6].inputSchema.type" == "object"
jsonpath "$.result.tools[6].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/stats/response"
jsonpath "$.result.tools[6].outputSchema.type" == "object"
jsonpath "$.result.tools[6].annotations.title" == "Get Schema Stats"
jsonpath "$.result.tools[6].annotations.readOnlyHint" == true
jsonpath "$.result.tools[6].annotations.destructiveHint" == false
jsonpath "$.result.tools[6].annotations.idempotentHint" == true
jsonpath "$.result.tools[6].annotations.openWorldHint" == false
jsonpath "$.result.tools[7].name" == "get_schema_metadata"
jsonpath "$.result.tools[7].description" == "Return navigation metadata for a schema in the catalog, including title, description, dialect, byte size, dependency count, and breadcrumb path"
jsonpath "$.result.tools[7].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/metadata/rpc"
jsonpath "$.result.tools[7].inputSchema.type" == "object"
jsonpath "$.result.tools[7].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/metadata/response"
jsonpath "$.result.tools[7].outputSchema.type" == "object"
jsonpath "$.result.tools[7].annotations.title" == "Get Schema Metadata"
jsonpath "$.result.tools[7].annotations.readOnlyHint" == true
jsonpath "$.result.tools[7].annotations.destructiveHint" == false
jsonpath "$.result.tools[7].annotations.idempotentHint" == true
jsonpath "$.result.tools[7].annotations.openWorldHint" == false
jsonpath "$.result.tools[8].name" == "evaluate_schema"
jsonpath "$.result.tools[8].description" == "Validate a JSON instance against a schema and return whether it is valid plus any errors"
jsonpath "$.result.tools[8].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/rpc"
jsonpath "$.result.tools[8].inputSchema.type" == "object"
jsonpath "$.result.tools[8].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.tools[8].outputSchema.type" == "object"
jsonpath "$.result.tools[8].annotations.title" == "Evaluate Schema"
jsonpath "$.result.tools[8].annotations.readOnlyHint" == true
jsonpath "$.result.tools[8].annotations.destructiveHint" == false
jsonpath "$.result.tools[8].annotations.idempotentHint" == true
jsonpath "$.result.tools[8].annotations.openWorldHint" == false
jsonpath "$.result.tools[9].name" == "trace_schema_evaluation"
jsonpath "$.result.tools[9].description" == "Validate a JSON instance against a schema and return a step-by-step trace of the evaluation. The trace can be substantial, so prefer non-tracing evaluation when only a valid or invalid verdict is needed and use this when per-step detail is required"
jsonpath "$.result.tools[9].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/rpc"
jsonpath "$.result.tools[9].inputSchema.type" == "object"
jsonpath "$.result.tools[9].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/response"
jsonpath "$.result.tools[9].outputSchema.type" == "object"
jsonpath "$.result.tools[9].annotations.title" == "Trace Schema Evaluation"
jsonpath "$.result.tools[9].annotations.readOnlyHint" == true
jsonpath "$.result.tools[9].annotations.destructiveHint" == false
jsonpath "$.result.tools[9].annotations.idempotentHint" == true
jsonpath "$.result.tools[9].annotations.openWorldHint" == false
jsonpath "$.result.tools[10].name" == "search_schemas"
jsonpath "$.result.tools[10].description" == "Search for schemas by query term"
jsonpath "$.result.tools[10].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/rpc"
jsonpath "$.result.tools[10].inputSchema.type" == "object"
jsonpath "$.result.tools[10].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/response"
jsonpath "$.result.tools[10].outputSchema.type" == "object"
jsonpath "$.result.tools[10].annotations.title" == "Search Schemas"
jsonpath "$.result.tools[10].annotations.readOnlyHint" == true
jsonpath "$.result.tools[10].annotations.destructiveHint" == false
jsonpath "$.result.tools[10].annotations.idempotentHint" == true
jsonpath "$.result.tools[10].annotations.openWorldHint" == false

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
  "id": 51,
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
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 51
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.path" == "/v1/catalog/"
jsonpath "$.result.structuredContent.url" == "{{base}}/v1/catalog"
jsonpath "$.result.structuredContent.entries" count >= 1

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
  "id": 52,
  "method": "tools/call",
  "params": {
    "name": "no_such_tool",
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
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 52
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data" == "no_such_tool"

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
  "id": 53,
  "method": "tools/call",
  "params": {
    "name": "evaluate_schema",
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
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 53
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/rpc#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The object value was expected to only define properties \"schema\", and \"stringifiedInstance\""

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
  "id": 54,
  "method": "initialize",
  "params": {
    "protocolVersion": "2025-11-25",
    "capabilities": {},
    "clientInfo": { "name": "hurl", "version": "1.0.0" }
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
jsonpath "$.id" == 54
jsonpath "$.result.capabilities.resources" isObject
jsonpath "$.result.capabilities.resources" isEmpty
jsonpath "$.result.capabilities.tools" isObject
jsonpath "$.result.capabilities.tools" isEmpty

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
