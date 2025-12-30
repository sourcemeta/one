GET {{base}}/self/api/schemas/stats/test/v2.0/schema
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/schemas/api/schemas/stats/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$['http://json-schema.org/draft-07/schema#']['$id']" == 1
jsonpath "$['http://json-schema.org/draft-07/schema#']['$schema']" == 1
jsonpath "$['http://json-schema.org/draft-07/schema#']['type']" == 1

POST {{base}}/self/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

HEAD {{base}}/self/api/schemas/stats/test/v2.0/schema
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/schemas/api/schemas/stats/response>; rel="describedby"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
bytes count == 0

POST {{base}}/self/api/schemas/stats/test/v2.0/schema
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
jsonpath "$.detail" == "This HTTP method is invalid for this URL"

POST {{base}}/self/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/api/schemas/stats/xyz/
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/api/schemas/stats
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
