# What this instance serves for the entry document of a description
# built out of two documents, where a Discriminator Object mapping that
# names a schema by URI carries the path segment that says so wherever a
# single name would otherwise read as the name of a component
GET {{base}}/api/split-entry
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
entry_document: body
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Link" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/api/split-entry\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/api/split-entry",
  "info": {
    "title": "Split",
    "version": "1.0.0"
  },
  "paths": {
    "/pets": {
      "$ref": "split-shared#/components/pathItems/Pets"
    }
  },
  "components": {
    "schemas": {
      "Any": {
        "oneOf": [
          {
            "$ref": "name"
          },
          {
            "$ref": "#/components/schemas/Dog"
          }
        ],
        "discriminator": {
          "propertyName": "kind",
          "mapping": {
            "sibling": "./name",
            "nested": "nested/name",
            "dog": "Dog"
          }
        }
      },
      "Dog": {
        "type": "object",
        "properties": {
          "name": {
            "$ref": "name"
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
MCP-Protocol-Version: 2025-11-25
Mcp-Method: resources/read
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 50,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/api/split-entry"
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
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
jsonpath "$.id" == 50
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/api/split-entry"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{entry_document}}"
jsonpath "$.result.resultType" not exists
jsonpath "$.result._meta" not exists
jsonpath "$.result.ttlMs" not exists
jsonpath "$.result.cacheScope" not exists

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
# Object, the second of them under a name of its own
GET {{base}}/api/split-entry?bundle
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
entry_bundle: body
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Link" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
body startsWith "{\n  \"openapi\": \"3.2.0\",\n  \"$self\": \"{{base}}/api/split-entry\",\n"
{
  "openapi": "3.2.0",
  "$self": "{{base}}/api/split-entry",
  "info": {
    "title": "Split",
    "version": "1.0.0"
  },
  "paths": {
    "/pets": {
      "$ref": "#/components/pathItems/Pets"
    }
  },
  "components": {
    "schemas": {
      "Any": {
        "oneOf": [
          {
            "$ref": "{{base}}/api/name"
          },
          {
            "$ref": "#/components/schemas/Dog"
          }
        ],
        "discriminator": {
          "propertyName": "kind",
          "mapping": {
            "sibling": "./name",
            "nested": "nested/name",
            "dog": "Dog"
          }
        }
      },
      "Dog": {
        "type": "object",
        "properties": {
          "name": {
            "$ref": "{{base}}/api/name"
          }
        }
      },
      "name": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "{{base}}/api/name",
        "title": "Name",
        "type": "string"
      },
      "name_": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "{{base}}/api/nested/name",
        "title": "Nested Name",
        "type": "string",
        "minLength": 1
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
    }
  }
}

# The bundle of a description reads as a resource of its own, which is the
# query the resource template names
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
MCP-Protocol-Version: 2025-11-25
Mcp-Method: resources/read
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 51,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/api/split-entry?bundle"
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
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
jsonpath "$.id" == 51
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/api/split-entry?bundle"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{entry_bundle}}"
jsonpath "$.result.resultType" not exists
jsonpath "$.result._meta" not exists
jsonpath "$.result.ttlMs" not exists
jsonpath "$.result.cacheScope" not exists

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
GET {{base}}/api/petstore?bundle
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/openapi+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
petstore_bundle: body
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
MCP-Protocol-Version: 2025-11-25
Mcp-Method: resources/read
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 52,
  "method": "resources/read",
  "params": {
    "uri": "{{base}}/api/petstore?bundle"
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/mcp/response>; rel="describedby"
MCP-Protocol-Version: 2025-11-25
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
jsonpath "$.id" == 52
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{base}}/api/petstore?bundle"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{petstore_bundle}}"
jsonpath "$.result.resultType" not exists
jsonpath "$.result._meta" not exists
jsonpath "$.result.ttlMs" not exists
jsonpath "$.result.cacheScope" not exists

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
