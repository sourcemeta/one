# The revision that reserves no field for an identity is served under the
# one this instance gives it, with every reference it makes left where it
# already pointed
GET {{base}}/test/schemas/openapi-3-0
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.0>; rel="describedby"
[Captures]
baseline_body: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
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

# That description conforms to the schema it names as describing it
POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{baseline_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# A reference an OpenAPI description writes as its own absolute identity
# would lead nowhere once the identity is the one this instance serves it under
GET {{base}}/test/schemas/openapi-3-1
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.1>; rel="describedby"
[Captures]
body_3_1: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
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

# That description conforms to the schema it names as describing it
POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{body_3_1}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# The revision that reserves a field for the identity is served under the one
# this instance gives it, written absolute and ranked where the specification
# puts the field rather than where the document wrote it
GET {{base}}/test/schemas/petstore-api
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
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
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Captures]
body_3_2: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
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

# That description conforms to the schema it names as describing it
POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{body_3_2}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# An OpenAPI description built out of several documents is served as the
# documents it is made of, each of them reaching the others under the identity
# this instance gives them rather than the one they came with. Every position
# the specification lets a reference sit in is one of those, the key of a
# Security Requirement Object included, and so is a Discriminator Object
# mapping naming a schema by URI, while one naming a component by name is left
# as it was written
GET {{base}}/test/schemas/openapi-split-entry
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/schemas/openapi-split-entry\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/schemas/openapi-split-entry",
  "info": {
    "title": "Split",
    "version": "1.0.0"
  },
  "security": [
    {
      "openapi-split-shared#/components/securitySchemes/Key": []
    }
  ],
  "paths": {
    "/dogs": {
      "get": {
        "parameters": [
          {
            "$ref": "openapi-split-shared#/components/parameters/Page"
          }
        ],
        "requestBody": {
          "$ref": "openapi-split-shared#/components/requestBodies/NewPet"
        },
        "responses": {
          "200": {
            "description": "Every dog",
            "headers": {
              "X-Limit": {
                "$ref": "openapi-split-shared#/components/headers/Limit"
              }
            },
            "links": {
              "self": {
                "$ref": "openapi-split-shared#/components/links/Self"
              }
            },
            "content": {
              "application/json": {
                "$ref": "openapi-split-shared#/components/mediaTypes/Json"
              }
            }
          },
          "404": {
            "$ref": "openapi-split-shared#/components/responses/NotFound"
          }
        },
        "callbacks": {
          "onData": {
            "$ref": "openapi-split-shared#/components/callbacks/OnData"
          }
        }
      }
    },
    "/pets": {
      "$ref": "openapi-split-shared#/components/pathItems/Pets"
    }
  },
  "components": {
    "schemas": {
      "Any": {
        "oneOf": [
          {
            "$ref": "string"
          },
          {
            "$ref": "#/components/schemas/Dog"
          }
        ],
        "discriminator": {
          "propertyName": "kind",
          "mapping": {
            "name": "./string",
            "dog": "Dog"
          }
        }
      },
      "Dog": {
        "type": "object",
        "properties": {
          "name": {
            "$ref": "string"
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
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/schemas/openapi-split-shared\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/schemas/openapi-split-shared",
  "info": {
    "title": "Shared",
    "version": "1.0.0"
  },
  "components": {
    "responses": {
      "NotFound": {
        "description": "Missing"
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
              "type": "object"
            }
          }
        }
      }
    },
    "mediaTypes": {
      "Json": {
        "schema": {
          "type": "object"
        }
      }
    },
    "headers": {
      "Limit": {
        "description": "How many",
        "schema": {
          "type": "integer"
        }
      }
    },
    "securitySchemes": {
      "Key": {
        "type": "apiKey",
        "name": "key",
        "in": "header"
      }
    },
    "links": {
      "Self": {
        "operationId": "listSharedPets"
      }
    },
    "callbacks": {
      "OnData": {
        "{$request.query.url}": {
          "post": {
            "responses": {
              "200": {
                "description": "Taken"
              }
            }
          }
        }
      }
    },
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

# A Discriminator Object mapping that names a schema by URI is served with the
# path segment that says so, as the specification reads a value that holds up
# as the name of a component as such a name and this document holds a component
# of exactly that name. A mapping that does name a component is left as it was
# written, and so is one that comes out of this naming a place of the document
# that spells it
GET {{base}}/test/schemas/openapi-discriminator
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/schemas/openapi-discriminator\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/schemas/openapi-discriminator",
  "info": {
    "title": "Discriminator",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "Any": {
        "oneOf": [
          {
            "$ref": "string"
          },
          {
            "$ref": "#/components/schemas/Dog"
          }
        ],
        "discriminator": {
          "propertyName": "kind",
          "mapping": {
            "text": "./string",
            "dog": "Dog",
            "anchored": "#Dog"
          }
        }
      },
      "Dog": {
        "$anchor": "Dog",
        "type": "object",
        "properties": {
          "bark": {
            "type": "boolean"
          }
        }
      },
      "string": {
        "title": "A component going by the name that the schema reached by URI is spelled with",
        "type": "object"
      }
    }
  }
}

# Bundling the same description embeds the schema it reaches under a name of
# its own, since the one it would take is taken, and the mapping goes on
# naming it by the URI it answers to rather than by either component name
GET {{base}}/test/schemas/openapi-discriminator?bundle
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/schemas/openapi-discriminator\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/schemas/openapi-discriminator",
  "info": {
    "title": "Discriminator",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "Any": {
        "oneOf": [
          {
            "$ref": "{{base}}/test/schemas/string"
          },
          {
            "$ref": "#/components/schemas/Dog"
          }
        ],
        "discriminator": {
          "propertyName": "kind",
          "mapping": {
            "text": "{{base}}/test/schemas/string",
            "dog": "Dog",
            "anchored": "#Dog"
          }
        }
      },
      "Dog": {
        "$anchor": "Dog",
        "type": "object",
        "properties": {
          "bark": {
            "type": "boolean"
          }
        }
      },
      "string": {
        "title": "A component going by the name that the schema reached by URI is spelled with",
        "type": "object"
      },
      "string_": {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "$id": "{{base}}/test/schemas/string",
        "examples": [
          "example_01",
          "example_02",
          "example_03",
          "example_04",
          5,
          "example_06",
          "example_07",
          "example_08",
          "example_09",
          "example_10",
          "example_11",
          "example_12"
        ],
        "type": "string"
      }
    }
  }
}

