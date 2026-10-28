# An OpenAPI 3.0 Schema Object is served exactly as it was written, carrying
# neither a dialect nor an identifier because its dialect reserves no keyword
# for either, and names that dialect through the header instead
GET {{base}}/openapi3.0/product.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <tag:spec.openapis.org,2024-10-18:oas/3.0/dialect>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
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
      "exclusiveMinimum": true,
      "minimum": 0
    },
    "label": {
      "type": "string",
      "nullable": true
    },
    "legacy": {
      "deprecated": true,
      "type": "boolean"
    },
    "slug": {
      "readOnly": true,
      "type": "string"
    }
  },
  "additionalProperties": false,
  "xml": {
    "name": "Product"
  }
}

# The same holds for a schema that declares nothing at all beyond its own
# keywords, which is the only way a Schema Object can be written
GET {{base}}/openapi3.0/identifier.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <tag:spec.openapis.org,2024-10-18:oas/3.0/dialect>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "title": "Identifier",
  "description": "A product identifier, which cannot declare a dialect of its own",
  "example": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
  "type": "string",
  "format": "uuid"
}

# A schema in the same collection that does declare a dialect keeps the one it
# declares, so the collection default applies only where a schema is silent,
# and such a schema is still given an identifier of its own
GET {{base}}/openapi3.0/modern.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/openapi3.0/modern",
  "title": "Modern",
  "description": "A schema that declares its own dialect, so the collection default does not apply to it",
  "examples": [ 1 ],
  "type": "integer",
  "exclusiveMinimum": 0
}

# Bundling embeds the closure under the container the dialect admits, pointing
# at it by location rather than by name, as a Schema Object cannot be named
GET {{base}}/openapi3.0/product.json?bundle=1
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <tag:spec.openapis.org,2024-10-18:oas/3.0/dialect>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
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
      "$ref": "#/x-definitions/http:~1~1localhost:8000~1openapi3.0~1identifier"
    },
    "kind": {
      "enum": [ "book", "film" ]
    },
    "price": {
      "type": "number",
      "exclusiveMinimum": true,
      "minimum": 0
    },
    "label": {
      "type": "string",
      "nullable": true
    },
    "legacy": {
      "deprecated": true,
      "type": "boolean"
    },
    "slug": {
      "readOnly": true,
      "type": "string"
    }
  },
  "additionalProperties": false,
  "xml": {
    "name": "Product"
  },
  "x-definitions": {
    "{{base}}/openapi3.0/identifier": {
      "title": "Identifier",
      "description": "A product identifier, which cannot declare a dialect of its own",
      "example": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "type": "string",
      "format": "uuid"
    }
  }
}

# The dialect is reported as declared, the base dialect it rests on, and every
# dialect this instance can convert it into
GET {{base}}/self/v1/api/schemas/metadata/openapi3.0/product
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
  "bytes": 917,
  "bytesBundled": 1274,
  "identifier": "{{base}}/openapi3.0/product",
  "path": "/openapi3.0/product",
  "baseDialect": "http://json-schema.org/draft-04/schema#",
  "dialect": "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect",
  "metaschema": false,
  "conversions": {
    "draft4": {
      "mediaType": "application/schema+json"
    },
    "draft6": {
      "mediaType": "application/schema+json"
    },
    "draft7": {
      "mediaType": "application/schema+json"
    },
    "2019-09": {
      "mediaType": "application/schema+json"
    },
    "2020-12": {
      "mediaType": "application/schema+json"
    },
    "openapi3.1": {
      "mediaType": "application/schema+json"
    },
    "openapi3.2": {
      "mediaType": "application/schema+json"
    }
  },
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
  "examples": [],
  "health": 100,
  "dependencies": 1,
  "alert": null,
  "priority": 50,
  "breadcrumb": [
    {
      "name": "openapi3.0",
      "path": "/openapi3.0/"
    },
    {
      "name": "product",
      "path": "/openapi3.0/product"
    }
  ],
  "private": false
}

# That response conforms to the schema it names as describing it
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

# Linting reaches a Schema Object through its own vocabulary
GET {{base}}/self/v1/api/schemas/health/openapi3.0/product
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

