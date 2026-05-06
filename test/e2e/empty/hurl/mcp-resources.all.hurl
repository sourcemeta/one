# resources/templates/list publishes the single registry URI template

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 12,
  "method": "resources/templates/list"
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 12
jsonpath "$.result.resourceTemplates" count == 1
jsonpath "$.result.resourceTemplates[0].uriTemplate" == "{{base}}/{+path}{?bundle}"
jsonpath "$.result.resourceTemplates[0].name" == "JSON Schema"
jsonpath "$.result.resourceTemplates[0].description" == "A JSON Schema in this catalog (optionally bundled)"
jsonpath "$.result.resourceTemplates[0].mimeType" == "application/schema+json"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/read with a malformed URI returns -32002

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 20,
  "method": "resources/read",
  "params": {
    "uri": "not a uri"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 20
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "not a uri"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/read with a URI outside the registry's space returns -32002

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 21,
  "method": "resources/read",
  "params": {
    "uri": "https://elsewhere.example.com/foo"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 21
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "https://elsewhere.example.com/foo"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/read rejects URIs containing dot-segment traversal

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 100,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/../../etc/passwd"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 100
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "{{base}}/../../etc/passwd"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/read rejects relative URIs with absolute-path references

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 102,
  "method": "resources/read",
  "params": {
    "uri": "/etc/passwd"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 102
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "/etc/passwd"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# resources/read with a URI in our space but pointing at no schema returns -32002

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 22,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/does/not/exist"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 22
jsonpath "$.error.code" == -32002
jsonpath "$.error.message" == "Resource not found"
jsonpath "$.error.data" == "{{base}}/does/not/exist"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
