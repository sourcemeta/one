# A key lives in an environment variable, and an operator can set that variable
# to nothing without noticing: a compose file interpolating a shell variable
# that was never exported, or a secret whose value was left blank. The variable
# then exists and holds an empty string. Such a variable holds no key, so it
# admits nobody, and a path gated only by one stays gated rather than turning
# public. Here the blank variable governs a path two working keys also govern,
# so these cases pin both halves: the blank one lets nobody through, and it
# does not get in the way of the keys beside it.

# A caller carrying nothing is denied, which is what a blank variable must not
# change. An empty credential is not a credential, however a policy spells the
# value it compares against
GET {{base}}/mixed/thing.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Spelling the empty credential out rather than omitting the header denies the
# same way, so there is no wording of nothing that the blank variable matches
GET {{base}}/mixed/thing.json
Authorization: Bearer
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
empty_body: body
empty_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
body == {{denied_body}}
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{empty_schema}}
```
{{empty_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Nor does any other credential match the blank variable
GET {{base}}/mixed/thing.json
Authorization: Bearer not-a-key
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
wrong_body: body
wrong_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
body == {{denied_body}}
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{wrong_schema}}
```
{{wrong_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The gated directory's own listing is not enumerable either, so a blank
# variable discloses no name behind the boundary it governs
GET {{base}}/self/v1/api/list/mixed
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
listing_body: body
listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
body == {{denied_body}}
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{listing_schema}}
```
{{listing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The tools reach the gate through their own route, so the same holds there
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": { "name": "list_directory", "arguments": { "path": "/mixed" } }
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
mcp_body: body
mcp_schema: header "Link" regex "<([^>]+)>"
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
    "isError": true
  }
}

POST {{base}}/self/v1/api/schemas/evaluate{{mcp_schema}}
```
{{mcp_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The keys declared beside the blank one are unaffected, so a blank variable
# costs nothing but the access it was never meant to grant
GET {{base}}/mixed/thing.json
Authorization: Bearer mixed-plain-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/mixed/thing",
  "type": "integer"
}

GET {{base}}/mixed/thing.json
Authorization: Bearer mixed-hashed-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/mixed/thing",
  "type": "integer"
}

# And the listing enumerates for a real key, marked private, since a policy
# that admits nobody still governs
GET {{base}}/self/v1/api/list/mixed
Authorization: Bearer mixed-plain-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
enumerated_body: body
enumerated_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.path" == "/mixed"
jsonpath "$.url" == "{{base}}/mixed"
jsonpath "$.schemas" == 1
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "mixed"
jsonpath "$.breadcrumb[0].path" == "/mixed/"
jsonpath "$.private" == true
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "thing"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/mixed/thing"
jsonpath "$.entries[0].identifier" == "{{base}}/mixed/thing"
jsonpath "$.entries[0].private" == true

POST {{base}}/self/v1/api/schemas/evaluate{{enumerated_schema}}
```
{{enumerated_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
