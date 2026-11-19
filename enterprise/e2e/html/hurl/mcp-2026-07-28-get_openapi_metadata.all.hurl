# Resolve the tool's `outputSchema.$ref` once via `tools/list`. Every
# successful `tools/call` below validates its `structuredContent` against
# this schema so the advertised output shape and the actual one stay in lockstep.
#
# TODO: Drop the `structured_content` body-regex captures in favour of a plain
# `jsonpath "$.result.structuredContent"` once hurl can inline a captured JSON
# object as a request body. Tracked upstream at
# https://github.com/Orange-OpenSource/hurl/issues/2152
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/list
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "tools/list",
  "params": {
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "hurl",
        "version": "1.0.0"
      }
    }
  }
}
```
HTTP 200
Cache-Control: no-store
[Captures]
output_schema_path: jsonpath "$.result.tools[?@.name=='get_openapi_metadata'].outputSchema['$ref']" regex "https?://[^/]+(.+)"
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.result.tools[?@.name=='get_openapi_metadata'].outputSchema['$ref']" == "{{base}}/self/v1/schemas/mcp/tools/call/get-openapi-metadata/response"
jsonpath "$.result.resultType" == "complete"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# What a description answers about itself over MCP, which is the same document
# the REST surface serves

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: get_openapi_metadata
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 800,
  "method": "tools/call",
  "params": {
    "name": "get_openapi_metadata",
    "arguments": {
      "openapi": "{{base}}/test/openapi-split-entry"
    },
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "hurl",
        "version": "1.0.0"
      }
    }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
structured_content: body regex "(?s)\"structuredContent\": ((?:\\{.*?\n    \\})|(?:\\[.*?\n    \\]))"
[Asserts]
header "Vary" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 800
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.bytes" == 818
variable "text" jsonpath "$.bytes" == 818
jsonpath "$.result.structuredContent.bytesBundled" == 1741
variable "text" jsonpath "$.bytesBundled" == 1741
jsonpath "$.result.structuredContent.identifier" == "{{base}}/test/openapi-split-entry"
variable "text" jsonpath "$.identifier" == "{{base}}/test/openapi-split-entry"
jsonpath "$.result.structuredContent.path" == "/test/openapi-split-entry"
variable "text" jsonpath "$.path" == "/test/openapi-split-entry"
jsonpath "$.result.structuredContent.openapi" == "3.2"
variable "text" jsonpath "$.openapi" == "3.2"
jsonpath "$.result.structuredContent.version" == "1.0.0"
variable "text" jsonpath "$.version" == "1.0.0"
jsonpath "$.result.structuredContent.dialect" == "https://spec.openapis.org/oas/3.2/dialect/2025-09-17"
variable "text" jsonpath "$.dialect" == "https://spec.openapis.org/oas/3.2/dialect/2025-09-17"
jsonpath "$.result.structuredContent.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.title" == "Split"
variable "text" jsonpath "$.title" == "Split"
jsonpath "$.result.structuredContent.health" == 100
variable "text" jsonpath "$.health" == 100
jsonpath "$.result.structuredContent.operations" == 0
variable "text" jsonpath "$.operations" == 0
jsonpath "$.result.structuredContent.dependencies" == 5
variable "text" jsonpath "$.dependencies" == 5
jsonpath "$.result.structuredContent.alert" == null
variable "text" jsonpath "$.alert" == null
jsonpath "$.result.structuredContent.priority" == 50
variable "text" jsonpath "$.priority" == 50
jsonpath "$.result.structuredContent.private" == false
variable "text" jsonpath "$.private" == false
jsonpath "$.result.structuredContent.breadcrumb" count == 2
variable "text" jsonpath "$.breadcrumb" count == 2
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "test"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/test/"
jsonpath "$.result.structuredContent.breadcrumb[1].name" == "openapi-split-entry"
jsonpath "$.result.structuredContent.breadcrumb[1].path" == "/test/openapi-split-entry"
jsonpath "$.result.structuredContent.metaschema" not exists
jsonpath "$.result.structuredContent.conversions" not exists
jsonpath "$.result.resultType" == "complete"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{output_schema_path}}
```
{{structured_content}}
```
HTTP 200
Cache-Control: no-store
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# A schema keeps its own metadata, so this tool holds nothing for one
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: get_openapi_metadata
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 801,
  "method": "tools/call",
  "params": {
    "name": "get_openapi_metadata",
    "arguments": {
      "openapi": "{{base}}/test/object"
    },
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "hurl",
        "version": "1.0.0"
      }
    }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.id" == 801
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "OpenAPI description not found"
jsonpath "$.result.structuredContent" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Nor for a path nothing here answers to
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: get_openapi_metadata
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 802,
  "method": "tools/call",
  "params": {
    "name": "get_openapi_metadata",
    "arguments": {
      "openapi": "{{base}}/test/does-not-exist"
    },
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "hurl",
        "version": "1.0.0"
      }
    }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.id" == 802
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].text" == "OpenAPI description not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
