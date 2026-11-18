GET {{base}}/self/v1/api/schemas/dependents/test/schemas/bundling-single
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
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
jsonpath "$" count == 3
jsonpath "$[0].from" == "{{base}}/test/schemas/bundling-double"
jsonpath "$[0].to" == "{{base}}/test/schemas/bundling-single"
jsonpath "$[0].at" == "/properties/foo/$ref"
jsonpath "$[0].kind" == "schema"
jsonpath "$[0].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[1].from" == "{{base}}/test/schemas/with-rebase"
jsonpath "$[1].to" == "{{base}}/test/schemas/bundling-single"
jsonpath "$[1].at" == "/$ref"
jsonpath "$[1].kind" == "schema"
jsonpath "$[1].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[2].from" == "{{base}}/test/schemas/with-rebase-same-host"
jsonpath "$[2].to" == "{{base}}/test/schemas/bundling-single"
jsonpath "$[2].at" == "/$ref"
jsonpath "$[2].kind" == "schema"
jsonpath "$[2].baseDialect" == "https://json-schema.org/draft/2020-12/schema"

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

GET {{base}}/self/v1/api/schemas/dependents/test/schemas/no-id
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
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
jsonpath "$" count == 4
jsonpath "$[0].from" == "{{base}}/test/schemas/bundling-double"
jsonpath "$[0].to" == "{{base}}/test/schemas/bundling-single"
jsonpath "$[0].at" == "/properties/foo/$ref"
jsonpath "$[0].kind" == "schema"
jsonpath "$[0].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[1].from" == "{{base}}/test/schemas/bundling-single"
jsonpath "$[1].to" == "{{base}}/test/schemas/no-id"
jsonpath "$[1].at" == "/properties/foo/$ref"
jsonpath "$[1].kind" == "schema"
jsonpath "$[1].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$[2].from" == "{{base}}/test/schemas/with-rebase"
jsonpath "$[2].to" == "{{base}}/test/schemas/bundling-single"
jsonpath "$[2].at" == "/$ref"
jsonpath "$[2].kind" == "schema"
jsonpath "$[2].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[3].from" == "{{base}}/test/schemas/with-rebase-same-host"
jsonpath "$[3].to" == "{{base}}/test/schemas/bundling-single"
jsonpath "$[3].at" == "/$ref"
jsonpath "$[3].kind" == "schema"
jsonpath "$[3].baseDialect" == "https://json-schema.org/draft/2020-12/schema"

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

# The same holds of the incoming edges, where one schema named from several
# positions of another is one entry for each of them. An OpenAPI description
# that names a schema embeds it when bundled, so it is a dependent of that
# schema as much as another schema is, and each entry says which of the two
# kinds reached it. A description that reaches this schema through another
# schema is one as well, which is the entry that names that other schema
GET {{base}}/self/v1/api/schemas/dependents/test/schemas/string
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
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
jsonpath "$" count == 10
jsonpath "$[0].from" == "{{base}}/test/schemas/absolute-refs"
jsonpath "$[0].to" == "{{base}}/test/schemas/string"
jsonpath "$[0].at" == "/$ref"
jsonpath "$[0].kind" == "schema"
jsonpath "$[0].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[1].from" == "{{base}}/test/schemas/openapi-chain"
jsonpath "$[1].to" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[1].at" == "/components/schemas/Deep/$ref"
jsonpath "$[1].kind" == "openapi"
jsonpath "$[1].baseDialect" not exists
jsonpath "$[2].from" == "{{base}}/test/schemas/openapi-discriminator"
jsonpath "$[2].to" == "{{base}}/test/schemas/string"
jsonpath "$[2].at" == "/components/schemas/Any/discriminator/mapping/text"
jsonpath "$[2].kind" == "openapi"
jsonpath "$[2].baseDialect" not exists
jsonpath "$[3].from" == "{{base}}/test/schemas/openapi-discriminator"
jsonpath "$[3].to" == "{{base}}/test/schemas/string"
jsonpath "$[3].at" == "/components/schemas/Any/oneOf/0/$ref"
jsonpath "$[3].kind" == "openapi"
jsonpath "$[3].baseDialect" not exists
jsonpath "$[4].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[4].to" == "{{base}}/test/schemas/string"
jsonpath "$[4].at" == "/components/schemas/Any/discriminator/mapping/name"
jsonpath "$[4].kind" == "openapi"
jsonpath "$[4].baseDialect" not exists
jsonpath "$[5].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[5].to" == "{{base}}/test/schemas/string"
jsonpath "$[5].at" == "/components/schemas/Any/oneOf/0/$ref"
jsonpath "$[5].kind" == "openapi"
jsonpath "$[5].baseDialect" not exists
jsonpath "$[6].from" == "{{base}}/test/schemas/openapi-split-entry"
jsonpath "$[6].to" == "{{base}}/test/schemas/string"
jsonpath "$[6].at" == "/components/schemas/Dog/properties/name/$ref"
jsonpath "$[6].kind" == "openapi"
jsonpath "$[6].baseDialect" not exists
jsonpath "$[7].from" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[7].to" == "{{base}}/test/schemas/absolute-refs"
jsonpath "$[7].at" == "/properties/three/$ref"
jsonpath "$[7].kind" == "schema"
jsonpath "$[7].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[8].from" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[8].to" == "{{base}}/test/schemas/string"
jsonpath "$[8].at" == "/properties/one/$ref"
jsonpath "$[8].kind" == "schema"
jsonpath "$[8].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$[9].from" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[9].to" == "{{base}}/test/schemas/string"
jsonpath "$[9].at" == "/properties/two/$ref"
jsonpath "$[9].kind" == "schema"
jsonpath "$[9].baseDialect" == "https://json-schema.org/draft/2020-12/schema"

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

