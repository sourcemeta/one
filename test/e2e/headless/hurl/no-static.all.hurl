GET {{base}}/self/v1/static/style.min.css
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

HEAD {{base}}/self/v1/static/style.min.css
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *

POST {{base}}/self/v1/static/style.min.css
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

PUT {{base}}/self/v1/static/style.min.css
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

DELETE {{base}}/self/v1/static/style.min.css
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
