GET {{base}}/self/v1/api/schemas/health/enterprise/object
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
jsonpath "$.score" == 67
jsonpath "$.errors" count == 1
jsonpath "$.errors[0].name" == "custom/all_properties_camelcase"
jsonpath "$.errors[0].message" == "Ensure camelCase properties"
jsonpath "$.errors[0].custom" == true
jsonpath "$.errors[0].pointers[0]" == "/properties/INVALID_PROPERTY_1"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
