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
jsonpath "$.health" == 67
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.documentation" matches "^/self/v1/static/[a-f0-9]{64}$"
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "object"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/test/object"
jsonpath "$.entries[0].identifier" == "{{base}}/test/object"
jsonpath "$.entries[0].title" == "My Object"
jsonpath "$.entries[0].description" == "An Object Example"
jsonpath "$.entries[0].health" == 67
jsonpath "$.entries[0].documentation" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The static file endpoint serves the raw markdown content
GET {{base}}{{documentation_url}}
HTTP 200
[Asserts]
header "ETag" exists
header "Last-Modified" exists
body contains "# Test Collection"
body contains "**test schemas**"
body contains "[example guide](https://example.com/guide)"

# HTML page for collection with documentation renders the markdown
GET {{base}}/test
Accept: text/html
HTTP 200
Content-Type: text/html
[Asserts]
xpath "boolean(//hr)" == true
xpath "boolean(//h1[text()='Test Collection'])" == true
xpath "boolean(//strong[text()='test schemas'])" == true
xpath "boolean(//a[@href='https://example.com/guide'][text()='example guide'])" == true
xpath "boolean(//em[text()='emphasis'])" == true
xpath "boolean(//code[text()='inline code'])" == true

# XSS in documentation is filtered (check within the documentation container)
GET {{base}}/test
Accept: text/html
HTTP 200
Content-Type: text/html
[Asserts]
xpath "count(//div[contains(@class, 'pb-4')]//script)" == 0
xpath "count(//div[contains(@class, 'pb-4')]//iframe)" == 0

# The self collection does not have the documentation field
GET {{base}}/self/v1/api/list/self
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
jsonpath "$.path" == "/self"
jsonpath "$.url" == "{{base}}/self"
jsonpath "$.schemas" == 31
jsonpath "$.health" == 100
jsonpath "$.documentation" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The self collection HTML page does not render documentation
GET {{base}}/self
Accept: text/html
HTTP 200
Content-Type: text/html
[Asserts]
xpath "boolean(//h1[text()='Test Collection'])" == false

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
jsonpath "$.schemas" == 32
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "self"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 100
jsonpath "$.entries[0].schemas" == 31
jsonpath "$.entries[0].path" == "/self/"
jsonpath "$.entries[0].documentation" not exists
jsonpath "$.entries[1].name" == "test"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].health" == 67
jsonpath "$.entries[1].schemas" == 1
jsonpath "$.entries[1].path" == "/test/"
jsonpath "$.entries[1].documentation" not exists

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
