# The outgoing edges of an OpenAPI description, which reach both the other
# documents of the description and the schemas of this catalog its Schema
# Objects name.

# An OpenAPI description built out of several documents reaches the other
# document of it, and a schema of this catalog besides, so one report carries
# both kinds of edge and says which kind each one reaches for. A document that
# several positions name is an edge for each of them, as where a reference
# sits is part of what this report is for. A Discriminator Object mapping that
# names a schema by URI is one of those positions
GET {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-split-entry
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/openapi/dependencies/response>; rel="describedby"
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
jsonpath "$[0].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[0].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[0].at" == "/paths/~1dogs/get/callbacks/onData/$ref"
jsonpath "$[0].kind" == "callback"
jsonpath "$[0].baseDialect" not exists
jsonpath "$[1].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[1].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[1].at" == "/paths/~1dogs/get/parameters/0/$ref"
jsonpath "$[1].kind" == "parameter"
jsonpath "$[1].baseDialect" not exists
jsonpath "$[2].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[2].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[2].at" == "/paths/~1dogs/get/requestBody/$ref"
jsonpath "$[2].kind" == "request-body"
jsonpath "$[2].baseDialect" not exists
jsonpath "$[3].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[3].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[3].at" == "/paths/~1dogs/get/responses/200/content/application~1json/$ref"
jsonpath "$[3].kind" == "media-type"
jsonpath "$[3].baseDialect" not exists
jsonpath "$[4].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[4].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[4].at" == "/paths/~1dogs/get/responses/200/headers/X-Limit/$ref"
jsonpath "$[4].kind" == "header"
jsonpath "$[4].baseDialect" not exists
jsonpath "$[5].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[5].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[5].at" == "/paths/~1dogs/get/responses/200/links/self/$ref"
jsonpath "$[5].kind" == "link"
jsonpath "$[5].baseDialect" not exists
jsonpath "$[6].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[6].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[6].at" == "/paths/~1dogs/get/responses/404/$ref"
jsonpath "$[6].kind" == "response"
jsonpath "$[6].baseDialect" not exists
jsonpath "$[7].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[7].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[7].at" == "/paths/~1pets/$ref"
jsonpath "$[7].kind" == "path-item"
jsonpath "$[7].baseDialect" not exists
jsonpath "$[8].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[8].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[8].at" == "/security/0/openapi-split-shared#~1components~1securitySchemes~1Key"
jsonpath "$[8].kind" == "security-scheme"
jsonpath "$[8].baseDialect" not exists
jsonpath "$[9].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[9].to" == "{{base}}/test/schemas/string"
jsonpath "$[9].at" == "/components/schemas/Any/discriminator/mapping/name"
jsonpath "$[9].kind" == "schema"
jsonpath "$[9].baseDialect" not exists
jsonpath "$[10].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[10].to" == "{{base}}/test/schemas/string"
jsonpath "$[10].at" == "/components/schemas/Any/oneOf/0/$ref"
jsonpath "$[10].kind" == "schema"
jsonpath "$[10].baseDialect" not exists
jsonpath "$[11].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[11].to" == "{{base}}/test/schemas/string"
jsonpath "$[11].at" == "/components/schemas/Dog/properties/name/$ref"
jsonpath "$[11].kind" == "schema"
jsonpath "$[11].baseDialect" not exists

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
GET {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-split-entry.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/openapi/dependencies/response>; rel="describedby"
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
jsonpath "$[0].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[0].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[0].at" == "/paths/~1dogs/get/callbacks/onData/$ref"
jsonpath "$[0].kind" == "callback"
jsonpath "$[0].baseDialect" not exists
jsonpath "$[1].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[1].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[1].at" == "/paths/~1dogs/get/parameters/0/$ref"
jsonpath "$[1].kind" == "parameter"
jsonpath "$[1].baseDialect" not exists
jsonpath "$[2].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[2].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[2].at" == "/paths/~1dogs/get/requestBody/$ref"
jsonpath "$[2].kind" == "request-body"
jsonpath "$[2].baseDialect" not exists
jsonpath "$[3].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[3].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[3].at" == "/paths/~1dogs/get/responses/200/content/application~1json/$ref"
jsonpath "$[3].kind" == "media-type"
jsonpath "$[3].baseDialect" not exists
jsonpath "$[4].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[4].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[4].at" == "/paths/~1dogs/get/responses/200/headers/X-Limit/$ref"
jsonpath "$[4].kind" == "header"
jsonpath "$[4].baseDialect" not exists
jsonpath "$[5].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[5].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[5].at" == "/paths/~1dogs/get/responses/200/links/self/$ref"
jsonpath "$[5].kind" == "link"
jsonpath "$[5].baseDialect" not exists
jsonpath "$[6].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[6].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[6].at" == "/paths/~1dogs/get/responses/404/$ref"
jsonpath "$[6].kind" == "response"
jsonpath "$[6].baseDialect" not exists
jsonpath "$[7].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[7].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[7].at" == "/paths/~1pets/$ref"
jsonpath "$[7].kind" == "path-item"
jsonpath "$[7].baseDialect" not exists
jsonpath "$[8].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[8].to" == "{{base}}/test/schemas/openapi-split-shared"
jsonpath "$[8].at" == "/security/0/openapi-split-shared#~1components~1securitySchemes~1Key"
jsonpath "$[8].kind" == "security-scheme"
jsonpath "$[8].baseDialect" not exists
jsonpath "$[9].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[9].to" == "{{base}}/test/schemas/string"
jsonpath "$[9].at" == "/components/schemas/Any/discriminator/mapping/name"
jsonpath "$[9].kind" == "schema"
jsonpath "$[9].baseDialect" not exists
jsonpath "$[10].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[10].to" == "{{base}}/test/schemas/string"
jsonpath "$[10].at" == "/components/schemas/Any/oneOf/0/$ref"
jsonpath "$[10].kind" == "schema"
jsonpath "$[10].baseDialect" not exists
jsonpath "$[11].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[11].to" == "{{base}}/test/schemas/string"
jsonpath "$[11].at" == "/components/schemas/Dog/properties/name/$ref"
jsonpath "$[11].kind" == "schema"
jsonpath "$[11].baseDialect" not exists


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
GET {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-discriminator
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/openapi/dependencies/response>; rel="describedby"
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
jsonpath "$[0].from" == "{{base}}/test/schemas/openapi-discriminator"
jsonpath "$[0].to" == "{{base}}/test/schemas/string"
jsonpath "$[0].at" == "/components/schemas/Any/discriminator/mapping/text"
jsonpath "$[0].kind" == "schema"
jsonpath "$[0].baseDialect" not exists
jsonpath "$[1].from" == "{{base}}/test/schemas/openapi-discriminator"
jsonpath "$[1].to" == "{{base}}/test/schemas/string"
jsonpath "$[1].at" == "/components/schemas/Any/oneOf/0/$ref"
jsonpath "$[1].kind" == "schema"
jsonpath "$[1].baseDialect" not exists

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
GET {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-split-shared
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/openapi/dependencies/response>; rel="describedby"
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
GET {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-3-0
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/openapi/dependencies/response>; rel="describedby"
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
HEAD {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-split-entry
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/openapi/dependencies/response>; rel="describedby"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
bytes count == 0

# A schema records its edges under the same name, and what it records is
# shaped for the schema surface, so this one holds nothing about it
GET {{base}}/self/v1/api/openapi/dependencies/test/schemas/string
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

# Nor about a path nothing here answers to
GET {{base}}/self/v1/api/openapi/dependencies/test/schemas/nowhere
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
GET {{base}}/self/v1/api/openapi/dependencies
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
GET {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-split-entry%23/paths
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
POST {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-split-entry
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

OPTIONS {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-split-entry
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
GET {{base}}/self/v1/api/openapi/dependencies/test/schemas/openapi-chain
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/openapi/dependencies/response>; rel="describedby"
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
jsonpath "$[0].from" == "{{base}}/test/schemas/openapi-chain"
jsonpath "$[0].to" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[0].at" == "/components/schemas/Deep/$ref"
jsonpath "$[0].kind" == "schema"
jsonpath "$[0].baseDialect" not exists
jsonpath "$[1].from" == "{{base}}/test/schemas/openapi-chain"
jsonpath "$[1].to" == "{{base}}/test/schemas/openapi-two-positions"
jsonpath "$[1].at" == "/components/schemas/Name/$ref"
jsonpath "$[1].kind" == "schema"
jsonpath "$[1].baseDialect" not exists
jsonpath "$[2].from" == "{{base}}/test/schemas/openapi-two-positions"
jsonpath "$[2].to" == "{{base}}/test/schemas/named-places"
jsonpath "$[2].at" == "/components/schemas/Age/$ref"
jsonpath "$[2].kind" == "schema"
jsonpath "$[2].baseDialect" not exists
jsonpath "$[3].from" == "{{base}}/test/schemas/openapi-two-positions"
jsonpath "$[3].to" == "{{base}}/test/schemas/named-places"
jsonpath "$[3].at" == "/components/schemas/Name/$ref"
jsonpath "$[3].kind" == "schema"
jsonpath "$[3].baseDialect" not exists
jsonpath "$[4].from" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[4].to" == "{{base}}/test/schemas/string"
jsonpath "$[4].at" == "/properties/one/$ref"
jsonpath "$[4].kind" == "schema"
jsonpath "$[4].baseDialect" not exists
jsonpath "$[5].from" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[5].to" == "{{base}}/test/schemas/absolute-refs"
jsonpath "$[5].at" == "/properties/three/$ref"
jsonpath "$[5].kind" == "schema"
jsonpath "$[5].baseDialect" not exists
jsonpath "$[6].from" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[6].to" == "{{base}}/test/schemas/string"
jsonpath "$[6].at" == "/properties/two/$ref"
jsonpath "$[6].kind" == "schema"
jsonpath "$[6].baseDialect" not exists
jsonpath "$[7].from" == "{{base}}/test/schemas/absolute-refs"
jsonpath "$[7].to" == "{{base}}/test/schemas/string"
jsonpath "$[7].at" == "/$ref"
jsonpath "$[7].kind" == "schema"
jsonpath "$[7].baseDialect" not exists

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
