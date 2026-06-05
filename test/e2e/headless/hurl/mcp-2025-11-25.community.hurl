POST {{base}}/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "protocolVersion": "2025-11-25",
    "capabilities": {},
    "clientInfo": { "name": "hurl", "version": "1.0.0" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.result.protocolVersion" == "2025-11-25"
jsonpath "$.result.capabilities" isObject
jsonpath "$.result.capabilities.resources" isObject
jsonpath "$.result.capabilities.resources" isEmpty
jsonpath "$.result.serverInfo.name" == "sourcemeta-one"
jsonpath "$.result.serverInfo.title" == "Sourcemeta One"
jsonpath "$.result.serverInfo.version" matches "^.+$"
jsonpath "$.result.serverInfo.description" not exists
jsonpath "$.result.serverInfo.websiteUrl" not exists
jsonpath "$.result.instructions" == "This is an instance of Sourcemeta One. Sourcemeta One is a JSON Schema registry. It serves a catalog of JSON Schemas organized as a tree of directories. Every schema is exposed as an MCP resource via the `JSON Schema` resource template (see `resources/templates/list`) and is addressable by its canonical absolute URI. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Use `tools/list` to discover the operations this catalog exposes. Because this server is a JSON Schema registry, the schemas defining each tool's input parameters are themselves resources in this registry. To inspect a tool's full parameter shape, call `resources/read` on the `$ref` URL in that tool's `inputSchema`. Learn more at https://one.sourcemeta.com"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