# That response conforms to the schema it names as describing it
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

# And reads it against the dialect it actually rests on, so a format the
# OpenAPI 3.0 lineage never defined is reported rather than taken on trust
GET {{base}}/self/v1/api/schemas/health/openapi3.0/identifier
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
  "score": 0,
  "errors": [
    {
      "name": "unknown_format_prefix",
      "message": "For interoperability purposes, the JSON Schema specification advises against the use of `format` values that are not explicitly defined by the specification",
      "description": null,
      "custom": false,
      "pointers": [ "/format" ]
    }
  ]
}

# That response conforms to the schema it names as describing it
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

# A Schema Object is located as a resource under the URL that serves it
GET {{base}}/self/v1/api/schemas/locations/openapi3.0/identifier
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/locations/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
{
  "static": {
    "{{base}}/openapi3.0/identifier": {
      "parent": null,
      "type": "resource",
      "root": "{{base}}/openapi3.0/identifier",
      "base": "{{base}}/openapi3.0/identifier",
      "pointer": "",
      "position": [ 1, 1, 7, 1 ],
      "relativePointer": "",
      "dialect": "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect",
      "baseDialect": "http://json-schema.org/draft-04/schema#",
      "propertyName": false,
      "orphan": false
    }
  },
  "dynamic": {}
}

# That response conforms to the schema it names as describing it
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

# A reference between two Schema Objects is traced like any other
GET {{base}}/self/v1/api/schemas/dependencies/openapi3.0/product
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/dependencies/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
[
  {
    "from": "{{base}}/openapi3.0/product",
    "to": "{{base}}/openapi3.0/identifier",
    "at": "/properties/id/$ref",
    "baseDialect": "http://json-schema.org/draft-04/schema#"
  }
]

# That response conforms to the schema it names as describing it
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

# Keyword counts are attributed to the OpenAPI 3.0 base vocabulary
GET {{base}}/self/v1/api/schemas/stats/openapi3.0/product
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
  "tag:spec.openapis.org,2017:oas/3.0/vocab/base": {
    "$ref": 1,
    "additionalProperties": 1,
    "deprecated": 1,
    "description": 1,
    "discriminator": 1,
    "enum": 1,
    "example": 1,
    "exclusiveMinimum": 1,
    "externalDocs": 1,
    "minimum": 1,
    "nullable": 1,
    "properties": 1,
    "readOnly": 1,
    "required": 1,
    "title": 1,
    "type": 5,
    "xml": 1
  }
}

# That response conforms to the schema it names as describing it
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

# An instance validates against a Schema Object, with a nullable member
# admitting the null it was written to admit
POST {{base}}/self/v1/api/schemas/evaluate/openapi3.0/product
```
{ "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9", "kind": "book", "label": null, "price": 5 }
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "valid": true
}

# A nullable member still refuses anything else
POST {{base}}/self/v1/api/schemas/evaluate/openapi3.0/product
```
{ "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9", "kind": "book", "label": 7 }
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "valid": false,
  "errors": [
    {
      "keywordLocation": "/properties/label/type",
      "absoluteKeywordLocation": "{{base}}/openapi3.0/product#/properties/label/type",
      "instanceLocation": "/label",
      "error": "The value was expected to be of type null, or string but it was of type integer"
    },
    {
      "keywordLocation": "/properties",
      "absoluteKeywordLocation": "{{base}}/openapi3.0/product#/properties",
      "instanceLocation": "",
      "error": "The object value was expected to validate against the 6 defined properties subschemas"
    }
  ]
}

