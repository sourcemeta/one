POST {{base}}/v1/catalog/self/v1/mcp
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
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.result.protocolVersion" == "2025-11-25"
jsonpath "$.result.capabilities.resources" isObject
jsonpath "$.result.capabilities.resources" isEmpty
jsonpath "$.result.serverInfo.name" == "sourcemeta-one-enterprise"
jsonpath "$.result.serverInfo.title" == "Sourcemeta One Enterprise"
jsonpath "$.result.serverInfo.version" matches "^.+$"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "abc", "method": "ping" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "abc"
jsonpath "$.result" isObject
jsonpath "$.result" isEmpty

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{ "jsonrpc": "2.0", "method": "notifications/initialized" }
```
HTTP 202
Access-Control-Allow-Origin: http://localhost:8000
[Asserts]
bytes count == 0
header "Link" not exists

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "method": "notifications/cancelled",
  "params": { "requestId": 99, "reason": "test" }
}
```
HTTP 202
Access-Control-Allow-Origin: http://localhost:8000
[Asserts]
bytes count == 0
header "Link" not exists

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "method": "notifications/cancelled",
  "params": { "requestId": "never-issued" }
}
```
HTTP 202
Access-Control-Allow-Origin: http://localhost:8000
[Asserts]
bytes count == 0
header "Link" not exists

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 5, "method": "tools/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 5
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 7, "method": "notifications/initialized" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 7
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The MCP route is mounted under /v1/catalog. Requests without the base path
# must not match.

GET {{base}}/self/v1/mcp
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
HTTP 404

OPTIONS {{base}}/self/v1/mcp
HTTP 404
