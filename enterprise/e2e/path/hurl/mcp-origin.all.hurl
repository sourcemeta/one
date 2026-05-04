POST {{base}}/v1/catalog/self/v1/mcp
Origin: http://localhost:8000
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
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
Origin: https://evil.example
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
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

POST {{base}}/v1/catalog/self/v1/mcp
Origin: http://localhost:1234
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
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
