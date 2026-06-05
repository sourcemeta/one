GET {{base}}/test/draft3/string.json
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-03/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-03/schema#",
  "id": "{{base}}/test/draft3/string",
  "type": "string"
}

GET {{base}}/test/draft3/string
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
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
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/evaluate/response
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

GET {{base}}/test/draft3/with-ref.json
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-03/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-03/schema#",
  "id": "{{base}}/test/draft3/with-ref",
  "type": "object",
  "properties": {
    "tag": {
      "$ref": "string"
    }
  }
}

GET {{base}}/test/draft3/with-ref.json?bundle=1
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-03/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-03/schema#",
  "id": "{{base}}/test/draft3/with-ref",
  "type": "object",
  "properties": {
    "tag": {
      "$ref": "string"
    }
  },
  "definitions": {
    "{{base}}/test/draft3/string": {
      "$schema": "http://json-schema.org/draft-03/schema#",
      "id": "{{base}}/test/draft3/string",
      "type": "string"
    }
  }
}

POST {{base}}/self/v1/api/schemas/evaluate/test/draft3/with-ref
Content-Type: application/json
{ "tag": "beta" }
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/evaluate/response
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate/test/draft3/with-ref
Content-Type: application/json
{ "tag": 99 }
HTTP 200
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == false
