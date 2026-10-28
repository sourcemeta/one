# The dialect is reported as declared and the base dialect it rests on. The
# enterprise edition answers with a conversions property this edition has no
# notion of, so each asserts this response in a file of its own
GET {{base}}/self/v1/api/schemas/metadata/test/openapi3.0/product
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
  "bytesBundled": 1285,
  "identifier": "{{base}}/test/openapi3.0/product",
  "path": "/test/openapi3.0/product",
  "baseDialect": "http://json-schema.org/draft-04/schema#",
  "dialect": "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect",
  "metaschema": false,
  "title": "Product",
  "description": "A product that exercises the keywords OpenAPI 3.0 adds to Draft 4",
  "examples": [],
  "health": 100,
  "dependencies": 1,
  "alert": null,
  "priority": 50,
  "breadcrumb": [
    {
      "name": "test",
      "path": "/test/"
    },
    {
      "name": "openapi3.0",
      "path": "/test/openapi3.0/"
    },
    {
      "name": "product",
      "path": "/test/openapi3.0/product"
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

