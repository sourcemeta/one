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
jsonpath "$.health" == 49
jsonpath "$.schemas" == 38
jsonpath "$.breadcrumb" count == 0
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].name" == "example"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 23
jsonpath "$.entries[0].schemas" == 5
jsonpath "$.entries[0].path" == "/v1/catalog/example/"
jsonpath "$.entries[1].name" == "rebased"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].health" == 25
jsonpath "$.entries[1].schemas" == 5
jsonpath "$.entries[1].path" == "/v1/catalog/rebased/"
jsonpath "$.entries[2].name" == "self"
jsonpath "$.entries[2].type" == "directory"
jsonpath "$.entries[2].health" == 100
jsonpath "$.entries[2].schemas" == 28
jsonpath "$.entries[2].path" == "/v1/catalog/self/"
jsonpath "$.entries[2].title" == "Self"
jsonpath "$.entries[2].description" == "The schemas that define the current version of this instance"

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
jsonpath "$.health" == 23
jsonpath "$.schemas" == 5
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "example"
jsonpath "$.breadcrumb[0].path" == "/v1/catalog/example/"
jsonpath "$.entries" count == 5
jsonpath "$.entries[0].name" == "nested"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].schemas" == 1
jsonpath "$.entries[0].path" == "/v1/catalog/example/nested/"
jsonpath "$.entries[1].name" == "integer"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].path" == "/v1/catalog/example/integer"
jsonpath "$.entries[1].identifier" == "{{base}}/v1/catalog/example/integer"
jsonpath "$.entries[1].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[2].name" == "object"
jsonpath "$.entries[2].type" == "schema"
jsonpath "$.entries[2].path" == "/v1/catalog/example/object"
jsonpath "$.entries[2].identifier" == "{{base}}/v1/catalog/example/object"
jsonpath "$.entries[3].name" == "string"
jsonpath "$.entries[3].type" == "schema"
jsonpath "$.entries[3].path" == "/v1/catalog/example/string"
jsonpath "$.entries[3].identifier" == "{{base}}/v1/catalog/example/string"
jsonpath "$.entries[4].name" == "with-ref"
jsonpath "$.entries[4].type" == "schema"
jsonpath "$.entries[4].path" == "/v1/catalog/example/with-ref"
jsonpath "$.entries[4].identifier" == "{{base}}/v1/catalog/example/with-ref"
jsonpath "$.entries[4].dependencies" == 1

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
jsonpath "$.entries[2].name" == "with-id"
jsonpath "$.entries[2].type" == "schema"
jsonpath "$.entries[2].path" == "/v1/catalog/rebased/with-id"
jsonpath "$.entries[2].identifier" == "{{base}}/v1/catalog/rebased/with-id"
jsonpath "$.entries[3].name" == "with-ref"
jsonpath "$.entries[3].type" == "schema"
jsonpath "$.entries[3].path" == "/v1/catalog/rebased/with-ref"
jsonpath "$.entries[3].identifier" == "{{base}}/v1/catalog/rebased/with-ref"
jsonpath "$.entries[3].dependencies" == 1

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/list/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