# A description may name more than one place of a single schema it reaches,
# and each reference is served pointing at the place it named
GET {{base}}/test/schemas/openapi-two-positions
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.1>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.1.1\",\n  \"info\": {\n"
{
  "openapi": "3.1.1",
  "info": {
    "title": "Two positions",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "Age": {
        "$ref": "named-places#/$defs/age"
      },
      "Name": {
        "$ref": "named-places#/$defs/name"
      }
    }
  }
}

# Bundling that description brings the schema in once, under the identity it
# answers to, and both references go on naming the places they named before
GET {{base}}/test/schemas/openapi-two-positions?bundle
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.1>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.1.1\",\n  \"info\": {\n"
{
  "openapi": "3.1.1",
  "info": {
    "title": "Two positions",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "Age": {
        "$ref": "{{base}}/test/schemas/named-places#/$defs/age"
      },
      "Name": {
        "$ref": "{{base}}/test/schemas/named-places#/$defs/name"
      },
      "named-places": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "{{base}}/test/schemas/named-places",
        "$defs": {
          "name": {
            "type": "string"
          },
          "age": {
            "type": "integer"
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
Link: </self/v1/schemas/common/openapi/3.0>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
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
Link: </self/v1/schemas/common/openapi/3.0>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
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
Link: </self/v1/schemas/common/openapi/3.0>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
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

# A description that reaches nothing beyond itself bundles to what it already
# holds
GET {{base}}/test/schemas/openapi-3-0?bundle
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.0>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body == "{{baseline_body}}"

# A description built out of several documents bundles into one, with what it
# reached landing in its own Components Object and every reference naming it
# there, whichever position the reference sat in. The key of a Security
# Requirement Object becomes the name of the scheme it brought in, as that key
# is the member the scopes sit under rather than a value of its own. A
# Discriminator Object mapping that named a schema by URI names the component
# it became, and one that named a component by name is left as it was written
GET {{base}}/test/schemas/openapi-split-entry?bundle
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Captures]
bundled_body: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/schemas/openapi-split-entry\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/schemas/openapi-split-entry",
  "info": {
    "title": "Split",
    "version": "1.0.0"
  },
  "security": [
    {
      "Key": []
    }
  ],
  "paths": {
    "/dogs": {
      "get": {
        "parameters": [
          {
            "$ref": "#/components/parameters/Page"
          }
        ],
        "requestBody": {
          "$ref": "#/components/requestBodies/NewPet"
        },
        "responses": {
          "200": {
            "description": "Every dog",
            "headers": {
              "X-Limit": {
                "$ref": "#/components/headers/Limit"
              }
            },
            "links": {
              "self": {
                "$ref": "#/components/links/Self"
              }
            },
            "content": {
              "application/json": {
                "$ref": "#/components/mediaTypes/Json"
              }
            }
          },
          "404": {
            "$ref": "#/components/responses/NotFound"
          }
        },
        "callbacks": {
          "onData": {
            "$ref": "#/components/callbacks/OnData"
          }
        }
      }
    },
    "/pets": {
      "$ref": "#/components/pathItems/Pets"
    }
  },
  "components": {
    "responses": {
      "NotFound": {
        "description": "Missing"
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
              "type": "object"
            }
          }
        }
      }
    },
    "mediaTypes": {
      "Json": {
        "schema": {
          "type": "object"
        }
      }
    },
    "headers": {
      "Limit": {
        "description": "How many",
        "schema": {
          "type": "integer"
        }
      }
    },
    "securitySchemes": {
      "Key": {
        "type": "apiKey",
        "name": "key",
        "in": "header"
      }
    },
    "links": {
      "Self": {
        "operationId": "listSharedPets"
      }
    },
    "callbacks": {
      "OnData": {
        "{$request.query.url}": {
          "post": {
            "responses": {
              "200": {
                "description": "Taken"
              }
            }
          }
        }
      }
    },
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
      "Any": {
        "oneOf": [
          {
            "$ref": "{{base}}/test/schemas/string"
          },
          {
            "$ref": "#/components/schemas/Dog"
          }
        ],
        "discriminator": {
          "propertyName": "kind",
          "mapping": {
            "name": "{{base}}/test/schemas/string",
            "dog": "Dog"
          }
        }
      },
      "Dog": {
        "type": "object",
        "properties": {
          "name": {
            "$ref": "{{base}}/test/schemas/string"
          }
        }
      },
      "string": {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "$id": "{{base}}/test/schemas/string",
        "examples": [
          "example_01",
          "example_02",
          "example_03",
          "example_04",
          5,
          "example_06",
          "example_07",
          "example_08",
          "example_09",
          "example_10",
          "example_11",
          "example_12"
        ],
        "type": "string"
      }
    }
  }
}

