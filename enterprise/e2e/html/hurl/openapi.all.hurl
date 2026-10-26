# An OpenAPI dialect sits on the same upgrade ladder as the official JSON
# Schema dialects, so a schema on one is offered the dialect it declares and
# every OpenAPI dialect newer than it. The community edition has no notion of
# the conversions property, so each edition asserts this response in a file of
# its own
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
  "bytesBundled": 1415,
  "identifier": "{{base}}/test/openapi/product",
  "path": "/test/openapi/product",
  "baseDialect": "https://json-schema.org/draft/2020-12/schema",
  "dialect": "https://spec.openapis.org/oas/3.1/dialect/base",
  "metaschema": false,
  "conversions": {
    "openapi3.1": {
      "mediaType": "application/schema+json"
    },
    "openapi3.2": {
      "mediaType": "application/schema+json"
    }
  },
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
  "bytesBundled": 2370,
  "identifier": "{{base}}/test/openapi/order",
  "path": "/test/openapi/order",
  "baseDialect": "https://json-schema.org/draft/2020-12/schema",
  "dialect": "https://spec.openapis.org/oas/3.2/dialect/2025-09-17",
  "metaschema": false,
  "conversions": {
    "openapi3.2": {
      "mediaType": "application/schema+json"
    }
  },
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

# Converting an OpenAPI 3.1 schema into OpenAPI 3.2 restates the dialect of
# the whole closure, the keywords the OpenAPI vocabulary contributes carried
# across untouched
GET {{base}}/test/openapi/product.json?as=openapi3.2
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <https://spec.openapis.org/oas/3.2/dialect/2025-09-17>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "https://spec.openapis.org/oas/3.2/dialect/2025-09-17",
  "$id": "{{base}}/test/openapi/product",
  "title": "Product",
  "description": "A product that exercises every keyword of the OpenAPI 3.1 base vocabulary",
  "examples": [
    {
      "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "kind": "book"
    }
  ],
  "example": {
    "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
    "kind": "book"
  },
  "externalDocs": {
    "url": "https://example.com/products",
    "description": "Product documentation"
  },
  "type": "object",
  "discriminator": {
    "propertyName": "kind"
  },
  "required": [ "id", "kind" ],
  "properties": {
    "id": {
      "$ref": "{{base}}/test/openapi/identifier"
    },
    "kind": {
      "enum": [ "book", "film" ]
    },
    "price": {
      "type": "number",
      "exclusiveMinimum": 0,
      "xml": {
        "name": "Price",
        "attribute": true
      }
    }
  },
  "additionalProperties": false,
  "xml": {
    "name": "Product"
  },
  "$defs": {
    "{{base}}/test/openapi/identifier": {
      "$schema": "https://spec.openapis.org/oas/3.2/dialect/2025-09-17",
      "$id": "{{base}}/test/openapi/identifier",
      "title": "Identifier",
      "description": "A product identifier",
      "examples": [ "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9" ],
      "type": "string",
      "format": "uuid"
    }
  }
}

# Only upgrades are offered, so asking an OpenAPI schema for the 2020-12 it
# builds on is asking for a dialect older than the one it declares
GET {{base}}/test/openapi/product.json?as=2020-12
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "type": "urn:sourcemeta:one:invalid-conversion",
  "title": "Bad Request",
  "status": 400,
  "detail": "The schema cannot be converted into this dialect"
}

# A meta-schema built on an OpenAPI dialect is still a meta-schema, so it is
# offered no conversion at all, whichever rung of the ladder its own dialect
# sits on
GET {{base}}/self/v1/api/schemas/metadata/test/openapi/dialect
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
  "bytes": 698,
  "bytesBundled": 16863,
  "identifier": "{{base}}/test/openapi/dialect",
  "path": "/test/openapi/dialect",
  "baseDialect": "https://json-schema.org/draft/2020-12/schema",
  "dialect": "https://spec.openapis.org/oas/3.1/dialect/base",
  "metaschema": true,
  "conversions": {},
  "title": "Custom OpenAPI Dialect",
  "description": "A meta-schema extending the OpenAPI 3.1 dialect with a vocabulary of its own",
  "examples": [
    {
      "type": "string"
    }
  ],
  "health": 100,
  "dependencies": 3,
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
      "name": "dialect",
      "path": "/test/openapi/dialect"
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

GET {{base}}/test/openapi/dialect.json?as=openapi3.2
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "type": "urn:sourcemeta:one:metaschema-conversion",
  "title": "Bad Request",
  "status": 400,
  "detail": "Meta-schemas are not converted, as restating the dialect that a meta-schema describes is not always safe"
}
