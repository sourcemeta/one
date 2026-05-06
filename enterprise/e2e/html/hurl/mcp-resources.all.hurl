# resources/list page 1 enumerates the first 50 entries with nextCursor
POST {{base}}/self/v1/mcp
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
jsonpath "$.result.resources[1].uri" == "{{base}}/self/v1/schemas/api/list/response"
jsonpath "$.result.resources[1].name" == "response"
jsonpath "$.result.resources[1].description" == "The response format for the directory listing API endpoint"
jsonpath "$.result.resources[1].mimeType" == "application/schema+json"
jsonpath "$.result.resources[1].size" == 3885
jsonpath "$.result.resources[2].uri" == "{{base}}/self/v1/schemas/api/schemas/dependencies/response"
jsonpath "$.result.resources[2].name" == "response"
jsonpath "$.result.resources[2].description" == "The response format for the schema dependencies API endpoint"
jsonpath "$.result.resources[2].mimeType" == "application/schema+json"
jsonpath "$.result.resources[2].size" == 1101
jsonpath "$.result.resources[3].uri" == "{{base}}/self/v1/schemas/api/schemas/dependents/response"
jsonpath "$.result.resources[3].name" == "response"
jsonpath "$.result.resources[3].description" == "The response format for the schema dependents API endpoint"
jsonpath "$.result.resources[3].mimeType" == "application/schema+json"
jsonpath "$.result.resources[3].size" == 896
jsonpath "$.result.resources[4].uri" == "{{base}}/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.resources[4].name" == "response"
jsonpath "$.result.resources[4].description" == "The response format for the schema evaluation API endpoint"
jsonpath "$.result.resources[4].mimeType" == "application/schema+json"
jsonpath "$.result.resources[4].size" == 3277
jsonpath "$.result.resources[5].uri" == "{{base}}/self/v1/schemas/api/schemas/health/response"
jsonpath "$.result.resources[5].name" == "response"
jsonpath "$.result.resources[5].description" == "The response format for the schema health API endpoint"
jsonpath "$.result.resources[5].mimeType" == "application/schema+json"
jsonpath "$.result.resources[5].size" == 1622
jsonpath "$.result.resources[6].uri" == "{{base}}/self/v1/schemas/api/schemas/locations/response"
jsonpath "$.result.resources[6].name" == "response"
jsonpath "$.result.resources[6].description" == "The response format for the schema frame locations API endpoint"
jsonpath "$.result.resources[6].mimeType" == "application/schema+json"
jsonpath "$.result.resources[6].size" == 2668
jsonpath "$.result.resources[7].uri" == "{{base}}/self/v1/schemas/api/schemas/metadata/response"
jsonpath "$.result.resources[7].name" == "response"
jsonpath "$.result.resources[7].description" == "The response format for the schema metadata API endpoint"
jsonpath "$.result.resources[7].mimeType" == "application/schema+json"
jsonpath "$.result.resources[7].size" == 2876
jsonpath "$.result.resources[8].uri" == "{{base}}/self/v1/schemas/api/schemas/position"
jsonpath "$.result.resources[8].name" == "position"
jsonpath "$.result.resources[8].description" == "Represents a position in a JSON document as line start, column start, line end, and column end"
jsonpath "$.result.resources[8].mimeType" == "application/schema+json"
jsonpath "$.result.resources[8].size" == 446
jsonpath "$.result.resources[9].uri" == "{{base}}/self/v1/schemas/api/schemas/positions/response"
jsonpath "$.result.resources[9].name" == "response"
jsonpath "$.result.resources[9].description" == "The response format for the schema positions API endpoint"
jsonpath "$.result.resources[9].mimeType" == "application/schema+json"
jsonpath "$.result.resources[9].size" == 435
jsonpath "$.result.resources[10].uri" == "{{base}}/self/v1/schemas/api/schemas/search/response"
jsonpath "$.result.resources[10].name" == "response"
jsonpath "$.result.resources[10].description" == "The response format for the schema search API endpoint"
jsonpath "$.result.resources[10].mimeType" == "application/schema+json"
jsonpath "$.result.resources[10].size" == 876
jsonpath "$.result.resources[11].uri" == "{{base}}/self/v1/schemas/api/schemas/stats/response"
jsonpath "$.result.resources[11].name" == "response"
jsonpath "$.result.resources[11].description" == "The response format for the schema statistics API endpoint"
jsonpath "$.result.resources[11].mimeType" == "application/schema+json"
jsonpath "$.result.resources[11].size" == 912
jsonpath "$.result.resources[12].uri" == "{{base}}/self/v1/schemas/api/schemas/trace/response"
jsonpath "$.result.resources[12].name" == "response"
jsonpath "$.result.resources[12].description" == "The response format for the schema trace API endpoint"
jsonpath "$.result.resources[12].mimeType" == "application/schema+json"
jsonpath "$.result.resources[12].size" == 2270
jsonpath "$.result.resources[13].uri" == "{{base}}/self/v1/schemas/mcp/error"
jsonpath "$.result.resources[13].name" == "error"
jsonpath "$.result.resources[13].description" == "The error response Sourcemeta One returns when an MCP request fails"
jsonpath "$.result.resources[13].mimeType" == "application/schema+json"
jsonpath "$.result.resources[13].size" == 5802
jsonpath "$.result.resources[14].uri" == "{{base}}/self/v1/schemas/mcp/initialize/request"
jsonpath "$.result.resources[14].name" == "request"
jsonpath "$.result.resources[14].description" == "Connection handshake from a client"
jsonpath "$.result.resources[14].mimeType" == "application/schema+json"
jsonpath "$.result.resources[14].size" == 4527
jsonpath "$.result.resources[15].uri" == "{{base}}/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.resources[15].name" == "response"
jsonpath "$.result.resources[15].description" == "Sourcemeta One's response to a client's initialization"
jsonpath "$.result.resources[15].mimeType" == "application/schema+json"
jsonpath "$.result.resources[15].size" == 1847
jsonpath "$.result.resources[16].uri" == "{{base}}/self/v1/schemas/mcp/notifications/cancelled"
jsonpath "$.result.resources[16].name" == "cancelled"
jsonpath "$.result.resources[16].description" == "Cancellation hint from a client for an in-flight request. Sourcemeta One accepts and discards these"
jsonpath "$.result.resources[16].mimeType" == "application/schema+json"
jsonpath "$.result.resources[16].size" == 1163
jsonpath "$.result.resources[17].uri" == "{{base}}/self/v1/schemas/mcp/notifications/initialized"
jsonpath "$.result.resources[17].name" == "initialized"
jsonpath "$.result.resources[17].description" == "Client signal that the handshake is complete and normal operation may begin"
jsonpath "$.result.resources[17].mimeType" == "application/schema+json"
jsonpath "$.result.resources[17].size" == 740
jsonpath "$.result.resources[18].uri" == "{{base}}/self/v1/schemas/mcp/ping/request"
jsonpath "$.result.resources[18].name" == "request"
jsonpath "$.result.resources[18].description" == "Liveness check from a client"
jsonpath "$.result.resources[18].mimeType" == "application/schema+json"
jsonpath "$.result.resources[18].size" == 695
jsonpath "$.result.resources[19].uri" == "{{base}}/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.resources[19].name" == "response"
jsonpath "$.result.resources[19].description" == "Sourcemeta One's response to a liveness check"
jsonpath "$.result.resources[19].mimeType" == "application/schema+json"
jsonpath "$.result.resources[19].size" == 598
jsonpath "$.result.resources[20].uri" == "{{base}}/self/v1/schemas/mcp/request"
jsonpath "$.result.resources[20].name" == "request"
jsonpath "$.result.resources[20].description" == "Any incoming MCP request the server accepts"
jsonpath "$.result.resources[20].mimeType" == "application/schema+json"
jsonpath "$.result.resources[20].size" == 1424
jsonpath "$.result.resources[21].uri" == "{{base}}/self/v1/schemas/mcp/resources/list/request"
jsonpath "$.result.resources[21].name" == "request"
jsonpath "$.result.resources[21].description" == "Request from a client to list MCP resources"
jsonpath "$.result.resources[21].mimeType" == "application/schema+json"
jsonpath "$.result.resources[21].size" == 1071
jsonpath "$.result.resources[22].uri" == "{{base}}/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.resources[22].name" == "response"
jsonpath "$.result.resources[22].description" == "Sourcemeta One's list of MCP resources"
jsonpath "$.result.resources[22].mimeType" == "application/schema+json"
jsonpath "$.result.resources[22].size" == 2536
jsonpath "$.result.resources[23].uri" == "{{base}}/self/v1/schemas/mcp/resources/read/request"
jsonpath "$.result.resources[23].name" == "request"
jsonpath "$.result.resources[23].description" == "Request from a client to read an MCP resource"
jsonpath "$.result.resources[23].mimeType" == "application/schema+json"
jsonpath "$.result.resources[23].size" == 1331
jsonpath "$.result.resources[24].uri" == "{{base}}/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.resources[24].name" == "response"
jsonpath "$.result.resources[24].description" == "Sourcemeta One's response containing an MCP resource"
jsonpath "$.result.resources[24].mimeType" == "application/schema+json"
jsonpath "$.result.resources[24].size" == 1792
jsonpath "$.result.resources[25].uri" == "{{base}}/self/v1/schemas/mcp/resources/templates/list/request"
jsonpath "$.result.resources[25].name" == "request"
jsonpath "$.result.resources[25].description" == "Request from a client to list MCP resource templates"
jsonpath "$.result.resources[25].mimeType" == "application/schema+json"
jsonpath "$.result.resources[25].size" == 922
jsonpath "$.result.resources[26].uri" == "{{base}}/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.resources[26].name" == "response"
jsonpath "$.result.resources[26].description" == "Sourcemeta One's list of MCP resource templates"
jsonpath "$.result.resources[26].mimeType" == "application/schema+json"
jsonpath "$.result.resources[26].size" == 1915
jsonpath "$.result.resources[27].uri" == "{{base}}/self/v1/schemas/mcp/response"
jsonpath "$.result.resources[27].name" == "response"
jsonpath "$.result.resources[27].description" == "Any outgoing MCP response the server returns"
jsonpath "$.result.resources[27].mimeType" == "application/schema+json"
jsonpath "$.result.resources[27].size" == 1943
jsonpath "$.result.resources[28].uri" == "{{base}}/test/object"
jsonpath "$.result.resources[28].name" == "object"
jsonpath "$.result.resources[28].description" == "An Object Example"
jsonpath "$.result.resources[28].mimeType" == "application/schema+json"
jsonpath "$.result.resources[28].size" == 322
jsonpath "$.result.resources[29].uri" == "{{base}}/test/full/01"
jsonpath "$.result.resources[29].name" == "01"
jsonpath "$.result.resources[29].description" == "A schema with both a title and a description (01)"
jsonpath "$.result.resources[29].mimeType" == "application/schema+json"
jsonpath "$.result.resources[29].size" == 222
jsonpath "$.result.resources[30].uri" == "{{base}}/test/full/02"
jsonpath "$.result.resources[30].name" == "02"
jsonpath "$.result.resources[30].description" == "A schema with both a title and a description (02)"
jsonpath "$.result.resources[30].mimeType" == "application/schema+json"
jsonpath "$.result.resources[30].size" == 222
jsonpath "$.result.resources[31].uri" == "{{base}}/test/full/03"
jsonpath "$.result.resources[31].name" == "03"
jsonpath "$.result.resources[31].description" == "A schema with both a title and a description (03)"
jsonpath "$.result.resources[31].mimeType" == "application/schema+json"
jsonpath "$.result.resources[31].size" == 222
jsonpath "$.result.resources[32].uri" == "{{base}}/test/full/04"
jsonpath "$.result.resources[32].name" == "04"
jsonpath "$.result.resources[32].description" == "A schema with both a title and a description (04)"
jsonpath "$.result.resources[32].mimeType" == "application/schema+json"
jsonpath "$.result.resources[32].size" == 222
jsonpath "$.result.resources[33].uri" == "{{base}}/test/full/05"
jsonpath "$.result.resources[33].name" == "05"
jsonpath "$.result.resources[33].description" == "A schema with both a title and a description (05)"
jsonpath "$.result.resources[33].mimeType" == "application/schema+json"
jsonpath "$.result.resources[33].size" == 222
jsonpath "$.result.resources[34].uri" == "{{base}}/test/description-only/01"
jsonpath "$.result.resources[34].name" == "01"
jsonpath "$.result.resources[34].description" == "A schema with only a description (01)"
jsonpath "$.result.resources[34].mimeType" == "application/schema+json"
jsonpath "$.result.resources[34].size" == 200
jsonpath "$.result.resources[35].uri" == "{{base}}/test/description-only/02"
jsonpath "$.result.resources[35].name" == "02"
jsonpath "$.result.resources[35].description" == "A schema with only a description (02)"
jsonpath "$.result.resources[35].mimeType" == "application/schema+json"
jsonpath "$.result.resources[35].size" == 200
jsonpath "$.result.resources[36].uri" == "{{base}}/test/description-only/03"
jsonpath "$.result.resources[36].name" == "03"
jsonpath "$.result.resources[36].description" == "A schema with only a description (03)"
jsonpath "$.result.resources[36].mimeType" == "application/schema+json"
jsonpath "$.result.resources[36].size" == 200
jsonpath "$.result.resources[37].uri" == "{{base}}/test/description-only/04"
jsonpath "$.result.resources[37].name" == "04"
jsonpath "$.result.resources[37].description" == "A schema with only a description (04)"
jsonpath "$.result.resources[37].mimeType" == "application/schema+json"
jsonpath "$.result.resources[37].size" == 200
jsonpath "$.result.resources[38].uri" == "{{base}}/test/description-only/05"
jsonpath "$.result.resources[38].name" == "05"
jsonpath "$.result.resources[38].description" == "A schema with only a description (05)"
jsonpath "$.result.resources[38].mimeType" == "application/schema+json"
jsonpath "$.result.resources[38].size" == 200
jsonpath "$.result.resources[39].uri" == "{{base}}/test/title-only/01"
jsonpath "$.result.resources[39].name" == "01"
jsonpath "$.result.resources[39].description" not exists
jsonpath "$.result.resources[39].mimeType" == "application/schema+json"
jsonpath "$.result.resources[39].size" == 164
jsonpath "$.result.resources[40].uri" == "{{base}}/test/title-only/02"
jsonpath "$.result.resources[40].name" == "02"
jsonpath "$.result.resources[40].description" not exists
jsonpath "$.result.resources[40].mimeType" == "application/schema+json"
jsonpath "$.result.resources[40].size" == 164
jsonpath "$.result.resources[41].uri" == "{{base}}/test/title-only/03"
jsonpath "$.result.resources[41].name" == "03"
jsonpath "$.result.resources[41].description" not exists
jsonpath "$.result.resources[41].mimeType" == "application/schema+json"
jsonpath "$.result.resources[41].size" == 164
jsonpath "$.result.resources[42].uri" == "{{base}}/test/title-only/04"
jsonpath "$.result.resources[42].name" == "04"
jsonpath "$.result.resources[42].description" not exists
jsonpath "$.result.resources[42].mimeType" == "application/schema+json"
jsonpath "$.result.resources[42].size" == 164
jsonpath "$.result.resources[43].uri" == "{{base}}/test/title-only/05"
jsonpath "$.result.resources[43].name" == "05"
jsonpath "$.result.resources[43].description" not exists
jsonpath "$.result.resources[43].mimeType" == "application/schema+json"
jsonpath "$.result.resources[43].size" == 164
jsonpath "$.result.resources[44].uri" == "{{base}}/test/bare/01"
jsonpath "$.result.resources[44].name" == "01"
jsonpath "$.result.resources[44].description" not exists
jsonpath "$.result.resources[44].mimeType" == "application/schema+json"
jsonpath "$.result.resources[44].size" == 130
jsonpath "$.result.resources[45].uri" == "{{base}}/test/bare/02"
jsonpath "$.result.resources[45].name" == "02"
jsonpath "$.result.resources[45].description" not exists
jsonpath "$.result.resources[45].mimeType" == "application/schema+json"
jsonpath "$.result.resources[45].size" == 130
jsonpath "$.result.resources[46].uri" == "{{base}}/test/bare/03"
jsonpath "$.result.resources[46].name" == "03"
jsonpath "$.result.resources[46].description" not exists
jsonpath "$.result.resources[46].mimeType" == "application/schema+json"
jsonpath "$.result.resources[46].size" == 130
jsonpath "$.result.resources[47].uri" == "{{base}}/test/bare/04"
jsonpath "$.result.resources[47].name" == "04"
jsonpath "$.result.resources[47].description" not exists
jsonpath "$.result.resources[47].mimeType" == "application/schema+json"
jsonpath "$.result.resources[47].size" == 130
jsonpath "$.result.resources[48].uri" == "{{base}}/test/bare/05"
jsonpath "$.result.resources[48].name" == "05"
jsonpath "$.result.resources[48].description" not exists
jsonpath "$.result.resources[48].mimeType" == "application/schema+json"
jsonpath "$.result.resources[48].size" == 130
jsonpath "$.result.resources[49].uri" == "{{base}}/test/bare/06"
jsonpath "$.result.resources[49].name" == "06"
jsonpath "$.result.resources[49].description" not exists
jsonpath "$.result.resources[49].mimeType" == "application/schema+json"
jsonpath "$.result.resources[49].size" == 130
jsonpath "$.result.nextCursor" == "50"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/list page 2 follows the cursor and emits the remaining 44 entries

