GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/draft3/string
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.identifier" == "{{base}}/v1/catalog/draft3/string"
jsonpath "$.path" == "/v1/catalog/draft3/string"
jsonpath "$.baseDialect" == "http://json-schema.org/draft-03/schema#"
jsonpath "$.dialect" == "http://json-schema.org/draft-03/schema#"
jsonpath "$.examples" count == 0
jsonpath "$.dependencies" == 0
jsonpath "$.alert" == null
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "draft3"
jsonpath "$.breadcrumb[0].path" == "/v1/catalog/draft3/"
jsonpath "$.breadcrumb[1].name" == "string"
jsonpath "$.breadcrumb[1].path" == "/v1/catalog/draft3/string"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/metadata/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/draft3/with-ref
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
jsonpath "$.identifier" == "{{base}}/v1/catalog/draft3/with-ref"
jsonpath "$.dependencies" == 1
jsonpath "$.baseDialect" == "http://json-schema.org/draft-03/schema#"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/metadata/response
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/draft3/with-ref.json?bundle=1
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-03/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-03/schema#",
  "id": "{{base}}/v1/catalog/draft3/with-ref",
  "type": "object",
  "properties": {
    "tag": {
      "$ref": "string"
    }
  },
  "definitions": {
    "{{base}}/v1/catalog/draft3/string": {
      "$schema": "http://json-schema.org/draft-03/schema#",
      "id": "{{base}}/v1/catalog/draft3/string",
      "type": "string"
    }
  }
}
