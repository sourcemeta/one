# The outgoing edges of an OpenAPI description, which reach both the other
# documents of the description and the schemas of this catalog its Schema
# Objects name.

# An OpenAPI description built out of several documents reaches the other
# document of it, and a schema of this catalog besides, so one report carries
# both kinds of edge and says which kind each one reaches for. A document that
# several positions name is an edge for each of them, as where a reference
# sits is part of what this report is for. A Discriminator Object mapping that
# names a schema by URI is one of those positions
GET {{base}}/self/v1/api/dependencies/test/schemas/openapi-split-entry
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/dependencies/response>; rel="describedby"
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
jsonpath "$" count == 12
jsonpath "$[0].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[0].from.type" == "openapi"
jsonpath "$[0].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[0].from.location" == "/paths/~1dogs/get/callbacks/onData/$ref"
jsonpath "$[0].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[0].to.type" == "openapi"
jsonpath "$[0].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[1].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[1].from.type" == "openapi"
jsonpath "$[1].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[1].from.location" == "/paths/~1dogs/get/parameters/0/$ref"
jsonpath "$[1].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[1].to.type" == "openapi"
jsonpath "$[1].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[2].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[2].from.type" == "openapi"
jsonpath "$[2].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[2].from.location" == "/paths/~1dogs/get/requestBody/$ref"
jsonpath "$[2].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[2].to.type" == "openapi"
jsonpath "$[2].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[3].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[3].from.type" == "openapi"
jsonpath "$[3].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[3].from.location" == "/paths/~1dogs/get/responses/200/content/application~1json/$ref"
jsonpath "$[3].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[3].to.type" == "openapi"
jsonpath "$[3].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[4].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[4].from.type" == "openapi"
jsonpath "$[4].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[4].from.location" == "/paths/~1dogs/get/responses/200/headers/X-Limit/$ref"
jsonpath "$[4].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[4].to.type" == "openapi"
jsonpath "$[4].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[5].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[5].from.type" == "openapi"
jsonpath "$[5].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[5].from.location" == "/paths/~1dogs/get/responses/200/links/self/$ref"
jsonpath "$[5].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[5].to.type" == "openapi"
jsonpath "$[5].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[6].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[6].from.type" == "openapi"
jsonpath "$[6].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[6].from.location" == "/paths/~1dogs/get/responses/404/$ref"
jsonpath "$[6].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[6].to.type" == "openapi"
jsonpath "$[6].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[7].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[7].from.type" == "openapi"
jsonpath "$[7].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[7].from.location" == "/paths/~1pets/$ref"
jsonpath "$[7].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[7].to.type" == "openapi"
jsonpath "$[7].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[8].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[8].from.type" == "openapi"
jsonpath "$[8].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[8].from.location" == "/security/0/openapi-split-shared#~1components~1securitySchemes~1Key"
jsonpath "$[8].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[8].to.type" == "openapi"
jsonpath "$[8].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[9].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[9].from.type" == "openapi"
jsonpath "$[9].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[9].from.location" == "/components/schemas/Any/discriminator/mapping/name"
jsonpath "$[9].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[9].to.type" == "schema"
jsonpath "$[9].to.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[10].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[10].from.type" == "openapi"
jsonpath "$[10].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[10].from.location" == "/components/schemas/Any/oneOf/0/$ref"
jsonpath "$[10].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[10].to.type" == "schema"
jsonpath "$[10].to.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[11].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[11].from.type" == "openapi"
jsonpath "$[11].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[11].from.location" == "/components/schemas/Dog/properties/name/$ref"
jsonpath "$[11].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[11].to.type" == "schema"
jsonpath "$[11].to.baseDialect" == "http://json-schema.org/draft-07/schema#"

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

