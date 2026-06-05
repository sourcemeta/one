GET {{base}}/v1/catalog/draft3/string.json
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-03/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-03/schema#",
  "id": "{{base}}/v1/catalog/draft3/string",
  "type": "string"
}

GET {{base}}/v1/catalog/draft3/object.json
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-03/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
{
  "$schema": "http://json-schema.org/draft-03/schema#",
  "id": "{{base}}/v1/catalog/draft3/object",
  "type": "object",
  "properties": {
    "name": {
      "type": "string",
      "required": true
    },
    "age": {
      "type": "integer"
    }
  }
}

GET {{base}}/v1/catalog/draft3/string.json
HTTP 200
[Captures]
draft3_string_etag: header "ETag"
draft3_string_content_length: header "Content-Length"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/

HEAD {{base}}/v1/catalog/draft3/string.json
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0
header "ETag" == {{draft3_string_etag}}
header "Content-Length" == {{draft3_string_content_length}}

GET {{base}}/v1/catalog/draft3/nonexistent.json
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"

POST {{base}}/v1/catalog/draft3/string.json
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
