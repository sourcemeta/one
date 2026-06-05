GET {{base}}/self/v1/api/list
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/"
jsonpath "$.url" == "{{base}}"
jsonpath "$.breadcrumb" count == 0
jsonpath "$.schemas" == 126
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].name" == "self"
jsonpath "$.entries[0].title" == "Self"
jsonpath "$.entries[0].description" == "The schemas that define the current version of this instance"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 100
jsonpath "$.entries[0].schemas" == 56
jsonpath "$.entries[0].path" == "/self/"
jsonpath "$.entries[1].name" == "test"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].health" == 15
jsonpath "$.entries[1].schemas" == 68
jsonpath "$.entries[1].path" == "/test/"
jsonpath "$.entries[2].name" == "twins"
jsonpath "$.entries[2].title" == "Twins"
jsonpath "$.entries[2].description" == "Schemas whose base name collides with a sibling directory"
jsonpath "$.entries[2].type" == "directory"
jsonpath "$.entries[2].schemas" == 2
jsonpath "$.entries[2].path" == "/twins/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
