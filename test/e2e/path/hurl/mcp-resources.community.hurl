# resources/read of a real catalog schema URI is gated behind enterprise on community

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 24,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/example/string"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 24
jsonpath "$.error.code" == 1
jsonpath "$.error.message" == "Enterprise edition required"
jsonpath "$.error.data" == "MCP support is only available in the Enterprise edition of Sourcemeta One"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/read also accepts the `.json` extension form of the URI

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 26,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/example/string.json"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 26
jsonpath "$.error.code" == 1
jsonpath "$.error.message" == "Enterprise edition required"
jsonpath "$.error.data" == "MCP support is only available in the Enterprise edition of Sourcemeta One"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Same with ?bundle=1 to exercise the bundled-variant resolution branch

POST {{base}}/v1/catalog/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 25,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/v1/catalog/example/string?bundle=1"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 25
jsonpath "$.error.code" == 1
jsonpath "$.error.message" == "Enterprise edition required"
jsonpath "$.error.data" == "MCP support is only available in the Enterprise edition of Sourcemeta One"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
