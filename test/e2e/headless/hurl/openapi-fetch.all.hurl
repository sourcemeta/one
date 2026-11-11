# The revision that reserves no field for an identity is served under the
# one this instance gives it, with every reference it makes left where it
# already pointed
GET {{base}}/test/schemas/openapi-3-0
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
baseline_body: body
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Link" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.0.4\",\n  \"info\": {\n"
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

# A reference an OpenAPI description writes as its own absolute identity
# would lead nowhere once the identity is the one this instance serves it under
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
body startsWith "{\n  \"openapi\": \"3.1.1\",\n  \"info\": {\n"
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
# this instance gives it, written absolute and ranked where the specification
# puts the field rather than where the document wrote it
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
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/schemas/petstore-api\",\n  \"info\": {\n"
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

# An OpenAPI description using the whole of the shell keeps every connection
# it makes inside itself, and comes back ordered the way the specification
# lists the fields rather than the way the document wrote them
GET {{base}}/test/schemas/openapi-3-2-detailed
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
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/schemas/openapi-3-2-detailed\",\n  \"info\": {\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/schemas/openapi-3-2-detailed",
  "info": {
    "title": "Petstore in full",
    "version": "1.0.0"
  },
  "security": [
    {
      "Key": []
    }
  ],
  "servers": [
    {
      "url": "https://api.example.com"
    }
  ],
  "tags": [
    {
      "name": "pets"
    },
    {
      "name": "dogs",
      "parent": "pets"
    }
  ],
  "webhooks": {
    "petAdded": {
      "$ref": "#/components/pathItems/Ping"
    }
  },
  "paths": {
    "/pets": {
      "parameters": [
        {
          "$ref": "#/components/parameters/Page"
        }
      ],
      "get": {
        "operationId": "listPets",
        "tags": [ "pets" ],
        "responses": {
          "200": {
            "$ref": "#/components/responses/Pets"
          }
        },
        "callbacks": {
          "onData": {
            "{$request.query.callbackUrl}": {
              "$ref": "#/components/pathItems/Ping"
            }
          }
        }
      },
      "post": {
        "requestBody": {
          "$ref": "#/components/requestBodies/NewPet"
        },
        "responses": {
          "201": {
            "description": "Created",
            "headers": {
              "Location": {
                "$ref": "#/components/headers/Location"
              }
            },
            "links": {
              "self": {
                "operationId": "listPets"
              }
            }
          }
        }
      }
    },
    "/pets/{id}": {
      "$ref": "#/components/pathItems/Pet"
    }
  },
  "components": {
    "responses": {
      "Pets": {
        "description": "Every pet",
        "content": {
          "application/json": {
            "schema": {
              "type": "array",
              "items": {
                "$ref": "#/components/schemas/Pet"
              }
            }
          }
        }
      }
    },
    "parameters": {
      "Page": {
        "name": "page",
        "in": "query",
        "schema": {
          "type": "integer"
        }
      }
    },
    "requestBodies": {
      "NewPet": {
        "content": {
          "application/json": {
            "schema": {
              "$ref": "#/components/schemas/Pet"
            }
          }
        }
      }
    },
    "headers": {
      "Location": {
        "schema": {
          "type": "string"
        }
      }
    },
    "securitySchemes": {
      "Key": {
        "type": "apiKey",
        "name": "X-Key",
        "in": "header"
      }
    },
    "pathItems": {
      "Pet": {
        "parameters": [
          {
            "name": "id",
            "in": "path",
            "required": true,
            "schema": {
              "type": "string"
            }
          }
        ],
        "get": {
          "responses": {
            "200": {
              "description": "One pet",
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
      },
      "Ping": {
        "get": {
          "responses": {
            "200": {
              "description": "Pong"
            }
          }
        }
      }
    },
    "schemas": {
      "Dog": {
        "allOf": [
          {
            "$ref": "#/components/schemas/Pet"
          }
        ]
      },
      "Pet": {
        "type": "object",
        "discriminator": {
          "propertyName": "kind",
          "mapping": {
            "dog": "#/components/schemas/Dog"
          }
        },
        "properties": {
          "kind": {
            "type": "string"
          }
        }
      }
    }
  }
}

# An OpenAPI description built out of several documents is served as the
# documents it is made of, each of them reaching the others under the identity
# this instance gives them rather than the one they came with
GET {{base}}/test/schemas/openapi-split-entry
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
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"http://localhost:8000/test/schemas/openapi-split-entry\",\n"
{
  "openapi": "3.2.0",
  "$self": "http://localhost:8000/test/schemas/openapi-split-entry",
  "info": {
    "title": "Split",
    "version": "1.0.0"
  },
  "paths": {
    "/pets": {
      "$ref": "openapi-split-shared#/components/pathItems/Pets"
    }
  },
  "components": {
    "schemas": {
      "Dog": {
        "type": "object",
        "properties": {
          "pet": {
            "$ref": "openapi-split-shared#/components/schemas/Pet"
          }
        }
      }
    }
  }
}

# And the document it reaches is a leaf of its own, served at the identity the
# references above now name
GET {{base}}/test/schemas/openapi-split-shared
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
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"http://localhost:8000/test/schemas/openapi-split-shared\",\n"
{
  "openapi": "3.2.0",
  "$self": "http://localhost:8000/test/schemas/openapi-split-shared",
  "info": {
    "title": "Shared",
    "version": "1.0.0"
  },
  "components": {
    "pathItems": {
      "Pets": {
        "get": {
          "operationId": "listSharedPets",
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
        "type": "object",
        "required": [ "name" ],
        "properties": {
          "name": {
            "type": "string"
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.0.4\",\n  \"info\": {\n"
body == "{{baseline_body}}"

# The editor branch is a workaround for a tool consuming schemas, so an
# OpenAPI description falls through to what is served at its path
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.0.4\",\n  \"info\": {\n"
body == "{{baseline_body}}"

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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.0.4\",\n  \"info\": {\n"
body == "{{baseline_body}}"

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

# Nothing bundles an OpenAPI description yet, and a leaf that is one is still
# a leaf, so asking for a bundle of it asks for what this version does not do
GET {{base}}/test/schemas/openapi-3-0?bundle
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# Nothing records an OpenAPI description for navigation yet
GET {{base}}/self/v1/api/schemas/metadata/test/schemas/openapi-3-0
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# Nothing records what an OpenAPI description depends on yet
GET {{base}}/self/v1/api/schemas/dependencies/test/schemas/openapi-3-0
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# Nothing records what depends on an OpenAPI description yet
GET {{base}}/self/v1/api/schemas/dependents/test/schemas/openapi-3-0
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# Nothing measures the health of an OpenAPI description yet
GET {{base}}/self/v1/api/schemas/health/test/schemas/openapi-3-0
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# Nothing records the URIs an OpenAPI description answers to yet
GET {{base}}/self/v1/api/schemas/locations/test/schemas/openapi-3-0
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# Nothing records where the members of an OpenAPI description sit yet
GET {{base}}/self/v1/api/schemas/positions/test/schemas/openapi-3-0
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# Nothing counts what an OpenAPI description is made of yet
GET {{base}}/self/v1/api/schemas/stats/test/schemas/openapi-3-0
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# Evaluating an instance against an OpenAPI description asks a document that
# is no schema to behave like one
POST {{base}}/self/v1/api/schemas/evaluate/test/schemas/openapi-3-0
Content-Type: application/json
```
{ "instance": {} }
```
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# Tracing an evaluation against an OpenAPI description asks a document that
# is no schema to behave like one
POST {{base}}/self/v1/api/schemas/trace/test/schemas/openapi-3-0
Content-Type: application/json
```
{ "instance": {} }
```
HTTP 501
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
jsonpath "$.status" == 501
jsonpath "$.type" == "urn:sourcemeta:one:openapi-description-unsupported"
jsonpath "$.title" == "Not Implemented"
jsonpath "$.detail" == "This operation is not supported on an OpenAPI description yet"

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

# And a path with nothing at it is still a path with nothing at it
GET {{base}}/self/v1/api/schemas/metadata/test/schemas/openapi-nothing
HTTP 404
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
jsonpath "$.status" == 404
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.title" == "Not Found"
jsonpath "$.detail" == "There is nothing at this URL"

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
