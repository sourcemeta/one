OPTIONS {{base}}/v1/catalog/self/v1/mcp
HTTP 204
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" == "http://localhost:8000"
header "Access-Control-Allow-Methods" == "POST, OPTIONS"
header "Access-Control-Allow-Headers" == "Content-Type, MCP-Protocol-Version"
header "Access-Control-Max-Age" == "3600"

OPTIONS {{base}}/v1/catalog/self/v1/mcp
Origin: http://localhost:8000
HTTP 204
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" == "http://localhost:8000"
header "Access-Control-Allow-Methods" == "POST, OPTIONS"
header "Access-Control-Allow-Headers" == "Content-Type, MCP-Protocol-Version"
header "Access-Control-Max-Age" == "3600"

OPTIONS {{base}}/v1/catalog/self/v1/mcp
Origin: https://evil.example
HTTP 403
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" not exists
jsonpath "$.error.code" == 2
jsonpath "$.error.message" == "Forbidden origin"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