# A schema that only an OpenAPI description reaches has that description for
# its dependent, and such an entry carries no dialect, as a description
# declares none of its own
GET {{base}}/self/v1/api/schemas/dependents/test/schemas/repeated-refs
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
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
jsonpath "$" count == 1
jsonpath "$[0].from" == "{{base}}/test/schemas/openapi-chain"
jsonpath "$[0].to" == "{{base}}/test/schemas/repeated-refs"
jsonpath "$[0].at" == "/components/schemas/Deep/$ref"
jsonpath "$[0].kind" == "openapi"
jsonpath "$[0].baseDialect" not exists

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

# A description that reaches this schema through another description is a
# dependent of it too, and the entry that names it says where in that other
# description the reference sits
GET {{base}}/self/v1/api/schemas/dependents/test/schemas/named-places
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
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
jsonpath "$" count == 3
jsonpath "$[0].from" == "{{base}}/test/schemas/openapi-chain"
jsonpath "$[0].to" == "{{base}}/test/schemas/openapi-two-positions"
jsonpath "$[0].at" == "/components/schemas/Name/$ref"
jsonpath "$[0].kind" == "openapi"
jsonpath "$[0].baseDialect" not exists
jsonpath "$[1].from" == "{{base}}/test/schemas/openapi-two-positions"
jsonpath "$[1].to" == "{{base}}/test/schemas/named-places"
jsonpath "$[1].at" == "/components/schemas/Age/$ref"
jsonpath "$[1].kind" == "openapi"
jsonpath "$[1].baseDialect" not exists
jsonpath "$[2].from" == "{{base}}/test/schemas/openapi-two-positions"
jsonpath "$[2].to" == "{{base}}/test/schemas/named-places"
jsonpath "$[2].at" == "/components/schemas/Name/$ref"
jsonpath "$[2].kind" == "openapi"
jsonpath "$[2].baseDialect" not exists

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

GET {{base}}/self/v1/api/schemas/dependents/test/schemas/bundling-double
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
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

GET {{base}}/self/v1/api/schemas/dependents/test/schemas/recursive
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
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

HEAD {{base}}/self/v1/api/schemas/dependents/test/schemas/bundling-double
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
bytes count == 0

POST {{base}}/self/v1/api/schemas/dependents/test/schemas/bundling-double
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
jsonpath "$.status" == 405
jsonpath "$.type" == "urn:sourcemeta:one:method-not-allowed"
jsonpath "$.title" == "Method Not Allowed"
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

GET {{base}}/self/v1/api/schemas/dependents/xyz/
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/dependents
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
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
