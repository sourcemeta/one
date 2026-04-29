# The /self collection is not mounted when API is disabled
GET {{base}}/self/v1/schemas/configuration/configuration.json
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

GET {{base}}/self/v1/schemas/api/error.json
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

GET {{base}}/self/v1/schemas/api/list/response.json
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

GET {{base}}/self/v1/schemas/api/schemas/evaluate/response.json
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"
