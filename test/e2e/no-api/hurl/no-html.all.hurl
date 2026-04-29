GET {{base}}/test/schemas/string.html
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

GET {{base}}/self/v1/static/main.js
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

GET {{base}}/self/v1/static/main.css
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"
