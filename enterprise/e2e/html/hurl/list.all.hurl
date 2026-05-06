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
jsonpath "$.schemas" == 94
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "self"
jsonpath "$.entries[0].title" == "Self"
jsonpath "$.entries[0].description" == "The schemas that define the current version of this instance"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 100
jsonpath "$.entries[0].schemas" == 28
jsonpath "$.entries[0].path" == "/self/"
jsonpath "$.entries[1].name" == "test"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].health" == 13
jsonpath "$.entries[1].schemas" == 66
jsonpath "$.entries[1].path" == "/test/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
