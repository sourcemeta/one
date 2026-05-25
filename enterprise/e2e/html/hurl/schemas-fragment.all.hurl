# The REST endpoints reject schema paths that embed a URI fragment.

GET {{base}}/self/v1/api/schemas/metadata/test/object%23meta
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"

GET {{base}}/self/v1/api/schemas/locations/test/object%23meta
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"

GET {{base}}/self/v1/api/schemas/positions/test/object%23meta
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"

GET {{base}}/self/v1/api/schemas/stats/test/object%23meta
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"

GET {{base}}/self/v1/api/schemas/health/test/object%23meta
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"

GET {{base}}/self/v1/api/schemas/dependencies/test/object%23meta
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"

GET {{base}}/self/v1/api/schemas/dependents/test/object%23meta
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"

POST {{base}}/self/v1/api/schemas/evaluate/test/object%23meta
Content-Type: application/json
```
"hi"
```
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"

POST {{base}}/self/v1/api/schemas/trace/test/object%23meta
Content-Type: application/json
```
"hi"
```
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"

GET {{base}}/test/object%23meta
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
jsonpath "$.title" == "sourcemeta:one/invalid-uri"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The schema URI must not contain a fragment"
