# A client fetches an icon without credentials, whatever credential reached the
# surface that named it, so an address this instance would refuse to a caller
# holding nothing is no use to anybody and is not offered at all. This instance
# governs everything under its root, which puts the icons behind a credential,
# so nothing here names any.
# https://modelcontextprotocol.io/specification/2026-07-28/basic/index#icons

# The addresses the icons would have live behind the root policy, which is what
# settles this. They are refused to the caller who would be fetching them
GET {{base}}/self/v1/static/icon_192x192.png
HTTP 401
[Asserts]
header "WWW-Authenticate" exists

GET {{base}}/self/v1/static/icon_512x512.png
HTTP 401
[Asserts]
header "WWW-Authenticate" exists

# So discovery names this instance without drawing it, and the whole of what it
# says about itself is asserted so an icon cannot reappear unnoticed
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: server/discover
Content-Type: application/json
Authorization: Bearer mcp-secret-key
```
{
  "jsonrpc": "2.0",
  "id": "icons-1",
  "method": "server/discover",
  "params": {
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "hurl",
        "version": "1.0.0"
      }
    }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
server_version: jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version"
[Asserts]
header "Mcp-Session-Id" not exists
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].icons" not exists
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].websiteUrl" == "https://registry:8000"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result.cacheScope" == "private"
jsonpath "$.result.ttlMs" == 300000
body not contains "icon_192x192"
body not contains "icon_512x512"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
Authorization: Bearer full-secret-key
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A tool listing is drawn by the same rule, so its descriptors carry a display
# title and no icon either. The credential that opens the endpoint alone
# reaches no route, so the one that opens everything is what has tools to show
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/list
Content-Type: application/json
Authorization: Bearer full-secret-key
```
{
  "jsonrpc": "2.0",
  "id": "icons-2",
  "method": "tools/list",
  "params": {
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "hurl",
        "version": "1.0.0"
      }
    }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.result.tools" count == 13
jsonpath "$.result.tools[0].name" == "list_directory"
jsonpath "$.result.tools[0].title" == "List Directory"
jsonpath "$.result.tools[0].icons" not exists
jsonpath "$.result.tools[12].icons" not exists
jsonpath "$.result.resultType" == "complete"
body not contains "icon_192x192"
body not contains "icon_512x512"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
Authorization: Bearer full-secret-key
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
