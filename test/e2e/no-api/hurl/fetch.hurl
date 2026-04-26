GET {{base}}/test/schemas/string.json
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-07/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/schemas/string",
  "type": "string"
}

GET {{base}}/test/schemas/string
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-07/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/schemas/string",
  "type": "string"
}

GET {{base}}/test/schemas/integer.json
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-07/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/schemas/integer",
  "type": "integer"
}

HEAD {{base}}/test/schemas/string.json
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
bytes count == 0
header "ETag" exists
