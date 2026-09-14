# A Draft 7 schema declared as the dialect of a private schema alone is a
# meta-schema whoever asks, since the answer does not depend on the view

# Nobody anonymous can see the schema declaring it as its dialect
GET {{base}}/self/v1/api/schemas/metadata/open/meta
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
anonymous_metadata: body
anonymous_metadata_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 136
jsonpath "$.bytesBundled" == 136
jsonpath "$.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.health" == 0
jsonpath "$.priority" == 50
jsonpath "$.dependencies" == 0
jsonpath "$.dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.metaschema" == true
jsonpath "$.conversions.*" count == 2
jsonpath "$.conversions['2019-09'].mediaType" == "application/schema+json"
jsonpath "$.conversions['2020-12'].mediaType" == "application/schema+json"
jsonpath "$.private" == false
jsonpath "$.identifier" == "{{base}}/open/meta"
jsonpath "$.path" == "/open/meta"
jsonpath "$.title" == "An open meta-schema"
jsonpath "$.alert" == null
jsonpath "$.examples" count == 0
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "open"
jsonpath "$.breadcrumb[0].path" == "/open/"
jsonpath "$.breadcrumb[1].name" == "meta"
jsonpath "$.breadcrumb[1].path" == "/open/meta"

POST {{base}}/self/v1/api/schemas/evaluate{{anonymous_metadata_schema}}
```
{{anonymous_metadata}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The view holding the private schema gets the same answer
GET {{base}}/self/v1/api/schemas/metadata/open/meta
Accept: application/json
Authorization: Bearer vault-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
vault_metadata: body
vault_metadata_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 136
jsonpath "$.bytesBundled" == 136
jsonpath "$.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.health" == 0
jsonpath "$.priority" == 50
jsonpath "$.dependencies" == 0
jsonpath "$.dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.metaschema" == true
jsonpath "$.conversions.*" count == 2
jsonpath "$.conversions['2019-09'].mediaType" == "application/schema+json"
jsonpath "$.conversions['2020-12'].mediaType" == "application/schema+json"
jsonpath "$.private" == false
jsonpath "$.identifier" == "{{base}}/open/meta"
jsonpath "$.path" == "/open/meta"
jsonpath "$.title" == "An open meta-schema"
jsonpath "$.alert" == null
jsonpath "$.examples" count == 0
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "open"
jsonpath "$.breadcrumb[0].path" == "/open/"
jsonpath "$.breadcrumb[1].name" == "meta"
jsonpath "$.breadcrumb[1].path" == "/open/meta"

POST {{base}}/self/v1/api/schemas/evaluate{{vault_metadata_schema}}
```
{{vault_metadata}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# So does a view admitted elsewhere, which cannot see the private schema either
GET {{base}}/self/v1/api/schemas/metadata/open/meta
Accept: application/json
Authorization: Bearer ledger-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
ledger_metadata: body
ledger_metadata_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 136
jsonpath "$.bytesBundled" == 136
jsonpath "$.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.health" == 0
jsonpath "$.priority" == 50
jsonpath "$.dependencies" == 0
jsonpath "$.dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.metaschema" == true
jsonpath "$.conversions.*" count == 2
jsonpath "$.conversions['2019-09'].mediaType" == "application/schema+json"
jsonpath "$.conversions['2020-12'].mediaType" == "application/schema+json"
jsonpath "$.private" == false
jsonpath "$.identifier" == "{{base}}/open/meta"
jsonpath "$.path" == "/open/meta"
jsonpath "$.title" == "An open meta-schema"
jsonpath "$.alert" == null
jsonpath "$.examples" count == 0
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "open"
jsonpath "$.breadcrumb[0].path" == "/open/"
jsonpath "$.breadcrumb[1].name" == "meta"
jsonpath "$.breadcrumb[1].path" == "/open/meta"

POST {{base}}/self/v1/api/schemas/evaluate{{ledger_metadata_schema}}
```
{{ledger_metadata}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The private schema declaring the dialect is not a meta-schema itself
GET {{base}}/self/v1/api/schemas/metadata/vault/typed
Accept: application/json
Authorization: Bearer vault-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
typed_metadata: body
typed_metadata_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 166
jsonpath "$.bytesBundled" == 166
jsonpath "$.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.health" == 0
jsonpath "$.priority" == 50
jsonpath "$.dependencies" == 1
jsonpath "$.dialect" == "{{base}}/open/meta"
jsonpath "$.metaschema" == false
jsonpath "$.conversions" isEmpty
jsonpath "$.private" == true
jsonpath "$.identifier" == "{{base}}/vault/typed"
jsonpath "$.path" == "/vault/typed"
jsonpath "$.title" == "A vault string with an open dialect"
jsonpath "$.alert" == null
jsonpath "$.examples" count == 0
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "vault"
jsonpath "$.breadcrumb[0].path" == "/vault/"
jsonpath "$.breadcrumb[1].name" == "typed"
jsonpath "$.breadcrumb[1].path" == "/vault/typed"

POST {{base}}/self/v1/api/schemas/evaluate{{typed_metadata_schema}}
```
{{typed_metadata}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# An agent is told the same as the endpoint, whoever asks
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
    "arguments": { "schema": "{{base}}/open/meta" }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
anonymous_tool: body
anonymous_text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 1
jsonpath "$.error" not exists
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.bytes" == 136
jsonpath "$.result.structuredContent.bytesBundled" == 136
jsonpath "$.result.structuredContent.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.result.structuredContent.health" == 0
jsonpath "$.result.structuredContent.priority" == 50
jsonpath "$.result.structuredContent.dependencies" == 0
jsonpath "$.result.structuredContent.dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.result.structuredContent.metaschema" == true
jsonpath "$.result.structuredContent.conversions.*" count == 2
jsonpath "$.result.structuredContent.conversions['2019-09'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.conversions['2020-12'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.private" == false
jsonpath "$.result.structuredContent.identifier" == "{{base}}/open/meta"
jsonpath "$.result.structuredContent.path" == "/open/meta"
jsonpath "$.result.structuredContent.title" == "An open meta-schema"
jsonpath "$.result.structuredContent.alert" == null
jsonpath "$.result.structuredContent.examples" count == 0
jsonpath "$.result.structuredContent.breadcrumb" count == 2
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "open"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/open/"
jsonpath "$.result.structuredContent.breadcrumb[1].name" == "meta"
jsonpath "$.result.structuredContent.breadcrumb[1].path" == "/open/meta"
variable "anonymous_text" jsonpath "$.metaschema" == true
variable "anonymous_text" jsonpath "$.identifier" == "{{base}}/open/meta"

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/mcp/response
```
{{anonymous_tool}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
Authorization: Bearer vault-secret-key
```
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
    "arguments": { "schema": "{{base}}/open/meta" }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
[Captures]
vault_tool: body
vault_text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 2
jsonpath "$.error" not exists
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.bytes" == 136
jsonpath "$.result.structuredContent.bytesBundled" == 136
jsonpath "$.result.structuredContent.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.result.structuredContent.health" == 0
jsonpath "$.result.structuredContent.priority" == 50
jsonpath "$.result.structuredContent.dependencies" == 0
jsonpath "$.result.structuredContent.dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.result.structuredContent.metaschema" == true
jsonpath "$.result.structuredContent.conversions.*" count == 2
jsonpath "$.result.structuredContent.conversions['2019-09'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.conversions['2020-12'].mediaType" == "application/schema+json"
jsonpath "$.result.structuredContent.private" == false
jsonpath "$.result.structuredContent.identifier" == "{{base}}/open/meta"
jsonpath "$.result.structuredContent.path" == "/open/meta"
jsonpath "$.result.structuredContent.title" == "An open meta-schema"
jsonpath "$.result.structuredContent.alert" == null
jsonpath "$.result.structuredContent.examples" count == 0
jsonpath "$.result.structuredContent.breadcrumb" count == 2
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "open"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/open/"
jsonpath "$.result.structuredContent.breadcrumb[1].name" == "meta"
jsonpath "$.result.structuredContent.breadcrumb[1].path" == "/open/meta"
variable "vault_text" jsonpath "$.metaschema" == true
variable "vault_text" jsonpath "$.identifier" == "{{base}}/open/meta"

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/mcp/response
```
{{vault_tool}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
