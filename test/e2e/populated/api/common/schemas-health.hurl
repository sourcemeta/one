GET {{base}}/self/v1/api/schemas/health/test/v2.0/schema
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/health/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.score" == 0
jsonpath "$.errors" count == 3
jsonpath "$.errors[0].custom" == false
jsonpath "$.errors[1].custom" == false
jsonpath "$.errors[2].custom" == false

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/health/xyz/
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/health
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
