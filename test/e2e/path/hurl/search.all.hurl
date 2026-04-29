GET {{base}}/v1/catalog/self/v1/api/schemas/search?q=string
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/v1/catalog/example/string"
jsonpath "$[0].title" == ""
jsonpath "$[0].description" == ""

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/search/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/api/schemas/search?q=nonexistent_term_xyz
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$" count == 0

GET {{base}}/v1/catalog/self/v1/api/schemas/search
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 400