# The `.json` suffix names the same leaf here as it does everywhere else
GET {{base}}/self/v1/api/dependencies/test/schemas/openapi-split-entry.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/dependencies/response>; rel="describedby"
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
jsonpath "$" count == 12
jsonpath "$[0].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[0].from.type" == "openapi"
jsonpath "$[0].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[0].from.location" == "/paths/~1dogs/get/callbacks/onData/$ref"
jsonpath "$[0].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[0].to.type" == "openapi"
jsonpath "$[0].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[1].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[1].from.type" == "openapi"
jsonpath "$[1].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[1].from.location" == "/paths/~1dogs/get/parameters/0/$ref"
jsonpath "$[1].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[1].to.type" == "openapi"
jsonpath "$[1].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[2].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[2].from.type" == "openapi"
jsonpath "$[2].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[2].from.location" == "/paths/~1dogs/get/requestBody/$ref"
jsonpath "$[2].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[2].to.type" == "openapi"
jsonpath "$[2].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[3].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[3].from.type" == "openapi"
jsonpath "$[3].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[3].from.location" == "/paths/~1dogs/get/responses/200/content/application~1json/$ref"
jsonpath "$[3].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[3].to.type" == "openapi"
jsonpath "$[3].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[4].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[4].from.type" == "openapi"
jsonpath "$[4].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[4].from.location" == "/paths/~1dogs/get/responses/200/headers/X-Limit/$ref"
jsonpath "$[4].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[4].to.type" == "openapi"
jsonpath "$[4].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[5].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[5].from.type" == "openapi"
jsonpath "$[5].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[5].from.location" == "/paths/~1dogs/get/responses/200/links/self/$ref"
jsonpath "$[5].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[5].to.type" == "openapi"
jsonpath "$[5].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[6].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[6].from.type" == "openapi"
jsonpath "$[6].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[6].from.location" == "/paths/~1dogs/get/responses/404/$ref"
jsonpath "$[6].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[6].to.type" == "openapi"
jsonpath "$[6].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[7].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[7].from.type" == "openapi"
jsonpath "$[7].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[7].from.location" == "/paths/~1pets/$ref"
jsonpath "$[7].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[7].to.type" == "openapi"
jsonpath "$[7].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[8].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[8].from.type" == "openapi"
jsonpath "$[8].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[8].from.location" == "/security/0/openapi-split-shared#~1components~1securitySchemes~1Key"
jsonpath "$[8].to.identifier" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[8].to.type" == "openapi"
jsonpath "$[8].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[9].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[9].from.type" == "openapi"
jsonpath "$[9].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[9].from.location" == "/components/schemas/Any/discriminator/mapping/name"
jsonpath "$[9].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[9].to.type" == "schema"
jsonpath "$[9].to.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[10].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[10].from.type" == "openapi"
jsonpath "$[10].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[10].from.location" == "/components/schemas/Any/oneOf/0/$ref"
jsonpath "$[10].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[10].to.type" == "schema"
jsonpath "$[10].to.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[11].from.identifier" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[11].from.type" == "openapi"
jsonpath "$[11].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[11].from.location" == "/components/schemas/Dog/properties/name/$ref"
jsonpath "$[11].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[11].to.type" == "schema"
jsonpath "$[11].to.baseDialect" == "http://json-schema.org/draft-07/schema#"

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

# A Discriminator Object mapping that names a schema by URI reaches it, so the
# position of the mapping is an edge of its own beside the reference that names
# the same schema. A mapping that names a component reaches nothing beyond the
# document spelling it, so it is no edge at all
GET {{base}}/self/v1/api/dependencies/test/schemas/openapi-discriminator
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/dependencies/response>; rel="describedby"
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
jsonpath "$" count == 2
jsonpath "$[0].from.identifier" == "{{base}}/test/schemas/openapi-discriminator"
jsonpath "$[0].from.type" == "openapi"
jsonpath "$[0].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[0].from.location" == "/components/schemas/Any/discriminator/mapping/text"
jsonpath "$[0].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[0].to.type" == "schema"
jsonpath "$[0].to.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[1].from.identifier" == "{{base}}/test/schemas/openapi-discriminator"
jsonpath "$[1].from.type" == "openapi"
jsonpath "$[1].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[1].from.location" == "/components/schemas/Any/oneOf/0/$ref"
jsonpath "$[1].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[1].to.type" == "schema"
jsonpath "$[1].to.baseDialect" == "http://json-schema.org/draft-07/schema#"

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

# The document reached by another reaches nothing itself, which is an empty
# report rather than an absent one
GET {{base}}/self/v1/api/dependencies/test/schemas/openapi-split-shared
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/dependencies/response>; rel="describedby"
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
jsonpath "$" count == 0
jsonpath "$" isEmpty

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

# And so does an OpenAPI description that keeps every connection it makes
# inside itself
GET {{base}}/self/v1/api/dependencies/test/schemas/openapi-3-0
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/dependencies/response>; rel="describedby"
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
jsonpath "$" count == 0
jsonpath "$" isEmpty

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

# A conditional request asks for the headers alone
HEAD {{base}}/self/v1/api/dependencies/test/schemas/openapi-split-entry
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/dependencies/response>; rel="describedby"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
bytes count == 0

# One endpoint answers for either kind of resource, so the edges of a
# schema come from the very same place
GET {{base}}/self/v1/api/dependencies/test/schemas/string
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/dependencies/response>; rel="describedby"
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
jsonpath "$" count == 0

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

