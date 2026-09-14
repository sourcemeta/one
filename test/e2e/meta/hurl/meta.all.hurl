GET {{base}}
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "Sourcemeta Schemas"
header "Access-Control-Allow-Origin" not exists

GET {{base}}/meta/custom.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/meta/custom",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://json-schema.org/draft/2020-12/vocab/meta-data": true,
    "https://json-schema.org/draft/2020-12/vocab/format-annotation": true,
    "https://json-schema.org/draft/2020-12/vocab/content": true
  },
  "title": "Custom Meta-Schema",
  "description": "A custom meta-schema that extends the JSON Schema 2020-12 dialect with additional top level and recursive constraints for testing purposes",
  "examples": [
    {
      "x-author": "Jane Doe",
      "title": "Example",
      "type": "object",
      "properties": {
        "name": {
          "type": "string"
        }
      },
      "additionalProperties": false
    }
  ],
  "allOf": [
    {
      "$ref": "circular"
    },
    {
      "$ref": "rules/top/x-author-required"
    }
  ]
}

GET {{base}}/meta/circular.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/meta/circular",
  "$dynamicAnchor": "meta",
  "title": "Circular Schema",
  "description": "The recursive schema definition that applies to every nested subschema, disallowing any keyword not defined by the dialect",
  "examples": [
    {
      "type": "string"
    }
  ],
  "allOf": [
    {
      "$ref": "https://json-schema.org/draft/2020-12/meta/core"
    },
    {
      "$ref": "https://json-schema.org/draft/2020-12/meta/applicator"
    },
    {
      "$ref": "https://json-schema.org/draft/2020-12/meta/validation"
    },
    {
      "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data"
    },
    {
      "$ref": "https://json-schema.org/draft/2020-12/meta/format-annotation"
    },
    {
      "$ref": "https://json-schema.org/draft/2020-12/meta/content"
    },
    {
      "$ref": "keywords/x-author"
    },
    {
      "$ref": "rules/circular/object-additional-properties"
    },
    {
      "$ref": "rules/circular/no-conditionals"
    }
  ],
  "unevaluatedProperties": false
}

GET {{base}}/with-relative-default-dialect/sample.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <{{base}}/meta/custom>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "{{base}}/meta/custom",
  "$id": "{{base}}/with-relative-default-dialect/sample",
  "x-author": "Jane Doe",
  "type": "string"
}

GET {{base}}/with-jsonschema-json/sample.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <{{base}}/meta/custom>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "{{base}}/meta/custom",
  "$id": "{{base}}/with-jsonschema-json/sample",
  "x-author": "Jane Doe",
  "type": "string"
}

GET {{base}}/with-schema-resolve-and-default-dialect/sample.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <{{base}}/meta/custom>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "$schema": "{{base}}/meta/custom",
  "$id": "{{base}}/with-schema-resolve-and-default-dialect/sample",
  "x-author": "Jane Doe",
  "type": "string"
}

# A 2020-12 schema declaring its vocabularies is a meta-schema even when no
# schema declares it as its dialect
GET {{base}}/self/v1/api/schemas/metadata/meta/keywords/x-author
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 484
jsonpath "$.bytesBundled" == 484
jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.health" == 100
jsonpath "$.priority" == 50
jsonpath "$.dependencies" == 0
jsonpath "$.dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.metaschema" == true
jsonpath "$.private" == false
jsonpath "$.identifier" == "{{base}}/meta/keywords/x-author"
jsonpath "$.path" == "/meta/keywords/x-author"
jsonpath "$.title" == "Author Keyword"
jsonpath "$.description" == "Keyword for attributing schemas to their authors"
jsonpath "$.alert" == null
jsonpath "$.examples" count == 1
jsonpath "$.examples[0]['x-author']" == "Jane Doe"
jsonpath "$.breadcrumb" count == 3
jsonpath "$.breadcrumb[0].name" == "meta"
jsonpath "$.breadcrumb[0].path" == "/meta/"
jsonpath "$.breadcrumb[1].name" == "keywords"
jsonpath "$.breadcrumb[1].path" == "/meta/keywords/"
jsonpath "$.breadcrumb[2].name" == "x-author"
jsonpath "$.breadcrumb[2].path" == "/meta/keywords/x-author"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# A schema taking a custom dialect from its collection is not a meta-schema, and
# names that dialect as the identifier of the schema it points at
GET {{base}}/self/v1/api/schemas/metadata/with-relative-default-dialect/sample
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 169
jsonpath "$.bytesBundled" == 169
jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.health" == 0
jsonpath "$.priority" == 50
jsonpath "$.dependencies" == 12
jsonpath "$.dialect" == "{{base}}/meta/custom"
jsonpath "$.metaschema" == false
jsonpath "$.private" == false
jsonpath "$.identifier" == "{{base}}/with-relative-default-dialect/sample"
jsonpath "$.path" == "/with-relative-default-dialect/sample"
jsonpath "$.alert" == null
jsonpath "$.examples" count == 0
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "with-relative-default-dialect"
jsonpath "$.breadcrumb[0].path" == "/with-relative-default-dialect/"
jsonpath "$.breadcrumb[1].name" == "sample"
jsonpath "$.breadcrumb[1].path" == "/with-relative-default-dialect/sample"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
