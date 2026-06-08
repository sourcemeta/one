GET {{base}}/test/schemas/string.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <http://json-schema.org/draft-07/schema\#>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
test_schemas_string_json_etag: header "ETag"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
# Identity response: strong ETag form `"<hex>"` per RFC 9110 §8.8.1
header "ETag" matches /^"[0-9a-f]+"$/

# Gzip-encoded response: weak ETag form `W/"<hex>"` per RFC 9110 §8.8.1
# https://datatracker.ietf.org/doc/html/rfc9110#section-8.8.1
GET {{base}}/test/schemas/string.json
Accept-Encoding: gzip
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Content-Encoding: gzip
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
test_schemas_string_json_etag_weak: header "ETag"
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" matches /^W\/"[0-9a-f]+"$/

# Weak-form ETag captured from a gzip-encoded response still matches when
# sent as If-None-Match on an identity request: the comparison strips the
# W/ prefix and the underlying SHA-256 is identical across encodings.
GET {{base}}/test/schemas/string.json
If-None-Match: {{test_schemas_string_json_etag_weak}}
HTTP 304
Cache-Control: public, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
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
Cache-Control: public, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Content-Type" not exists
header "Content-Encoding" not exists
bytes count == 0

GET {{base}}/test/schemas/string.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" == {{test_schemas_string_json_etag}}

GET {{base}}/test/schemas/string.json
If-None-Match: {{test_schemas_string_json_etag}}
HTTP 304
Cache-Control: public, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Content-Type" not exists
bytes count == 0

GET {{base}}/test/schemas/string.json
If-None-Match: W/{{test_schemas_string_json_etag}}
HTTP 304
Cache-Control: public, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Content-Type" not exists
bytes count == 0

GET {{base}}/test/schemas/string.json
If-None-Match: "12345"
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count > 0

# RFC 9110 §13.1.3 / §13.2.2: If-None-Match takes precedence over
# If-Modified-Since. When both headers are present the recipient MUST
# ignore If-Modified-Since and evaluate If-None-Match alone, regardless
# of the match outcome.
# https://datatracker.ietf.org/doc/html/rfc9110#section-13.1.3
# https://datatracker.ietf.org/doc/html/rfc9110#section-13.2.2

# (1) IMS in the past (would normally return 200) + matching INM:
# INM wins, response is 304 by ETag match.
GET {{base}}/test/schemas/string.json
If-Modified-Since: Thu, 01 Jan 1970 00:00:00 GMT
If-None-Match: {{test_schemas_string_json_etag}}
HTTP 304
Cache-Control: public, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Content-Type" not exists
bytes count == 0

# (2) IMS in the future (would normally return 304) + non-matching INM:
# INM wins, response is 200 because INM did not match. The IMS that
# would have produced 304 is ignored per the MUST in §13.1.3. This is
# the key regression the precondition-order fix prevents: previously
# the server short-circuited on IMS before INM was even consulted.
GET {{base}}/test/schemas/string.json
If-Modified-Since: Thu, 01 Jan 2100 00:00:00 GMT
If-None-Match: "definitely-not-a-real-checksum"
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" matches /^"[0-9a-f]+"$/
bytes count > 0

# (3) Sanity baseline: IMS in the future + no INM evaluates IMS
# normally and returns 304.
GET {{base}}/test/schemas/string.json
If-Modified-Since: Thu, 01 Jan 2100 00:00:00 GMT
HTTP 304
Cache-Control: public, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Content-Type" not exists
bytes count == 0
