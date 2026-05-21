POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/draft3/string
Content-Type: application/json
"hello"
HTTP 200
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/evaluate/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/draft3/string
Content-Type: application/json
1234
HTTP 200
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
jsonpath "$.valid" == false

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/evaluate/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/draft3/object
Content-Type: application/json
{ "name": "Ada" }
HTTP 200
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/evaluate/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/draft3/object
Content-Type: application/json
{ "age": 7 }
HTTP 200
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
jsonpath "$.valid" == false

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/evaluate/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
