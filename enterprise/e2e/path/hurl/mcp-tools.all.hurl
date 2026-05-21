# tools/list enumerates every tool-eligible route registered in routes.bin

POST {{base}}/v1/catalog/self/v1/mcp
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
jsonpath "$.result.tools[0].description" == "Read a navigation artifact for browsing schemas"
jsonpath "$.result.tools[0].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/list/rpc"
jsonpath "$.result.tools[0].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/list/response"
jsonpath "$.result.tools[1].name" == "get_schema_dependencies"
jsonpath "$.result.tools[1].description" == "Look up a precomputed artifact about a specific schema by its absolute URI"
jsonpath "$.result.tools[1].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/rpc"
jsonpath "$.result.tools[1].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/response"
jsonpath "$.result.tools[2].name" == "get_schema_dependents"
jsonpath "$.result.tools[2].description" == "Look up a precomputed artifact about a specific schema by its absolute URI"
jsonpath "$.result.tools[2].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependents/rpc"
jsonpath "$.result.tools[2].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependents/response"
jsonpath "$.result.tools[3].name" == "get_schema_health"
jsonpath "$.result.tools[3].description" == "Look up a precomputed artifact about a specific schema by its absolute URI"
jsonpath "$.result.tools[3].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/health/rpc"
jsonpath "$.result.tools[3].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/health/response"
jsonpath "$.result.tools[4].name" == "get_schema_locations"
jsonpath "$.result.tools[4].description" == "Look up a precomputed artifact about a specific schema by its absolute URI"
jsonpath "$.result.tools[4].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/locations/rpc"
jsonpath "$.result.tools[4].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/locations/response"
jsonpath "$.result.tools[5].name" == "get_schema_positions"
jsonpath "$.result.tools[5].description" == "Look up a precomputed artifact about a specific schema by its absolute URI"
jsonpath "$.result.tools[5].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/positions/rpc"
jsonpath "$.result.tools[5].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/positions/response"
jsonpath "$.result.tools[6].name" == "get_schema_stats"
jsonpath "$.result.tools[6].description" == "Look up a precomputed artifact about a specific schema by its absolute URI"
jsonpath "$.result.tools[6].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/stats/rpc"
jsonpath "$.result.tools[6].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/stats/response"
jsonpath "$.result.tools[7].name" == "get_schema_metadata"
jsonpath "$.result.tools[7].description" == "Read a navigation artifact for browsing schemas"
jsonpath "$.result.tools[7].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/metadata/rpc"
jsonpath "$.result.tools[7].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/metadata/response"
jsonpath "$.result.tools[8].name" == "evaluate_schema"
jsonpath "$.result.tools[8].description" == "Validate a JSON instance against a schema and return whether it is valid plus any errors"
jsonpath "$.result.tools[8].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/rpc"
jsonpath "$.result.tools[8].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.tools[9].name" == "trace_schema_evaluation"
jsonpath "$.result.tools[9].description" == "Validate a JSON instance against a schema and return detailed information about every step of the evaluation process"
jsonpath "$.result.tools[9].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/rpc"
jsonpath "$.result.tools[9].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/response"
jsonpath "$.result.tools[10].name" == "search_schemas"
jsonpath "$.result.tools[10].description" == "Search for schemas by query term"
jsonpath "$.result.tools[10].inputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/rpc"
jsonpath "$.result.tools[10].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/response"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# tools/call list_directory at the catalog root

POST {{base}}/v1/catalog/self/v1/mcp
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

# tools/call with an unknown tool name

POST {{base}}/v1/catalog/self/v1/mcp
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

# tools/call with arguments that fail rpcSchema validation

POST {{base}}/v1/catalog/self/v1/mcp
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

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# initialize advertises capabilities.tools alongside capabilities.resources

POST {{base}}/v1/catalog/self/v1/mcp
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
