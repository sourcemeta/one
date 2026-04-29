GET {{base}}/test/schemas/bundling-single.json?bundle=1
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-07/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists

GET {{base}}/test/schemas/bundling-single.json?bundle=1
HTTP 200
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/schemas/bundling-single",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "http://localhost:8000/test/schemas/no-id"
    }
  },
  "definitions": {
    "http://localhost:8000/test/schemas/no-id": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "$id": "http://localhost:8000/test/schemas/no-id",
      "type": "string"
    }
  }
}

GET {{base}}/test/schemas/bundling-single.json?bundle=0
HTTP 200
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/schemas/bundling-single",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "http://localhost:8000/test/schemas/no-id"
    }
  },
  "definitions": {
    "http://localhost:8000/test/schemas/no-id": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "$id": "http://localhost:8000/test/schemas/no-id",
      "type": "string"
    }
  }
}

GET {{base}}/test/schemas/bundling-single.json?bundle=foo
HTTP 200
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/schemas/bundling-single",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "http://localhost:8000/test/schemas/no-id"
    }
  },
  "definitions": {
    "http://localhost:8000/test/schemas/no-id": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "$id": "http://localhost:8000/test/schemas/no-id",
      "type": "string"
    }
  }
}

GET {{base}}/test/schemas/no-id.json?bundle=1
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-07/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists

GET {{base}}/test/schemas/no-id.json?bundle=1
HTTP 200
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/schemas/no-id",
  "type": "string"
}

GET {{base}}/test/schemas/bundling-single.json?foo=2&bundle=1
HTTP 200
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "{{base}}/test/schemas/bundling-single",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "http://localhost:8000/test/schemas/no-id"
    }
  },
  "definitions": {
    "http://localhost:8000/test/schemas/no-id": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "$id": "http://localhost:8000/test/schemas/no-id",
      "type": "string"
    }
  }
}