# Converted into draft4, the oldest dialect anything converts into, where
# every keyword OpenAPI 3.0 adds has to be set aside and an exclusive bound
# is still a flag on another
GET {{base}}/openapi3.0/product.json?as=draft4
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <http://json-schema.org/draft-04/schema\#>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-04/schema#",
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
  "x-discriminator": {
    "propertyName": "kind"
  },
  "x-example": {
    "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
    "kind": "book"
  },
  "x-externalDocs": {
    "url": "https://example.com/products",
    "description": "Product documentation"
  },
  "x-xml": {
    "name": "Product"
  },
  "type": "object",
  "required": [ "id", "kind" ],
  "properties": {
    "id": {
      "$ref": "#/definitions/http:~1~1localhost:8000~1openapi3.0~1identifier"
    },
    "kind": {
      "enum": [ "book", "film" ]
    },
    "price": {
      "type": "number",
      "exclusiveMinimum": true,
      "minimum": 0
    },
    "label": {
      "type": [ "string", "null" ]
    },
    "legacy": {
      "x-deprecated": true,
      "type": "boolean"
    },
    "slug": {
      "x-readOnly": true,
      "type": "string"
    }
  },
  "additionalProperties": false,
  "definitions": {
    "{{base}}/openapi3.0/identifier": {
      "title": "Identifier",
      "description": "A product identifier, which cannot declare a dialect of its own",
      "x-example": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "type": "string",
      "format": "uuid"
    }
  }
}

# Converted into draft6, where an exclusive bound becomes a number in its
# own right
GET {{base}}/openapi3.0/product.json?as=draft6
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <http://json-schema.org/draft-06/schema\#>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-06/schema#",
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
  "x-discriminator": {
    "propertyName": "kind"
  },
  "x-example": {
    "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
    "kind": "book"
  },
  "x-externalDocs": {
    "url": "https://example.com/products",
    "description": "Product documentation"
  },
  "x-xml": {
    "name": "Product"
  },
  "type": "object",
  "required": [ "id", "kind" ],
  "properties": {
    "id": {
      "$ref": "#/definitions/http:~1~1localhost:8000~1openapi3.0~1identifier"
    },
    "kind": {
      "enum": [ "book", "film" ]
    },
    "price": {
      "type": "number",
      "exclusiveMinimum": 0
    },
    "label": {
      "type": [ "string", "null" ]
    },
    "legacy": {
      "x-deprecated": true,
      "type": "boolean"
    },
    "slug": {
      "x-readOnly": true,
      "type": "string"
    }
  },
  "additionalProperties": false,
  "definitions": {
    "{{base}}/openapi3.0/identifier": {
      "title": "Identifier",
      "description": "A product identifier, which cannot declare a dialect of its own",
      "x-example": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "type": "string",
      "format": "uuid"
    }
  }
}

# Converted into draft7, where `readOnly` becomes a keyword of the dialect
# rather than an extension
GET {{base}}/openapi3.0/product.json?as=draft7
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <http://json-schema.org/draft-07/schema\#>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
  "x-discriminator": {
    "propertyName": "kind"
  },
  "x-example": {
    "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
    "kind": "book"
  },
  "x-externalDocs": {
    "url": "https://example.com/products",
    "description": "Product documentation"
  },
  "x-xml": {
    "name": "Product"
  },
  "type": "object",
  "required": [ "id", "kind" ],
  "properties": {
    "id": {
      "$ref": "#/definitions/http:~1~1localhost:8000~1openapi3.0~1identifier"
    },
    "kind": {
      "enum": [ "book", "film" ]
    },
    "price": {
      "type": "number",
      "exclusiveMinimum": 0
    },
    "label": {
      "type": [ "string", "null" ]
    },
    "legacy": {
      "x-deprecated": true,
      "type": "boolean"
    },
    "slug": {
      "readOnly": true,
      "type": "string"
    }
  },
  "additionalProperties": false,
  "definitions": {
    "{{base}}/openapi3.0/identifier": {
      "title": "Identifier",
      "description": "A product identifier, which cannot declare a dialect of its own",
      "x-example": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "type": "string",
      "format": "uuid"
    }
  }
}

