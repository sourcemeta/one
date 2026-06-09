POST {{base}}/v1/catalog/self/v1/api/schemas/trace/example/string
Content-Type: application/json
"hello"
HTTP 200
Cache-Control: no-store
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/api/schemas/trace/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
jsonpath "$.steps" count == 2
jsonpath "$.steps[0].keywordLocation" == "{{base}}/v1/catalog/example/string#/type"
jsonpath "$.steps[0].name" == "AssertionTypeStrict"
jsonpath "$.steps[0].type" == "push"
jsonpath "$.steps[0].vocabulary" == "https://json-schema.org/draft/2020-12/vocab/validation"
jsonpath "$.steps[1].keywordLocation" == "{{base}}/v1/catalog/example/string#/type"
jsonpath "$.steps[1].type" == "pass"
jsonpath "$.steps[1].message" == "The value was expected to be of type string"

POST {{base}}/v1/catalog/self/v1/api/schemas/trace/example/nested/deep
Content-Type: application/json
3.14
HTTP 200
Cache-Control: no-store
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
jsonpath "$.steps[0].keywordLocation" == "{{base}}/v1/catalog/example/nested/deep#/type"

POST {{base}}/v1/catalog/self/v1/api/schemas/trace/rebased/with-id
Content-Type: application/json
true
HTTP 200
Cache-Control: no-store
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
jsonpath "$.steps[0].keywordLocation" == "{{base}}/v1/catalog/rebased/with-id#/type"

POST {{base}}/v1/catalog/self/v1/api/schemas/trace/example/with-ref
Content-Type: application/json
{"name": "hello"}
HTTP 200
Cache-Control: no-store
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

POST {{base}}/v1/catalog/self/v1/api/schemas/trace/example/string
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 400
jsonpath "$.type" == "urn:sourcemeta:one:no-instance"
jsonpath "$.title" == "Bad Request"
