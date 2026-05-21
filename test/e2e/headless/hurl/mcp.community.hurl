POST {{base}}/self/v1/mcp
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
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.result.protocolVersion" == "2025-11-25"
jsonpath "$.result.capabilities" isObject
jsonpath "$.result.capabilities.resources" isObject
jsonpath "$.result.capabilities.resources" isEmpty
jsonpath "$.result.serverInfo.name" == "sourcemeta-one"
jsonpath "$.result.serverInfo.title" == "Sourcemeta One"
jsonpath "$.result.serverInfo.version" matches "^.+$"
jsonpath "$.result.instructions" == "This is an instance of Sourcemeta One. Sourcemeta One is a JSON Schema registry. It serves a catalog of JSON Schemas organized as a tree of directories. Every schema is exposed as an MCP resource via the `JSON Schema` resource template (see `resources/templates/list`) and is addressable by its canonical absolute URI. Use `tools/list` to discover the operations this catalog exposes. Learn more at https://one.sourcemeta.com"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
