GET {{base}}/v1/catalog/example/string.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/example/string",
  "type": "string"
}

GET {{base}}/v1/catalog/example/string
HTTP 200
Content-Type: application/schema+json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/example/string",
  "type": "string"
}

GET {{base}}/v1/catalog/example/integer.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/example/integer",
  "type": "integer"
}

GET {{base}}/v1/catalog/example/object.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/example/object",
  "type": "object",
  "properties": {
    "name": {
      "type": "string"
    },
    "age": {
      "type": "integer"
    }
  }
}

GET {{base}}/v1/catalog/rebased/with-id.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/rebased/with-id",
  "type": "boolean"
}

GET {{base}}/v1/catalog/rebased/no-id.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/rebased/no-id",
  "type": "array",
  "items": {
    "type": "string"
  }
}

# Schema with $ref to sibling (no baseUri, no $id)
GET {{base}}/v1/catalog/example/with-ref.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/example/with-ref",
  "type": "object",
  "properties": {
    "name": {
      "$ref": "string"
    }
  }
}

# Nested subdirectory schema (no baseUri, no $id)
GET {{base}}/v1/catalog/example/nested/deep.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/example/nested/deep",
  "type": "number"
}

# Schema with $ref to sibling (with baseUri, with $id)
GET {{base}}/v1/catalog/rebased/with-ref.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/rebased/with-ref",
  "type": "object",
  "properties": {
    "value": {
      "$ref": "with-id"
    }
  }
}

# Nested subdirectory schema (with baseUri, with $id)
GET {{base}}/v1/catalog/rebased/nested/deep-with-id.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/rebased/nested/deep-with-id",
  "type": "null"
}

# Nested subdirectory schema (with baseUri, no $id)
GET {{base}}/v1/catalog/rebased/nested/deep-no-id.json
HTTP 200
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/v1/catalog/rebased/nested/deep-no-id",
  "type": "number",
  "minimum": 0
}

GET {{base}}/v1/catalog/example/string.json
HTTP 200
[Captures]
string_etag: header "ETag"
string_content_length: header "Content-Length"

HEAD {{base}}/v1/catalog/example/string.json
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
bytes count == 0
header "ETag" == {{string_etag}}
header "Content-Length" == {{string_content_length}}

GET {{base}}/v1/catalog/example/nonexistent.json
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"

GET {{base}}/example/string.json
HTTP 404

GET {{base}}/
HTTP 404

GET {{base}}/v1
HTTP 404

GET {{base}}/v1/catalog
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"

GET {{base}}/v1/catalog
Accept: text/html
HTTP 200
Content-Type: text/html

POST {{base}}/v1/catalog/example/string.json
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
