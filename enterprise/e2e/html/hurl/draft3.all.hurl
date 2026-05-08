GET {{base}}/test/draft3/string.json
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-03/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-03/schema#",
  "id": "{{base}}/test/draft3/string",
  "type": "string"
}

GET {{base}}/test/draft3/string
Accept: text/html
HTTP 200
Content-Type: text/html
[Asserts]
body contains "Draft3"
body contains "https://www.learnjsonschema.com/draft3"

POST {{base}}/self/v1/api/schemas/evaluate/test/draft3/string
Content-Type: application/json
"alpha"
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/evaluate/response
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
