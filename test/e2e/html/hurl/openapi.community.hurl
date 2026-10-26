# An OpenAPI dialect reports itself as the dialect and the dialect it builds on
# as the base dialect. The enterprise edition answers with a conversions
# property this edition has no notion of, so each asserts this response in a
# file of its own
GET {{base}}/self/v1/api/schemas/metadata/test/openapi/product
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
{
  "bytes": 991,
  "bytesBundled": 1481,
  "identifier": "{{base}}/test/openapi/product",
  "path": "/test/openapi/product",
  "baseDialect": "https://json-schema.org/draft/2020-12/schema",
  "dialect": "https://spec.openapis.org/oas/3.1/dialect/base",
  "metaschema": false,
  "title": "Product",
  "description": "A product that exercises every keyword of the OpenAPI 3.1 base vocabulary",
  "examples": [
    {
      "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "kind": "book"
    }
  ],
  "health": 100,
  "dependencies": 4,
  "alert": null,
  "priority": 50,
  "breadcrumb": [
    {
      "name": "test",
      "path": "/test/"
    },
    {
      "name": "openapi",
      "path": "/test/openapi/"
    },
    {
      "name": "product",
      "path": "/test/openapi/product"
    }
  ],
  "private": false
}

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/metadata/test/openapi/order
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
{
  "bytes": 694,
  "bytesBundled": 2436,
  "identifier": "{{base}}/test/openapi/order",
  "path": "/test/openapi/order",
  "baseDialect": "https://json-schema.org/draft/2020-12/schema",
  "dialect": "https://spec.openapis.org/oas/3.2/dialect/2025-09-17",
  "metaschema": false,
  "title": "Order",
  "description": "An order on the OpenAPI 3.2 dialect that references a schema on the OpenAPI 3.1 one",
  "examples": [
    {
      "product": {
        "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
        "kind": "book"
      },
      "quantity": 2
    }
  ],
  "health": 100,
  "dependencies": 7,
  "alert": null,
  "priority": 50,
  "breadcrumb": [
    {
      "name": "test",
      "path": "/test/"
    },
    {
      "name": "openapi",
      "path": "/test/openapi/"
    },
    {
      "name": "order",
      "path": "/test/openapi/order"
    }
  ],
  "private": false
}

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
