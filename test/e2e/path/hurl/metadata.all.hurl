GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/example/string
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 143
jsonpath "$.bytesBundled" == 143
jsonpath "$.identifier" == "{{base}}/v1/catalog/example/string"
jsonpath "$.path" == "/v1/catalog/example/string"
jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.examples" count == 0
jsonpath "$.health" == 0
jsonpath "$.priority" == 50
jsonpath "$.dependencies" == 0
jsonpath "$.alert" == null
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "example"
jsonpath "$.breadcrumb[0].path" == "/v1/catalog/example/"
jsonpath "$.breadcrumb[1].name" == "string"
jsonpath "$.breadcrumb[1].path" == "/v1/catalog/example/string"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/metadata/response
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/rebased/with-id
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.bytes" == 145
jsonpath "$.bytesBundled" == 145
jsonpath "$.identifier" == "{{base}}/v1/catalog/rebased/with-id"
jsonpath "$.path" == "/v1/catalog/rebased/with-id"
jsonpath "$.baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.examples" count == 0
jsonpath "$.health" == 0
jsonpath "$.priority" == 50
jsonpath "$.dependencies" == 0
jsonpath "$.alert" == null
jsonpath "$.breadcrumb" count == 2
jsonpath "$.breadcrumb[0].name" == "rebased"
jsonpath "$.breadcrumb[0].path" == "/v1/catalog/rebased/"
jsonpath "$.breadcrumb[1].name" == "with-id"
jsonpath "$.breadcrumb[1].path" == "/v1/catalog/rebased/with-id"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/metadata/response
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# The metadata endpoint returns the schema's title verbatim with no
# size cap, even when the source title is large. Truncation only applies
# at the binary-extension boundary used by directory listings and search

GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/example/zzz-truncate-target
Accept: application/json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "Vary" == "Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.identifier" == "{{base}}/v1/catalog/example/zzz-truncate-target"
jsonpath "$.path" == "/v1/catalog/example/zzz-truncate-target"
jsonpath "$.title" matches "^Z{70000}$"
jsonpath "$.priority" == 50
jsonpath "$.health" == 0
jsonpath "$.dependencies" == 0
jsonpath "$.alert" == null

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/metadata/response
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