# Nor about a path nothing here answers to
GET {{base}}/self/v1/api/dependencies/test/schemas/nowhere
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
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.title" == "Not Found"
jsonpath "$.status" == 404
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Nor about the endpoint named without a leaf
GET {{base}}/self/v1/api/dependencies
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
jsonpath "$.type" == "urn:sourcemeta:one:not-found"
jsonpath "$.title" == "Not Found"
jsonpath "$.status" == 404
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# An OpenAPI description is named by its URI, and a fragment names a place
# within one rather than the document itself
GET {{base}}/self/v1/api/dependencies/test/schemas/openapi-split-entry%23/paths
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
jsonpath "$.type" == "urn:sourcemeta:one:invalid-schema-uri"
jsonpath "$.title" == "Bad Request"
jsonpath "$.status" == 400
jsonpath "$.detail" == "The URI must not contain a fragment"

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

# This surface only reads
POST {{base}}/self/v1/api/dependencies/test/schemas/openapi-split-entry
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Allow: GET, HEAD, OPTIONS
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
jsonpath "$.type" == "urn:sourcemeta:one:method-not-allowed"
jsonpath "$.title" == "Method Not Allowed"
jsonpath "$.status" == 405
jsonpath "$.detail" == "This HTTP method is invalid for this URL"

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

OPTIONS {{base}}/self/v1/api/dependencies/test/schemas/openapi-split-entry
HTTP 204
Cache-Control: no-store
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Access-Control-Allow-Methods: GET, HEAD, OPTIONS
Access-Control-Allow-Headers: Accept-Encoding, If-None-Match, If-Modified-Since
Access-Control-Max-Age: 3600
Allow: GET, HEAD, OPTIONS
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

# What a description reaches carries on through the schemas it reaches, so a
# description that names a Schema Object of another description and a schema of
# this catalog reports the edges of both of those as well as its own
GET {{base}}/self/v1/api/dependencies/test/schemas/openapi-chain
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/dependencies/response>; rel="describedby"
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
jsonpath "$" count == 8
jsonpath "$[0].from.identifier" == "{{base}}/test/schemas/openapi-chain"
jsonpath "$[0].from.type" == "openapi"
jsonpath "$[0].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[0].from.location" == "/components/schemas/Deep/$ref"
jsonpath "$[0].to.identifier" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[0].to.type" == "schema"
jsonpath "$[0].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[1].from.identifier" == "{{base}}/test/schemas/openapi-chain"
jsonpath "$[1].from.type" == "openapi"
jsonpath "$[1].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[1].from.location" == "/components/schemas/Name/$ref"
jsonpath "$[1].to.identifier" == "{{base}}/test/schemas/openapi-two-positions"
jsonpath "$[1].to.type" == "openapi"
jsonpath "$[1].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[2].from.identifier" == "{{base}}/test/schemas/openapi-two-positions"
jsonpath "$[2].from.type" == "openapi"
jsonpath "$[2].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[2].from.location" == "/components/schemas/Age/$ref"
jsonpath "$[2].to.identifier" == "{{base}}/test/schemas/named-places"
jsonpath "$[2].to.type" == "schema"
jsonpath "$[2].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[3].from.identifier" == "{{base}}/test/schemas/openapi-two-positions"
jsonpath "$[3].from.type" == "openapi"
jsonpath "$[3].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[3].from.location" == "/components/schemas/Name/$ref"
jsonpath "$[3].to.identifier" == "{{base}}/test/schemas/named-places"
jsonpath "$[3].to.type" == "schema"
jsonpath "$[3].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[4].from.identifier" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[4].from.type" == "schema"
jsonpath "$[4].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[4].from.location" == "/properties/one/$ref"
jsonpath "$[4].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[4].to.type" == "schema"
jsonpath "$[4].to.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[5].from.identifier" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[5].from.type" == "schema"
jsonpath "$[5].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[5].from.location" == "/properties/three/$ref"
jsonpath "$[5].to.identifier" == "{{base}}/test/schemas/absolute-refs"
jsonpath "$[5].to.type" == "schema"
jsonpath "$[5].to.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[6].from.identifier" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[6].from.type" == "schema"
jsonpath "$[6].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[6].from.location" == "/properties/two/$ref"
jsonpath "$[6].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[6].to.type" == "schema"
jsonpath "$[6].to.baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[7].from.identifier" == "{{base}}/test/schemas/absolute-refs"
jsonpath "$[7].from.type" == "schema"
jsonpath "$[7].from.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[7].from.location" == "/$ref"
jsonpath "$[7].to.identifier" == "{{base}}/test/schemas/string"
jsonpath "$[7].to.type" == "schema"
jsonpath "$[7].to.baseDialect" == "http://json-schema.org/draft-07/schema#"

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
