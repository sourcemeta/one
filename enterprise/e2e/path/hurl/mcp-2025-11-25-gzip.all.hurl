POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "ping-identity", "method": "ping" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Content-Encoding" not exists
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "ping-identity"
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
MCP-Protocol-Version: 2025-11-25
Accept-Encoding: gzip
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "ping-gzip", "method": "ping" }
```
HTTP 200
Content-Type: application/json
Content-Encoding: gzip
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "ping-gzip"
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
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "init-identity",
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
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Content-Encoding" not exists
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "init-identity"
jsonpath "$.result.protocolVersion" == "2025-11-25"
jsonpath "$.result.capabilities.resources" isObject
jsonpath "$.result.capabilities.resources" isEmpty
jsonpath "$.result.serverInfo.name" == "sourcemeta-one-enterprise"
jsonpath "$.result.serverInfo.title" == "Sourcemeta One Enterprise"
jsonpath "$.result.serverInfo.version" matches "^.+$"
jsonpath "$.result.instructions" matches "^This is an instance of Sourcemeta One"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Accept-Encoding: gzip
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "init-gzip",
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
Content-Encoding: gzip
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "init-gzip"
jsonpath "$.result.protocolVersion" == "2025-11-25"
jsonpath "$.result.capabilities.resources" isObject
jsonpath "$.result.capabilities.resources" isEmpty
jsonpath "$.result.serverInfo.name" == "sourcemeta-one-enterprise"
jsonpath "$.result.serverInfo.title" == "Sourcemeta One Enterprise"
jsonpath "$.result.serverInfo.version" matches "^.+$"
jsonpath "$.result.instructions" matches "^This is an instance of Sourcemeta One"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "tools-identity", "method": "tools/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Content-Encoding" not exists
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "tools-identity"
jsonpath "$.result.tools" count == 11
jsonpath "$.result.tools[0].name" == "list_directory"
jsonpath "$.result.tools[0].annotations.title" == "List Directory"
jsonpath "$.result.tools[10].name" == "search_schemas"
jsonpath "$.result.tools[10].annotations.title" == "Search Schemas"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Accept-Encoding: gzip
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "tools-gzip", "method": "tools/list" }
```
HTTP 200
Content-Type: application/json
Content-Encoding: gzip
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "tools-gzip"
jsonpath "$.result.tools" count == 11
jsonpath "$.result.tools[0].name" == "list_directory"
jsonpath "$.result.tools[0].annotations.title" == "List Directory"
jsonpath "$.result.tools[10].name" == "search_schemas"
jsonpath "$.result.tools[10].annotations.title" == "Search Schemas"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "call-identity",
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "example" }
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
text: jsonpath "$.result.content[0].text"
[Asserts]
header "Content-Encoding" not exists
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "call-identity"
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 5
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.path" == "/v1/catalog/example"
jsonpath "$.result.structuredContent.entries" count == 4
variable "text" jsonpath "$.path" == "/v1/catalog/example"
variable "text" jsonpath "$.entries" count == 4

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Accept-Encoding: gzip
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "call-gzip",
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "example" }
  }
}
```
HTTP 200
Content-Type: application/json
Content-Encoding: gzip
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "call-gzip"
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 5
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.structuredContent.path" == "/v1/catalog/example"
jsonpath "$.result.structuredContent.entries" count == 4
variable "text" jsonpath "$.path" == "/v1/catalog/example"
variable "text" jsonpath "$.entries" count == 4

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "rlist-identity", "method": "resources/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Content-Encoding" not exists
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "rlist-identity"
jsonpath "$.result.resources" count == 50
jsonpath "$.result.resources[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/error"
jsonpath "$.result.resources[0].name" == "error"
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.nextCursor" == "50"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Accept-Encoding: gzip
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "rlist-gzip", "method": "resources/list" }
```
HTTP 200
Content-Type: application/json
Content-Encoding: gzip
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "rlist-gzip"
jsonpath "$.result.resources" count == 50
jsonpath "$.result.resources[0].uri" == "{{base}}/v1/catalog/self/v1/schemas/api/error"
jsonpath "$.result.resources[0].name" == "error"
jsonpath "$.result.resources[0].mimeType" == "application/schema+json"
jsonpath "$.result.nextCursor" == "50"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/example/bare
HTTP 200
[Captures]
schema_text: body

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "rread-identity",
  "method": "resources/read",
  "params": { "uri": "{{base}}/v1/catalog/example/bare" }
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
header "Content-Encoding" not exists
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "rread-identity"
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Accept-Encoding: gzip
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "rread-gzip",
  "method": "resources/read",
  "params": { "uri": "{{base}}/v1/catalog/example/bare" }
}
```
HTTP 200
Content-Type: application/json
Content-Encoding: gzip
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "rread-gzip"
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/v1/catalog/example/bare"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "tlist-identity", "method": "resources/templates/list" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Content-Encoding" not exists
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "tlist-identity"
jsonpath "$.result.resourceTemplates" count == 1
jsonpath "$.result.resourceTemplates[0].uriTemplate" == "{{base}}/v1/catalog/{+path}{?bundle}"
jsonpath "$.result.resourceTemplates[0].name" == "JSON Schema"
jsonpath "$.result.resourceTemplates[0].description" == "A JSON Schema in this catalog (optionally bundled)"
jsonpath "$.result.resourceTemplates[0].mimeType" == "application/schema+json"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Accept-Encoding: gzip
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "tlist-gzip", "method": "resources/templates/list" }
```
HTTP 200
Content-Type: application/json
Content-Encoding: gzip
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "tlist-gzip"
jsonpath "$.result.resourceTemplates" count == 1
jsonpath "$.result.resourceTemplates[0].uriTemplate" == "{{base}}/v1/catalog/{+path}{?bundle}"
jsonpath "$.result.resourceTemplates[0].name" == "JSON Schema"
jsonpath "$.result.resourceTemplates[0].description" == "A JSON Schema in this catalog (optionally bundled)"
jsonpath "$.result.resourceTemplates[0].mimeType" == "application/schema+json"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/mcp
MCP-Protocol-Version: 2025-11-25
Accept-Encoding: gzip;q=0
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "neg-gzip-q0", "method": "ping" }
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </v1/catalog/self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Content-Encoding" not exists
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "neg-gzip-q0"
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
MCP-Protocol-Version: 2025-11-25
Accept-Encoding: identity;q=0
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": "neg-identity-q0", "method": "ping" }
```
HTTP 406
Content-Type: application/problem+json
[Asserts]
jsonpath "$.status" == 406
jsonpath "$.title" == "sourcemeta:one/cannot-satisfy-content-encoding"
