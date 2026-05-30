POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 10,
  "method": "resources/list"
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
jsonpath "$.id" == 10
jsonpath "$.result.resources" count == 50
jsonpath "$.result.resources[0].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.resources[0].name" == "Example With Both"
jsonpath "$.result.resources[0].description" == "A schema that has both a title and a description"
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.resources[0].size" == 242
jsonpath "$.result.resources[0].annotations.priority" == 0.5
jsonpath "$.result.resources[1].uri" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.resources[1].name" == "/v1/catalog/example/description-only"
jsonpath "$.result.resources[1].description" == "A schema with only a description and no title"
jsonpath "$.result.resources[1].mimeType" == "application/schema+json"
jsonpath "$.result.resources[1].size" == 219
jsonpath "$.result.resources[1].annotations.priority" == 0.5
jsonpath "$.result.resources[2].uri" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.resources[2].name" == "Example With Title Only"
jsonpath "$.result.resources[2].mimeType" == "application/schema+json"
jsonpath "$.result.resources[2].size" == 185
jsonpath "$.result.resources[2].annotations.priority" == 0.5
jsonpath "$.result.resources[3].uri" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.result.resources[3].name" matches "^Z{65532}\\.\\.\\.$"
jsonpath "$.result.resources[3].mimeType" == "application/schema+json"
jsonpath "$.result.resources[3].annotations.priority" == 0.5
jsonpath "$.result.resources[4].uri" == "{{base}}/v1/catalog/draft3/object"
jsonpath "$.result.resources[4].name" == "/v1/catalog/draft3/object"
jsonpath "$.result.resources[4].mimeType" == "application/schema+json"
jsonpath "$.result.resources[4].size" == 270
jsonpath "$.result.resources[4].annotations.priority" == 0.5
jsonpath "$.result.resources[5].uri" == "{{base}}/v1/catalog/draft3/string"
jsonpath "$.result.resources[5].name" == "/v1/catalog/draft3/string"
jsonpath "$.result.resources[5].mimeType" == "application/schema+json"
jsonpath "$.result.resources[5].size" == 136
jsonpath "$.result.resources[5].annotations.priority" == 0.5
jsonpath "$.result.resources[6].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.resources[6].name" == "/v1/catalog/example/bare"
jsonpath "$.result.resources[6].mimeType" == "application/schema+json"
jsonpath "$.result.resources[6].size" == 141
jsonpath "$.result.resources[6].annotations.priority" == 0.5
jsonpath "$.result.resources[7].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/error"
jsonpath "$.result.resources[7].name" == "Sourcemeta One API Error"
jsonpath "$.result.resources[7].description" == "The error response format returned by the Sourcemeta One API endpoints"
jsonpath "$.result.resources[7].mimeType" == "application/schema+json"
jsonpath "$.result.resources[7].size" == 1046
jsonpath "$.result.resources[7].annotations.priority" == 0
jsonpath "$.result.resources[8].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/list/response"
jsonpath "$.result.resources[8].name" == "Sourcemeta One List API Response"
jsonpath "$.result.resources[8].description" == "The response format for the directory listing API endpoint"
jsonpath "$.result.resources[8].mimeType" == "application/schema+json"
jsonpath "$.result.resources[8].size" == 3896
jsonpath "$.result.resources[8].annotations.priority" == 0
jsonpath "$.result.resources[9].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/list/rpc"
jsonpath "$.result.resources[9].name" == "Sourcemeta One Directory Listing RPC Input"
jsonpath "$.result.resources[9].description" == "List the contents of a directory in the catalog"
jsonpath "$.result.resources[9].mimeType" == "application/schema+json"
jsonpath "$.result.resources[9].size" == 535
jsonpath "$.result.resources[9].annotations.priority" == 0
jsonpath "$.result.resources[10].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/response"
jsonpath "$.result.resources[10].name" == "Sourcemeta One Schema Dependencies API Response"
jsonpath "$.result.resources[10].description" == "The response format for the schema dependencies API endpoint"
jsonpath "$.result.resources[10].mimeType" == "application/schema+json"
jsonpath "$.result.resources[10].size" == 1112
jsonpath "$.result.resources[10].annotations.priority" == 0
jsonpath "$.result.resources[11].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/rpc"
jsonpath "$.result.resources[11].name" == "Sourcemeta One Schema Dependencies RPC Input"
jsonpath "$.result.resources[11].description" == "Get all direct and indirect dependencies of a schema in the catalog"
jsonpath "$.result.resources[11].mimeType" == "application/schema+json"
jsonpath "$.result.resources[11].size" == 689
jsonpath "$.result.resources[11].annotations.priority" == 0
jsonpath "$.result.resources[12].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependents/response"
jsonpath "$.result.resources[12].name" == "Sourcemeta One Schema Dependents API Response"
jsonpath "$.result.resources[12].description" == "The response format for the schema dependents API endpoint"
jsonpath "$.result.resources[12].mimeType" == "application/schema+json"
jsonpath "$.result.resources[12].size" == 907
jsonpath "$.result.resources[12].annotations.priority" == 0
jsonpath "$.result.resources[13].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependents/rpc"
jsonpath "$.result.resources[13].name" == "Sourcemeta One Schema Dependents RPC Input"
jsonpath "$.result.resources[13].description" == "Get all direct and transitive dependents of a schema in the catalog"
jsonpath "$.result.resources[13].mimeType" == "application/schema+json"
jsonpath "$.result.resources[13].size" == 685
jsonpath "$.result.resources[13].annotations.priority" == 0
jsonpath "$.result.resources[14].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/request"
jsonpath "$.result.resources[14].name" == "Sourcemeta One Schema Evaluate API Request"
jsonpath "$.result.resources[14].description" == "The JSON instance to validate against a schema in the catalog"
jsonpath "$.result.resources[14].mimeType" == "application/schema+json"
jsonpath "$.result.resources[14].size" == 428
jsonpath "$.result.resources[14].annotations.priority" == 0
jsonpath "$.result.resources[15].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.resources[15].name" == "Sourcemeta One Schema Evaluate API Response"
jsonpath "$.result.resources[15].description" == "The response format for the schema evaluation API endpoint"
jsonpath "$.result.resources[15].mimeType" == "application/schema+json"
jsonpath "$.result.resources[15].size" == 3288
jsonpath "$.result.resources[15].annotations.priority" == 0
jsonpath "$.result.resources[16].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/rpc"
jsonpath "$.result.resources[16].name" == "Sourcemeta One Schema Evaluate RPC Input"
jsonpath "$.result.resources[16].description" == "Evaluate a JSON instance against a schema in the catalog and return whether it is valid plus any errors"
jsonpath "$.result.resources[16].mimeType" == "application/schema+json"
jsonpath "$.result.resources[16].size" == 1572
jsonpath "$.result.resources[16].annotations.priority" == 0
jsonpath "$.result.resources[17].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/health/response"
jsonpath "$.result.resources[17].name" == "Sourcemeta One Schema Health API Response"
jsonpath "$.result.resources[17].description" == "The response format for the schema health API endpoint"
jsonpath "$.result.resources[17].mimeType" == "application/schema+json"
jsonpath "$.result.resources[17].size" == 1633
jsonpath "$.result.resources[17].annotations.priority" == 0
jsonpath "$.result.resources[18].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/health/rpc"
jsonpath "$.result.resources[18].name" == "Sourcemeta One Schema Health RPC Input"
jsonpath "$.result.resources[18].description" == "Get the health analysis and score of a schema in the catalog"
jsonpath "$.result.resources[18].mimeType" == "application/schema+json"
jsonpath "$.result.resources[18].size" == 670
jsonpath "$.result.resources[18].annotations.priority" == 0
jsonpath "$.result.resources[19].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/locations/response"
jsonpath "$.result.resources[19].name" == "Sourcemeta One Schema Locations API Response"
jsonpath "$.result.resources[19].description" == "The response format for the schema frame locations API endpoint"
jsonpath "$.result.resources[19].mimeType" == "application/schema+json"
jsonpath "$.result.resources[19].size" == 2679
jsonpath "$.result.resources[19].annotations.priority" == 0
jsonpath "$.result.resources[20].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/locations/rpc"
jsonpath "$.result.resources[20].name" == "Sourcemeta One Schema Locations RPC Input"
jsonpath "$.result.resources[20].description" == "Get metadata about every URI associated with a schema, including schema resources, subschemas, and anchors"
jsonpath "$.result.resources[20].mimeType" == "application/schema+json"
jsonpath "$.result.resources[20].size" == 722
jsonpath "$.result.resources[20].annotations.priority" == 0
jsonpath "$.result.resources[21].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/metadata/response"
jsonpath "$.result.resources[21].name" == "Sourcemeta One Schema Metadata API Response"
jsonpath "$.result.resources[21].description" == "The response format for the schema metadata API endpoint"
jsonpath "$.result.resources[21].mimeType" == "application/schema+json"
jsonpath "$.result.resources[21].size" == 3041
jsonpath "$.result.resources[21].annotations.priority" == 0
jsonpath "$.result.resources[22].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/metadata/rpc"
jsonpath "$.result.resources[22].name" == "Sourcemeta One Schema Metadata RPC Input"
jsonpath "$.result.resources[22].description" == "Get metadata information about a schema in the catalog"
jsonpath "$.result.resources[22].mimeType" == "application/schema+json"
jsonpath "$.result.resources[22].size" == 668
jsonpath "$.result.resources[22].annotations.priority" == 0
jsonpath "$.result.resources[23].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/position"
jsonpath "$.result.resources[23].name" == "Sourcemeta One Schema Position"
jsonpath "$.result.resources[23].description" == "Represents a position in a JSON document as line start, column start, line end, and column end"
jsonpath "$.result.resources[23].mimeType" == "application/schema+json"
jsonpath "$.result.resources[23].size" == 457
jsonpath "$.result.resources[23].annotations.priority" == 0
jsonpath "$.result.resources[24].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/positions/response"
jsonpath "$.result.resources[24].name" == "Sourcemeta One Schema Positions API Response"
jsonpath "$.result.resources[24].description" == "The response format for the schema positions API endpoint"
jsonpath "$.result.resources[24].mimeType" == "application/schema+json"
jsonpath "$.result.resources[24].size" == 446
jsonpath "$.result.resources[24].annotations.priority" == 0
jsonpath "$.result.resources[25].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/positions/rpc"
jsonpath "$.result.resources[25].name" == "Sourcemeta One Schema Positions RPC Input"
jsonpath "$.result.resources[25].description" == "Get line and column position information for every token in a schema"
jsonpath "$.result.resources[25].mimeType" == "application/schema+json"
jsonpath "$.result.resources[25].size" == 684
jsonpath "$.result.resources[25].annotations.priority" == 0
jsonpath "$.result.resources[26].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/response"
jsonpath "$.result.resources[26].name" == "Sourcemeta One Schema Search API Response"
jsonpath "$.result.resources[26].description" == "The response format for the schema search API endpoint"
jsonpath "$.result.resources[26].mimeType" == "application/schema+json"
jsonpath "$.result.resources[26].size" == 1428
jsonpath "$.result.resources[26].annotations.priority" == 0
jsonpath "$.result.resources[27].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/rpc"
jsonpath "$.result.resources[27].name" == "Sourcemeta One Schema Search RPC Input"
jsonpath "$.result.resources[27].description" == "Search for schemas in the catalog by query term"
jsonpath "$.result.resources[27].mimeType" == "application/schema+json"
jsonpath "$.result.resources[27].size" == 2151
jsonpath "$.result.resources[27].annotations.priority" == 0
jsonpath "$.result.resources[28].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/stats/response"
jsonpath "$.result.resources[28].name" == "Sourcemeta One Schema Stats API Response"
jsonpath "$.result.resources[28].description" == "The response format for the schema statistics API endpoint"
jsonpath "$.result.resources[28].mimeType" == "application/schema+json"
jsonpath "$.result.resources[28].size" == 2356
jsonpath "$.result.resources[28].annotations.priority" == 0
jsonpath "$.result.resources[29].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/stats/rpc"
jsonpath "$.result.resources[29].name" == "Sourcemeta One Schema Statistics RPC Input"
jsonpath "$.result.resources[29].description" == "Get keyword statistics by vocabulary about every URI associated with a schema in the catalog"
jsonpath "$.result.resources[29].mimeType" == "application/schema+json"
jsonpath "$.result.resources[29].size" == 705
jsonpath "$.result.resources[29].annotations.priority" == 0
jsonpath "$.result.resources[30].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/request"
jsonpath "$.result.resources[30].name" == "Sourcemeta One Schema Trace API Request"
jsonpath "$.result.resources[30].description" == "The JSON instance to trace evaluation against a schema in the catalog"
jsonpath "$.result.resources[30].mimeType" == "application/schema+json"
jsonpath "$.result.resources[30].size" == 430
jsonpath "$.result.resources[30].annotations.priority" == 0
jsonpath "$.result.resources[31].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/response"
jsonpath "$.result.resources[31].name" == "Sourcemeta One Schema Trace API Response"
jsonpath "$.result.resources[31].description" == "The response format for the schema trace API endpoint"
jsonpath "$.result.resources[31].mimeType" == "application/schema+json"
jsonpath "$.result.resources[31].size" == 2281
jsonpath "$.result.resources[31].annotations.priority" == 0
jsonpath "$.result.resources[32].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/rpc"
jsonpath "$.result.resources[32].name" == "Sourcemeta One Schema Trace RPC Input"
jsonpath "$.result.resources[32].description" == "Trace step-by-step evaluation of a JSON instance against a schema in the catalog, exposing each step, rule application, and decision point"
jsonpath "$.result.resources[32].mimeType" == "application/schema+json"
jsonpath "$.result.resources[32].size" == 1730
jsonpath "$.result.resources[32].annotations.priority" == 0
jsonpath "$.result.resources[33].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.resources[33].name" == "Sourcemeta One MCP Error Response"
jsonpath "$.result.resources[33].description" == "The error response Sourcemeta One returns when an MCP request fails"
jsonpath "$.result.resources[33].mimeType" == "application/schema+json"
jsonpath "$.result.resources[33].size" == 10844
jsonpath "$.result.resources[33].annotations.priority" == 0
jsonpath "$.result.resources[34].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/request"
jsonpath "$.result.resources[34].name" == "Sourcemeta One MCP Initialize Request"
jsonpath "$.result.resources[34].description" == "Connection handshake from a client"
jsonpath "$.result.resources[34].mimeType" == "application/schema+json"
jsonpath "$.result.resources[34].size" == 4538
jsonpath "$.result.resources[34].annotations.priority" == 0
jsonpath "$.result.resources[35].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.resources[35].name" == "Sourcemeta One MCP Initialize Response"
jsonpath "$.result.resources[35].description" == "Sourcemeta One's response to a client's initialization"
jsonpath "$.result.resources[35].mimeType" == "application/schema+json"
jsonpath "$.result.resources[35].size" == 2493
jsonpath "$.result.resources[35].annotations.priority" == 0
jsonpath "$.result.resources[36].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/notifications/cancelled"
jsonpath "$.result.resources[36].name" == "Sourcemeta One MCP Cancelled Notification"
jsonpath "$.result.resources[36].description" == "Cancellation hint from a client for an in-flight request. Sourcemeta One accepts and discards these"
jsonpath "$.result.resources[36].mimeType" == "application/schema+json"
jsonpath "$.result.resources[36].size" == 1174
jsonpath "$.result.resources[36].annotations.priority" == 0
jsonpath "$.result.resources[37].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/notifications/initialized"
jsonpath "$.result.resources[37].name" == "Sourcemeta One MCP Initialized Notification"
jsonpath "$.result.resources[37].description" == "Client signal that the handshake is complete and normal operation may begin"
jsonpath "$.result.resources[37].mimeType" == "application/schema+json"
jsonpath "$.result.resources[37].size" == 751
jsonpath "$.result.resources[37].annotations.priority" == 0
jsonpath "$.result.resources[38].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/request"
jsonpath "$.result.resources[38].name" == "Sourcemeta One MCP Ping Request"
jsonpath "$.result.resources[38].description" == "Liveness check from a client"
jsonpath "$.result.resources[38].mimeType" == "application/schema+json"
jsonpath "$.result.resources[38].size" == 706
jsonpath "$.result.resources[38].annotations.priority" == 0
jsonpath "$.result.resources[39].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.resources[39].name" == "Sourcemeta One MCP Ping Response"
jsonpath "$.result.resources[39].description" == "Sourcemeta One's response to a liveness check"
jsonpath "$.result.resources[39].mimeType" == "application/schema+json"
jsonpath "$.result.resources[39].size" == 609
jsonpath "$.result.resources[39].annotations.priority" == 0
jsonpath "$.result.resources[40].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/request"
jsonpath "$.result.resources[40].name" == "Sourcemeta One MCP Request"
jsonpath "$.result.resources[40].description" == "Any incoming MCP request the server accepts"
jsonpath "$.result.resources[40].mimeType" == "application/schema+json"
jsonpath "$.result.resources[40].size" == 1784
jsonpath "$.result.resources[40].annotations.priority" == 0
jsonpath "$.result.resources[41].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/request"
jsonpath "$.result.resources[41].name" == "Sourcemeta One MCP Resources List Request"
jsonpath "$.result.resources[41].description" == "Request from a client to list MCP resources"
jsonpath "$.result.resources[41].mimeType" == "application/schema+json"
jsonpath "$.result.resources[41].size" == 1082
jsonpath "$.result.resources[41].annotations.priority" == 0
jsonpath "$.result.resources[42].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.resources[42].name" == "Sourcemeta One MCP Resources List Response"
jsonpath "$.result.resources[42].description" == "Sourcemeta One's list of MCP resources"
jsonpath "$.result.resources[42].mimeType" == "application/schema+json"
jsonpath "$.result.resources[42].size" == 3111
jsonpath "$.result.resources[42].annotations.priority" == 0
jsonpath "$.result.resources[43].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/request"
jsonpath "$.result.resources[43].name" == "Sourcemeta One MCP Resources Read Request"
jsonpath "$.result.resources[43].description" == "Request from a client to read an MCP resource"
jsonpath "$.result.resources[43].mimeType" == "application/schema+json"
jsonpath "$.result.resources[43].size" == 1342
jsonpath "$.result.resources[43].annotations.priority" == 0
jsonpath "$.result.resources[44].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.resources[44].name" == "Sourcemeta One MCP Resources Read Response"
jsonpath "$.result.resources[44].description" == "Sourcemeta One's response containing an MCP resource"
jsonpath "$.result.resources[44].mimeType" == "application/schema+json"
jsonpath "$.result.resources[44].size" == 1803
jsonpath "$.result.resources[44].annotations.priority" == 0
jsonpath "$.result.resources[45].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/request"
jsonpath "$.result.resources[45].name" == "Sourcemeta One MCP Resources Templates List Request"
jsonpath "$.result.resources[45].description" == "Request from a client to list MCP resource templates"
jsonpath "$.result.resources[45].mimeType" == "application/schema+json"
jsonpath "$.result.resources[45].size" == 933
jsonpath "$.result.resources[45].annotations.priority" == 0
jsonpath "$.result.resources[46].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.resources[46].name" == "Sourcemeta One MCP Resources Templates List Response"
jsonpath "$.result.resources[46].description" == "Sourcemeta One's list of MCP resource templates"
jsonpath "$.result.resources[46].mimeType" == "application/schema+json"
jsonpath "$.result.resources[46].size" == 2976
jsonpath "$.result.resources[46].annotations.priority" == 0
jsonpath "$.result.resources[47].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.resources[47].name" == "Sourcemeta One MCP Response"
jsonpath "$.result.resources[47].description" == "Any outgoing MCP response the server returns"
jsonpath "$.result.resources[47].mimeType" == "application/schema+json"
jsonpath "$.result.resources[47].size" == 4149
jsonpath "$.result.resources[47].annotations.priority" == 0
jsonpath "$.result.resources[48].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/request"
jsonpath "$.result.resources[48].name" == "Sourcemeta One MCP Tools Call Request"
jsonpath "$.result.resources[48].description" == "Request from a client to invoke an MCP tool by name"
jsonpath "$.result.resources[48].mimeType" == "application/schema+json"
jsonpath "$.result.resources[48].size" == 1172
jsonpath "$.result.resources[48].annotations.priority" == 0
jsonpath "$.result.resources[49].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.resources[49].name" == "Sourcemeta One MCP Tools Call Response"
jsonpath "$.result.resources[49].description" == "Sourcemeta One's response to a tool invocation"
jsonpath "$.result.resources[49].mimeType" == "application/schema+json"
jsonpath "$.result.resources[49].size" == 4717
jsonpath "$.result.resources[49].annotations.priority" == 0
jsonpath "$.result.nextCursor" == "50"

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
  "id": 12,
  "method": "resources/templates/list"
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
jsonpath "$.id" == 12
jsonpath "$.result.resourceTemplates" count == 1
jsonpath "$.result.resourceTemplates[0].uriTemplate" == "{{base}}/v1/catalog/{+path}{?bundle}"
jsonpath "$.result.resourceTemplates[0].name" == "JSON Schema"
jsonpath "$.result.resourceTemplates[0].description" == "A JSON Schema in this catalog, addressable by its canonical absolute URI under this server's origin. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Substitute `{+path}` with the schema's catalog path. Include `{?bundle}` (its presence alone triggers bundling, any value is ignored) to receive the schema with every external `$ref` inlined into a single self-contained document. The URI must not contain a fragment. Use `resources/list` to discover the available schemas rather than guessing paths"
jsonpath "$.result.resourceTemplates[0].mimeType" == "application/schema+json"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/schemas/mcp/error
HTTP 200
[Captures]
schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 30,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
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
jsonpath "$.id" == 30
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/schemas/mcp/error.json
HTTP 200
[Captures]
schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 31,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/error.json"
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
jsonpath "$.id" == 31
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error.json"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The `.json` suffix is stripped case-insensitively, so any mixed-case
# variant resolves to the same schema as the bare URI

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 33,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/error.JSON"
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
jsonpath "$.id" == 33
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error.JSON"
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
  "id": 34,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/error.Json"
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
jsonpath "$.id" == 34
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error.Json"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle=1
HTTP 200
[Captures]
bundled_schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 32,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle=1"
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
jsonpath "$.id" == 32
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle=1"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{bundled_schema_text}}"

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
  "id": 35,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle"
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
jsonpath "$.id" == 35
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{bundled_schema_text}}"

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
  "id": 36,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle="
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
jsonpath "$.id" == 36
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle="
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{bundled_schema_text}}"

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
  "id": 20,
  "method": "resources/read",
  "params": {
    "uri": "not a uri"
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
jsonpath "$.id" == 20
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "not a uri"

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
  "id": 21,
  "method": "resources/read",
  "params": {
    "uri": "https://elsewhere.example.com/foo"
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
jsonpath "$.id" == 21
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data.uri" == "https://elsewhere.example.com/foo"
jsonpath "$.error.data.reason" == "foreign-origin"
jsonpath "$.error.data.instanceOrigin" == "{{base}}/v1/catalog"
jsonpath "$.error.data.message" == "This URL lies outside this catalog's URL namespace. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Query the appropriate registry for foreign URLs"

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
  "id": 22,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/example/string"
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
jsonpath "$.id" == 22
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "{{base}}/example/string"

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
  "id": 40,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/response#/properties"
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
jsonpath "$.id" == 40
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response#/properties"
jsonpath "$.error.data.reason" == "fragment-present"
jsonpath "$.error.data.message" == "URIs passed to resources/read must not contain a fragment"

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
  "id": 41,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/response#"
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
jsonpath "$.id" == 41
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response#"
jsonpath "$.error.data.reason" == "fragment-present"
jsonpath "$.error.data.message" == "URIs passed to resources/read must not contain a fragment"

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
  "id": 50,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/response?foo=bar"
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
jsonpath "$.id" == 50
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response?foo=bar"
jsonpath "$.error.data.reason" == "unexpected-query-parameter"
jsonpath "$.error.data.message" == "The only query parameter accepted by resources/read is `bundle`"

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
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle&foo=bar"
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
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data.uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle&foo=bar"
jsonpath "$.error.data.reason" == "unexpected-query-parameter"
jsonpath "$.error.data.message" == "The only query parameter accepted by resources/read is `bundle`"

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
unbundled_response_text: body

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 42,
  "method": "resources/read",
  "params": {
    "uri": "HTTP://localhost:{{port}}/v1/catalog/self/v1/schemas/mcp/response"
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
jsonpath "$.id" == 42
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "HTTP://localhost:{{port}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{unbundled_response_text}}"

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
  "id": 43,
  "method": "resources/read",
  "params": {
    "uri": "http://LOCALHOST:{{port}}/v1/catalog/self/v1/schemas/mcp/response"
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
jsonpath "$.id" == 43
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "http://LOCALHOST:{{port}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{unbundled_response_text}}"

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
  "id": 44,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/SELF/V1/SCHEMAS/MCP/RESPONSE"
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
jsonpath "$.id" == 44
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/SELF/V1/SCHEMAS/MCP/RESPONSE"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{unbundled_response_text}}"

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
  "id": 100,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/../../etc/passwd"
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
jsonpath "$.id" == 100
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "{{base}}/v1/catalog/../../etc/passwd"

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
  "id": 101,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/spec/example/string"
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
jsonpath "$.id" == 101
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "{{base}}/spec/example/string"

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
  "id": 102,
  "method": "resources/read",
  "params": {
    "uri": "/etc/passwd"
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
jsonpath "$.id" == 102
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "/etc/passwd"

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
  "id": 23,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/does/not/exist"
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
jsonpath "$.id" == 23
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "{{base}}/v1/catalog/does/not/exist"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
