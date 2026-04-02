GET {{base}}/v1/catalog/self/v1/health
HTTP 200
Access-Control-Allow-Origin: *
[Asserts]
body == ""
header "Link" not exists

HEAD {{base}}/v1/catalog/self/v1/health
HTTP 200
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists

POST {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

PUT {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

DELETE {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

PATCH {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

OPTIONS {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
