# Protocol revision 2026-07-28 against an instance that holds
# nothing.

# Discovery is the one request this revision requires every server to
# answer, and an instance holding nothing still answers it
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: server/discover
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "server/discover",
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.result.supportedVersions" count == 4
jsonpath "$.result.supportedVersions[0]" == "2025-03-26"
jsonpath "$.result.supportedVersions[1]" == "2025-06-18"
jsonpath "$.result.supportedVersions[2]" == "2025-11-25"
jsonpath "$.result.supportedVersions[3]" == "2026-07-28"
jsonpath "$.result.capabilities.resources" isObject
jsonpath "$.result.capabilities.resources" isEmpty
jsonpath "$.result.instructions" == "This is an instance of Sourcemeta One named \"Sourcemeta\" (The next-generation JSON Schema platform). Sourcemeta One is a JSON Schema registry. It serves a catalog of JSON Schemas organized as a tree of directories. Every schema is exposed as an MCP resource via the `JSON Schema` resource template (see `resources/templates/list`) and is addressable by its canonical absolute URI. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Use `tools/list` to discover the operations this catalog exposes. Because this server is a JSON Schema registry, the schemas defining each tool's input parameters are themselves resources in this registry. To inspect a tool's full parameter shape, call `resources/read` on the `$ref` URL in that tool's `inputSchema`. Learn more at https://one.sourcemeta.com"
jsonpath "$.result.ttlMs" == 300000
jsonpath "$.result.cacheScope" == "public"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].description" == "The next-generation JSON Schema platform"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].websiteUrl" == "{{base}}"

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

# An instance configured with no catalog of its own still serves the
# schemas that describe it, so a listing names those and the pages
# are cut where any other listing would be
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: resources/list
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "resources/list",
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.id" == 2
jsonpath "$.result.resources" count == 50
jsonpath "$.result.nextCursor" == "50"
jsonpath "$.result.ttlMs" == 300000
jsonpath "$.result.cacheScope" == "public"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].description" == "The next-generation JSON Schema platform"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].websiteUrl" == "{{base}}"

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

# The last page says so by naming no page after it, and says the same
# thing about who may keep it as the page before it did
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: resources/list
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 3,
  "method": "resources/list",
  "params": {
    "cursor": "50",
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.id" == 3
jsonpath "$.result.resources" count == 17
jsonpath "$.result.nextCursor" not exists
jsonpath "$.result.ttlMs" == 300000
jsonpath "$.result.cacheScope" == "public"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].description" == "The next-generation JSON Schema platform"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].websiteUrl" == "{{base}}"

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

# Anything past the last page is not a page at all
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: resources/list
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 30,
  "method": "resources/list",
  "params": {
    "cursor": "100",
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.id" == 30
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid resource list cursor"

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

# The template that says how a schema is addressed is published even
# where there is no schema to address yet
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: resources/templates/list
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 4,
  "method": "resources/templates/list",
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.id" == 4
jsonpath "$.result.resourceTemplates" count == 1
jsonpath "$.result.resourceTemplates[0].name" == "JSON Schema"
jsonpath "$.result.ttlMs" == 300000
jsonpath "$.result.cacheScope" == "public"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].description" == "The next-generation JSON Schema platform"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].websiteUrl" == "{{base}}"

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

# The handshake this revision retired is missing rather than answered,
# which is what a client needs in order to stop asking for it
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: initialize
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 5,
  "method": "initialize",
  "params": {
    "protocolVersion": "2026-07-28",
    "capabilities": {},
    "clientInfo": {
      "name": "hurl",
      "version": "1.0.0"
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
HTTP 404
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.id" == 5
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"

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
