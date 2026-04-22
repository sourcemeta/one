# Collection with documentation has the documentation URL in the full list response
GET {{base}}/self/v1/api/list/test
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
documentation_url: jsonpath "$.documentation"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test"
jsonpath "$.url" == "{{base}}/test"
jsonpath "$.schemas" == 1
jsonpath "$.health" == 0
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.documentation" matches "^/self/v1/static/[a-f0-9]{64}$"
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "string"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/test/string"
jsonpath "$.entries[0].identifier" == "{{base}}/test/string"
jsonpath "$.entries[0].bytes" == 129
jsonpath "$.entries[0].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[0].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[0].health" == 0
jsonpath "$.entries[0].dependencies" == 0
jsonpath "$.entries[0].alert" == null
jsonpath "$.entries[0].provenance" == null
jsonpath "$.entries[0].documentation" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The static file endpoint serves the raw markdown content in headless mode
GET {{base}}{{documentation_url}}
HTTP 200
[Asserts]
header "ETag" exists
header "Last-Modified" exists
body contains "# Headless Documentation"
body contains "**test**"

# Collection without documentation does not have the documentation field
GET {{base}}/self/v1/api/list/other
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
jsonpath "$.path" == "/other"
jsonpath "$.url" == "{{base}}/other"
jsonpath "$.schemas" == 1
jsonpath "$.health" == 0
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "other"
jsonpath "$.breadcrumb[0].path" == "/other/"
jsonpath "$.documentation" not exists
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "integer"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/other/integer"
jsonpath "$.entries[0].identifier" == "{{base}}/other/integer"
jsonpath "$.entries[0].bytes" == 132
jsonpath "$.entries[0].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[0].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.entries[0].health" == 0
jsonpath "$.entries[0].dependencies" == 0
jsonpath "$.entries[0].alert" == null
jsonpath "$.entries[0].provenance" == null
jsonpath "$.entries[0].documentation" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# No HTML pages in headless mode
GET {{base}}/test
Accept: text/html
HTTP 404

# Parent listing does not include documentation on child entries
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
jsonpath "$.health" == 33
jsonpath "$.schemas" == 33
jsonpath "$.documentation" not exists
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].name" == "other"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 0
jsonpath "$.entries[0].schemas" == 1
jsonpath "$.entries[0].path" == "/other/"
jsonpath "$.entries[0].documentation" not exists
jsonpath "$.entries[1].name" == "self"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].health" == 100
jsonpath "$.entries[1].schemas" == 31
jsonpath "$.entries[1].path" == "/self/"
jsonpath "$.entries[1].title" == "Self"
jsonpath "$.entries[1].description" == "The schemas that define the current version of this instance"
jsonpath "$.entries[1].documentation" not exists
jsonpath "$.entries[2].name" == "test"
jsonpath "$.entries[2].type" == "directory"
jsonpath "$.entries[2].health" == 0
jsonpath "$.entries[2].schemas" == 1
jsonpath "$.entries[2].path" == "/test/"
jsonpath "$.entries[2].documentation" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
