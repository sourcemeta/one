GET {{base}}/test/schemas/openapi-3-0
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Link" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "openapi": "3.0.4",
  "info": {
    "title": "Petstore",
    "version": "1.0.0"
  },
  "paths": {
    "/pets": {
      "get": {
        "responses": {
          "200": {
            "description": "Every pet",
            "content": {
              "application/json": {
                "schema": {
                  "$ref": "#/components/schemas/Pet"
                }
              }
            }
          }
        }
      }
    }
  },
  "components": {
    "schemas": {
      "Pet": {
        "type": "object",
        "properties": {
          "name": {
            "type": "string"
          }
        }
      }
    }
  }
}

# A reference a description writes as its own absolute identity would lead
# nowhere once the identity is the one this instance serves it under
GET {{base}}/test/schemas/openapi-3-1
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Link" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "openapi": "3.1.1",
  "info": {
    "title": "Petstore",
    "version": "1.0.0"
  },
  "paths": {
    "/pets": {
      "$ref": "#/components/pathItems/Pets"
    }
  },
  "components": {
    "pathItems": {
      "Pets": {
        "get": {
          "responses": {
            "200": {
              "description": "Every pet"
            }
          }
        }
      }
    },
    "schemas": {
      "Pet": {
        "$ref": "#/components/schemas/Tag"
      },
      "Tag": {
        "type": "string"
      }
    }
  }
}

# The revision that reserves a field for the identity is served under the one
# this instance gives it, written absolute and always
GET {{base}}/test/schemas/petstore-api
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Link" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/schemas/petstore-api",
  "info": {
    "title": "Petstore",
    "version": "1.0.0"
  },
  "paths": {
    "/pets": {
      "get": {
        "responses": {
          "200": {
            "description": "Every pet"
          }
        }
      }
    }
  }
}

# The `.json` suffix names the same leaf, as it does for a schema
GET {{base}}/test/schemas/openapi-3-0.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Link" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.openapi" == "3.0.4"

# The editor branch is a workaround for a tool consuming schemas, so a
# description falls through to what is served at its path
GET {{base}}/test/schemas/openapi-3-0
User-Agent: Visual Studio Code
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Link" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.openapi" == "3.0.4"

# And so does the Deno branch, content type included
GET {{base}}/test/schemas/openapi-3-0
User-Agent: Deno/2.4.5
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Link" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.openapi" == "3.0.4"

# Conversion is a JSON Schema dialect conversion, which is a different thing
# from migrating a document to another OpenAPI revision
GET {{base}}/test/schemas/openapi-3-0?as=2020-12
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 400
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-conversion"
jsonpath "$.title" == "Bad Request"
jsonpath "$.detail" == "An OpenAPI description cannot be converted into a JSON Schema dialect"

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

# Nothing bundles a description yet, so there is nothing at this URL
GET {{base}}/test/schemas/openapi-3-0?bundle
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 404
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.title" == "Not Found"
jsonpath "$.detail" == "There is nothing at this URL"

# Nothing emits the artifacts describing a description yet, so every surface
# derived from them is without it
GET {{base}}/self/v1/api/schemas/metadata/test/schemas/openapi-3-0
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 404
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.title" == "Not Found"
jsonpath "$.detail" == "There is nothing at this URL"

# A catalog schema cannot reference into a description, which is the ordinary
# refusal of a reference to nothing
GET {{base}}/self/v1/api/schemas/dependencies/test/schemas/openapi-3-0
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 404
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.title" == "Not Found"
jsonpath "$.detail" == "There is nothing at this URL"
