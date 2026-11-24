# What an MCP client reads for an OpenAPI description, which is the document
# this instance serves at the same URI, and the bundle of it under the query
# the resource template names. A description carries the media type of what it
# holds rather than the one a schema carries

# What this instance serves for the entry document of a description
# built out of two documents, where a Discriminator Object mapping names a
# schema by URI two ways and a component by name once
GET {{base}}/test/openapi-split-entry
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Captures]
entry_document: body
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/openapi-split-entry\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/openapi-split-entry",
  "info": {
    "title": "Split",
    "version": "1.0.0",
    "summary": "What a listing says of this API",
    "description": "The long form, which a summary beside it stands in for"
  },
  "paths": {
    "/pets": {
      "$ref": "openapi-split-shared#/components/pathItems/Pets"
    }
  },
  "components": {
    "schemas": {
      "Any": {
        "oneOf": [
          {
            "$ref": "object"
          },
          {
            "$ref": "#/components/schemas/Dog"
          }
        ],
        "discriminator": {
          "propertyName": "kind",
          "mapping": {
            "sibling": "./object",
            "nested": "bare/01",
            "dog": "Dog"
          }
        }
      },
      "Dog": {
        "type": "object",
        "properties": {
          "name": {
            "$ref": "object"
          }
        }
      }
    }
  }
}

# A description reads as the resource at its own URI, the same document
# this instance serves over HTTP
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: resources/read
Mcp-Name: {{base}}/test/openapi-split-entry
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 40,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/test/openapi-split-entry",
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "hurl",
        "version": "1.0.0"
      }
    }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 40
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/test/openapi-split-entry"
jsonpath "$.result.contents[0].mimeType" == "application/openapi+json"
jsonpath "$.result.contents[0].text" == "{{entry_document}}"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
jsonpath "$.result.ttlMs" == 300000
jsonpath "$.result.cacheScope" == "public"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# And what it serves for the bundle of that description, with the other
# document and both schemas it reaches embedded into its own Components
# Object, each mapping going on naming what it named
GET {{base}}/test/openapi-split-entry?bundle
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.2>; rel="describedby"
[Captures]
entry_bundle: body
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/test/openapi-split-entry\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/test/openapi-split-entry",
  "info": {
    "title": "Split",
    "version": "1.0.0",
    "summary": "What a listing says of this API",
    "description": "The long form, which a summary beside it stands in for"
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
      "01": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "{{base}}/test/bare/01",
        "type": "string"
      },
      "Any": {
        "oneOf": [
          {
            "$ref": "{{base}}/test/object"
          },
          {
            "$ref": "#/components/schemas/Dog"
          }
        ],
        "discriminator": {
          "propertyName": "kind",
          "mapping": {
            "sibling": "{{base}}/test/object",
            "nested": "{{base}}/test/bare/01",
            "dog": "Dog"
          }
        }
      },
      "Dog": {
        "type": "object",
        "properties": {
          "name": {
            "$ref": "{{base}}/test/object"
          }
        }
      },
      "object": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "{{base}}/test/object",
        "title": "My Object",
        "description": "An Object Example",
        "examples": [
          {}
        ],
        "properties": {
          "INVALID_PROPERTY_1": {
            "type": "string"
          },
          "foo": {
            "type": "string"
          }
        }
      }
    }
  }
}

# The bundle of a description reads as a resource of its own, which is the
# query the resource template names
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: resources/read
Mcp-Name: {{base}}/test/openapi-split-entry?bundle
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 41,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/test/openapi-split-entry?bundle",
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "hurl",
        "version": "1.0.0"
      }
    }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 41
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/test/openapi-split-entry?bundle"
jsonpath "$.result.contents[0].mimeType" == "application/openapi+json"
jsonpath "$.result.contents[0].text" == "{{entry_bundle}}"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
jsonpath "$.result.ttlMs" == 300000
jsonpath "$.result.cacheScope" == "public"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# A description that reaches nothing bundles to what it already holds
GET {{base}}/test/openapi-petstore?bundle
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/common/openapi/3.0>; rel="describedby"
[Captures]
petstore_bundle: body
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
    "version": "1.0.0",
    "description": "Everything about the pets this shop keeps"
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

# And that one reads back the same way, so the query answers for every
# description rather than only one that spans documents
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2026-07-28
Mcp-Method: resources/read
Mcp-Name: {{base}}/test/openapi-petstore?bundle
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 42,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/test/openapi-petstore?bundle",
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "hurl",
        "version": "1.0.0"
      }
    }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2026-07-28
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Mcp-Session-Id" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 42
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/test/openapi-petstore?bundle"
jsonpath "$.result.contents[0].mimeType" == "application/openapi+json"
jsonpath "$.result.contents[0].text" == "{{petstore_bundle}}"
jsonpath "$.result.resultType" == "complete"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].name" == "sourcemeta-one-enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].title" == "Sourcemeta One Enterprise"
jsonpath "$.result._meta['io.modelcontextprotocol/serverInfo'].version" matches "^.+$"
jsonpath "$.result.ttlMs" == 300000
jsonpath "$.result.cacheScope" == "public"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
