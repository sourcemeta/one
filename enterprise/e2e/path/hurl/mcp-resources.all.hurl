# resources/list enumerates the catalog with raw and bundled twin entries

POST {{base}}/v1/catalog/self/v1/mcp
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
jsonpath "$.result.resources[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/error"
jsonpath "$.result.resources[0].name" == "error"
jsonpath "$.result.resources[0].description" == "The error response format returned by the Sourcemeta One API endpoints"
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.resources[0].size" == 1046
jsonpath "$.result.resources[1].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/list/response"
jsonpath "$.result.resources[1].name" == "response"
jsonpath "$.result.resources[1].description" == "The response format for the directory listing API endpoint"
jsonpath "$.result.resources[1].mimeType" == "application/schema+json"
jsonpath "$.result.resources[1].size" == 3896
jsonpath "$.result.resources[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/list/rpc"
jsonpath "$.result.resources[2].name" == "rpc"
jsonpath "$.result.resources[2].description" == "List the contents of a directory in the catalog"
jsonpath "$.result.resources[2].mimeType" == "application/schema+json"
jsonpath "$.result.resources[2].size" == 535
jsonpath "$.result.resources[3].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/response"
jsonpath "$.result.resources[3].name" == "response"
jsonpath "$.result.resources[3].description" == "The response format for the schema dependencies API endpoint"
jsonpath "$.result.resources[3].mimeType" == "application/schema+json"
jsonpath "$.result.resources[3].size" == 1112
jsonpath "$.result.resources[4].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/rpc"
jsonpath "$.result.resources[4].name" == "rpc"
jsonpath "$.result.resources[4].description" == "Get all direct and indirect dependencies of a schema in the catalog"
jsonpath "$.result.resources[4].mimeType" == "application/schema+json"
jsonpath "$.result.resources[4].size" == 689
jsonpath "$.result.resources[5].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependents/response"
jsonpath "$.result.resources[5].name" == "response"
jsonpath "$.result.resources[5].description" == "The response format for the schema dependents API endpoint"
jsonpath "$.result.resources[5].mimeType" == "application/schema+json"
jsonpath "$.result.resources[5].size" == 907
jsonpath "$.result.resources[6].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependents/rpc"
jsonpath "$.result.resources[6].name" == "rpc"
jsonpath "$.result.resources[6].description" == "Get all direct and transitive dependents of a schema in the catalog"
jsonpath "$.result.resources[6].mimeType" == "application/schema+json"
jsonpath "$.result.resources[6].size" == 685
jsonpath "$.result.resources[7].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/request"
jsonpath "$.result.resources[7].name" == "request"
jsonpath "$.result.resources[7].description" == "The JSON instance to validate against a schema in the catalog"
jsonpath "$.result.resources[7].mimeType" == "application/schema+json"
jsonpath "$.result.resources[7].size" == 428
jsonpath "$.result.resources[8].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.resources[8].name" == "response"
jsonpath "$.result.resources[8].description" == "The response format for the schema evaluation API endpoint"
jsonpath "$.result.resources[8].mimeType" == "application/schema+json"
jsonpath "$.result.resources[8].size" == 3288
jsonpath "$.result.resources[9].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/rpc"
jsonpath "$.result.resources[9].name" == "rpc"
jsonpath "$.result.resources[9].description" == "Evaluate a JSON instance against a schema in the catalog and return whether it is valid plus any errors"
jsonpath "$.result.resources[9].mimeType" == "application/schema+json"
jsonpath "$.result.resources[9].size" == 1081
jsonpath "$.result.resources[10].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/health/response"
jsonpath "$.result.resources[10].name" == "response"
jsonpath "$.result.resources[10].description" == "The response format for the schema health API endpoint"
jsonpath "$.result.resources[10].mimeType" == "application/schema+json"
jsonpath "$.result.resources[10].size" == 1633
jsonpath "$.result.resources[11].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/health/rpc"
jsonpath "$.result.resources[11].name" == "rpc"
jsonpath "$.result.resources[11].description" == "Get the health analysis and score of a schema in the catalog"
jsonpath "$.result.resources[11].mimeType" == "application/schema+json"
jsonpath "$.result.resources[11].size" == 670
jsonpath "$.result.resources[12].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/locations/response"
jsonpath "$.result.resources[12].name" == "response"
jsonpath "$.result.resources[12].description" == "The response format for the schema frame locations API endpoint"
jsonpath "$.result.resources[12].mimeType" == "application/schema+json"
jsonpath "$.result.resources[12].size" == 2679
jsonpath "$.result.resources[13].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/locations/rpc"
jsonpath "$.result.resources[13].name" == "rpc"
jsonpath "$.result.resources[13].description" == "Get metadata about every URI associated with a schema, including schema resources, subschemas, and anchors"
jsonpath "$.result.resources[13].mimeType" == "application/schema+json"
jsonpath "$.result.resources[13].size" == 722
jsonpath "$.result.resources[14].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/metadata/response"
jsonpath "$.result.resources[14].name" == "response"
jsonpath "$.result.resources[14].description" == "The response format for the schema metadata API endpoint"
jsonpath "$.result.resources[14].mimeType" == "application/schema+json"
jsonpath "$.result.resources[14].size" == 2887
jsonpath "$.result.resources[15].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/metadata/rpc"
jsonpath "$.result.resources[15].name" == "rpc"
jsonpath "$.result.resources[15].description" == "Get metadata information about a schema in the catalog"
jsonpath "$.result.resources[15].mimeType" == "application/schema+json"
jsonpath "$.result.resources[15].size" == 668
jsonpath "$.result.resources[16].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/position"
jsonpath "$.result.resources[16].name" == "position"
jsonpath "$.result.resources[16].description" == "Represents a position in a JSON document as line start, column start, line end, and column end"
jsonpath "$.result.resources[16].mimeType" == "application/schema+json"
jsonpath "$.result.resources[16].size" == 457
jsonpath "$.result.resources[17].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/positions/response"
jsonpath "$.result.resources[17].name" == "response"
jsonpath "$.result.resources[17].description" == "The response format for the schema positions API endpoint"
jsonpath "$.result.resources[17].mimeType" == "application/schema+json"
jsonpath "$.result.resources[17].size" == 446
jsonpath "$.result.resources[18].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/positions/rpc"
jsonpath "$.result.resources[18].name" == "rpc"
jsonpath "$.result.resources[18].description" == "Get line and column position information for every token in a schema"
jsonpath "$.result.resources[18].mimeType" == "application/schema+json"
jsonpath "$.result.resources[18].size" == 684
jsonpath "$.result.resources[19].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/response"
jsonpath "$.result.resources[19].name" == "response"
jsonpath "$.result.resources[19].description" == "The response format for the schema search API endpoint"
jsonpath "$.result.resources[19].mimeType" == "application/schema+json"
jsonpath "$.result.resources[19].size" == 887
jsonpath "$.result.resources[20].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/rpc"
jsonpath "$.result.resources[20].name" == "rpc"
jsonpath "$.result.resources[20].description" == "Search for schemas in the catalog by query term"
jsonpath "$.result.resources[20].mimeType" == "application/schema+json"
jsonpath "$.result.resources[20].size" == 1131
jsonpath "$.result.resources[21].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/stats/response"
jsonpath "$.result.resources[21].name" == "response"
jsonpath "$.result.resources[21].description" == "The response format for the schema statistics API endpoint"
jsonpath "$.result.resources[21].mimeType" == "application/schema+json"
jsonpath "$.result.resources[21].size" == 2356
jsonpath "$.result.resources[22].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/stats/rpc"
jsonpath "$.result.resources[22].name" == "rpc"
jsonpath "$.result.resources[22].description" == "Get keyword statistics by vocabulary about every URI associated with a schema in the catalog"
jsonpath "$.result.resources[22].mimeType" == "application/schema+json"
jsonpath "$.result.resources[22].size" == 705
jsonpath "$.result.resources[23].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/request"
jsonpath "$.result.resources[23].name" == "request"
jsonpath "$.result.resources[23].description" == "The JSON instance to trace evaluation against a schema in the catalog"
jsonpath "$.result.resources[23].mimeType" == "application/schema+json"
jsonpath "$.result.resources[23].size" == 430
jsonpath "$.result.resources[24].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/response"
jsonpath "$.result.resources[24].name" == "response"
jsonpath "$.result.resources[24].description" == "The response format for the schema trace API endpoint"
jsonpath "$.result.resources[24].mimeType" == "application/schema+json"
jsonpath "$.result.resources[24].size" == 2281
jsonpath "$.result.resources[25].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/rpc"
jsonpath "$.result.resources[25].name" == "rpc"
jsonpath "$.result.resources[25].description" == "Trace step-by-step evaluation of a JSON instance against a schema in the catalog, exposing each step, rule application, and decision point"
jsonpath "$.result.resources[25].mimeType" == "application/schema+json"
jsonpath "$.result.resources[25].size" == 1107
jsonpath "$.result.resources[26].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.resources[26].name" == "error"
jsonpath "$.result.resources[26].description" == "The error response Sourcemeta One returns when an MCP request fails"
jsonpath "$.result.resources[26].mimeType" == "application/schema+json"
jsonpath "$.result.resources[26].size" == 6644
jsonpath "$.result.resources[27].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/request"
jsonpath "$.result.resources[27].name" == "request"
jsonpath "$.result.resources[27].description" == "Connection handshake from a client"
jsonpath "$.result.resources[27].mimeType" == "application/schema+json"
jsonpath "$.result.resources[27].size" == 4538
jsonpath "$.result.resources[28].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.resources[28].name" == "response"
jsonpath "$.result.resources[28].description" == "Sourcemeta One's response to a client's initialization"
jsonpath "$.result.resources[28].mimeType" == "application/schema+json"
jsonpath "$.result.resources[28].size" == 1972
jsonpath "$.result.resources[29].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/notifications/cancelled"
jsonpath "$.result.resources[29].name" == "cancelled"
jsonpath "$.result.resources[29].description" == "Cancellation hint from a client for an in-flight request. Sourcemeta One accepts and discards these"
jsonpath "$.result.resources[29].mimeType" == "application/schema+json"
jsonpath "$.result.resources[29].size" == 1174
jsonpath "$.result.resources[30].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/notifications/initialized"
jsonpath "$.result.resources[30].name" == "initialized"
jsonpath "$.result.resources[30].description" == "Client signal that the handshake is complete and normal operation may begin"
jsonpath "$.result.resources[30].mimeType" == "application/schema+json"
jsonpath "$.result.resources[30].size" == 751
jsonpath "$.result.resources[31].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/request"
jsonpath "$.result.resources[31].name" == "request"
jsonpath "$.result.resources[31].description" == "Liveness check from a client"
jsonpath "$.result.resources[31].mimeType" == "application/schema+json"
jsonpath "$.result.resources[31].size" == 706
jsonpath "$.result.resources[32].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.resources[32].name" == "response"
jsonpath "$.result.resources[32].description" == "Sourcemeta One's response to a liveness check"
jsonpath "$.result.resources[32].mimeType" == "application/schema+json"
jsonpath "$.result.resources[32].size" == 609
jsonpath "$.result.resources[33].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/request"
jsonpath "$.result.resources[33].name" == "request"
jsonpath "$.result.resources[33].description" == "Any incoming MCP request the server accepts"
jsonpath "$.result.resources[33].mimeType" == "application/schema+json"
jsonpath "$.result.resources[33].size" == 1784
jsonpath "$.result.resources[34].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/request"
jsonpath "$.result.resources[34].name" == "request"
jsonpath "$.result.resources[34].description" == "Request from a client to list MCP resources"
jsonpath "$.result.resources[34].mimeType" == "application/schema+json"
jsonpath "$.result.resources[34].size" == 1082
jsonpath "$.result.resources[35].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.resources[35].name" == "response"
jsonpath "$.result.resources[35].description" == "Sourcemeta One's list of MCP resources"
jsonpath "$.result.resources[35].mimeType" == "application/schema+json"
jsonpath "$.result.resources[35].size" == 2547
jsonpath "$.result.resources[36].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/request"
jsonpath "$.result.resources[36].name" == "request"
jsonpath "$.result.resources[36].description" == "Request from a client to read an MCP resource"
jsonpath "$.result.resources[36].mimeType" == "application/schema+json"
jsonpath "$.result.resources[36].size" == 1342
jsonpath "$.result.resources[37].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.resources[37].name" == "response"
jsonpath "$.result.resources[37].description" == "Sourcemeta One's response containing an MCP resource"
jsonpath "$.result.resources[37].mimeType" == "application/schema+json"
jsonpath "$.result.resources[37].size" == 1803
jsonpath "$.result.resources[38].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/request"
jsonpath "$.result.resources[38].name" == "request"
jsonpath "$.result.resources[38].description" == "Request from a client to list MCP resource templates"
jsonpath "$.result.resources[38].mimeType" == "application/schema+json"
jsonpath "$.result.resources[38].size" == 933
jsonpath "$.result.resources[39].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.resources[39].name" == "response"
jsonpath "$.result.resources[39].description" == "Sourcemeta One's list of MCP resource templates"
jsonpath "$.result.resources[39].mimeType" == "application/schema+json"
jsonpath "$.result.resources[39].size" == 1926
jsonpath "$.result.resources[40].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.resources[40].name" == "response"
jsonpath "$.result.resources[40].description" == "Any outgoing MCP response the server returns"
jsonpath "$.result.resources[40].mimeType" == "application/schema+json"
jsonpath "$.result.resources[40].size" == 2849
jsonpath "$.result.resources[41].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/request"
jsonpath "$.result.resources[41].name" == "request"
jsonpath "$.result.resources[41].description" == "Request from a client to invoke an MCP tool by name"
jsonpath "$.result.resources[41].mimeType" == "application/schema+json"
jsonpath "$.result.resources[41].size" == 1172
jsonpath "$.result.resources[42].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.resources[42].name" == "response"
jsonpath "$.result.resources[42].description" == "Sourcemeta One's response to a tool invocation"
jsonpath "$.result.resources[42].mimeType" == "application/schema+json"
jsonpath "$.result.resources[42].size" == 2161
jsonpath "$.result.resources[43].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/request"
jsonpath "$.result.resources[43].name" == "request"
jsonpath "$.result.resources[43].description" == "Request from a client to list MCP tools"
jsonpath "$.result.resources[43].mimeType" == "application/schema+json"
jsonpath "$.result.resources[43].size" == 864
jsonpath "$.result.resources[44].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.resources[44].name" == "response"
jsonpath "$.result.resources[44].description" == "Sourcemeta One's list of MCP tools"
jsonpath "$.result.resources[44].mimeType" == "application/schema+json"
jsonpath "$.result.resources[44].size" == 2680
jsonpath "$.result.resources[45].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.resources[45].name" == "full"
jsonpath "$.result.resources[45].description" == "A schema that has both a title and a description"
jsonpath "$.result.resources[45].mimeType" == "application/schema+json"
jsonpath "$.result.resources[45].size" == 242
jsonpath "$.result.resources[46].uri" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.resources[46].name" == "description-only"
jsonpath "$.result.resources[46].description" == "A schema with only a description and no title"
jsonpath "$.result.resources[46].mimeType" == "application/schema+json"
jsonpath "$.result.resources[46].size" == 219
jsonpath "$.result.resources[47].uri" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.resources[47].name" == "title-only"
jsonpath "$.result.resources[47].description" not exists
jsonpath "$.result.resources[47].mimeType" == "application/schema+json"
jsonpath "$.result.resources[47].size" == 185
jsonpath "$.result.resources[48].uri" == "{{base}}/v1/catalog/draft3/object"
jsonpath "$.result.resources[48].name" == "object"
jsonpath "$.result.resources[48].description" not exists
jsonpath "$.result.resources[48].mimeType" == "application/schema+json"
jsonpath "$.result.resources[48].size" == 270
jsonpath "$.result.resources[49].uri" == "{{base}}/v1/catalog/draft3/string"
jsonpath "$.result.resources[49].name" == "string"
jsonpath "$.result.resources[49].description" not exists
jsonpath "$.result.resources[49].mimeType" == "application/schema+json"
jsonpath "$.result.resources[49].size" == 136
jsonpath "$.result.nextCursor" == "50"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/templates/list publishes the single registry URI template

POST {{base}}/v1/catalog/self/v1/mcp
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
jsonpath "$.result.resourceTemplates[0].description" == "A JSON Schema in this catalog (optionally bundled)"
jsonpath "$.result.resourceTemplates[0].mimeType" == "application/schema+json"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/read of a self-served schema URI

GET {{base}}/v1/catalog/self/v1/schemas/mcp/error
HTTP 200
[Captures]
schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
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

# resources/read accepts the .json extension form of the URI

GET {{base}}/v1/catalog/self/v1/schemas/mcp/error.json
HTTP 200
[Captures]
schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
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

# resources/read with ?bundle=1 of a $ref-bearing schema

GET {{base}}/v1/catalog/self/v1/schemas/mcp/response?bundle=1
HTTP 200
[Captures]
bundled_schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
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
