GET {{base}}/test/schemas/bundling-single.json
User-Agent: Deno/2.4.5
HTTP 200
Content-Type: application/json
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
