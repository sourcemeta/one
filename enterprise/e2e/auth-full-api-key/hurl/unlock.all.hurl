# The single root key is the only thing standing between a caller and the whole
# registry. With it, every surface that denied anonymously now answers, and the
# responses carry the private interior the denials withheld. Credential-gated
# responses are served private so a shared cache cannot retain them.

# A schema is served, with its identity rebased onto the registry URL
GET {{base}}/catalog/example.json
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/catalog/example",
  "type": "string"
}

# The schema whose title and description were withheld anonymously is now served
# in full
GET {{base}}/catalog/classified.json
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
[Asserts]
jsonpath "$.title" == "Classified Acquisition Schema"
jsonpath "$.description" == "Internal-only structure describing a pending acquisition"
jsonpath "$['$id']" == "{{base}}/catalog/classified"

# The machine listing enumerates the collection, exposing the nested directory
# and the per-schema metadata, served private. It validates against its schema
GET {{base}}/self/v1/api/list/catalog
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
listing_body: body
listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].name" == "nested"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[1].name" == "classified"
jsonpath "$.entries[1].title" == "Classified Acquisition Schema"
jsonpath "$.entries[1].description" == "Internal-only structure describing a pending acquisition"
jsonpath "$.entries[1].identifier" == "{{base}}/catalog/classified"
jsonpath "$.entries[2].name" == "example"

POST {{base}}/self/v1/api/schemas/evaluate{{listing_schema}}
Authorization: Bearer full-secret-key
```
{{listing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The root listing enumerates the top-level collections, served private. It too
# validates against its schema
GET {{base}}/self/v1/api/list
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
root_listing_body: body
root_listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.entries[*].name" includes "catalog"
jsonpath "$.entries[*].name" includes "self"

POST {{base}}/self/v1/api/schemas/evaluate{{root_listing_schema}}
Authorization: Bearer full-secret-key
```
{{root_listing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Evaluate works with the key and validates an instance against a private schema
POST {{base}}/self/v1/api/schemas/evaluate/catalog/example
Authorization: Bearer full-secret-key
Content-Type: application/json
```
"a string"
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The static web UI assets are served with the key, with their immutable caching
GET {{base}}/self/v1/static/style.min.css
Authorization: Bearer full-secret-key
HTTP 200
Content-Type: text/css
[Asserts]
header "ETag" exists

# The MCP endpoint answers with the key
POST {{base}}/self/v1/mcp
Authorization: Bearer full-secret-key
Content-Type: application/json
```
{ "jsonrpc": "2.0", "id": 1, "method": "ping" }
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
mcp_body: body
mcp_schema: header "Link" regex "<([^>]+)>"
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {}
}

POST {{base}}/self/v1/api/schemas/evaluate{{mcp_schema}}
Authorization: Bearer full-secret-key
```
{{mcp_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
