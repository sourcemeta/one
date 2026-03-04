GET {{base}}/self/v1/api/list
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/"
jsonpath "$.url" == "{{base}}"
jsonpath "$.breadcrumb" count == 0
jsonpath "$.entries" count >= 2
jsonpath "$.entries[0].name" == "test"
jsonpath "$.entries[0].title" == "Test"
jsonpath "$.entries[0].description" == "A directory full of testing schemas"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 3
jsonpath "$.entries[0].path" == "/test/"
jsonpath "$.entries[1].name" == "self"
jsonpath "$.entries[1].title" == "Self"
jsonpath "$.entries[1].description" == "The schemas that define the current version of this instance"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].health" == 100
jsonpath "$.entries[1].path" == "/self/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/test
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test"
jsonpath "$.url" == "{{base}}/test"
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.description" == "A directory full of testing schemas"
jsonpath "$.title" == "Test"
jsonpath "$.entries" count == 3

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/test/schemas/v2.0
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test/schemas/v2.0"
jsonpath "$.url" == "{{base}}/test/schemas/v2.0"
jsonpath "$.breadcrumb" count == 3
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.breadcrumb[1].name" == "schemas"
jsonpath "$.breadcrumb[1].path" == "/test/schemas/"
jsonpath "$.breadcrumb[2].name" == "v2.0"
jsonpath "$.breadcrumb[2].path" == "/test/schemas/v2.0/"
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].bytes" == 138
jsonpath "$.entries[0].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[0].dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[0].identifier" == "{{base}}/test/schemas/v2.0/schema"
jsonpath "$.entries[0].name" == "schema"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].alert" == null
jsonpath "$.entries[0].provenance" == null
jsonpath "$.entries[0].path" == "/test/schemas/v2.0/schema"
jsonpath "$.entries[0].health" == 0
jsonpath "$.entries[0].dependencies" == 0

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/test/schemas/v2.0.json
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

GET {{base}}/self/v1/api/list/test/schemas/versions
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test/schemas/versions"
jsonpath "$.url" == "{{base}}/test/schemas/versions"
jsonpath "$.health" == 0
jsonpath "$.breadcrumb" count == 3
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.breadcrumb[1].name" == "schemas"
jsonpath "$.breadcrumb[1].path" == "/test/schemas/"
jsonpath "$.breadcrumb[2].name" == "versions"
jsonpath "$.breadcrumb[2].path" == "/test/schemas/versions/"
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].bytes" == 141
jsonpath "$.entries[0].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[0].dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[0].identifier" == "{{base}}/test/schemas/versions/v1.2.3"
jsonpath "$.entries[0].name" == "v1.2.3"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].alert" == null
jsonpath "$.entries[0].provenance" == null
jsonpath "$.entries[0].path" == "/test/schemas/versions/v1.2.3"
jsonpath "$.entries[0].health" == 0
jsonpath "$.entries[0].dependencies" == 0

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/test/schemas/clash/foo
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test/schemas/clash/foo"
jsonpath "$.url" == "{{base}}/test/schemas/clash/foo"
jsonpath "$.health" == 0
jsonpath "$.breadcrumb" count == 4
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.breadcrumb[1].name" == "schemas"
jsonpath "$.breadcrumb[1].path" == "/test/schemas/"
jsonpath "$.breadcrumb[2].name" == "clash"
jsonpath "$.breadcrumb[2].path" == "/test/schemas/clash/"
jsonpath "$.breadcrumb[3].name" == "foo"
jsonpath "$.breadcrumb[3].path" == "/test/schemas/clash/foo/"
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].bytes" == 124
jsonpath "$.entries[0].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[0].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[0].identifier" == "{{base}}/test/schemas/clash/foo/bar"
jsonpath "$.entries[0].name" == "bar"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].alert" == null
jsonpath "$.entries[0].provenance" == null
jsonpath "$.entries[0].path" == "/test/schemas/clash/foo/bar"
jsonpath "$.entries[0].health" == 0
jsonpath "$.entries[0].dependencies" == 0

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/test/schemas/clash
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test/schemas/clash"
jsonpath "$.url" == "{{base}}/test/schemas/clash"
jsonpath "$.health" == 0
jsonpath "$.breadcrumb" count == 3
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.breadcrumb[1].name" == "schemas"
jsonpath "$.breadcrumb[1].path" == "/test/schemas/"
jsonpath "$.breadcrumb[2].name" == "clash"
jsonpath "$.breadcrumb[2].path" == "/test/schemas/clash/"
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "foo"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 0
jsonpath "$.entries[0].path" == "/test/schemas/clash/foo/"
jsonpath "$.entries[1].name" == "foo"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].bytes" == 120
jsonpath "$.entries[1].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[1].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[1].identifier" == "{{base}}/test/schemas/clash/foo"
jsonpath "$.entries[1].path" == "/test/schemas/clash/foo"
jsonpath "$.entries[1].health" == 0
jsonpath "$.entries[1].dependencies" == 0
jsonpath "$.entries[1].alert" == null
jsonpath "$.entries[1].provenance" == null

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/test/schemas/extension
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test/schemas/extension"
jsonpath "$.url" == "{{base}}/test/schemas/extension"
jsonpath "$.health" == 17
jsonpath "$.breadcrumb" count == 3
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.breadcrumb[1].name" == "schemas"
jsonpath "$.breadcrumb[1].path" == "/test/schemas/"
jsonpath "$.breadcrumb[2].name" == "extension"
jsonpath "$.breadcrumb[2].path" == "/test/schemas/extension/"
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].bytes" == 143
jsonpath "$.entries[0].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[0].dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[0].identifier" == "{{base}}/test/schemas/extension/without"
jsonpath "$.entries[0].name" == "without"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].alert" == null
jsonpath "$.entries[0].provenance" == null
jsonpath "$.entries[0].path" == "/test/schemas/extension/without"
jsonpath "$.entries[0].health" == 0
jsonpath "$.entries[0].dependencies" == 0
jsonpath "$.entries[1].bytes" == 140
jsonpath "$.entries[1].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[1].dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[1].identifier" == "{{base}}/test/schemas/extension/with"
jsonpath "$.entries[1].name" == "with"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].alert" == null
jsonpath "$.entries[1].provenance" == null
jsonpath "$.entries[1].path" == "/test/schemas/extension/with"
jsonpath "$.entries[1].health" == 0
jsonpath "$.entries[1].dependencies" == 0
jsonpath "$.entries[2].bytes" == 180
jsonpath "$.entries[2].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[2].dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.entries[2].identifier" == "{{base}}/test/schemas/extension/to-without"
jsonpath "$.entries[2].name" == "to-without"
jsonpath "$.entries[2].type" == "schema"
jsonpath "$.entries[2].alert" == null
jsonpath "$.entries[2].provenance" == null
jsonpath "$.entries[2].path" == "/test/schemas/extension/to-without"
jsonpath "$.entries[2].health" == 50
jsonpath "$.entries[2].dependencies" == 1

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
