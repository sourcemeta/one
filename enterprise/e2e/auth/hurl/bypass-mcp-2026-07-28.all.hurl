# The version an instance reports is stamped at build time, so it is read from
# the instance once here and every answer below is held to the same value
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: server/discover
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "server/discover",
  "params": {
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
[Captures]
server_version: jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version"
[Asserts]
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"

# The MCP tools take a location as an argument rather than as a request target,
# so they reach the gate through their own route. A spelling that names a gated
# location has to be refused there too, and one such spelling once answered
# with the listing because the argument reached the gate as written.

# The canonical spelling of a gated directory is refused, and the whole
# envelope is asserted so a listing cannot hide inside it
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: list_directory
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {
      "path": "/private"
    },
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
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
canonical_denial: body
canonical_denial_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "content": [
      {
        "type": "text",
        "text": "Directory not found"
      }
    ],
    "isError": true,
    "resultType": "complete",
    "_meta": {
      "io.modelcontextprotocol/serverInfo": {
        "name": "sourcemeta-one-enterprise",
        "version": "{{server_version}}",
        "title": "Sourcemeta One Enterprise",
        "description": "Fixtures exercising a public registry with apiKey-gated components",
        "websiteUrl": "{{base}}"
      }
    }
  }
}

POST {{base}}/self/v1/api/schemas/evaluate{{canonical_denial_schema}}
```
{{canonical_denial}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An upper-cased spelling of the same directory answers byte for byte the same,
# so it cannot be told apart from the canonical refusal
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: list_directory
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {
      "path": "/PRIVATE"
    },
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
[Asserts]
body == {{canonical_denial}}

# So does a spelling carrying a repeated separator and a dot segment
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: list_directory
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {
      "path": "//PRIVATE/./"
    },
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
[Asserts]
body == {{canonical_denial}}

# And a traversal that lands on the gated directory from a public one
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: list_directory
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {
      "path": "/public/../private"
    },
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
[Asserts]
body == {{canonical_denial}}

# The key opens the same directory through a non-canonical spelling, so the
# tool reads the location the policy governs rather than refusing every unusual
# argument. The whole listing is asserted, then round-tripped through the
# response schema the surface advertises
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: list_directory
Content-Type: application/json
Authorization: Bearer primary-secret-key
```
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {
      "path": "/PRIVATE"
    },
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
admitted_listing: body
admitted_listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 2
jsonpath "$.error" not exists
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "http://localhost:8000/private/secret"
jsonpath "$.result.structuredContent.schemas" == 2
jsonpath "$.result.structuredContent.entries" count == 2
jsonpath "$.result.structuredContent.entries[0].name" == "open"
jsonpath "$.result.structuredContent.entries[1].name" == "secret"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"

POST {{base}}/self/v1/api/schemas/evaluate{{admitted_listing_schema}}
```
{{admitted_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The canonical spelling returns that same listing to that same caller, so
# admission does not depend on how the location was written either
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: tools/call
Mcp-Name: list_directory
Content-Type: application/json
Authorization: Bearer primary-secret-key
```
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {
      "path": "/private"
    },
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
[Asserts]
body == {{admitted_listing}}
