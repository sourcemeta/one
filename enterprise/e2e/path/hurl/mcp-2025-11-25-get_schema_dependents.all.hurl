# Resolve the tool's `outputSchema.$ref` once via `tools/list`. Every
# successful `tools/call` below validates its `structuredContent` against
# this schema so the advertised output shape and the actual one stay in lockstep.
#
# TODO: Drop the `structured_content` body-regex captures in favour of a plain
# `jsonpath "$.result.structuredContent"` once hurl can inline a captured JSON
# object as a request body. Tracked upstream at
# https://github.com/Orange-OpenSource/hurl/issues/2152
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
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
output_schema_path: jsonpath "$.result.tools[?@.name=='get_schema_dependents'].outputSchema['$ref']" regex "https?://[^/]+/v1/catalog(.+)"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.result.tools[?@.name=='get_schema_dependents'].outputSchema['$ref']" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/get-schema-dependents/response"

POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1500,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependents",
    "arguments": { "schema": "{{base}}/v1/catalog/self/v1/schemas/mcp/error" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
text: jsonpath "$.result.content[0].text"
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1500
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 3
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
jsonpath "$.result.structuredContent.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent.results[0].at" == "/$defs/single/anyOf/2/$ref"
variable "text" jsonpath "$.results" count == 1
variable "text" jsonpath "$.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[0].at" == "/$defs/single/anyOf/2/$ref"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1501,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependents",
    "arguments": { "schema": "{{base}}/v1/catalog/self/v1/schemas/mcp/error" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
text: jsonpath "$.result.content[0].text"
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
dependent_uri: jsonpath "$.result.content[2].uri"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1501
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 3
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.results" count == 1
jsonpath "$.result.structuredContent.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
jsonpath "$.result.structuredContent.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
jsonpath "$.result.structuredContent.results[0].at" == "/$defs/single/anyOf/2/$ref"
variable "text" jsonpath "$.results" count == 1
variable "text" jsonpath "$.results[0].from" == "{{base}}/v1/catalog/self/v1/schemas/mcp/response"
variable "text" jsonpath "$.results[0].to" == "{{base}}/v1/catalog/self/v1/schemas/mcp/error"
variable "text" jsonpath "$.results[0].at" == "/$defs/single/anyOf/2/$ref"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/schemas/mcp/response
HTTP 200
[Captures]
schema_text: body
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1502,
  "method": "resources/read",
  "params": {
    "uri": "{{dependent_uri}}"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1502
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{dependent_uri}}"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1599,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependents",
    "arguments": { "schema": "self/v1/schemas/mcp/error" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1599
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/schema/format"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/get-schema-dependents/request#/properties/schema/format"
jsonpath "$.error.data.errors[0].instanceLocation" == "/schema"
jsonpath "$.error.data.errors[0].error" == "The string value \"self/v1/schemas/mcp/error\" was expected to represent a valid URI"
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/get-schema-dependents/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the defined properties subschemas"
jsonpath "$.result" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Schema URI with a fragment is rejected
POST {{base}}/v1/catalog/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1600,
  "method": "tools/call",
  "params": {
    "name": "get_schema_dependents",
    "arguments": { "schema": "{{base}}/v1/catalog/test/object#meta" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1600
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Params fail against the tool request schema"
jsonpath "$.error.data.valid" == false
jsonpath "$.error.data.errors" count == 2
jsonpath "$.error.data.errors[0].keywordLocation" == "/properties/schema/pattern"
jsonpath "$.error.data.errors[0].absoluteKeywordLocation" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/get-schema-dependents/request#/properties/schema/pattern"
jsonpath "$.error.data.errors[0].instanceLocation" == "/schema"
jsonpath "$.error.data.errors[0].error" == "The string value \"{{base}}/v1/catalog/test/object#meta\" was expected to match the regular expression \"^[^#?]*$\""
jsonpath "$.error.data.errors[1].keywordLocation" == "/properties"
jsonpath "$.error.data.errors[1].absoluteKeywordLocation" == "{{base}}/v1/catalog/self/v1/schemas/mcp/tools/call/get-schema-dependents/request#/properties"
jsonpath "$.error.data.errors[1].instanceLocation" == ""
jsonpath "$.error.data.errors[1].error" == "The object value was expected to validate against the defined properties subschemas"
jsonpath "$.result" not exists

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
