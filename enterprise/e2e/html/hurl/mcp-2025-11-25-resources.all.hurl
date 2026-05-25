POST {{base}}/self/v1/mcp
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
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 10
jsonpath "$.result.resources" count == 50
jsonpath "$.result.resources[0].uri" == "{{base}}/self/v1/schemas/api/error"
jsonpath "$.result.resources[0].name" == "error"
jsonpath "$.result.resources[0].description" == "The error response format returned by the Sourcemeta One API endpoints"
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.resources[0].size" == 1035
jsonpath "$.result.resources[0].annotations.priority" == 0
jsonpath "$.result.resources[1].uri" == "{{base}}/self/v1/schemas/api/list/response"
jsonpath "$.result.resources[1].name" == "response"
jsonpath "$.result.resources[1].description" == "The response format for the directory listing API endpoint"
jsonpath "$.result.resources[1].mimeType" == "application/schema+json"
jsonpath "$.result.resources[1].size" == 3885
jsonpath "$.result.resources[1].annotations.priority" == 0
jsonpath "$.result.resources[2].uri" == "{{base}}/self/v1/schemas/api/list/rpc"
jsonpath "$.result.resources[2].name" == "rpc"
jsonpath "$.result.resources[2].description" == "List the contents of a directory in the catalog"
jsonpath "$.result.resources[2].mimeType" == "application/schema+json"
jsonpath "$.result.resources[2].size" == 524
jsonpath "$.result.resources[2].annotations.priority" == 0
jsonpath "$.result.resources[3].uri" == "{{base}}/self/v1/schemas/api/schemas/dependencies/response"
jsonpath "$.result.resources[3].name" == "response"
jsonpath "$.result.resources[3].description" == "The response format for the schema dependencies API endpoint"
jsonpath "$.result.resources[3].mimeType" == "application/schema+json"
jsonpath "$.result.resources[3].size" == 1101
jsonpath "$.result.resources[3].annotations.priority" == 0
jsonpath "$.result.resources[4].uri" == "{{base}}/self/v1/schemas/api/schemas/dependencies/rpc"
jsonpath "$.result.resources[4].name" == "rpc"
jsonpath "$.result.resources[4].description" == "Get all direct and indirect dependencies of a schema in the catalog"
jsonpath "$.result.resources[4].mimeType" == "application/schema+json"
jsonpath "$.result.resources[4].size" == 678
jsonpath "$.result.resources[4].annotations.priority" == 0
jsonpath "$.result.resources[5].uri" == "{{base}}/self/v1/schemas/api/schemas/dependents/response"
jsonpath "$.result.resources[5].name" == "response"
jsonpath "$.result.resources[5].description" == "The response format for the schema dependents API endpoint"
jsonpath "$.result.resources[5].mimeType" == "application/schema+json"
jsonpath "$.result.resources[5].size" == 896
jsonpath "$.result.resources[5].annotations.priority" == 0
jsonpath "$.result.resources[6].uri" == "{{base}}/self/v1/schemas/api/schemas/dependents/rpc"
jsonpath "$.result.resources[6].name" == "rpc"
jsonpath "$.result.resources[6].description" == "Get all direct and transitive dependents of a schema in the catalog"
jsonpath "$.result.resources[6].mimeType" == "application/schema+json"
jsonpath "$.result.resources[6].size" == 674
jsonpath "$.result.resources[6].annotations.priority" == 0
jsonpath "$.result.resources[7].uri" == "{{base}}/self/v1/schemas/api/schemas/evaluate/request"
jsonpath "$.result.resources[7].name" == "request"
jsonpath "$.result.resources[7].description" == "The JSON instance to validate against a schema in the catalog"
jsonpath "$.result.resources[7].mimeType" == "application/schema+json"
jsonpath "$.result.resources[7].size" == 417
jsonpath "$.result.resources[7].annotations.priority" == 0
jsonpath "$.result.resources[8].uri" == "{{base}}/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.resources[8].name" == "response"
jsonpath "$.result.resources[8].description" == "The response format for the schema evaluation API endpoint"
jsonpath "$.result.resources[8].mimeType" == "application/schema+json"
jsonpath "$.result.resources[8].size" == 3277
jsonpath "$.result.resources[8].annotations.priority" == 0
jsonpath "$.result.resources[9].uri" == "{{base}}/self/v1/schemas/api/schemas/evaluate/rpc"
jsonpath "$.result.resources[9].name" == "rpc"
jsonpath "$.result.resources[9].description" == "Evaluate a JSON instance against a schema in the catalog and return whether it is valid plus any errors"
jsonpath "$.result.resources[9].mimeType" == "application/schema+json"
jsonpath "$.result.resources[9].size" == 1561
jsonpath "$.result.resources[9].annotations.priority" == 0
jsonpath "$.result.resources[10].uri" == "{{base}}/self/v1/schemas/api/schemas/health/response"
jsonpath "$.result.resources[10].name" == "response"
jsonpath "$.result.resources[10].description" == "The response format for the schema health API endpoint"
jsonpath "$.result.resources[10].mimeType" == "application/schema+json"
jsonpath "$.result.resources[10].size" == 1622
jsonpath "$.result.resources[10].annotations.priority" == 0
jsonpath "$.result.resources[11].uri" == "{{base}}/self/v1/schemas/api/schemas/health/rpc"
jsonpath "$.result.resources[11].name" == "rpc"
jsonpath "$.result.resources[11].description" == "Get the health analysis and score of a schema in the catalog"
jsonpath "$.result.resources[11].mimeType" == "application/schema+json"
jsonpath "$.result.resources[11].size" == 659
jsonpath "$.result.resources[11].annotations.priority" == 0
jsonpath "$.result.resources[12].uri" == "{{base}}/self/v1/schemas/api/schemas/locations/response"
jsonpath "$.result.resources[12].name" == "response"
jsonpath "$.result.resources[12].description" == "The response format for the schema frame locations API endpoint"
jsonpath "$.result.resources[12].mimeType" == "application/schema+json"
jsonpath "$.result.resources[12].size" == 2668
jsonpath "$.result.resources[12].annotations.priority" == 0
jsonpath "$.result.resources[13].uri" == "{{base}}/self/v1/schemas/api/schemas/locations/rpc"
jsonpath "$.result.resources[13].name" == "rpc"
jsonpath "$.result.resources[13].description" == "Get metadata about every URI associated with a schema, including schema resources, subschemas, and anchors"
jsonpath "$.result.resources[13].mimeType" == "application/schema+json"
jsonpath "$.result.resources[13].size" == 711
jsonpath "$.result.resources[13].annotations.priority" == 0
jsonpath "$.result.resources[14].uri" == "{{base}}/self/v1/schemas/api/schemas/metadata/response"
jsonpath "$.result.resources[14].name" == "response"
jsonpath "$.result.resources[14].description" == "The response format for the schema metadata API endpoint"
jsonpath "$.result.resources[14].mimeType" == "application/schema+json"
jsonpath "$.result.resources[14].size" == 2876
jsonpath "$.result.resources[14].annotations.priority" == 0
jsonpath "$.result.resources[15].uri" == "{{base}}/self/v1/schemas/api/schemas/metadata/rpc"
jsonpath "$.result.resources[15].name" == "rpc"
jsonpath "$.result.resources[15].description" == "Get metadata information about a schema in the catalog"
jsonpath "$.result.resources[15].mimeType" == "application/schema+json"
jsonpath "$.result.resources[15].size" == 657
jsonpath "$.result.resources[15].annotations.priority" == 0
jsonpath "$.result.resources[16].uri" == "{{base}}/self/v1/schemas/api/schemas/position"
jsonpath "$.result.resources[16].name" == "position"
jsonpath "$.result.resources[16].description" == "Represents a position in a JSON document as line start, column start, line end, and column end"
jsonpath "$.result.resources[16].mimeType" == "application/schema+json"
jsonpath "$.result.resources[16].size" == 446
jsonpath "$.result.resources[16].annotations.priority" == 0
jsonpath "$.result.resources[17].uri" == "{{base}}/self/v1/schemas/api/schemas/positions/response"
jsonpath "$.result.resources[17].name" == "response"
jsonpath "$.result.resources[17].description" == "The response format for the schema positions API endpoint"
jsonpath "$.result.resources[17].mimeType" == "application/schema+json"
jsonpath "$.result.resources[17].size" == 435
jsonpath "$.result.resources[17].annotations.priority" == 0
jsonpath "$.result.resources[18].uri" == "{{base}}/self/v1/schemas/api/schemas/positions/rpc"
jsonpath "$.result.resources[18].name" == "rpc"
jsonpath "$.result.resources[18].description" == "Get line and column position information for every token in a schema"
jsonpath "$.result.resources[18].mimeType" == "application/schema+json"
jsonpath "$.result.resources[18].size" == 673
jsonpath "$.result.resources[18].annotations.priority" == 0
jsonpath "$.result.resources[19].uri" == "{{base}}/self/v1/schemas/api/schemas/search/response"
jsonpath "$.result.resources[19].name" == "response"
jsonpath "$.result.resources[19].description" == "The response format for the schema search API endpoint"
jsonpath "$.result.resources[19].mimeType" == "application/schema+json"
jsonpath "$.result.resources[19].size" == 1108
jsonpath "$.result.resources[19].annotations.priority" == 0
jsonpath "$.result.resources[20].uri" == "{{base}}/self/v1/schemas/api/schemas/search/rpc"
jsonpath "$.result.resources[20].name" == "rpc"
jsonpath "$.result.resources[20].description" == "Search for schemas in the catalog by query term"
jsonpath "$.result.resources[20].mimeType" == "application/schema+json"
jsonpath "$.result.resources[20].size" == 1166
jsonpath "$.result.resources[20].annotations.priority" == 0
jsonpath "$.result.resources[21].uri" == "{{base}}/self/v1/schemas/api/schemas/stats/response"
jsonpath "$.result.resources[21].name" == "response"
jsonpath "$.result.resources[21].description" == "The response format for the schema statistics API endpoint"
jsonpath "$.result.resources[21].mimeType" == "application/schema+json"
jsonpath "$.result.resources[21].size" == 2345
jsonpath "$.result.resources[21].annotations.priority" == 0
jsonpath "$.result.resources[22].uri" == "{{base}}/self/v1/schemas/api/schemas/stats/rpc"
jsonpath "$.result.resources[22].name" == "rpc"
jsonpath "$.result.resources[22].description" == "Get keyword statistics by vocabulary about every URI associated with a schema in the catalog"
jsonpath "$.result.resources[22].mimeType" == "application/schema+json"
jsonpath "$.result.resources[22].size" == 694
jsonpath "$.result.resources[22].annotations.priority" == 0
jsonpath "$.result.resources[23].uri" == "{{base}}/self/v1/schemas/api/schemas/trace/request"
jsonpath "$.result.resources[23].name" == "request"
jsonpath "$.result.resources[23].description" == "The JSON instance to trace evaluation against a schema in the catalog"
jsonpath "$.result.resources[23].mimeType" == "application/schema+json"
jsonpath "$.result.resources[23].size" == 419
jsonpath "$.result.resources[23].annotations.priority" == 0
jsonpath "$.result.resources[24].uri" == "{{base}}/self/v1/schemas/api/schemas/trace/response"
jsonpath "$.result.resources[24].name" == "response"
jsonpath "$.result.resources[24].description" == "The response format for the schema trace API endpoint"
jsonpath "$.result.resources[24].mimeType" == "application/schema+json"
jsonpath "$.result.resources[24].size" == 2270
jsonpath "$.result.resources[24].annotations.priority" == 0
jsonpath "$.result.resources[25].uri" == "{{base}}/self/v1/schemas/api/schemas/trace/rpc"
jsonpath "$.result.resources[25].name" == "rpc"
jsonpath "$.result.resources[25].description" == "Trace step-by-step evaluation of a JSON instance against a schema in the catalog, exposing each step, rule application, and decision point"
jsonpath "$.result.resources[25].mimeType" == "application/schema+json"
jsonpath "$.result.resources[25].size" == 1719
jsonpath "$.result.resources[25].annotations.priority" == 0
jsonpath "$.result.resources[26].uri" == "{{base}}/self/v1/schemas/mcp/error"
jsonpath "$.result.resources[26].name" == "error"
jsonpath "$.result.resources[26].description" == "The error response Sourcemeta One returns when an MCP request fails"
jsonpath "$.result.resources[26].mimeType" == "application/schema+json"
jsonpath "$.result.resources[26].size" == 6633
jsonpath "$.result.resources[26].annotations.priority" == 0
jsonpath "$.result.resources[27].uri" == "{{base}}/self/v1/schemas/mcp/initialize/request"
jsonpath "$.result.resources[27].name" == "request"
jsonpath "$.result.resources[27].description" == "Connection handshake from a client"
jsonpath "$.result.resources[27].mimeType" == "application/schema+json"
jsonpath "$.result.resources[27].size" == 4527
jsonpath "$.result.resources[27].annotations.priority" == 0
jsonpath "$.result.resources[28].uri" == "{{base}}/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.resources[28].name" == "response"
jsonpath "$.result.resources[28].description" == "Sourcemeta One's response to a client's initialization"
jsonpath "$.result.resources[28].mimeType" == "application/schema+json"
jsonpath "$.result.resources[28].size" == 2482
jsonpath "$.result.resources[28].annotations.priority" == 0
jsonpath "$.result.resources[29].uri" == "{{base}}/self/v1/schemas/mcp/notifications/cancelled"
jsonpath "$.result.resources[29].name" == "cancelled"
jsonpath "$.result.resources[29].description" == "Cancellation hint from a client for an in-flight request. Sourcemeta One accepts and discards these"
jsonpath "$.result.resources[29].mimeType" == "application/schema+json"
jsonpath "$.result.resources[29].size" == 1163
jsonpath "$.result.resources[29].annotations.priority" == 0
jsonpath "$.result.resources[30].uri" == "{{base}}/self/v1/schemas/mcp/notifications/initialized"
jsonpath "$.result.resources[30].name" == "initialized"
jsonpath "$.result.resources[30].description" == "Client signal that the handshake is complete and normal operation may begin"
jsonpath "$.result.resources[30].mimeType" == "application/schema+json"
jsonpath "$.result.resources[30].size" == 740
jsonpath "$.result.resources[30].annotations.priority" == 0
jsonpath "$.result.resources[31].uri" == "{{base}}/self/v1/schemas/mcp/ping/request"
jsonpath "$.result.resources[31].name" == "request"
jsonpath "$.result.resources[31].description" == "Liveness check from a client"
jsonpath "$.result.resources[31].mimeType" == "application/schema+json"
jsonpath "$.result.resources[31].size" == 695
jsonpath "$.result.resources[31].annotations.priority" == 0
jsonpath "$.result.resources[32].uri" == "{{base}}/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.resources[32].name" == "response"
jsonpath "$.result.resources[32].description" == "Sourcemeta One's response to a liveness check"
jsonpath "$.result.resources[32].mimeType" == "application/schema+json"
jsonpath "$.result.resources[32].size" == 598
jsonpath "$.result.resources[32].annotations.priority" == 0
jsonpath "$.result.resources[33].uri" == "{{base}}/self/v1/schemas/mcp/request"
jsonpath "$.result.resources[33].name" == "request"
jsonpath "$.result.resources[33].description" == "Any incoming MCP request the server accepts"
jsonpath "$.result.resources[33].mimeType" == "application/schema+json"
jsonpath "$.result.resources[33].size" == 1773
jsonpath "$.result.resources[33].annotations.priority" == 0
jsonpath "$.result.resources[34].uri" == "{{base}}/self/v1/schemas/mcp/resources/list/request"
jsonpath "$.result.resources[34].name" == "request"
jsonpath "$.result.resources[34].description" == "Request from a client to list MCP resources"
jsonpath "$.result.resources[34].mimeType" == "application/schema+json"
jsonpath "$.result.resources[34].size" == 1071
jsonpath "$.result.resources[34].annotations.priority" == 0
jsonpath "$.result.resources[35].uri" == "{{base}}/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.resources[35].name" == "response"
jsonpath "$.result.resources[35].description" == "Sourcemeta One's list of MCP resources"
jsonpath "$.result.resources[35].mimeType" == "application/schema+json"
jsonpath "$.result.resources[35].size" == 3015
jsonpath "$.result.resources[35].annotations.priority" == 0
jsonpath "$.result.resources[36].uri" == "{{base}}/self/v1/schemas/mcp/resources/read/request"
jsonpath "$.result.resources[36].name" == "request"
jsonpath "$.result.resources[36].description" == "Request from a client to read an MCP resource"
jsonpath "$.result.resources[36].mimeType" == "application/schema+json"
jsonpath "$.result.resources[36].size" == 1331
jsonpath "$.result.resources[36].annotations.priority" == 0
jsonpath "$.result.resources[37].uri" == "{{base}}/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.resources[37].name" == "response"
jsonpath "$.result.resources[37].description" == "Sourcemeta One's response containing an MCP resource"
jsonpath "$.result.resources[37].mimeType" == "application/schema+json"
jsonpath "$.result.resources[37].size" == 1792
jsonpath "$.result.resources[37].annotations.priority" == 0
jsonpath "$.result.resources[38].uri" == "{{base}}/self/v1/schemas/mcp/resources/templates/list/request"
jsonpath "$.result.resources[38].name" == "request"
jsonpath "$.result.resources[38].description" == "Request from a client to list MCP resource templates"
jsonpath "$.result.resources[38].mimeType" == "application/schema+json"
jsonpath "$.result.resources[38].size" == 922
jsonpath "$.result.resources[38].annotations.priority" == 0
jsonpath "$.result.resources[39].uri" == "{{base}}/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.resources[39].name" == "response"
jsonpath "$.result.resources[39].description" == "Sourcemeta One's list of MCP resource templates"
jsonpath "$.result.resources[39].mimeType" == "application/schema+json"
jsonpath "$.result.resources[39].size" == 1915
jsonpath "$.result.resources[39].annotations.priority" == 0
jsonpath "$.result.resources[40].uri" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.resources[40].name" == "response"
jsonpath "$.result.resources[40].description" == "Any outgoing MCP response the server returns"
jsonpath "$.result.resources[40].mimeType" == "application/schema+json"
jsonpath "$.result.resources[40].size" == 3279
jsonpath "$.result.resources[40].annotations.priority" == 0
jsonpath "$.result.resources[41].uri" == "{{base}}/self/v1/schemas/mcp/tools/call/request"
jsonpath "$.result.resources[41].name" == "request"
jsonpath "$.result.resources[41].description" == "Request from a client to invoke an MCP tool by name"
jsonpath "$.result.resources[41].mimeType" == "application/schema+json"
jsonpath "$.result.resources[41].size" == 1161
jsonpath "$.result.resources[41].annotations.priority" == 0
jsonpath "$.result.resources[42].uri" == "{{base}}/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.resources[42].name" == "response"
jsonpath "$.result.resources[42].description" == "Sourcemeta One's response to a tool invocation"
jsonpath "$.result.resources[42].mimeType" == "application/schema+json"
jsonpath "$.result.resources[42].size" == 4706
jsonpath "$.result.resources[42].annotations.priority" == 0
jsonpath "$.result.resources[43].uri" == "{{base}}/self/v1/schemas/mcp/tools/list/request"
jsonpath "$.result.resources[43].name" == "request"
jsonpath "$.result.resources[43].description" == "Request from a client to list MCP tools"
jsonpath "$.result.resources[43].mimeType" == "application/schema+json"
jsonpath "$.result.resources[43].size" == 853
jsonpath "$.result.resources[43].annotations.priority" == 0
jsonpath "$.result.resources[44].uri" == "{{base}}/twins/foo"
jsonpath "$.result.resources[44].name" == "foo"
jsonpath "$.result.resources[44].description" == "A schema whose base name collides with a sibling directory"
jsonpath "$.result.resources[44].mimeType" == "application/schema+json"
jsonpath "$.result.resources[44].size" == 263
jsonpath "$.result.resources[44].annotations.priority" == 1
jsonpath "$.result.resources[45].uri" == "{{base}}/twins/foo/inner"
jsonpath "$.result.resources[45].name" == "inner"
jsonpath "$.result.resources[45].description" == "A schema inside the foo directory whose name collides with a sibling schema"
jsonpath "$.result.resources[45].mimeType" == "application/schema+json"
jsonpath "$.result.resources[45].size" == 280
jsonpath "$.result.resources[45].annotations.priority" == 1
jsonpath "$.result.resources[46].uri" == "{{base}}/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.resources[46].name" == "response"
jsonpath "$.result.resources[46].description" == "Sourcemeta One's list of MCP tools"
jsonpath "$.result.resources[46].mimeType" == "application/schema+json"
jsonpath "$.result.resources[46].size" == 4074
jsonpath "$.result.resources[46].annotations.priority" == 0
jsonpath "$.result.resources[47].uri" == "{{base}}/test/object"
jsonpath "$.result.resources[47].name" == "object"
jsonpath "$.result.resources[47].description" == "An Object Example"
jsonpath "$.result.resources[47].mimeType" == "application/schema+json"
jsonpath "$.result.resources[47].size" == 322
jsonpath "$.result.resources[47].annotations.priority" == 1
jsonpath "$.result.resources[48].uri" == "{{base}}/test/full/01"
jsonpath "$.result.resources[48].name" == "01"
jsonpath "$.result.resources[48].description" == "A schema with both a title and a description (01)"
jsonpath "$.result.resources[48].mimeType" == "application/schema+json"
jsonpath "$.result.resources[48].size" == 222
jsonpath "$.result.resources[48].annotations.priority" == 1
jsonpath "$.result.resources[49].uri" == "{{base}}/test/full/02"
jsonpath "$.result.resources[49].name" == "02"
jsonpath "$.result.resources[49].description" == "A schema with both a title and a description (02)"
jsonpath "$.result.resources[49].mimeType" == "application/schema+json"
jsonpath "$.result.resources[49].size" == 222
jsonpath "$.result.resources[49].annotations.priority" == 1
jsonpath "$.result.nextCursor" == "50"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 11,
  "method": "resources/list",
  "params": {
    "cursor": "50"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 11
jsonpath "$.result.resources" count == 50
jsonpath "$.result.resources[0].uri" == "{{base}}/test/full/03"
jsonpath "$.result.resources[0].name" == "03"
jsonpath "$.result.resources[0].description" == "A schema with both a title and a description (03)"
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.resources[0].size" == 222
jsonpath "$.result.resources[0].annotations.priority" == 1
jsonpath "$.result.resources[1].uri" == "{{base}}/test/full/04"
jsonpath "$.result.resources[1].name" == "04"
jsonpath "$.result.resources[1].description" == "A schema with both a title and a description (04)"
jsonpath "$.result.resources[1].mimeType" == "application/schema+json"
jsonpath "$.result.resources[1].size" == 222
jsonpath "$.result.resources[1].annotations.priority" == 1
jsonpath "$.result.resources[2].uri" == "{{base}}/test/full/05"
jsonpath "$.result.resources[2].name" == "05"
jsonpath "$.result.resources[2].description" == "A schema with both a title and a description (05)"
jsonpath "$.result.resources[2].mimeType" == "application/schema+json"
jsonpath "$.result.resources[2].size" == 222
jsonpath "$.result.resources[2].annotations.priority" == 1
jsonpath "$.result.resources[3].uri" == "{{base}}/test/description-only/01"
jsonpath "$.result.resources[3].name" == "01"
jsonpath "$.result.resources[3].description" == "A schema with only a description (01)"
jsonpath "$.result.resources[3].mimeType" == "application/schema+json"
jsonpath "$.result.resources[3].size" == 200
jsonpath "$.result.resources[3].annotations.priority" == 1
jsonpath "$.result.resources[4].uri" == "{{base}}/test/description-only/02"
jsonpath "$.result.resources[4].name" == "02"
jsonpath "$.result.resources[4].description" == "A schema with only a description (02)"
jsonpath "$.result.resources[4].mimeType" == "application/schema+json"
jsonpath "$.result.resources[4].size" == 200
jsonpath "$.result.resources[4].annotations.priority" == 1
jsonpath "$.result.resources[5].uri" == "{{base}}/test/description-only/03"
jsonpath "$.result.resources[5].name" == "03"
jsonpath "$.result.resources[5].description" == "A schema with only a description (03)"
jsonpath "$.result.resources[5].mimeType" == "application/schema+json"
jsonpath "$.result.resources[5].size" == 200
jsonpath "$.result.resources[5].annotations.priority" == 1
jsonpath "$.result.resources[6].uri" == "{{base}}/test/description-only/04"
jsonpath "$.result.resources[6].name" == "04"
jsonpath "$.result.resources[6].description" == "A schema with only a description (04)"
jsonpath "$.result.resources[6].mimeType" == "application/schema+json"
jsonpath "$.result.resources[6].size" == 200
jsonpath "$.result.resources[6].annotations.priority" == 1
jsonpath "$.result.resources[7].uri" == "{{base}}/test/description-only/05"
jsonpath "$.result.resources[7].name" == "05"
jsonpath "$.result.resources[7].description" == "A schema with only a description (05)"
jsonpath "$.result.resources[7].mimeType" == "application/schema+json"
jsonpath "$.result.resources[7].size" == 200
jsonpath "$.result.resources[7].annotations.priority" == 1
jsonpath "$.result.resources[8].uri" == "{{base}}/test/title-only/01"
jsonpath "$.result.resources[8].name" == "01"
jsonpath "$.result.resources[8].description" not exists
jsonpath "$.result.resources[8].mimeType" == "application/schema+json"
jsonpath "$.result.resources[8].size" == 164
jsonpath "$.result.resources[8].annotations.priority" == 1
jsonpath "$.result.resources[9].uri" == "{{base}}/test/title-only/02"
jsonpath "$.result.resources[9].name" == "02"
jsonpath "$.result.resources[9].description" not exists
jsonpath "$.result.resources[9].mimeType" == "application/schema+json"
jsonpath "$.result.resources[9].size" == 164
jsonpath "$.result.resources[9].annotations.priority" == 1
jsonpath "$.result.resources[10].uri" == "{{base}}/test/title-only/03"
jsonpath "$.result.resources[10].name" == "03"
jsonpath "$.result.resources[10].description" not exists
jsonpath "$.result.resources[10].mimeType" == "application/schema+json"
jsonpath "$.result.resources[10].size" == 164
jsonpath "$.result.resources[10].annotations.priority" == 1
jsonpath "$.result.resources[11].uri" == "{{base}}/test/title-only/04"
jsonpath "$.result.resources[11].name" == "04"
jsonpath "$.result.resources[11].description" not exists
jsonpath "$.result.resources[11].mimeType" == "application/schema+json"
jsonpath "$.result.resources[11].size" == 164
jsonpath "$.result.resources[11].annotations.priority" == 1
jsonpath "$.result.resources[12].uri" == "{{base}}/test/title-only/05"
jsonpath "$.result.resources[12].name" == "05"
jsonpath "$.result.resources[12].description" not exists
jsonpath "$.result.resources[12].mimeType" == "application/schema+json"
jsonpath "$.result.resources[12].size" == 164
jsonpath "$.result.resources[12].annotations.priority" == 1
jsonpath "$.result.resources[13].uri" == "{{base}}/test/draft3/with-ref"
jsonpath "$.result.resources[13].name" == "with-ref"
jsonpath "$.result.resources[13].description" not exists
jsonpath "$.result.resources[13].mimeType" == "application/schema+json"
jsonpath "$.result.resources[13].size" == 197
jsonpath "$.result.resources[13].annotations.priority" == 1
jsonpath "$.result.resources[14].uri" == "{{base}}/test/bare/01"
jsonpath "$.result.resources[14].name" == "01"
jsonpath "$.result.resources[14].description" not exists
jsonpath "$.result.resources[14].mimeType" == "application/schema+json"
jsonpath "$.result.resources[14].size" == 130
jsonpath "$.result.resources[14].annotations.priority" == 1
jsonpath "$.result.resources[15].uri" == "{{base}}/test/bare/02"
jsonpath "$.result.resources[15].name" == "02"
jsonpath "$.result.resources[15].description" not exists
jsonpath "$.result.resources[15].mimeType" == "application/schema+json"
jsonpath "$.result.resources[15].size" == 130
jsonpath "$.result.resources[15].annotations.priority" == 1
jsonpath "$.result.resources[16].uri" == "{{base}}/test/bare/03"
jsonpath "$.result.resources[16].name" == "03"
jsonpath "$.result.resources[16].description" not exists
jsonpath "$.result.resources[16].mimeType" == "application/schema+json"
jsonpath "$.result.resources[16].size" == 130
jsonpath "$.result.resources[16].annotations.priority" == 1
jsonpath "$.result.resources[17].uri" == "{{base}}/test/bare/04"
jsonpath "$.result.resources[17].name" == "04"
jsonpath "$.result.resources[17].description" not exists
jsonpath "$.result.resources[17].mimeType" == "application/schema+json"
jsonpath "$.result.resources[17].size" == 130
jsonpath "$.result.resources[17].annotations.priority" == 1
jsonpath "$.result.resources[18].uri" == "{{base}}/test/bare/05"
jsonpath "$.result.resources[18].name" == "05"
jsonpath "$.result.resources[18].description" not exists
jsonpath "$.result.resources[18].mimeType" == "application/schema+json"
jsonpath "$.result.resources[18].size" == 130
jsonpath "$.result.resources[18].annotations.priority" == 1
jsonpath "$.result.resources[19].uri" == "{{base}}/test/bare/06"
jsonpath "$.result.resources[19].name" == "06"
jsonpath "$.result.resources[19].description" not exists
jsonpath "$.result.resources[19].mimeType" == "application/schema+json"
jsonpath "$.result.resources[19].size" == 130
jsonpath "$.result.resources[19].annotations.priority" == 1
jsonpath "$.result.resources[20].uri" == "{{base}}/test/bare/07"
jsonpath "$.result.resources[20].name" == "07"
jsonpath "$.result.resources[20].description" not exists
jsonpath "$.result.resources[20].mimeType" == "application/schema+json"
jsonpath "$.result.resources[20].size" == 130
jsonpath "$.result.resources[20].annotations.priority" == 1
jsonpath "$.result.resources[21].uri" == "{{base}}/test/bare/08"
jsonpath "$.result.resources[21].name" == "08"
jsonpath "$.result.resources[21].description" not exists
jsonpath "$.result.resources[21].mimeType" == "application/schema+json"
jsonpath "$.result.resources[21].size" == 130
jsonpath "$.result.resources[21].annotations.priority" == 1
jsonpath "$.result.resources[22].uri" == "{{base}}/test/bare/09"
jsonpath "$.result.resources[22].name" == "09"
jsonpath "$.result.resources[22].description" not exists
jsonpath "$.result.resources[22].mimeType" == "application/schema+json"
jsonpath "$.result.resources[22].size" == 130
jsonpath "$.result.resources[22].annotations.priority" == 1
jsonpath "$.result.resources[23].uri" == "{{base}}/test/bare/10"
jsonpath "$.result.resources[23].name" == "10"
jsonpath "$.result.resources[23].description" not exists
jsonpath "$.result.resources[23].mimeType" == "application/schema+json"
jsonpath "$.result.resources[23].size" == 130
jsonpath "$.result.resources[23].annotations.priority" == 1
jsonpath "$.result.resources[24].uri" == "{{base}}/test/bare/11"
jsonpath "$.result.resources[24].name" == "11"
jsonpath "$.result.resources[24].description" not exists
jsonpath "$.result.resources[24].mimeType" == "application/schema+json"
jsonpath "$.result.resources[24].size" == 130
jsonpath "$.result.resources[24].annotations.priority" == 1
jsonpath "$.result.resources[25].uri" == "{{base}}/test/bare/12"
jsonpath "$.result.resources[25].name" == "12"
jsonpath "$.result.resources[25].description" not exists
jsonpath "$.result.resources[25].mimeType" == "application/schema+json"
jsonpath "$.result.resources[25].size" == 130
jsonpath "$.result.resources[25].annotations.priority" == 1
jsonpath "$.result.resources[26].uri" == "{{base}}/test/bare/13"
jsonpath "$.result.resources[26].name" == "13"
jsonpath "$.result.resources[26].description" not exists
jsonpath "$.result.resources[26].mimeType" == "application/schema+json"
jsonpath "$.result.resources[26].size" == 130
jsonpath "$.result.resources[26].annotations.priority" == 1
jsonpath "$.result.resources[27].uri" == "{{base}}/test/bare/14"
jsonpath "$.result.resources[27].name" == "14"
jsonpath "$.result.resources[27].description" not exists
jsonpath "$.result.resources[27].mimeType" == "application/schema+json"
jsonpath "$.result.resources[27].size" == 130
jsonpath "$.result.resources[27].annotations.priority" == 1
jsonpath "$.result.resources[28].uri" == "{{base}}/test/bare/15"
jsonpath "$.result.resources[28].name" == "15"
jsonpath "$.result.resources[28].description" not exists
jsonpath "$.result.resources[28].mimeType" == "application/schema+json"
jsonpath "$.result.resources[28].size" == 130
jsonpath "$.result.resources[28].annotations.priority" == 1
jsonpath "$.result.resources[29].uri" == "{{base}}/test/bare/16"
jsonpath "$.result.resources[29].name" == "16"
jsonpath "$.result.resources[29].description" not exists
jsonpath "$.result.resources[29].mimeType" == "application/schema+json"
jsonpath "$.result.resources[29].size" == 130
jsonpath "$.result.resources[29].annotations.priority" == 1
jsonpath "$.result.resources[30].uri" == "{{base}}/test/bare/17"
jsonpath "$.result.resources[30].name" == "17"
jsonpath "$.result.resources[30].description" not exists
jsonpath "$.result.resources[30].mimeType" == "application/schema+json"
jsonpath "$.result.resources[30].size" == 130
jsonpath "$.result.resources[30].annotations.priority" == 1
jsonpath "$.result.resources[31].uri" == "{{base}}/test/bare/18"
jsonpath "$.result.resources[31].name" == "18"
jsonpath "$.result.resources[31].description" not exists
jsonpath "$.result.resources[31].mimeType" == "application/schema+json"
jsonpath "$.result.resources[31].size" == 130
jsonpath "$.result.resources[31].annotations.priority" == 1
jsonpath "$.result.resources[32].uri" == "{{base}}/test/bare/19"
jsonpath "$.result.resources[32].name" == "19"
jsonpath "$.result.resources[32].description" not exists
jsonpath "$.result.resources[32].mimeType" == "application/schema+json"
jsonpath "$.result.resources[32].size" == 130
jsonpath "$.result.resources[32].annotations.priority" == 1
jsonpath "$.result.resources[33].uri" == "{{base}}/test/bare/20"
jsonpath "$.result.resources[33].name" == "20"
jsonpath "$.result.resources[33].description" not exists
jsonpath "$.result.resources[33].mimeType" == "application/schema+json"
jsonpath "$.result.resources[33].size" == 130
jsonpath "$.result.resources[33].annotations.priority" == 1
jsonpath "$.result.resources[34].uri" == "{{base}}/test/bare/21"
jsonpath "$.result.resources[34].name" == "21"
jsonpath "$.result.resources[34].description" not exists
jsonpath "$.result.resources[34].mimeType" == "application/schema+json"
jsonpath "$.result.resources[34].size" == 130
jsonpath "$.result.resources[34].annotations.priority" == 1
jsonpath "$.result.resources[35].uri" == "{{base}}/test/bare/22"
jsonpath "$.result.resources[35].name" == "22"
jsonpath "$.result.resources[35].description" not exists
jsonpath "$.result.resources[35].mimeType" == "application/schema+json"
jsonpath "$.result.resources[35].size" == 130
jsonpath "$.result.resources[35].annotations.priority" == 1
jsonpath "$.result.resources[36].uri" == "{{base}}/test/bare/23"
jsonpath "$.result.resources[36].name" == "23"
jsonpath "$.result.resources[36].description" not exists
jsonpath "$.result.resources[36].mimeType" == "application/schema+json"
jsonpath "$.result.resources[36].size" == 130
jsonpath "$.result.resources[36].annotations.priority" == 1
jsonpath "$.result.resources[37].uri" == "{{base}}/test/bare/24"
jsonpath "$.result.resources[37].name" == "24"
jsonpath "$.result.resources[37].description" not exists
jsonpath "$.result.resources[37].mimeType" == "application/schema+json"
jsonpath "$.result.resources[37].size" == 130
jsonpath "$.result.resources[37].annotations.priority" == 1
jsonpath "$.result.resources[38].uri" == "{{base}}/test/bare/25"
jsonpath "$.result.resources[38].name" == "25"
jsonpath "$.result.resources[38].description" not exists
jsonpath "$.result.resources[38].mimeType" == "application/schema+json"
jsonpath "$.result.resources[38].size" == 130
jsonpath "$.result.resources[38].annotations.priority" == 1
jsonpath "$.result.resources[39].uri" == "{{base}}/test/bare/26"
jsonpath "$.result.resources[39].name" == "26"
jsonpath "$.result.resources[39].description" not exists
jsonpath "$.result.resources[39].mimeType" == "application/schema+json"
jsonpath "$.result.resources[39].size" == 130
jsonpath "$.result.resources[39].annotations.priority" == 1
jsonpath "$.result.resources[40].uri" == "{{base}}/test/bare/27"
jsonpath "$.result.resources[40].name" == "27"
jsonpath "$.result.resources[40].description" not exists
jsonpath "$.result.resources[40].mimeType" == "application/schema+json"
jsonpath "$.result.resources[40].size" == 130
jsonpath "$.result.resources[40].annotations.priority" == 1
jsonpath "$.result.resources[41].uri" == "{{base}}/test/bare/28"
jsonpath "$.result.resources[41].name" == "28"
jsonpath "$.result.resources[41].description" not exists
jsonpath "$.result.resources[41].mimeType" == "application/schema+json"
jsonpath "$.result.resources[41].size" == 130
jsonpath "$.result.resources[41].annotations.priority" == 1
jsonpath "$.result.resources[42].uri" == "{{base}}/test/bare/29"
jsonpath "$.result.resources[42].name" == "29"
jsonpath "$.result.resources[42].description" not exists
jsonpath "$.result.resources[42].mimeType" == "application/schema+json"
jsonpath "$.result.resources[42].size" == 130
jsonpath "$.result.resources[42].annotations.priority" == 1
jsonpath "$.result.resources[43].uri" == "{{base}}/test/bare/30"
jsonpath "$.result.resources[43].name" == "30"
jsonpath "$.result.resources[43].description" not exists
jsonpath "$.result.resources[43].mimeType" == "application/schema+json"
jsonpath "$.result.resources[43].size" == 130
jsonpath "$.result.resources[43].annotations.priority" == 1
jsonpath "$.result.resources[44].uri" == "{{base}}/test/bare/31"
jsonpath "$.result.resources[44].name" == "31"
jsonpath "$.result.resources[44].description" not exists
jsonpath "$.result.resources[44].mimeType" == "application/schema+json"
jsonpath "$.result.resources[44].size" == 130
jsonpath "$.result.resources[44].annotations.priority" == 1
jsonpath "$.result.resources[45].uri" == "{{base}}/test/bare/32"
jsonpath "$.result.resources[45].name" == "32"
jsonpath "$.result.resources[45].description" not exists
jsonpath "$.result.resources[45].mimeType" == "application/schema+json"
jsonpath "$.result.resources[45].size" == 130
jsonpath "$.result.resources[45].annotations.priority" == 1
jsonpath "$.result.resources[46].uri" == "{{base}}/test/bare/33"
jsonpath "$.result.resources[46].name" == "33"
jsonpath "$.result.resources[46].description" not exists
jsonpath "$.result.resources[46].mimeType" == "application/schema+json"
jsonpath "$.result.resources[46].size" == 130
jsonpath "$.result.resources[46].annotations.priority" == 1
jsonpath "$.result.resources[47].uri" == "{{base}}/test/bare/34"
jsonpath "$.result.resources[47].name" == "34"
jsonpath "$.result.resources[47].description" not exists
jsonpath "$.result.resources[47].mimeType" == "application/schema+json"
jsonpath "$.result.resources[47].size" == 130
jsonpath "$.result.resources[47].annotations.priority" == 1
jsonpath "$.result.resources[48].uri" == "{{base}}/test/bare/35"
jsonpath "$.result.resources[48].name" == "35"
jsonpath "$.result.resources[48].description" not exists
jsonpath "$.result.resources[48].mimeType" == "application/schema+json"
jsonpath "$.result.resources[48].size" == 130
jsonpath "$.result.resources[48].annotations.priority" == 1
jsonpath "$.result.resources[49].uri" == "{{base}}/test/bare/36"
jsonpath "$.result.resources[49].name" == "36"
jsonpath "$.result.resources[49].description" not exists
jsonpath "$.result.resources[49].mimeType" == "application/schema+json"
jsonpath "$.result.resources[49].size" == 130
jsonpath "$.result.resources[49].annotations.priority" == 1
jsonpath "$.result.nextCursor" == "100"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 12,
  "method": "resources/list",
  "params": {
    "cursor": "100"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 12
jsonpath "$.result.resources" count == 15
jsonpath "$.result.resources[0].uri" == "{{base}}/test/bare/37"
jsonpath "$.result.resources[0].name" == "37"
jsonpath "$.result.resources[0].description" not exists
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.resources[0].size" == 130
jsonpath "$.result.resources[0].annotations.priority" == 1
jsonpath "$.result.resources[1].uri" == "{{base}}/test/bare/38"
jsonpath "$.result.resources[1].name" == "38"
jsonpath "$.result.resources[1].description" not exists
jsonpath "$.result.resources[1].mimeType" == "application/schema+json"
jsonpath "$.result.resources[1].size" == 130
jsonpath "$.result.resources[1].annotations.priority" == 1
jsonpath "$.result.resources[2].uri" == "{{base}}/test/bare/39"
jsonpath "$.result.resources[2].name" == "39"
jsonpath "$.result.resources[2].description" not exists
jsonpath "$.result.resources[2].mimeType" == "application/schema+json"
jsonpath "$.result.resources[2].size" == 130
jsonpath "$.result.resources[2].annotations.priority" == 1
jsonpath "$.result.resources[3].uri" == "{{base}}/test/bare/40"
jsonpath "$.result.resources[3].name" == "40"
jsonpath "$.result.resources[3].description" not exists
jsonpath "$.result.resources[3].mimeType" == "application/schema+json"
jsonpath "$.result.resources[3].size" == 130
jsonpath "$.result.resources[3].annotations.priority" == 1
jsonpath "$.result.resources[4].uri" == "{{base}}/test/bare/41"
jsonpath "$.result.resources[4].name" == "41"
jsonpath "$.result.resources[4].description" not exists
jsonpath "$.result.resources[4].mimeType" == "application/schema+json"
jsonpath "$.result.resources[4].size" == 130
jsonpath "$.result.resources[4].annotations.priority" == 1
jsonpath "$.result.resources[5].uri" == "{{base}}/test/bare/42"
jsonpath "$.result.resources[5].name" == "42"
jsonpath "$.result.resources[5].description" not exists
jsonpath "$.result.resources[5].mimeType" == "application/schema+json"
jsonpath "$.result.resources[5].size" == 130
jsonpath "$.result.resources[5].annotations.priority" == 1
jsonpath "$.result.resources[6].uri" == "{{base}}/test/bare/43"
jsonpath "$.result.resources[6].name" == "43"
jsonpath "$.result.resources[6].description" not exists
jsonpath "$.result.resources[6].mimeType" == "application/schema+json"
jsonpath "$.result.resources[6].size" == 130
jsonpath "$.result.resources[6].annotations.priority" == 1
jsonpath "$.result.resources[7].uri" == "{{base}}/test/bare/44"
jsonpath "$.result.resources[7].name" == "44"
jsonpath "$.result.resources[7].description" not exists
jsonpath "$.result.resources[7].mimeType" == "application/schema+json"
jsonpath "$.result.resources[7].size" == 130
jsonpath "$.result.resources[7].annotations.priority" == 1
jsonpath "$.result.resources[8].uri" == "{{base}}/test/bare/45"
jsonpath "$.result.resources[8].name" == "45"
jsonpath "$.result.resources[8].description" not exists
jsonpath "$.result.resources[8].mimeType" == "application/schema+json"
jsonpath "$.result.resources[8].size" == 130
jsonpath "$.result.resources[8].annotations.priority" == 1
jsonpath "$.result.resources[9].uri" == "{{base}}/test/bare/46"
jsonpath "$.result.resources[9].name" == "46"
jsonpath "$.result.resources[9].description" not exists
jsonpath "$.result.resources[9].mimeType" == "application/schema+json"
jsonpath "$.result.resources[9].size" == 130
jsonpath "$.result.resources[9].annotations.priority" == 1
jsonpath "$.result.resources[10].uri" == "{{base}}/test/bare/47"
jsonpath "$.result.resources[10].name" == "47"
jsonpath "$.result.resources[10].description" not exists
jsonpath "$.result.resources[10].mimeType" == "application/schema+json"
jsonpath "$.result.resources[10].size" == 130
jsonpath "$.result.resources[10].annotations.priority" == 1
jsonpath "$.result.resources[11].uri" == "{{base}}/test/bare/48"
jsonpath "$.result.resources[11].name" == "48"
jsonpath "$.result.resources[11].description" not exists
jsonpath "$.result.resources[11].mimeType" == "application/schema+json"
jsonpath "$.result.resources[11].size" == 130
jsonpath "$.result.resources[11].annotations.priority" == 1
jsonpath "$.result.resources[12].uri" == "{{base}}/test/bare/49"
jsonpath "$.result.resources[12].name" == "49"
jsonpath "$.result.resources[12].description" not exists
jsonpath "$.result.resources[12].mimeType" == "application/schema+json"
jsonpath "$.result.resources[12].size" == 130
jsonpath "$.result.resources[12].annotations.priority" == 1
jsonpath "$.result.resources[13].uri" == "{{base}}/test/bare/50"
jsonpath "$.result.resources[13].name" == "50"
jsonpath "$.result.resources[13].description" not exists
jsonpath "$.result.resources[13].mimeType" == "application/schema+json"
jsonpath "$.result.resources[13].size" == 130
jsonpath "$.result.resources[13].annotations.priority" == 1
jsonpath "$.result.resources[14].uri" == "{{base}}/test/draft3/string"
jsonpath "$.result.resources[14].name" == "string"
jsonpath "$.result.resources[14].description" not exists
jsonpath "$.result.resources[14].mimeType" == "application/schema+json"
jsonpath "$.result.resources[14].size" == 130
jsonpath "$.result.resources[14].annotations.priority" == 1
jsonpath "$.result.nextCursor" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 40,
  "method": "resources/list",
  "params": {
    "cursor": "abc"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 40
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data" == "abc"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 41,
  "method": "resources/list",
  "params": {
    "cursor": "-1"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 41
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data" == "-1"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/list returns empty for out-of-range cursors so a stale cursor
# from before a reindex no longer surfaces an error

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 42,
  "method": "resources/list",
  "params": {
    "cursor": "999998"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 42
jsonpath "$.result.resources" count == 0
jsonpath "$.result.nextCursor" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 44,
  "method": "resources/list",
  "params": {
    "cursor": "00"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 44
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"
jsonpath "$.error.data" == "00"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
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
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 12
jsonpath "$.result.resourceTemplates" count == 1
jsonpath "$.result.resourceTemplates[0].uriTemplate" == "{{base}}/{+path}{?bundle}"
jsonpath "$.result.resourceTemplates[0].name" == "JSON Schema"
jsonpath "$.result.resourceTemplates[0].description" == "A JSON Schema in this catalog (optionally bundled)"
jsonpath "$.result.resourceTemplates[0].mimeType" == "application/schema+json"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
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
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 20
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "not a uri"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 22,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/does/not/exist"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 22
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "{{base}}/does/not/exist"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/test/object
HTTP 200
[Captures]
schema_text: body

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 30,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/test/object"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 30
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/test/object"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/test/object.json
HTTP 200
[Captures]
schema_text: body

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 31,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/test/object.json"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 31
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/test/object.json"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/schemas/mcp/response?bundle=1
HTTP 200
[Captures]
bundled_schema_text: body

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 32,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/self/v1/schemas/mcp/response?bundle=1"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 32
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/self/v1/schemas/mcp/response?bundle=1"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{bundled_schema_text}}"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
