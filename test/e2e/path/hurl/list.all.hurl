GET {{base}}/v1/catalog/self/v1/api/list
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/v1/catalog/"
jsonpath "$.url" == "{{base}}/v1/catalog"
jsonpath "$.health" == 44
jsonpath "$.schemas" == 71
jsonpath "$.breadcrumb" count == 0
jsonpath "$.entries" count == 4
jsonpath "$.entries[0].name" == "draft3"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 29
jsonpath "$.entries[0].schemas" == 4
jsonpath "$.entries[0].path" == "/v1/catalog/draft3/"
jsonpath "$.entries[1].name" == "example"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].health" == 20
jsonpath "$.entries[1].schemas" == 6
jsonpath "$.entries[1].path" == "/v1/catalog/example/"
jsonpath "$.entries[2].name" == "rebased"
jsonpath "$.entries[2].type" == "directory"
jsonpath "$.entries[2].health" == 25
jsonpath "$.entries[2].schemas" == 5
jsonpath "$.entries[2].path" == "/v1/catalog/rebased/"
jsonpath "$.entries[3].name" == "self"
jsonpath "$.entries[3].type" == "directory"
jsonpath "$.entries[3].health" == 100
jsonpath "$.entries[3].schemas" == 56
jsonpath "$.entries[3].path" == "/v1/catalog/self/"
jsonpath "$.entries[3].title" == "Self"
jsonpath "$.entries[3].description" == "The schemas that define the current version of this instance"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/list/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/api/list/draft3
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/v1/catalog/draft3"
jsonpath "$.url" == "{{base}}/v1/catalog/draft3"
jsonpath "$.health" == 29
jsonpath "$.schemas" == 4
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "draft3"
jsonpath "$.breadcrumb[0].path" == "/v1/catalog/draft3/"
jsonpath "$.entries" count == 4
jsonpath "$.entries[0].name" == "divisible"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/v1/catalog/draft3/divisible"
jsonpath "$.entries[0].identifier" == "{{base}}/v1/catalog/draft3/divisible"
jsonpath "$.entries[0].dialect" == "http://json-schema.org/draft-03/schema#"
jsonpath "$.entries[0].priority" == 50
jsonpath "$.entries[1].name" == "object"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].path" == "/v1/catalog/draft3/object"
jsonpath "$.entries[1].identifier" == "{{base}}/v1/catalog/draft3/object"
jsonpath "$.entries[1].dialect" == "http://json-schema.org/draft-03/schema#"
jsonpath "$.entries[1].priority" == 50
jsonpath "$.entries[2].name" == "string"
jsonpath "$.entries[2].type" == "schema"
jsonpath "$.entries[2].path" == "/v1/catalog/draft3/string"
jsonpath "$.entries[2].identifier" == "{{base}}/v1/catalog/draft3/string"
jsonpath "$.entries[2].dialect" == "http://json-schema.org/draft-03/schema#"
jsonpath "$.entries[2].priority" == 50
jsonpath "$.entries[3].name" == "with-ref"
jsonpath "$.entries[3].type" == "schema"
jsonpath "$.entries[3].path" == "/v1/catalog/draft3/with-ref"
jsonpath "$.entries[3].identifier" == "{{base}}/v1/catalog/draft3/with-ref"
jsonpath "$.entries[3].dialect" == "http://json-schema.org/draft-03/schema#"
jsonpath "$.entries[3].dependencies" == 1
jsonpath "$.entries[3].priority" == 50

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/list/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/api/list/example
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/v1/catalog/example"
jsonpath "$.url" == "{{base}}/v1/catalog/example"
jsonpath "$.health" == 20
jsonpath "$.schemas" == 6
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "example"
jsonpath "$.breadcrumb[0].path" == "/v1/catalog/example/"
jsonpath "$.entries" count == 6
jsonpath "$.entries[0].name" == "nested"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].schemas" == 1
jsonpath "$.entries[0].path" == "/v1/catalog/example/nested/"
jsonpath "$.entries[1].name" == "integer"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].path" == "/v1/catalog/example/integer"
jsonpath "$.entries[1].identifier" == "{{base}}/v1/catalog/example/integer"
jsonpath "$.entries[1].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[1].priority" == 50
jsonpath "$.entries[2].name" == "object"
jsonpath "$.entries[2].type" == "schema"
jsonpath "$.entries[2].path" == "/v1/catalog/example/object"
jsonpath "$.entries[2].identifier" == "{{base}}/v1/catalog/example/object"
jsonpath "$.entries[2].priority" == 50
jsonpath "$.entries[3].name" == "string"
jsonpath "$.entries[3].type" == "schema"
jsonpath "$.entries[3].path" == "/v1/catalog/example/string"
jsonpath "$.entries[3].identifier" == "{{base}}/v1/catalog/example/string"
jsonpath "$.entries[3].priority" == 50
jsonpath "$.entries[4].name" == "with-ref"
jsonpath "$.entries[4].type" == "schema"
jsonpath "$.entries[4].path" == "/v1/catalog/example/with-ref"
jsonpath "$.entries[4].identifier" == "{{base}}/v1/catalog/example/with-ref"
jsonpath "$.entries[4].dependencies" == 1
jsonpath "$.entries[4].priority" == 50
# The truncated title surfaces in directory listings the same way it
# does in search results: 65535 bytes ending in "..."
jsonpath "$.entries[5].name" == "zzz-truncate-target"
jsonpath "$.entries[5].type" == "schema"
jsonpath "$.entries[5].path" == "/v1/catalog/example/zzz-truncate-target"
jsonpath "$.entries[5].identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.entries[5].priority" == 50
jsonpath "$.entries[5].title" matches "^Z{65532}\\.\\.\\.$"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/list/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/api/list/rebased
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/v1/catalog/rebased"
jsonpath "$.url" == "{{base}}/v1/catalog/rebased"
jsonpath "$.health" == 25
jsonpath "$.schemas" == 5
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "rebased"
jsonpath "$.breadcrumb[0].path" == "/v1/catalog/rebased/"
jsonpath "$.entries" count == 4
jsonpath "$.entries[0].name" == "nested"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].schemas" == 2
jsonpath "$.entries[0].path" == "/v1/catalog/rebased/nested/"
jsonpath "$.entries[1].name" == "no-id"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].path" == "/v1/catalog/rebased/no-id"
jsonpath "$.entries[1].identifier" == "{{base}}/v1/catalog/rebased/no-id"
jsonpath "$.entries[1].priority" == 50
jsonpath "$.entries[2].name" == "with-id"
jsonpath "$.entries[2].type" == "schema"
jsonpath "$.entries[2].path" == "/v1/catalog/rebased/with-id"
jsonpath "$.entries[2].identifier" == "{{base}}/v1/catalog/rebased/with-id"
jsonpath "$.entries[2].priority" == 50
jsonpath "$.entries[3].name" == "with-ref"
jsonpath "$.entries[3].type" == "schema"
jsonpath "$.entries[3].path" == "/v1/catalog/rebased/with-ref"
jsonpath "$.entries[3].identifier" == "{{base}}/v1/catalog/rebased/with-ref"
jsonpath "$.entries[3].dependencies" == 1
jsonpath "$.entries[3].priority" == 50

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/list/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
