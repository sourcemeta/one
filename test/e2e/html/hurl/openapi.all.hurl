# A schema on an OpenAPI dialect is served as it was written, the keywords of
# the OpenAPI base vocabulary included, and names that dialect as what
# describes it
GET {{base}}/test/openapi/product.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <https://spec.openapis.org/oas/3.1/dialect/base>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "https://spec.openapis.org/oas/3.1/dialect/base",
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
      "$ref": "./identifier"
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
  }
}

GET {{base}}/test/openapi/order.json
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
  "$id": "{{base}}/test/openapi/order",
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
  "externalDocs": {
    "url": "https://example.com/orders"
  },
  "type": "object",
  "required": [ "product" ],
  "properties": {
    "product": {
      "$ref": "./product"
    },
    "quantity": {
      "type": "integer",
      "minimum": 1
    }
  },
  "additionalProperties": false
}

# The collection names an OpenAPI dialect as its default, which this schema
# takes for its own, as a schema lifted out of an OpenAPI document declares
# none
GET {{base}}/test/openapi/identifier.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <https://spec.openapis.org/oas/3.1/dialect/base>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "https://spec.openapis.org/oas/3.1/dialect/base",
  "$id": "{{base}}/test/openapi/identifier",
  "title": "Identifier",
  "description": "A product identifier, taking its dialect from the collection rather than declaring one",
  "examples": [ "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9" ],
  "type": "string",
  "format": "uuid"
}

# Bundling carries each resource across with the dialect it was written on, so
# a document on one OpenAPI dialect can embed a schema on another
GET {{base}}/test/openapi/order.json?bundle=1
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
  "$id": "{{base}}/test/openapi/order",
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
  "externalDocs": {
    "url": "https://example.com/orders"
  },
  "type": "object",
  "required": [ "product" ],
  "properties": {
    "product": {
      "$ref": "{{base}}/test/openapi/product"
    },
    "quantity": {
      "type": "integer",
      "minimum": 1
    }
  },
  "additionalProperties": false,
  "$defs": {
    "{{base}}/test/openapi/identifier": {
      "$schema": "https://spec.openapis.org/oas/3.1/dialect/base",
      "$id": "{{base}}/test/openapi/identifier",
      "title": "Identifier",
      "description": "A product identifier, taking its dialect from the collection rather than declaring one",
      "examples": [ "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9" ],
      "type": "string",
      "format": "uuid"
    },
    "{{base}}/test/openapi/product": {
      "$schema": "https://spec.openapis.org/oas/3.1/dialect/base",
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
      }
    }
  }
}

