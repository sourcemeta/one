GET {{base}}/v1/catalog/self/v1/mcp
HTTP 405
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" not exists
jsonpath "$.error.code" == 4
jsonpath "$.error.message" == "Method not allowed"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

DELETE {{base}}/v1/catalog/self/v1/mcp
HTTP 405
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" not exists
jsonpath "$.error.code" == 4
jsonpath "$.error.message" == "Method not allowed"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

PUT {{base}}/v1/catalog/self/v1/mcp
HTTP 405
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" not exists
jsonpath "$.error.code" == 4
jsonpath "$.error.message" == "Method not allowed"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

OPTIONS {{base}}/v1/catalog/self/v1/mcp
HTTP 204
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" == "http://localhost:8000"
header "Access-Control-Allow-Methods" == "POST, OPTIONS"
header "Access-Control-Allow-Headers" == "Content-Type, MCP-Protocol-Version"
header "Access-Control-Max-Age" == "3600"

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
