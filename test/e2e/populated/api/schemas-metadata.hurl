GET {{base}}/self/v1/api/schemas/metadata/test/v2.0/schema
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
jsonpath "$.bytes" == 130
jsonpath "$.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.health" == 0
jsonpath "$.dependencies" == 0
jsonpath "$.dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.identifier" == "{{base}}/test/v2.0/schema"
jsonpath "$.path" == "/test/v2.0/schema"
jsonpath "$.alert" == null
jsonpath "$.provenance" == null
jsonpath "$.examples" count == 0
jsonpath "$.breadcrumb" count == 3
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test"
jsonpath "$.breadcrumb[1].name" == "v2.0"
jsonpath "$.breadcrumb[1].path" == "/test/v2.0"
jsonpath "$.breadcrumb[2].name" == "schema"
jsonpath "$.breadcrumb[2].path" == "/test/v2.0/schema"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/metadata/test/schemas/string
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
jsonpath "$.bytes" == 357
jsonpath "$.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.health" == 0
jsonpath "$.dependencies" == 0
jsonpath "$.dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.identifier" == "{{base}}/test/schemas/string"
jsonpath "$.path" == "/test/schemas/string"
jsonpath "$.alert" == null
jsonpath "$.provenance" == null
jsonpath "$.examples" count == 10
jsonpath "$.examples[0]" == "example_01"
jsonpath "$.examples[1]" == "example_02"
jsonpath "$.examples[2]" == "example_03"
jsonpath "$.examples[3]" == "example_04"
jsonpath "$.examples[4]" == 5
jsonpath "$.examples[5]" == "example_06"
jsonpath "$.examples[6]" == "example_07"
jsonpath "$.examples[7]" == "example_08"
jsonpath "$.examples[8]" == "example_09"
jsonpath "$.examples[9]" == "example_10"
jsonpath "$.breadcrumb" count == 3
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test"
jsonpath "$.breadcrumb[1].name" == "schemas"
jsonpath "$.breadcrumb[1].path" == "/test/schemas"
jsonpath "$.breadcrumb[2].name" == "string"
jsonpath "$.breadcrumb[2].path" == "/test/schemas/string"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

HEAD {{base}}/self/v1/api/schemas/metadata/test/v2.0/schema
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
bytes count == 0

POST {{base}}/self/v1/api/schemas/metadata/test/v2.0/schema
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
jsonpath "$.detail" == "This HTTP method is invalid for this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/metadata/xyz
Accept: application/json
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# With built-in library provenance
GET {{base}}/self/v1/api/schemas/metadata/sourcemeta/std/v0/ietf/geojson/point
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
jsonpath "$.bytes" == 882
jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.health" == 100
jsonpath "$.dependencies" == 5
jsonpath "$.dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.identifier" == "{{base}}/sourcemeta/std/v0/ietf/geojson/point"
jsonpath "$.path" == "/sourcemeta/std/v0/ietf/geojson/point"
jsonpath "$.alert" == "By using this schema, you confirm you agree to its <a href=\"https://github.com/sourcemeta/std/blob/main/LICENSE\">license</a>"
jsonpath "$.provenance" == "@sourcemeta/std/v0"
jsonpath "$.examples" count == 2
jsonpath "$.breadcrumb" count == 6
jsonpath "$.breadcrumb[0].name" == "sourcemeta"
jsonpath "$.breadcrumb[0].path" == "/sourcemeta"
jsonpath "$.breadcrumb[1].name" == "std"
jsonpath "$.breadcrumb[1].path" == "/sourcemeta/std"
jsonpath "$.breadcrumb[2].name" == "v0"
jsonpath "$.breadcrumb[2].path" == "/sourcemeta/std/v0"
jsonpath "$.breadcrumb[3].name" == "ietf"
jsonpath "$.breadcrumb[3].path" == "/sourcemeta/std/v0/ietf"
jsonpath "$.breadcrumb[4].name" == "geojson"
jsonpath "$.breadcrumb[4].path" == "/sourcemeta/std/v0/ietf/geojson"
jsonpath "$.breadcrumb[5].name" == "point"
jsonpath "$.breadcrumb[5].path" == "/sourcemeta/std/v0/ietf/geojson/point"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/metadata
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/metadata/test/bundling/double
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
jsonpath "$.bytes" == 269
jsonpath "$.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.health" == 67
jsonpath "$.dependencies" == 3
jsonpath "$.dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.identifier" == "{{base}}/test/bundling/double"
jsonpath "$.path" == "/test/bundling/double"
jsonpath "$.alert" == null
jsonpath "$.provenance" == null
jsonpath "$.examples" count == 0
jsonpath "$.breadcrumb" count == 3
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test"
jsonpath "$.breadcrumb[1].name" == "bundling"
jsonpath "$.breadcrumb[1].path" == "/test/bundling"
jsonpath "$.breadcrumb[2].name" == "double"
jsonpath "$.breadcrumb[2].path" == "/test/bundling/double"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
