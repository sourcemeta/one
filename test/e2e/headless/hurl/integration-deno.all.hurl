GET {{base}}/test/schemas/bundling-single.json
User-Agent: Deno/2.4.5
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/schemas/bundling-single",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "{{base}}/test/schemas/no-id"
    }
  },
  "definitions": {
    "{{base}}/test/schemas/no-id": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "$id": "{{base}}/test/schemas/no-id",
      "type": "string"
    }
  }
}
