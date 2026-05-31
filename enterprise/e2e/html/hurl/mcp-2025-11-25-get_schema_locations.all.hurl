# Resolve the tool's `outputSchema.$ref` once via `tools/list`. Every
# successful `tools/call` below validates its `structuredContent` against
# this schema so the advertised output shape and the actual one stay in lockstep.
#
# TODO: Drop the `structured_content` body-regex captures in favour of a plain
# `jsonpath "$.result.structuredContent"` once hurl can inline a captured JSON
# object as a request body. Tracked upstream at
# https://github.com/Orange-OpenSource/hurl/issues/2152
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "tools/list"
}
```
HTTP 200
[Captures]
output_schema_path: jsonpath "$.result.tools[?@.name=='get_schema_locations'].outputSchema['$ref']" regex "https?://[^/]+(.+)"
[Asserts]
jsonpath "$.result.tools[?@.name=='get_schema_locations'].outputSchema['$ref']" == "{{base}}/self/v1/schemas/mcp/tools/call/get-schema-locations/response"

POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 500,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/bare/01" }
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
text: jsonpath "$.result.content[0].text"
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 500
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.dynamic" exists
variable "text" jsonpath "$.dynamic" exists
jsonpath "$.result.structuredContent.static['{{base}}/test/bare/01'].type" == "resource"
variable "text" jsonpath "$.static['{{base}}/test/bare/01'].type" == "resource"
jsonpath "$.result.structuredContent.static['{{base}}/test/bare/01'].pointer" == ""
variable "text" jsonpath "$.static['{{base}}/test/bare/01'].pointer" == ""
jsonpath "$.result.structuredContent.static['{{base}}/test/bare/01'].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.static['{{base}}/test/bare/01'].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.static['{{base}}/test/bare/01'].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.static['{{base}}/test/bare/01'].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.static['{{base}}/test/bare/01#/$id'].type" == "pointer"
variable "text" jsonpath "$.static['{{base}}/test/bare/01#/$id'].type" == "pointer"
jsonpath "$.result.structuredContent.static['{{base}}/test/bare/01#/$schema'].type" == "pointer"
variable "text" jsonpath "$.static['{{base}}/test/bare/01#/$schema'].type" == "pointer"
jsonpath "$.result.structuredContent.static['{{base}}/test/bare/01#/type'].type" == "pointer"
variable "text" jsonpath "$.static['{{base}}/test/bare/01#/type'].type" == "pointer"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 501,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/object" }
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
text: jsonpath "$.result.content[0].text"
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 501
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.dynamic" exists
variable "text" jsonpath "$.dynamic" exists
jsonpath "$.result.structuredContent.static['{{base}}/test/object'].type" == "resource"
variable "text" jsonpath "$.static['{{base}}/test/object'].type" == "resource"
jsonpath "$.result.structuredContent.static['{{base}}/test/object#/title'].type" == "pointer"
variable "text" jsonpath "$.static['{{base}}/test/object#/title'].type" == "pointer"
jsonpath "$.result.structuredContent.static['{{base}}/test/object#/description'].type" == "pointer"
variable "text" jsonpath "$.static['{{base}}/test/object#/description'].type" == "pointer"
jsonpath "$.result.structuredContent.static['{{base}}/test/object#/properties/foo'].type" == "subschema"
variable "text" jsonpath "$.static['{{base}}/test/object#/properties/foo'].type" == "subschema"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 502,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "test/object" }
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
jsonpath "$.id" == 502
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid tool input schema URI"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment or query parameters"
jsonpath "$.result" not exists

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
  "id": 510,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/does-not-exist" }
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
jsonpath "$.id" == 510
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Schema not found"

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
  "id": 511,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test" }
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
jsonpath "$.id" == 511
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Schema not found"

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
  "id": 512,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "https://example.com/test/object" }
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
jsonpath "$.id" == 512
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].text" == "Schema not found"

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
  "id": 513,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/foo/../object" }
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
text: jsonpath "$.result.content[0].text"
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 513
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" exists
variable "text" jsonpath "$" exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 514,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/../etc/passwd" }
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
jsonpath "$.id" == 514
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].text" == "Schema not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Schema URI with a fragment is rejected
POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 515,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/object#meta" }
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
jsonpath "$.id" == 515
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid tool input schema URI"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment or query parameters"
jsonpath "$.result" not exists

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
  "id": 520,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": {}
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
jsonpath "$.id" == 520
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/mcp/tools/call/get-schema-locations/request#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The value was expected to be an object that defines the property \"schema\""

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
  "id": 521,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "" }
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
jsonpath "$.id" == 521
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid tool input schema URI"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment or query parameters"
jsonpath "$.result" not exists

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
  "id": 522,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": 42 }
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
jsonpath "$.id" == 522
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/schema/type"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/mcp/tools/call/get-schema-locations/request#/properties/schema/type"
jsonpath "$.error.data.errors[0].instanceLocation" == "/schema"
jsonpath "$.error.data.errors[0].error" == "The value was expected to be of type string but it was of type integer"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/mcp/tools/call/get-schema-locations/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the defined properties subschemas"

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
  "id": 523,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/object", "extra": "foo" }
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
jsonpath "$.id" == 523
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/additionalProperties"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/mcp/tools/call/get-schema-locations/request#/additionalProperties"
jsonpath "$.error.data.errors[0].instanceLocation" == "/extra"
jsonpath "$.error.data.errors[0].error" == "The object value was not expected to define the property \"extra\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/additionalProperties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/mcp/tools/call/get-schema-locations/request#/additionalProperties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was not expected to define additional properties"

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
  "id": 524,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations"
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
jsonpath "$.id" == 524
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 1
jsonpath "$.error.data.errors[0].keywordLocation" == "/required"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/self/v1/schemas/mcp/tools/call/get-schema-locations/request#/required"
jsonpath "$.error.data.errors[0].instanceLocation" == ""
jsonpath "$.error.data.errors[0].error" == "The value was expected to be an object that defines the property \"schema\""

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
  "id": 525,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": null
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
jsonpath "$.id" == 525
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"

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
  "id": "locations-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/object" }
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
text: jsonpath "$.result.content[0].text"
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "locations-call-string-id"
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent" exists
variable "text" jsonpath "$" exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 700,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/object" }
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
locations_baseline_text: jsonpath "$.result.content[0].text"
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 700
jsonpath "$.result.isError" == false

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 701,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/TEST/OBJECT" }
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
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 701
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{locations_baseline_text}}"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 702,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/object.JSON" }
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
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 702
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{locations_baseline_text}}"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 703,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "HTTP://LOCALHOST:{{port}}/test/object" }
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
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 703
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" == "{{locations_baseline_text}}"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
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
  "id": 704,
  "method": "tools/call",
  "params": {
    "name": "get_schema_locations",
    "arguments": { "schema": "{{base}}/test/object?foo=bar" }
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
jsonpath "$.id" == 704
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid tool input schema URI"
jsonpath "$.error.data" == "The schema must be an absolute URI without a fragment or query parameters"
jsonpath "$.result" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
