# get_schema_dependencies on a base-path-prefixed sandbox. The
# mcp/response umbrella schema $refs eight other schemas, so the
# response surfaces 9 unique in-catalog URIs (the 8 dependencies plus
# the schema itself as the `from` of every entry). All URIs include
# the /v1/catalog prefix.

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1400,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "{{base}}/v1/catalog/self/v1/schemas/mcp/response" }
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
jsonpath "$.id" == 1400
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 10
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.content[6].type" == "resource_link"
jsonpath "$.result.content[6].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.content[6].mimeType" == "application/schema+json"
jsonpath "$.result.content[7].type" == "resource_link"
jsonpath "$.result.content[7].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.content[7].mimeType" == "application/schema+json"
jsonpath "$.result.content[8].type" == "resource_link"
jsonpath "$.result.content[8].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.content[8].mimeType" == "application/schema+json"
jsonpath "$.result.content[9].type" == "resource_link"
jsonpath "$.result.content[9].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.content[9].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent" count == 8
jsonpath "$.result.structuredContent[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.structuredContent[0].at" == "/anyOf/0/$ref"
jsonpath "$.result.structuredContent[1].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[1].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.structuredContent[1].at" == "/anyOf/1/$ref"
jsonpath "$.result.structuredContent[2].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[2].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent[2].at" == "/anyOf/2/$ref"
jsonpath "$.result.structuredContent[3].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[3].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.structuredContent[3].at" == "/anyOf/3/$ref"
jsonpath "$.result.structuredContent[4].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[4].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.structuredContent[4].at" == "/anyOf/4/$ref"
jsonpath "$.result.structuredContent[5].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[5].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.structuredContent[5].at" == "/anyOf/5/$ref"
jsonpath "$.result.structuredContent[6].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[6].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.structuredContent[6].at" == "/anyOf/6/$ref"
jsonpath "$.result.structuredContent[7].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[7].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.structuredContent[7].at" == "/anyOf/7/$ref"
variable "text" jsonpath "$" count == 8
variable "text" jsonpath "$[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
variable "text" jsonpath "$[0].at" == "/anyOf/0/$ref"
variable "text" jsonpath "$[1].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[1].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
variable "text" jsonpath "$[1].at" == "/anyOf/1/$ref"
variable "text" jsonpath "$[2].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[2].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$[2].at" == "/anyOf/2/$ref"
variable "text" jsonpath "$[3].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[3].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
variable "text" jsonpath "$[3].at" == "/anyOf/3/$ref"
variable "text" jsonpath "$[4].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[4].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
variable "text" jsonpath "$[4].at" == "/anyOf/4/$ref"
variable "text" jsonpath "$[5].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[5].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
variable "text" jsonpath "$[5].at" == "/anyOf/5/$ref"
variable "text" jsonpath "$[6].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[6].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
variable "text" jsonpath "$[6].at" == "/anyOf/6/$ref"
variable "text" jsonpath "$[7].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[7].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
variable "text" jsonpath "$[7].at" == "/anyOf/7/$ref"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Round-trip: capture a resource_link URI from a dependency entry and
# read it back through resources/read.

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1401,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependencies",
    "arguments": { "schema": "{{base}}/v1/catalog/self/v1/schemas/mcp/response" }
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
dependency_uri: jsonpath "$.result.content[1].uri"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1401
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 10
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.content[6].type" == "resource_link"
jsonpath "$.result.content[6].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.content[6].mimeType" == "application/schema+json"
jsonpath "$.result.content[7].type" == "resource_link"
jsonpath "$.result.content[7].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.content[7].mimeType" == "application/schema+json"
jsonpath "$.result.content[8].type" == "resource_link"
jsonpath "$.result.content[8].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.content[8].mimeType" == "application/schema+json"
jsonpath "$.result.content[9].type" == "resource_link"
jsonpath "$.result.content[9].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.content[9].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent" count == 8
jsonpath "$.result.structuredContent[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
jsonpath "$.result.structuredContent[0].at" == "/anyOf/0/$ref"
jsonpath "$.result.structuredContent[1].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[1].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
jsonpath "$.result.structuredContent[1].at" == "/anyOf/1/$ref"
jsonpath "$.result.structuredContent[2].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[2].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent[2].at" == "/anyOf/2/$ref"
jsonpath "$.result.structuredContent[3].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[3].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
jsonpath "$.result.structuredContent[3].at" == "/anyOf/3/$ref"
jsonpath "$.result.structuredContent[4].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[4].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
jsonpath "$.result.structuredContent[4].at" == "/anyOf/4/$ref"
jsonpath "$.result.structuredContent[5].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[5].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
jsonpath "$.result.structuredContent[5].at" == "/anyOf/5/$ref"
jsonpath "$.result.structuredContent[6].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[6].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
jsonpath "$.result.structuredContent[6].at" == "/anyOf/6/$ref"
jsonpath "$.result.structuredContent[7].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent[7].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
jsonpath "$.result.structuredContent[7].at" == "/anyOf/7/$ref"
variable "text" jsonpath "$" count == 8
variable "text" jsonpath "$[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/initialize/response"
variable "text" jsonpath "$[0].at" == "/anyOf/0/$ref"
variable "text" jsonpath "$[1].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[1].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/ping/response"
variable "text" jsonpath "$[1].at" == "/anyOf/1/$ref"
variable "text" jsonpath "$[2].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[2].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$[2].at" == "/anyOf/2/$ref"
variable "text" jsonpath "$[3].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[3].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/list/response"
variable "text" jsonpath "$[3].at" == "/anyOf/3/$ref"
variable "text" jsonpath "$[4].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[4].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/templates/list/response"
variable "text" jsonpath "$[4].at" == "/anyOf/4/$ref"
variable "text" jsonpath "$[5].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[5].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/resources/read/response"
variable "text" jsonpath "$[5].at" == "/anyOf/5/$ref"
variable "text" jsonpath "$[6].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[6].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/list/response"
variable "text" jsonpath "$[6].at" == "/anyOf/6/$ref"
variable "text" jsonpath "$[7].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$[7].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/response"
variable "text" jsonpath "$[7].at" == "/anyOf/7/$ref"

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
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1402,
  "method": "resources/read",
  "params": {
    "uri": "{{dependency_uri}}"
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
jsonpath "$.id" == 1402
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{dependency_uri}}"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