# Converted into 2019-09, where `deprecated` becomes a keyword of the
# dialect too
GET {{base}}/openapi3.0/product.json?as=2019-09
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <https://json-schema.org/draft/2019-09/schema>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2019-09/schema",
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
  "x-discriminator": {
    "propertyName": "kind"
  },
  "x-example": {
    "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
    "kind": "book"
  },
  "x-externalDocs": {
    "url": "https://example.com/products",
    "description": "Product documentation"
  },
  "x-xml": {
    "name": "Product"
  },
  "type": "object",
  "required": [ "id", "kind" ],
  "properties": {
    "id": {
      "$ref": "#/$defs/http:~1~1localhost:8000~1openapi3.0~1identifier"
    },
    "kind": {
      "enum": [ "book", "film" ]
    },
    "price": {
      "type": "number",
      "exclusiveMinimum": 0
    },
    "label": {
      "type": [ "string", "null" ]
    },
    "legacy": {
      "deprecated": true,
      "type": "boolean"
    },
    "slug": {
      "readOnly": true,
      "type": "string"
    }
  },
  "additionalProperties": false,
  "$defs": {
    "{{base}}/openapi3.0/identifier": {
      "title": "Identifier",
      "description": "A product identifier, which cannot declare a dialect of its own",
      "x-example": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "type": "string",
      "format": "uuid"
    }
  }
}

# Converted into 2020-12, the newest dialect of JSON Schema itself, which
# still gives the OpenAPI keywords no meaning
GET {{base}}/openapi3.0/product.json?as=2020-12
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
  "x-discriminator": {
    "propertyName": "kind"
  },
  "x-example": {
    "id": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
    "kind": "book"
  },
  "x-externalDocs": {
    "url": "https://example.com/products",
    "description": "Product documentation"
  },
  "x-xml": {
    "name": "Product"
  },
  "type": "object",
  "required": [ "id", "kind" ],
  "properties": {
    "id": {
      "$ref": "#/$defs/http:~1~1localhost:8000~1openapi3.0~1identifier"
    },
    "kind": {
      "enum": [ "book", "film" ]
    },
    "price": {
      "type": "number",
      "exclusiveMinimum": 0
    },
    "label": {
      "type": [ "string", "null" ]
    },
    "legacy": {
      "deprecated": true,
      "type": "boolean"
    },
    "slug": {
      "readOnly": true,
      "type": "string"
    }
  },
  "additionalProperties": false,
  "$defs": {
    "{{base}}/openapi3.0/identifier": {
      "title": "Identifier",
      "description": "A product identifier, which cannot declare a dialect of its own",
      "x-example": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "type": "string",
      "format": "uuid"
    }
  }
}

# Converted into openapi3.1, the first dialect that defines the OpenAPI
# keywords, so they are carried over as they were written rather than set
# aside
GET {{base}}/openapi3.0/product.json?as=openapi3.1
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
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
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
      "$ref": "#/$defs/http:~1~1localhost:8000~1openapi3.0~1identifier"
    },
    "kind": {
      "enum": [ "book", "film" ]
    },
    "price": {
      "type": "number",
      "exclusiveMinimum": 0
    },
    "label": {
      "type": [ "string", "null" ]
    },
    "legacy": {
      "deprecated": true,
      "type": "boolean"
    },
    "slug": {
      "readOnly": true,
      "type": "string"
    }
  },
  "additionalProperties": false,
  "xml": {
    "name": "Product"
  },
  "$defs": {
    "{{base}}/openapi3.0/identifier": {
      "title": "Identifier",
      "description": "A product identifier, which cannot declare a dialect of its own",
      "example": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "type": "string",
      "format": "uuid"
    }
  }
}

# Converted into openapi3.2, and the same holds one revision later
GET {{base}}/openapi3.0/product.json?as=openapi3.2
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
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
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
      "$ref": "#/$defs/http:~1~1localhost:8000~1openapi3.0~1identifier"
    },
    "kind": {
      "enum": [ "book", "film" ]
    },
    "price": {
      "type": "number",
      "exclusiveMinimum": 0
    },
    "label": {
      "type": [ "string", "null" ]
    },
    "legacy": {
      "deprecated": true,
      "type": "boolean"
    },
    "slug": {
      "readOnly": true,
      "type": "string"
    }
  },
  "additionalProperties": false,
  "xml": {
    "name": "Product"
  },
  "$defs": {
    "{{base}}/openapi3.0/identifier": {
      "title": "Identifier",
      "description": "A product identifier, which cannot declare a dialect of its own",
      "example": "0a1b2c3d-4e5f-6071-8293-a4b5c6d7e8f9",
      "type": "string",
      "format": "uuid"
    }
  }
}