# Statistics attribute each keyword to the vocabulary that gives it meaning, so
# an OpenAPI dialect puts its own keywords under its own vocabulary, and the
# report still answers to the schema that describes it
GET {{base}}/self/v1/api/schemas/stats/test/openapi/product
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/stats/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
{
  "https://json-schema.org/draft/2020-12/vocab/applicator": {
    "additionalProperties": 1,
    "properties": 1
  },
  "https://json-schema.org/draft/2020-12/vocab/core": {
    "$id": 1,
    "$ref": 1,
    "$schema": 1
  },
  "https://json-schema.org/draft/2020-12/vocab/meta-data": {
    "description": 1,
    "examples": 1,
    "title": 1
  },
  "https://json-schema.org/draft/2020-12/vocab/validation": {
    "enum": 1,
    "exclusiveMinimum": 1,
    "required": 1,
    "type": 2
  },
  "https://spec.openapis.org/oas/3.1/vocab/base": {
    "discriminator": 1,
    "example": 1,
    "externalDocs": 1,
    "xml": 2
  }
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

GET {{base}}/self/v1/api/schemas/stats/test/openapi/order
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/stats/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
{
  "https://json-schema.org/draft/2020-12/vocab/applicator": {
    "additionalProperties": 1,
    "properties": 1
  },
  "https://json-schema.org/draft/2020-12/vocab/core": {
    "$id": 1,
    "$ref": 1,
    "$schema": 1
  },
  "https://json-schema.org/draft/2020-12/vocab/meta-data": {
    "description": 1,
    "examples": 1,
    "title": 1
  },
  "https://json-schema.org/draft/2020-12/vocab/validation": {
    "minimum": 1,
    "required": 1,
    "type": 2
  },
  "https://spec.openapis.org/oas/3.2/vocab/base": {
    "externalDocs": 1
  }
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

GET {{base}}/self/v1/api/schemas/stats/test/openapi/identifier
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/stats/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
{
  "https://json-schema.org/draft/2020-12/vocab/core": {
    "$id": 1,
    "$schema": 1
  },
  "https://json-schema.org/draft/2020-12/vocab/format-annotation": {
    "format": 1
  },
  "https://json-schema.org/draft/2020-12/vocab/meta-data": {
    "description": 1,
    "examples": 1,
    "title": 1
  },
  "https://json-schema.org/draft/2020-12/vocab/validation": {
    "type": 1
  }
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

# The linter judges a schema on an OpenAPI dialect as it judges any other
GET {{base}}/self/v1/api/schemas/health/test/openapi/product
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/health/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
{
  "score": 100,
  "errors": []
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

# What the registry compiles for an OpenAPI dialect asserts the keywords the
# dialect shares with the one it builds on, and collects the annotations of
# every resource the closure reaches, whichever dialect each was written on
POST {{base}}/self/v1/api/schemas/evaluate/test/openapi/order
```
{
  "product": {
    "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
    "kind": "book",
    "price": 9.99
  },
  "quantity": 2
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "valid": true,
  "annotations": [
    {
      "keywordLocation": "/description",
      "absoluteKeywordLocation": "{{base}}/test/openapi/order#/description",
      "instanceLocation": "",
      "annotation": "An order on the OpenAPI 3.2 dialect that references a schema on the OpenAPI 3.1 one"
    },
    {
      "keywordLocation": "/examples",
      "absoluteKeywordLocation": "{{base}}/test/openapi/order#/examples",
      "instanceLocation": "",
      "annotation": [
        {
          "product": {
            "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
            "kind": "book"
          },
          "quantity": 2
        }
      ]
    },
    {
      "keywordLocation": "/properties",
      "absoluteKeywordLocation": "{{base}}/test/openapi/order#/properties",
      "instanceLocation": "",
      "annotation": [ "product", "quantity" ]
    },
    {
      "keywordLocation": "/title",
      "absoluteKeywordLocation": "{{base}}/test/openapi/order#/title",
      "instanceLocation": "",
      "annotation": "Order"
    },
    {
      "keywordLocation": "/properties/product/$ref/description",
      "absoluteKeywordLocation": "{{base}}/test/openapi/product#/description",
      "instanceLocation": "/product",
      "annotation": "A product that exercises every keyword of the OpenAPI 3.1 base vocabulary"
    },
    {
      "keywordLocation": "/properties/product/$ref/examples",
      "absoluteKeywordLocation": "{{base}}/test/openapi/product#/examples",
      "instanceLocation": "/product",
      "annotation": [
        {
          "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
          "kind": "book"
        }
      ]
    },
    {
      "keywordLocation": "/properties/product/$ref/properties",
      "absoluteKeywordLocation": "{{base}}/test/openapi/product#/properties",
      "instanceLocation": "/product",
      "annotation": [ "id", "kind", "price" ]
    },
    {
      "keywordLocation": "/properties/product/$ref/title",
      "absoluteKeywordLocation": "{{base}}/test/openapi/product#/title",
      "instanceLocation": "/product",
      "annotation": "Product"
    },
    {
      "keywordLocation": "/properties/product/$ref/properties/id/$ref/description",
      "absoluteKeywordLocation": "{{base}}/test/openapi/identifier#/description",
      "instanceLocation": "/product/id",
      "annotation": "A product identifier, taking its dialect from the collection rather than declaring one"
    },
    {
      "keywordLocation": "/properties/product/$ref/properties/id/$ref/examples",
      "absoluteKeywordLocation": "{{base}}/test/openapi/identifier#/examples",
      "instanceLocation": "/product/id",
      "annotation": [ "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9" ]
    },
    {
      "keywordLocation": "/properties/product/$ref/properties/id/$ref/format",
      "absoluteKeywordLocation": "{{base}}/test/openapi/identifier#/format",
      "instanceLocation": "/product/id",
      "annotation": "uuid"
    },
    {
      "keywordLocation": "/properties/product/$ref/properties/id/$ref/title",
      "absoluteKeywordLocation": "{{base}}/test/openapi/identifier#/title",
      "instanceLocation": "/product/id",
      "annotation": "Identifier"
    }
  ]
}

POST {{base}}/self/v1/api/schemas/evaluate/test/openapi/order
```
{
  "product": {
    "id": "not-a-uuid",
    "kind": "vinyl"
  },
  "quantity": 0
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "valid": false,
  "errors": [
    {
      "keywordLocation": "/properties/product/$ref/properties/kind/enum",
      "absoluteKeywordLocation": "{{base}}/test/openapi/product#/properties/kind/enum",
      "instanceLocation": "/product/kind",
      "error": "The string value \"vinyl\" was expected to equal one of the following values: \"book\", and \"film\""
    },
    {
      "keywordLocation": "/properties/product/$ref/properties",
      "absoluteKeywordLocation": "{{base}}/test/openapi/product#/properties",
      "instanceLocation": "/product",
      "error": "The object value was expected to validate against the 3 defined properties subschemas"
    },
    {
      "keywordLocation": "/properties/product/$ref",
      "absoluteKeywordLocation": "{{base}}/test/openapi/order#/properties/product/$ref",
      "instanceLocation": "/product",
      "error": "The object value was expected to validate against the referenced schema"
    },
    {
      "keywordLocation": "/properties",
      "absoluteKeywordLocation": "{{base}}/test/openapi/order#/properties",
      "instanceLocation": "",
      "error": "The object value was expected to validate against the 2 defined properties subschemas"
    }
  ]
}
