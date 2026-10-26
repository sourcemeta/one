# Discovery on protocol revision 2026-07-28, read through each credential.
#
# Discovery is assembled from the same per-view artifact the listings are, so
# who may keep its answer follows from which view answered it. An answer put
# together for a credential is only ever that caller's to keep, while the one
# anybody is handed may be kept by anybody, and a shared cache told the wrong
# thing here would pass one caller's answer to another.

# What a caller holding nothing is told may be kept by anybody
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: server/discover
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "discover-1",
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
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
server_version: jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "jsonrpc": "2.0",
  "id": "discover-1",
  "result": {
    "resultType": "complete",
    "supportedVersions": [
      "2025-03-26",
      "2025-06-18",
      "2025-11-25",
      "2026-07-28"
    ],
    "capabilities": {
      "resources": {},
      "tools": {}
    },
    "_meta": {
      "io.modelcontextprotocol/serverInfo": {
        "name": "sourcemeta-one-enterprise",
        "version": "{{server_version}}",
        "title": "Sourcemeta One Enterprise",
        "description": "A registry gated by machine credentials alone, with nobody to sign in interactively",
        "websiteUrl": "{{base}}"
      }
    },
    "instructions": "This is an instance of Sourcemeta One named \"Keys Sandbox\" (A registry gated by machine credentials alone, with nobody to sign in interactively). Sourcemeta One is a JSON Schema registry. It serves a catalog of JSON Schemas organized as a tree of directories. Every schema is exposed as an MCP resource via the `JSON Schema` resource template (see `resources/templates/list`) and is addressable by its canonical absolute URI. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Use `tools/list` to discover the operations this catalog exposes. Because this server is a JSON Schema registry, the schemas defining each tool's input parameters are themselves resources in this registry. To inspect a tool's full parameter shape, call `resources/read` on the `$ref` URL in that tool's `inputSchema`. Learn more at https://one.sourcemeta.com",
    "ttlMs": 300000,
    "cacheScope": "public"
  }
}

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# The same question behind a credential is answered for that caller alone
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: server/discover
Content-Type: application/json
Authorization: Bearer vault-secret-key
```
{
  "jsonrpc": "2.0",
  "id": "discover-1",
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
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "jsonrpc": "2.0",
  "id": "discover-1",
  "result": {
    "resultType": "complete",
    "supportedVersions": [
      "2025-03-26",
      "2025-06-18",
      "2025-11-25",
      "2026-07-28"
    ],
    "capabilities": {
      "resources": {},
      "tools": {}
    },
    "_meta": {
      "io.modelcontextprotocol/serverInfo": {
        "name": "sourcemeta-one-enterprise",
        "version": "{{server_version}}",
        "title": "Sourcemeta One Enterprise",
        "description": "A registry gated by machine credentials alone, with nobody to sign in interactively",
        "websiteUrl": "{{base}}"
      }
    },
    "instructions": "This is an instance of Sourcemeta One named \"Keys Sandbox\" (A registry gated by machine credentials alone, with nobody to sign in interactively). Sourcemeta One is a JSON Schema registry. It serves a catalog of JSON Schemas organized as a tree of directories. Every schema is exposed as an MCP resource via the `JSON Schema` resource template (see `resources/templates/list`) and is addressable by its canonical absolute URI. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Use `tools/list` to discover the operations this catalog exposes. Because this server is a JSON Schema registry, the schemas defining each tool's input parameters are themselves resources in this registry. To inspect a tool's full parameter shape, call `resources/read` on the `$ref` URL in that tool's `inputSchema`. Learn more at https://one.sourcemeta.com",
    "ttlMs": 300000,
    "cacheScope": "private"
  }
}

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Which holds for the other credential too, since what settles this is that
# a credential answered rather than which one did
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: server/discover
Content-Type: application/json
Authorization: Bearer ledger-secret-key
```
{
  "jsonrpc": "2.0",
  "id": "discover-1",
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
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "jsonrpc": "2.0",
  "id": "discover-1",
  "result": {
    "resultType": "complete",
    "supportedVersions": [
      "2025-03-26",
      "2025-06-18",
      "2025-11-25",
      "2026-07-28"
    ],
    "capabilities": {
      "resources": {},
      "tools": {}
    },
    "_meta": {
      "io.modelcontextprotocol/serverInfo": {
        "name": "sourcemeta-one-enterprise",
        "version": "{{server_version}}",
        "title": "Sourcemeta One Enterprise",
        "description": "A registry gated by machine credentials alone, with nobody to sign in interactively",
        "websiteUrl": "{{base}}"
      }
    },
    "instructions": "This is an instance of Sourcemeta One named \"Keys Sandbox\" (A registry gated by machine credentials alone, with nobody to sign in interactively). Sourcemeta One is a JSON Schema registry. It serves a catalog of JSON Schemas organized as a tree of directories. Every schema is exposed as an MCP resource via the `JSON Schema` resource template (see `resources/templates/list`) and is addressable by its canonical absolute URI. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Use `tools/list` to discover the operations this catalog exposes. Because this server is a JSON Schema registry, the schemas defining each tool's input parameters are themselves resources in this registry. To inspect a tool's full parameter shape, call `resources/read` on the `$ref` URL in that tool's `inputSchema`. Learn more at https://one.sourcemeta.com",
    "ttlMs": 300000,
    "cacheScope": "private"
  }
}

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# A credential that opens nothing is the caller holding nothing, so it is
# handed what anybody is handed
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: server/discover
Content-Type: application/json
Authorization: Bearer not-any-key
```
{
  "jsonrpc": "2.0",
  "id": "discover-1",
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
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "jsonrpc": "2.0",
  "id": "discover-1",
  "result": {
    "resultType": "complete",
    "supportedVersions": [
      "2025-03-26",
      "2025-06-18",
      "2025-11-25",
      "2026-07-28"
    ],
    "capabilities": {
      "resources": {},
      "tools": {}
    },
    "_meta": {
      "io.modelcontextprotocol/serverInfo": {
        "name": "sourcemeta-one-enterprise",
        "version": "{{server_version}}",
        "title": "Sourcemeta One Enterprise",
        "description": "A registry gated by machine credentials alone, with nobody to sign in interactively",
        "websiteUrl": "{{base}}"
      }
    },
    "instructions": "This is an instance of Sourcemeta One named \"Keys Sandbox\" (A registry gated by machine credentials alone, with nobody to sign in interactively). Sourcemeta One is a JSON Schema registry. It serves a catalog of JSON Schemas organized as a tree of directories. Every schema is exposed as an MCP resource via the `JSON Schema` resource template (see `resources/templates/list`) and is addressable by its canonical absolute URI. This instance is sovereign over its own URL namespace. Schemas whose URIs lie outside this namespace will not be served here. Use `tools/list` to discover the operations this catalog exposes. Because this server is a JSON Schema registry, the schemas defining each tool's input parameters are themselves resources in this registry. To inspect a tool's full parameter shape, call `resources/read` on the `$ref` URL in that tool's `inputSchema`. Learn more at https://one.sourcemeta.com",
    "ttlMs": 300000,
    "cacheScope": "public"
  }
}

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