POST {{base}}/self/v1/mcp
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
jsonpath "$.result.resources" count == 44
jsonpath "$.result.resources[0].uri" == "{{base}}/test/bare/07"
jsonpath "$.result.resources[0].name" == "07"
jsonpath "$.result.resources[0].description" not exists
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.resources[0].size" == 130
jsonpath "$.result.resources[1].uri" == "{{base}}/test/bare/08"
jsonpath "$.result.resources[1].name" == "08"
jsonpath "$.result.resources[1].description" not exists
jsonpath "$.result.resources[1].mimeType" == "application/schema+json"
jsonpath "$.result.resources[1].size" == 130
jsonpath "$.result.resources[2].uri" == "{{base}}/test/bare/09"
jsonpath "$.result.resources[2].name" == "09"
jsonpath "$.result.resources[2].description" not exists
jsonpath "$.result.resources[2].mimeType" == "application/schema+json"
jsonpath "$.result.resources[2].size" == 130
jsonpath "$.result.resources[3].uri" == "{{base}}/test/bare/10"
jsonpath "$.result.resources[3].name" == "10"
jsonpath "$.result.resources[3].description" not exists
jsonpath "$.result.resources[3].mimeType" == "application/schema+json"
jsonpath "$.result.resources[3].size" == 130
jsonpath "$.result.resources[4].uri" == "{{base}}/test/bare/11"
jsonpath "$.result.resources[4].name" == "11"
jsonpath "$.result.resources[4].description" not exists
jsonpath "$.result.resources[4].mimeType" == "application/schema+json"
jsonpath "$.result.resources[4].size" == 130
jsonpath "$.result.resources[5].uri" == "{{base}}/test/bare/12"
jsonpath "$.result.resources[5].name" == "12"
jsonpath "$.result.resources[5].description" not exists
jsonpath "$.result.resources[5].mimeType" == "application/schema+json"
jsonpath "$.result.resources[5].size" == 130
jsonpath "$.result.resources[6].uri" == "{{base}}/test/bare/13"
jsonpath "$.result.resources[6].name" == "13"
jsonpath "$.result.resources[6].description" not exists
jsonpath "$.result.resources[6].mimeType" == "application/schema+json"
jsonpath "$.result.resources[6].size" == 130
jsonpath "$.result.resources[7].uri" == "{{base}}/test/bare/14"
jsonpath "$.result.resources[7].name" == "14"
jsonpath "$.result.resources[7].description" not exists
jsonpath "$.result.resources[7].mimeType" == "application/schema+json"
jsonpath "$.result.resources[7].size" == 130
jsonpath "$.result.resources[8].uri" == "{{base}}/test/bare/15"
jsonpath "$.result.resources[8].name" == "15"
jsonpath "$.result.resources[8].description" not exists
jsonpath "$.result.resources[8].mimeType" == "application/schema+json"
jsonpath "$.result.resources[8].size" == 130
jsonpath "$.result.resources[9].uri" == "{{base}}/test/bare/16"
jsonpath "$.result.resources[9].name" == "16"
jsonpath "$.result.resources[9].description" not exists
jsonpath "$.result.resources[9].mimeType" == "application/schema+json"
jsonpath "$.result.resources[9].size" == 130
jsonpath "$.result.resources[10].uri" == "{{base}}/test/bare/17"
jsonpath "$.result.resources[10].name" == "17"
jsonpath "$.result.resources[10].description" not exists
jsonpath "$.result.resources[10].mimeType" == "application/schema+json"
jsonpath "$.result.resources[10].size" == 130
jsonpath "$.result.resources[11].uri" == "{{base}}/test/bare/18"
jsonpath "$.result.resources[11].name" == "18"
jsonpath "$.result.resources[11].description" not exists
jsonpath "$.result.resources[11].mimeType" == "application/schema+json"
jsonpath "$.result.resources[11].size" == 130
jsonpath "$.result.resources[12].uri" == "{{base}}/test/bare/19"
jsonpath "$.result.resources[12].name" == "19"
jsonpath "$.result.resources[12].description" not exists
jsonpath "$.result.resources[12].mimeType" == "application/schema+json"
jsonpath "$.result.resources[12].size" == 130
jsonpath "$.result.resources[13].uri" == "{{base}}/test/bare/20"
jsonpath "$.result.resources[13].name" == "20"
jsonpath "$.result.resources[13].description" not exists
jsonpath "$.result.resources[13].mimeType" == "application/schema+json"
jsonpath "$.result.resources[13].size" == 130
jsonpath "$.result.resources[14].uri" == "{{base}}/test/bare/21"
jsonpath "$.result.resources[14].name" == "21"
jsonpath "$.result.resources[14].description" not exists
jsonpath "$.result.resources[14].mimeType" == "application/schema+json"
jsonpath "$.result.resources[14].size" == 130
jsonpath "$.result.resources[15].uri" == "{{base}}/test/bare/22"
jsonpath "$.result.resources[15].name" == "22"
jsonpath "$.result.resources[15].description" not exists
jsonpath "$.result.resources[15].mimeType" == "application/schema+json"
jsonpath "$.result.resources[15].size" == 130
jsonpath "$.result.resources[16].uri" == "{{base}}/test/bare/23"
jsonpath "$.result.resources[16].name" == "23"
jsonpath "$.result.resources[16].description" not exists
jsonpath "$.result.resources[16].mimeType" == "application/schema+json"
jsonpath "$.result.resources[16].size" == 130
jsonpath "$.result.resources[17].uri" == "{{base}}/test/bare/24"
jsonpath "$.result.resources[17].name" == "24"
jsonpath "$.result.resources[17].description" not exists
jsonpath "$.result.resources[17].mimeType" == "application/schema+json"
jsonpath "$.result.resources[17].size" == 130
jsonpath "$.result.resources[18].uri" == "{{base}}/test/bare/25"
jsonpath "$.result.resources[18].name" == "25"
jsonpath "$.result.resources[18].description" not exists
jsonpath "$.result.resources[18].mimeType" == "application/schema+json"
jsonpath "$.result.resources[18].size" == 130
jsonpath "$.result.resources[19].uri" == "{{base}}/test/bare/26"
jsonpath "$.result.resources[19].name" == "26"
jsonpath "$.result.resources[19].description" not exists
jsonpath "$.result.resources[19].mimeType" == "application/schema+json"
jsonpath "$.result.resources[19].size" == 130
jsonpath "$.result.resources[20].uri" == "{{base}}/test/bare/27"
jsonpath "$.result.resources[20].name" == "27"
jsonpath "$.result.resources[20].description" not exists
jsonpath "$.result.resources[20].mimeType" == "application/schema+json"
jsonpath "$.result.resources[20].size" == 130
jsonpath "$.result.resources[21].uri" == "{{base}}/test/bare/28"
jsonpath "$.result.resources[21].name" == "28"
jsonpath "$.result.resources[21].description" not exists
jsonpath "$.result.resources[21].mimeType" == "application/schema+json"
jsonpath "$.result.resources[21].size" == 130
jsonpath "$.result.resources[22].uri" == "{{base}}/test/bare/29"
jsonpath "$.result.resources[22].name" == "29"
jsonpath "$.result.resources[22].description" not exists
jsonpath "$.result.resources[22].mimeType" == "application/schema+json"
jsonpath "$.result.resources[22].size" == 130
jsonpath "$.result.resources[23].uri" == "{{base}}/test/bare/30"
jsonpath "$.result.resources[23].name" == "30"
jsonpath "$.result.resources[23].description" not exists
jsonpath "$.result.resources[23].mimeType" == "application/schema+json"
jsonpath "$.result.resources[23].size" == 130
jsonpath "$.result.resources[24].uri" == "{{base}}/test/bare/31"
jsonpath "$.result.resources[24].name" == "31"
jsonpath "$.result.resources[24].description" not exists
jsonpath "$.result.resources[24].mimeType" == "application/schema+json"
jsonpath "$.result.resources[24].size" == 130
jsonpath "$.result.resources[25].uri" == "{{base}}/test/bare/32"
jsonpath "$.result.resources[25].name" == "32"
jsonpath "$.result.resources[25].description" not exists
jsonpath "$.result.resources[25].mimeType" == "application/schema+json"
jsonpath "$.result.resources[25].size" == 130
jsonpath "$.result.resources[26].uri" == "{{base}}/test/bare/33"
jsonpath "$.result.resources[26].name" == "33"
jsonpath "$.result.resources[26].description" not exists
jsonpath "$.result.resources[26].mimeType" == "application/schema+json"
jsonpath "$.result.resources[26].size" == 130
jsonpath "$.result.resources[27].uri" == "{{base}}/test/bare/34"
jsonpath "$.result.resources[27].name" == "34"
jsonpath "$.result.resources[27].description" not exists
jsonpath "$.result.resources[27].mimeType" == "application/schema+json"
jsonpath "$.result.resources[27].size" == 130
jsonpath "$.result.resources[28].uri" == "{{base}}/test/bare/35"
jsonpath "$.result.resources[28].name" == "35"
jsonpath "$.result.resources[28].description" not exists
jsonpath "$.result.resources[28].mimeType" == "application/schema+json"
jsonpath "$.result.resources[28].size" == 130
jsonpath "$.result.resources[29].uri" == "{{base}}/test/bare/36"
jsonpath "$.result.resources[29].name" == "36"
jsonpath "$.result.resources[29].description" not exists
jsonpath "$.result.resources[29].mimeType" == "application/schema+json"
jsonpath "$.result.resources[29].size" == 130
jsonpath "$.result.resources[30].uri" == "{{base}}/test/bare/37"
jsonpath "$.result.resources[30].name" == "37"
jsonpath "$.result.resources[30].description" not exists
jsonpath "$.result.resources[30].mimeType" == "application/schema+json"
jsonpath "$.result.resources[30].size" == 130
jsonpath "$.result.resources[31].uri" == "{{base}}/test/bare/38"
jsonpath "$.result.resources[31].name" == "38"
jsonpath "$.result.resources[31].description" not exists
jsonpath "$.result.resources[31].mimeType" == "application/schema+json"
jsonpath "$.result.resources[31].size" == 130
jsonpath "$.result.resources[32].uri" == "{{base}}/test/bare/39"
jsonpath "$.result.resources[32].name" == "39"
jsonpath "$.result.resources[32].description" not exists
jsonpath "$.result.resources[32].mimeType" == "application/schema+json"
jsonpath "$.result.resources[32].size" == 130
jsonpath "$.result.resources[33].uri" == "{{base}}/test/bare/40"
jsonpath "$.result.resources[33].name" == "40"
jsonpath "$.result.resources[33].description" not exists
jsonpath "$.result.resources[33].mimeType" == "application/schema+json"
jsonpath "$.result.resources[33].size" == 130
jsonpath "$.result.resources[34].uri" == "{{base}}/test/bare/41"
jsonpath "$.result.resources[34].name" == "41"
jsonpath "$.result.resources[34].description" not exists
jsonpath "$.result.resources[34].mimeType" == "application/schema+json"
jsonpath "$.result.resources[34].size" == 130
jsonpath "$.result.resources[35].uri" == "{{base}}/test/bare/42"
jsonpath "$.result.resources[35].name" == "42"
jsonpath "$.result.resources[35].description" not exists
jsonpath "$.result.resources[35].mimeType" == "application/schema+json"
jsonpath "$.result.resources[35].size" == 130
jsonpath "$.result.resources[36].uri" == "{{base}}/test/bare/43"
jsonpath "$.result.resources[36].name" == "43"
jsonpath "$.result.resources[36].description" not exists
jsonpath "$.result.resources[36].mimeType" == "application/schema+json"
jsonpath "$.result.resources[36].size" == 130
jsonpath "$.result.resources[37].uri" == "{{base}}/test/bare/44"
jsonpath "$.result.resources[37].name" == "44"
jsonpath "$.result.resources[37].description" not exists
jsonpath "$.result.resources[37].mimeType" == "application/schema+json"
jsonpath "$.result.resources[37].size" == 130
jsonpath "$.result.resources[38].uri" == "{{base}}/test/bare/45"
jsonpath "$.result.resources[38].name" == "45"
jsonpath "$.result.resources[38].description" not exists
jsonpath "$.result.resources[38].mimeType" == "application/schema+json"
jsonpath "$.result.resources[38].size" == 130
jsonpath "$.result.resources[39].uri" == "{{base}}/test/bare/46"
jsonpath "$.result.resources[39].name" == "46"
jsonpath "$.result.resources[39].description" not exists
jsonpath "$.result.resources[39].mimeType" == "application/schema+json"
jsonpath "$.result.resources[39].size" == 130
jsonpath "$.result.resources[40].uri" == "{{base}}/test/bare/47"
jsonpath "$.result.resources[40].name" == "47"
jsonpath "$.result.resources[40].description" not exists
jsonpath "$.result.resources[40].mimeType" == "application/schema+json"
jsonpath "$.result.resources[40].size" == 130
jsonpath "$.result.resources[41].uri" == "{{base}}/test/bare/48"
jsonpath "$.result.resources[41].name" == "48"
jsonpath "$.result.resources[41].description" not exists
jsonpath "$.result.resources[41].mimeType" == "application/schema+json"
jsonpath "$.result.resources[41].size" == 130
jsonpath "$.result.resources[42].uri" == "{{base}}/test/bare/49"
jsonpath "$.result.resources[42].name" == "49"
jsonpath "$.result.resources[42].description" not exists
jsonpath "$.result.resources[42].mimeType" == "application/schema+json"
jsonpath "$.result.resources[42].size" == 130
jsonpath "$.result.resources[43].uri" == "{{base}}/test/bare/50"
jsonpath "$.result.resources[43].name" == "50"
jsonpath "$.result.resources[43].description" not exists
jsonpath "$.result.resources[43].mimeType" == "application/schema+json"
jsonpath "$.result.resources[43].size" == 130
jsonpath "$.result.nextCursor" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true


# resources/list rejects malformed cursors

POST {{base}}/self/v1/mcp
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

# resources/list rejects negative cursors

POST {{base}}/self/v1/mcp
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

# resources/list rejects cursors with leading zeros

POST {{base}}/self/v1/mcp
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

# resources/templates/list publishes the single registry URI template

POST {{base}}/self/v1/mcp
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

# resources/read with a malformed URI returns -32002

POST {{base}}/self/v1/mcp
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

# resources/read with a URI in our space but pointing at no schema returns -32002

POST {{base}}/self/v1/mcp
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

# resources/read of a real catalog schema URI

GET {{base}}/test/object
HTTP 200
[Captures]
schema_text: body

POST {{base}}/self/v1/mcp
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

# resources/read accepts the .json extension form of the URI

GET {{base}}/test/object.json
HTTP 200
[Captures]
schema_text: body

POST {{base}}/self/v1/mcp
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

# resources/read with ?bundle=1 of a $ref-bearing schema

GET {{base}}/self/v1/schemas/mcp/response?bundle=1
HTTP 200
[Captures]
bundled_schema_text: body

POST {{base}}/self/v1/mcp
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
