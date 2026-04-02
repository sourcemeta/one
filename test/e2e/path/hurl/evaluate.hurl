POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/example/string
Content-Type: application/json
"hello"
HTTP 200
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/example/string
Content-Type: application/json
1234
HTTP 200
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.valid" == false

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/example/with-ref
Content-Type: application/json
{ "name": "hello" }
HTTP 200
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.valid" == true
jsonpath "$.annotations" count == 1
jsonpath "$.annotations[0].absoluteKeywordLocation" == "{{base}}/v1/catalog/example/with-ref#/properties"
jsonpath "$.annotations[0].instanceLocation" == ""
jsonpath "$.annotations[0].keywordLocation" == "/properties"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/example/with-ref
Content-Type: application/json
{ "name": 123 }
HTTP 200
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == false

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/rebased/with-ref
Content-Type: application/json
{ "value": true }
HTTP 200
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.valid" == true
jsonpath "$.annotations" count == 1
jsonpath "$.annotations[0].absoluteKeywordLocation" == "{{base}}/v1/catalog/rebased/with-ref#/properties"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/example/nested/deep
Content-Type: application/json
3.14
HTTP 200
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/example/nested/deep
Content-Type: application/json
"not a number"
HTTP 200
[Asserts]
jsonpath "$.valid" == false

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/rebased/nested/deep-with-id
Content-Type: application/json
null
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/rebased/nested/deep-no-id
Content-Type: application/json
5
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/rebased/nested/deep-no-id
Content-Type: application/json
-1
HTTP 200
[Asserts]
jsonpath "$.valid" == false

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/example/string
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/no-instance"
jsonpath "$.detail" == "You must pass an instance to validate against"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
