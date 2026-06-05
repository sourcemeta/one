GET {{base}}/test/bundling/single.json
User-Agent: Deno/2.4.5
HTTP 200
Content-Type: application/json
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/bundling/single",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "{{base}}/test/v2.0/schema"
    }
  },
  "definitions": {
    "{{base}}/test/v2.0/schema": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "$id": "{{base}}/test/v2.0/schema",
      "type": "integer"
    }
  }
}
