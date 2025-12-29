GET {{base}}/self/api/schemas/health/test/v2.0/schema
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.score" == 0
jsonpath "$.errors" count == 3

POST {{base}}/self/api/schemas/evaluate/sourcemeta/one/api/schemas/health/response
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/api/schemas/health/sourcemeta/std/v0/ietf/geojson/polygon
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.score" == 100
jsonpath "$.errors" count == 0

POST {{base}}/self/api/schemas/evaluate/sourcemeta/one/api/schemas/health/response
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

HEAD {{base}}/self/api/schemas/health/sourcemeta/std/v0/ietf/geojson/polygon
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
header "Last-Modified" exists
bytes count == 0

POST {{base}}/self/api/schemas/health/sourcemeta/std/v0/ietf/geojson/polygon
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Captures]
last_response: body
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
jsonpath "$.detail" == "This HTTP method is invalid for this URL"

POST {{base}}/self/api/schemas/evaluate/sourcemeta/one/api/error
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/api/schemas/health/xyz/
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Captures]
last_response: body
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/api/schemas/evaluate/sourcemeta/one/api/error
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/api/schemas/health
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Captures]
last_response: body
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/api/schemas/evaluate/sourcemeta/one/api/error
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
