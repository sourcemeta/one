GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/draft3/string
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Vary" == "Accept-Encoding"
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/draft3/with-ref
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.identifier" == "{{base}}/v1/catalog/draft3/with-ref"
jsonpath "$.dependencies" == 1
jsonpath "$.baseDialect" == "http://json-schema.org/draft-03/schema#"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/metadata/response
```
{{last_response}}
```
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/draft3/with-ref.json?bundle=1
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-03/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Vary" == "Accept-Encoding"
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
