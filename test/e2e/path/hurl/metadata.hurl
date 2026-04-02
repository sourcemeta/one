GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/example/string
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 143
jsonpath "$.identifier" == "{{base}}/v1/catalog/example/string"
jsonpath "$.path" == "/example/string"
jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.examples" count == 0
jsonpath "$.health" == 0
jsonpath "$.dependencies" == 0
jsonpath "$.alert" == null
jsonpath "$.provenance" == null
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "example"
jsonpath "$.breadcrumb[0].path" == "/example/"
jsonpath "$.breadcrumb[1].name" == "string"
jsonpath "$.breadcrumb[1].path" == "/example/string"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/rebased/with-id
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 145
jsonpath "$.identifier" == "{{base}}/v1/catalog/rebased/with-id"
jsonpath "$.path" == "/rebased/with-id"
jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.examples" count == 0
jsonpath "$.health" == 0
jsonpath "$.dependencies" == 0
jsonpath "$.alert" == null
jsonpath "$.provenance" == null
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "rebased"
jsonpath "$.breadcrumb[0].path" == "/rebased/"
jsonpath "$.breadcrumb[1].name" == "with-id"
jsonpath "$.breadcrumb[1].path" == "/rebased/with-id"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
