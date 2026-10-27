POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: list_directory
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1200,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {},
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
test_path: jsonpath "$.result.structuredContent.entries[2].path"
text: jsonpath "$.result.content[0].text"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1200
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/"
variable "text" jsonpath "$.path" == "/"
jsonpath "$.result.structuredContent.entries[2].name" == "test"
jsonpath "$.result.structuredContent.entries[2].private" == false
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
variable "text" jsonpath "$.entries[2].name" == "test"
variable "text" jsonpath "$.entries[2].private" == false
variable "test_path" == "/test/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: list_directory
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1201,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {
      "path": "test"
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
object_path: jsonpath "$.result.structuredContent.entries[7].path"
text: jsonpath "$.result.content[0].text"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1201
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/test"
variable "text" jsonpath "$.path" == "/test"
jsonpath "$.result.structuredContent.entries[7].name" == "object"
jsonpath "$.result.structuredContent.entries[7].private" == false
variable "text" jsonpath "$.entries[7].name" == "object"
variable "text" jsonpath "$.entries[7].private" == false
jsonpath "$.result.structuredContent.entries[7].type" == "schema"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
variable "text" jsonpath "$.entries[7].type" == "schema"
variable "object_path" == "/test/object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: get_schema_metadata
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1202,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
    "arguments": {
      "schema": "{{base}}{{object_path}}"
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
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1202
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.metaschema" == false
jsonpath "$.result.structuredContent.conversions.*" count == 3
jsonpath "$.result.structuredContent.conversions['2020-12'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.conversions['openapi3.1'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.conversions['openapi3.2'].mediaType" == "application/schema+json"
variable "text" jsonpath "$.metaschema" == false
variable "text" jsonpath "$.conversions.*" count == 3
variable "text" jsonpath "$.conversions['2020-12'].mediaType" == "application/schema+json"
variable "text" jsonpath "$.conversions['openapi3.1'].mediaType" == "application/schema+json"
variable "text" jsonpath "$.conversions['openapi3.2'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.path" == "/test/object"
variable "text" jsonpath "$.path" == "/test/object"
jsonpath "$.result.structuredContent.title" == "My Object"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
variable "text" jsonpath "$.title" == "My Object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: search_schemas
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1210,
  "method": "tools/call",
  "params": {
    "name": "search_schemas",
    "arguments": {
      "q": "Object"
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
hit_path: jsonpath "$.result.structuredContent.results[0].path"
text: jsonpath "$.result.content[0].text"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1210
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.results" count == 1
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
variable "text" jsonpath "$.results" count == 1
variable "hit_path" == "/test/object"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: get_schema_metadata
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1211,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
    "arguments": {
      "schema": "{{base}}{{hit_path}}"
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
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1211
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.metaschema" == false
jsonpath "$.result.structuredContent.conversions.*" count == 3
jsonpath "$.result.structuredContent.conversions['2020-12'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.conversions['openapi3.1'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.conversions['openapi3.2'].mediaType" == "application/schema+json"
variable "text" jsonpath "$.metaschema" == false
variable "text" jsonpath "$.conversions.*" count == 3
variable "text" jsonpath "$.conversions['2020-12'].mediaType" == "application/schema+json"
variable "text" jsonpath "$.conversions['openapi3.1'].mediaType" == "application/schema+json"
variable "text" jsonpath "$.conversions['openapi3.2'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.title" == "My Object"
variable "text" jsonpath "$.title" == "My Object"
jsonpath "$.result.structuredContent.health" == 67
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
variable "text" jsonpath "$.health" == 67

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: evaluate_schema
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1220,
  "method": "tools/call",
  "params": {
    "name": "evaluate_schema",
    "arguments": {
      "schema": "{{base}}/test/bare/01",
      "stringifiedInstance": "42"
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
error_location: jsonpath "$.result.structuredContent.errors[0].absoluteKeywordLocation"
text: jsonpath "$.result.content[0].text"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1220
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.valid" == false
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
variable "text" jsonpath "$.valid" == false
variable "error_location" == "{{base}}/test/bare/01#/type"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: trace_schema_evaluation
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1230,
  "method": "tools/call",
  "params": {
    "name": "trace_schema_evaluation",
    "arguments": {
      "schema": "{{base}}/test/bare/01",
      "stringifiedInstance": "42"
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
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1230
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.valid" == false
variable "text" jsonpath "$.valid" == false
jsonpath "$.result.structuredContent.steps[1].type" == "fail"
variable "text" jsonpath "$.steps[1].type" == "fail"
jsonpath "$.result.structuredContent.steps[1].keywordLocation" == "{{error_location}}"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
variable "text" jsonpath "$.steps[1].keywordLocation" == "{{error_location}}"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
