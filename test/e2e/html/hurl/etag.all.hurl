GET {{base}}/test/schemas/string.json
HTTP 200
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-07/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
test_schemas_string_json_etag: header "ETag"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
# Identity response: strong ETag form `"<hex>"` per RFC 9110 §8.8.1
header "ETag" matches /^"[0-9a-f]+"$/
header "Vary" == "Accept-Encoding"

# Gzip-encoded response: weak ETag form `W/"<hex>"` per RFC 9110 §8.8.1
# https://datatracker.ietf.org/doc/html/rfc9110#section-8.8.1
GET {{base}}/test/schemas/string.json
Accept-Encoding: gzip
HTTP 200
Content-Type: application/schema+json
Content-Encoding: gzip
Access-Control-Allow-Origin: *
[Captures]
test_schemas_string_json_etag_weak: header "ETag"
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" matches /^W\/"[0-9a-f]+"$/
header "Vary" == "Accept-Encoding"

# Weak-form ETag captured from a gzip-encoded response still matches when
# sent as If-None-Match on an identity request: the comparison strips the
# W/ prefix and the underlying SHA-256 is identical across encodings.
GET {{base}}/test/schemas/string.json
If-None-Match: {{test_schemas_string_json_etag_weak}}
HTTP 304
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Content-Type" not exists
bytes count == 0

# Reverse direction: strong-form ETag captured from an identity response
# still matches when sent as If-None-Match on a gzip-encoded request.
GET {{base}}/test/schemas/string.json
Accept-Encoding: gzip
If-None-Match: {{test_schemas_string_json_etag}}
HTTP 304
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Content-Type" not exists
header "Content-Encoding" not exists
bytes count == 0

GET {{base}}/test/schemas/string.json
HTTP 200
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" == {{test_schemas_string_json_etag}}
header "Vary" == "Accept-Encoding"

GET {{base}}/test/schemas/string.json
If-None-Match: {{test_schemas_string_json_etag}}
HTTP 304
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Content-Type" not exists
bytes count == 0

GET {{base}}/test/schemas/string.json
If-None-Match: W/{{test_schemas_string_json_etag}}
HTTP 304
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Content-Type" not exists
bytes count == 0

GET {{base}}/test/schemas/string.json
If-None-Match: "12345"
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count > 0
