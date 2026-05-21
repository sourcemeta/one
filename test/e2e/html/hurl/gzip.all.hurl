GET {{base}}/test/schemas/string.json
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
bytes count > 0

GET {{base}}/test/schemas/string.json
Accept-Encoding: gzip
HTTP 200
Content-Type: application/schema+json
Content-Encoding: gzip
Access-Control-Allow-Origin: *
[Asserts]
bytes count > 0

GET {{base}}/test/schemas/string.json
Accept-Encoding: gzip;q=0
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
header "Content-Encoding" not exists
bytes count > 0

GET {{base}}/test/schemas/string.json
Accept-Encoding: gzip;q=0, identity
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
header "Content-Encoding" not exists
bytes count > 0

GET {{base}}/test/schemas/string.json
Accept-Encoding: identity;q=0
HTTP 406
Content-Type: application/problem+json
[Asserts]
jsonpath "$.status" == 406
jsonpath "$.title" == "sourcemeta:one/cannot-satisfy-content-encoding"

GET {{base}}/test/schemas/string.json
Accept-Encoding: *;q=0
HTTP 406
Content-Type: application/problem+json
[Asserts]
jsonpath "$.status" == 406
jsonpath "$.title" == "sourcemeta:one/cannot-satisfy-content-encoding"

GET {{base}}/test/schemas/string.json
Accept-Encoding: gzip;
HTTP 200
Content-Type: application/schema+json
Content-Encoding: gzip
Access-Control-Allow-Origin: *
[Asserts]
bytes count > 0

GET {{base}}/test/schemas/string.json
Accept-Encoding: gzip;q
HTTP 200
Content-Type: application/schema+json
Content-Encoding: gzip
Access-Control-Allow-Origin: *
[Asserts]
bytes count > 0

GET {{base}}/test/schemas/string.json
Accept-Encoding: gzip;q=
HTTP 200
Content-Type: application/schema+json
Content-Encoding: gzip
Access-Control-Allow-Origin: *
[Asserts]
bytes count > 0
