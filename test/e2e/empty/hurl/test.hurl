# HTML

GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html
[Asserts]
xpath "string(/html/head/title)" == "Sourcemeta Schemas"
header "Access-Control-Allow-Origin" not exists

# JSON

GET {{base}}/self/v1/api/list
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/"
jsonpath "$.url" == "{{base}}"
jsonpath "$.breadcrumb" count == 0
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "self"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].title" == "Self"
jsonpath "$.entries[0].description" == "The schemas that define the current version of this instance"
jsonpath "$.entries[0].path" == "/self/"
jsonpath "$.entries[0].health" == 100
jsonpath "$.entries[0].schemas" == 17
