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
jsonpath "$.result.resources" count == 32
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
jsonpath "$.result.resources[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependencies/response"
jsonpath "$.result.resources[2].name" == "response"
jsonpath "$.result.resources[2].description" == "The response format for the schema dependencies API endpoint"
jsonpath "$.result.resources[2].mimeType" == "application/schema+json"
jsonpath "$.result.resources[2].size" == 1112
jsonpath "$.result.resources[3].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/dependents/response"
jsonpath "$.result.resources[3].name" == "response"
jsonpath "$.result.resources[3].description" == "The response format for the schema dependents API endpoint"
jsonpath "$.result.resources[3].mimeType" == "application/schema+json"
jsonpath "$.result.resources[3].size" == 907
jsonpath "$.result.resources[4].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/evaluate/response"
jsonpath "$.result.resources[4].name" == "response"
jsonpath "$.result.resources[4].description" == "The response format for the schema evaluation API endpoint"
jsonpath "$.result.resources[4].mimeType" == "application/schema+json"
jsonpath "$.result.resources[4].size" == 3288
jsonpath "$.result.resources[5].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/health/response"
jsonpath "$.result.resources[5].name" == "response"
jsonpath "$.result.resources[5].description" == "The response format for the schema health API endpoint"
jsonpath "$.result.resources[5].mimeType" == "application/schema+json"
jsonpath "$.result.resources[5].size" == 1633
jsonpath "$.result.resources[6].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/locations/response"
jsonpath "$.result.resources[6].name" == "response"
jsonpath "$.result.resources[6].description" == "The response format for the schema frame locations API endpoint"
jsonpath "$.result.resources[6].mimeType" == "application/schema+json"
jsonpath "$.result.resources[6].size" == 2679
jsonpath "$.result.resources[7].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/metadata/response"
jsonpath "$.result.resources[7].name" == "response"
jsonpath "$.result.resources[7].description" == "The response format for the schema metadata API endpoint"
jsonpath "$.result.resources[7].mimeType" == "application/schema+json"
jsonpath "$.result.resources[7].size" == 2887
jsonpath "$.result.resources[8].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/position"
jsonpath "$.result.resources[8].name" == "position"
jsonpath "$.result.resources[8].description" == "Represents a position in a JSON document as line start, column start, line end, and column end"
jsonpath "$.result.resources[8].mimeType" == "application/schema+json"
jsonpath "$.result.resources[8].size" == 457
jsonpath "$.result.resources[9].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/positions/response"
jsonpath "$.result.resources[9].name" == "response"
jsonpath "$.result.resources[9].description" == "The response format for the schema positions API endpoint"
jsonpath "$.result.resources[9].mimeType" == "application/schema+json"
jsonpath "$.result.resources[9].size" == 446
jsonpath "$.result.resources[10].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/search/response"
jsonpath "$.result.resources[10].name" == "response"
jsonpath "$.result.resources[10].description" == "The response format for the schema search API endpoint"
jsonpath "$.result.resources[10].mimeType" == "application/schema+json"
jsonpath "$.result.resources[10].size" == 887
jsonpath "$.result.resources[11].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/stats/response"
jsonpath "$.result.resources[11].name" == "response"
jsonpath "$.result.resources[11].description" == "The response format for the schema statistics API endpoint"
jsonpath "$.result.resources[11].mimeType" == "application/schema+json"
jsonpath "$.result.resources[11].size" == 923
jsonpath "$.result.resources[12].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/schemas/trace/response"
jsonpath "$.result.resources[12].name" == "response"
jsonpath "$.result.resources[12].description" == "The response format for the schema trace API endpoint"
jsonpath "$.result.resources[12].mimeType" == "application/schema+json"
jsonpath "$.result.resources[12].size" == 2281
jsonpath "$.result.resources[13].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.resources[13].name" == "error"
jsonpath "$.result.resources[13].description" == "The error response Sourcemeta One returns when an MCP request fails"
jsonpath "$.result.resources[13].mimeType" == "application/schema+json"
jsonpath "$.result.resources[13].size" == 5813
jsonpath "$.result.resources[14].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/request"
jsonpath "$.result.resources[14].name" == "request"
jsonpath "$.result.resources[14].description" == "Connection handshake from a client"
jsonpath "$.result.resources[14].mimeType" == "application/schema+json"
jsonpath "$.result.resources[14].size" == 4538
jsonpath "$.result.resources[15].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.resources[15].name" == "response"
jsonpath "$.result.resources[15].description" == "Sourcemeta One's response to a client's initialization"
jsonpath "$.result.resources[15].mimeType" == "application/schema+json"
jsonpath "$.result.resources[15].size" == 1858
jsonpath "$.result.resources[16].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/notifications/cancelled"
jsonpath "$.result.resources[16].name" == "cancelled"
jsonpath "$.result.resources[16].description" == "Cancellation hint from a client for an in-flight request. Sourcemeta One accepts and discards these"
jsonpath "$.result.resources[16].mimeType" == "application/schema+json"
jsonpath "$.result.resources[16].size" == 1174
jsonpath "$.result.resources[17].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/notifications/initialized"
jsonpath "$.result.resources[17].name" == "initialized"
jsonpath "$.result.resources[17].description" == "Client signal that the handshake is complete and normal operation may begin"
jsonpath "$.result.resources[17].mimeType" == "application/schema+json"
jsonpath "$.result.resources[17].size" == 751
jsonpath "$.result.resources[18].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/request"
jsonpath "$.result.resources[18].name" == "request"
jsonpath "$.result.resources[18].description" == "Liveness check from a client"
jsonpath "$.result.resources[18].mimeType" == "application/schema+json"
jsonpath "$.result.resources[18].size" == 706
jsonpath "$.result.resources[19].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.resources[19].name" == "response"
jsonpath "$.result.resources[19].description" == "Sourcemeta One's response to a liveness check"
jsonpath "$.result.resources[19].mimeType" == "application/schema+json"
jsonpath "$.result.resources[19].size" == 609
jsonpath "$.result.resources[20].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/request"
jsonpath "$.result.resources[20].name" == "request"
jsonpath "$.result.resources[20].description" == "Any incoming MCP request the server accepts"
jsonpath "$.result.resources[20].mimeType" == "application/schema+json"
jsonpath "$.result.resources[20].size" == 1435
jsonpath "$.result.resources[21].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/request"
jsonpath "$.result.resources[21].name" == "request"
jsonpath "$.result.resources[21].description" == "Request from a client to list MCP resources"
jsonpath "$.result.resources[21].mimeType" == "application/schema+json"
jsonpath "$.result.resources[21].size" == 1082
jsonpath "$.result.resources[22].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.resources[22].name" == "response"
jsonpath "$.result.resources[22].description" == "Sourcemeta One's list of MCP resources"
jsonpath "$.result.resources[22].mimeType" == "application/schema+json"
jsonpath "$.result.resources[22].size" == 2547
jsonpath "$.result.resources[23].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/request"
jsonpath "$.result.resources[23].name" == "request"
jsonpath "$.result.resources[23].description" == "Request from a client to read an MCP resource"
jsonpath "$.result.resources[23].mimeType" == "application/schema+json"
jsonpath "$.result.resources[23].size" == 1342
jsonpath "$.result.resources[24].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.resources[24].name" == "response"
jsonpath "$.result.resources[24].description" == "Sourcemeta One's response containing an MCP resource"
jsonpath "$.result.resources[24].mimeType" == "application/schema+json"
jsonpath "$.result.resources[24].size" == 1803
jsonpath "$.result.resources[25].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/request"
jsonpath "$.result.resources[25].name" == "request"
jsonpath "$.result.resources[25].description" == "Request from a client to list MCP resource templates"
jsonpath "$.result.resources[25].mimeType" == "application/schema+json"
jsonpath "$.result.resources[25].size" == 933
jsonpath "$.result.resources[26].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.resources[26].name" == "response"
jsonpath "$.result.resources[26].description" == "Sourcemeta One's list of MCP resource templates"
jsonpath "$.result.resources[26].mimeType" == "application/schema+json"
jsonpath "$.result.resources[26].size" == 1926
jsonpath "$.result.resources[27].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.resources[27].name" == "response"
jsonpath "$.result.resources[27].description" == "Any outgoing MCP response the server returns"
jsonpath "$.result.resources[27].mimeType" == "application/schema+json"
jsonpath "$.result.resources[27].size" == 1954
jsonpath "$.result.resources[28].uri" == "{{base}}/v1/catalog/example/full"
jsonpath "$.result.resources[28].name" == "full"
jsonpath "$.result.resources[28].description" == "A schema that has both a title and a description"
jsonpath "$.result.resources[28].mimeType" == "application/schema+json"
jsonpath "$.result.resources[28].size" == 242
jsonpath "$.result.resources[29].uri" == "{{base}}/v1/catalog/example/description-only"
jsonpath "$.result.resources[29].name" == "description-only"
jsonpath "$.result.resources[29].description" == "A schema with only a description and no title"
jsonpath "$.result.resources[29].mimeType" == "application/schema+json"
jsonpath "$.result.resources[29].size" == 219
jsonpath "$.result.resources[30].uri" == "{{base}}/v1/catalog/example/title-only"
jsonpath "$.result.resources[30].name" == "title-only"
jsonpath "$.result.resources[30].description" not exists
jsonpath "$.result.resources[30].mimeType" == "application/schema+json"
jsonpath "$.result.resources[30].size" == 185
jsonpath "$.result.resources[31].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.resources[31].name" == "bare"
jsonpath "$.result.resources[31].description" not exists
jsonpath "$.result.resources[31].mimeType" == "application/schema+json"
jsonpath "$.result.resources[31].size" == 141
jsonpath "$.result.nextCursor" not exists

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
