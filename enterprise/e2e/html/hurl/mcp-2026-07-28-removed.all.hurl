# Methods that are gone or unimplemented on revision 2026-07-28.

# Every method this revision retired, and every method it defines that
# this instance does not answer, is missing rather than malformed
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: initialize
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 100,
  "method": "initialize",
  "params": {
    "protocolVersion": "2026-07-28",
    "capabilities": {},
    "clientInfo": {
      "name": "hurl",
      "version": "1.0.0"
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
HTTP 404
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
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 100
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"
jsonpath "$.error.data" not exists
jsonpath "$.result" not exists

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

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: ping
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 101,
  "method": "ping",
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
HTTP 404
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
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 101
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"
jsonpath "$.error.data" not exists
jsonpath "$.result" not exists

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

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: logging/setLevel
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 102,
  "method": "logging/setLevel",
  "params": {
    "level": "debug",
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
HTTP 404
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
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 102
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"
jsonpath "$.error.data" not exists
jsonpath "$.result" not exists

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

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: resources/subscribe
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 103,
  "method": "resources/subscribe",
  "params": {
    "uri": "{{base}}/test/bare/01",
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
HTTP 404
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
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 103
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"
jsonpath "$.error.data" not exists
jsonpath "$.result" not exists

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

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: resources/unsubscribe
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 104,
  "method": "resources/unsubscribe",
  "params": {
    "uri": "{{base}}/test/bare/01",
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
HTTP 404
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
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 104
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"
jsonpath "$.error.data" not exists
jsonpath "$.result" not exists

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

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: prompts/list
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 105,
  "method": "prompts/list",
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
HTTP 404
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
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 105
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"
jsonpath "$.error.data" not exists
jsonpath "$.result" not exists

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

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: prompts/get
Mcp-Name: anything
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 106,
  "method": "prompts/get",
  "params": {
    "name": "anything",
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
HTTP 404
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
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 106
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"
jsonpath "$.error.data" not exists
jsonpath "$.result" not exists

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

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: completion/complete
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 107,
  "method": "completion/complete",
  "params": {
    "ref": {
      "type": "ref/prompt",
      "name": "x"
    },
    "argument": {
      "name": "y",
      "value": "z"
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
HTTP 404
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
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 107
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"
jsonpath "$.error.data" not exists
jsonpath "$.result" not exists

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

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: subscriptions/listen
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 108,
  "method": "subscriptions/listen",
  "params": {
    "notifications": {
      "toolsListChanged": true
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
HTTP 404
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
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 108
jsonpath "$.error.code" == -32601
jsonpath "$.error.message" == "Method not found"
jsonpath "$.error.data" not exists
jsonpath "$.result" not exists

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

# A method no revision ever named is answered the same way
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: nonsense/method
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 200,
  "method": "nonsense/method",
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
HTTP 404
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
jsonpath "$.id" == 200
jsonpath "$.error.code" == -32601

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
