OPTIONS {{base}}/self/v1/mcp
HTTP 204
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0
header "Access-Control-Allow-Origin" == "{{base}}"
header "Access-Control-Allow-Methods" == "POST, OPTIONS"
header "Access-Control-Allow-Headers" == "Content-Type, MCP-Protocol-Version"
header "Access-Control-Max-Age" == "3600"
header "Allow" == "POST, OPTIONS"
header "MCP-Protocol-Version" == "2025-03-26"

OPTIONS {{base}}/self/v1/mcp
Origin: http://localhost:8000
HTTP 204
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0
header "Access-Control-Allow-Origin" == "{{base}}"
header "Access-Control-Allow-Methods" == "POST, OPTIONS"
header "Access-Control-Allow-Headers" == "Content-Type, MCP-Protocol-Version"
header "Access-Control-Max-Age" == "3600"
header "Allow" == "POST, OPTIONS"
header "MCP-Protocol-Version" == "2025-03-26"

OPTIONS {{base}}/self/v1/mcp
Origin: https://evil.example
HTTP 403
Content-Type: application/json
Access-Control-Allow-Origin: http://localhost:8000
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-03-26
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == null
jsonpath "$.error.code" == -32001
jsonpath "$.error.message" == "Forbidden origin"
jsonpath "$.error.data" == "https://evil.example"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
