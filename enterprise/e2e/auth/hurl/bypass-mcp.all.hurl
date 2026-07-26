# The MCP tools take a path as an argument rather than as a request target, so
# they reach the gate through their own route. A spelling that names a gated
# location has to be denied there too, and once answered a listing through this
# tool once because the argument reached the gate uncanonicalised.

# The canonical spelling of a gated directory is refused
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": { "name": "list_directory", "arguments": { "path": "/private" } }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.error" not exists
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Authentication required"

# An upper-cased spelling of the same directory is refused identically
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "tools/call",
  "params": { "name": "list_directory", "arguments": { "path": "/PRIVATE" } }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 2
jsonpath "$.error" not exists
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Authentication required"

# So is a spelling carrying a repeated separator and a dot segment
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 3,
  "method": "tools/call",
  "params": { "name": "list_directory", "arguments": { "path": "//PRIVATE/./" } }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 3
jsonpath "$.error" not exists
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Authentication required"

# And a traversal that lands on the gated directory from a public one
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 4,
  "method": "tools/call",
  "params": { "name": "list_directory", "arguments": { "path": "/public/../private" } }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 4
jsonpath "$.error" not exists
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Authentication required"

# The credential opens the same directory through a non-canonical spelling, so
# the tool is reading the location the gate governs rather than refusing every
# unusual argument
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
Authorization: Bearer primary-secret-key
```
{
  "jsonrpc": "2.0",
  "id": 5,
  "method": "tools/call",
  "params": { "name": "list_directory", "arguments": { "path": "/PRIVATE" } }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 5
jsonpath "$.error" not exists
jsonpath "$.result.isError" == false
jsonpath "$.result.content[0].text" contains "secret"