# The bundled form conforms to the same schema, which is the one place a
# reference the bundler rewrote could have left a document that does not
POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{bundled_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# And the document it reached bundles to what it already holds, as it reaches
# nothing of its own
GET {{base}}/test/schemas/openapi-split-shared?bundle
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/schemas/openapi-split-shared\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/schemas/openapi-split-shared",
  "info": {
    "title": "Shared",
    "version": "1.0.0"
  },
  "components": {
    "responses": {
      "NotFound": {
        "description": "Missing"
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
              "type": "object"
            }
          }
        }
      }
    },
    "mediaTypes": {
      "Json": {
        "schema": {
          "type": "object"
        }
      }
    },
    "headers": {
      "Limit": {
        "description": "How many",
        "schema": {
          "type": "integer"
        }
      }
    },
    "securitySchemes": {
      "Key": {
        "type": "apiKey",
        "name": "key",
        "in": "header"
      }
    },
    "links": {
      "Self": {
        "operationId": "listSharedPets"
      }
    },
    "callbacks": {
      "OnData": {
        "{$request.query.url}": {
          "post": {
            "responses": {
              "200": {
                "description": "Taken"
              }
            }
          }
        }
      }
    },
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

# An OpenAPI description keeps its own navigation metadata, which the schema
# surface holds nothing of, as the two kinds record it under one filename and
# what each records differs enough to have a surface of its own
GET {{base}}/self/v1/api/schemas/metadata/test/schemas/openapi-3-0
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

# The schema surface holds the edges of a schema, and what an OpenAPI
# description records is shaped for a surface of its own, so there is
# nothing about one here
GET {{base}}/self/v1/api/schemas/dependencies/test/schemas/openapi-3-0
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

# And the same the other way around
GET {{base}}/self/v1/api/schemas/dependents/test/schemas/openapi-3-0
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
