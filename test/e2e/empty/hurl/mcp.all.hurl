GET {{base}}/self/v1/mcp
HTTP 405
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" not exists
jsonpath "$.error.code" == 4
jsonpath "$.error.message" == "Method not allowed"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

DELETE {{base}}/self/v1/mcp
HTTP 405
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" not exists
jsonpath "$.error.code" == 4
jsonpath "$.error.message" == "Method not allowed"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

PUT {{base}}/self/v1/mcp
HTTP 405
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" not exists
jsonpath "$.error.code" == 4
jsonpath "$.error.message" == "Method not allowed"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

OPTIONS {{base}}/self/v1/mcp
HTTP 204
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" == "*"
header "Access-Control-Allow-Methods" == "POST, OPTIONS"
header "Access-Control-Allow-Headers" == "Content-Type, MCP-Protocol-Version"
header "Access-Control-Max-Age" == "3600"
