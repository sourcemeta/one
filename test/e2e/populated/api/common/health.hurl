GET {{base}}/self/v1/health
HTTP 200
Access-Control-Allow-Origin: *
[Asserts]
body == ""
header "Link" not exists

HEAD {{base}}/self/v1/health
HTTP 200
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists

POST {{base}}/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

PUT {{base}}/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

DELETE {{base}}/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

PATCH {{base}}/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

OPTIONS {{base}}/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
