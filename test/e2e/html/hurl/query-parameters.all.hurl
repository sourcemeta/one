# A surface that declares which query parameters it reads refuses a name it
# does not know rather than ignoring it. The endpoints under
# `/self/v1/api` declare theirs, so a caller that mistypes one is told
# instead of being handed the answer to a different question.

# The search endpoint reads `q`, `limit` and `scope`, and all three together
# are honoured
GET {{base}}/self/v1/api/schemas/search?q=test&limit=2&scope=path,title
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count <= 2

# A mistyped `limit` is refused rather than silently answered with the default
GET {{base}}/self/v1/api/schemas/search?q=test&limti=2
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"
jsonpath "$.title" == "Bad Request"
jsonpath "$.detail" == "This endpoint does not recognise the limti query parameter"

# The refusal is itself an error document this instance describes
POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A parameter name is matched exactly, so a differently cased one is not the
# one this endpoint reads
GET {{base}}/self/v1/api/schemas/search?q=test&LIMIT=2
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"
jsonpath "$.detail" == "This endpoint does not recognise the LIMIT query parameter"

# An invalid value for a parameter this endpoint does read is still reported
# as that parameter being wrong rather than unknown
GET {{base}}/self/v1/api/schemas/search?q=test&scope=bogus
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:invalid-search-scope"

# Every other endpoint under `/self/v1/api` reads nothing out of the query
# string, so any name at all is refused
GET {{base}}/self/v1/api/list?bogus=1
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"
jsonpath "$.detail" == "This endpoint does not recognise the bogus query parameter"

GET {{base}}/self/v1/api/list/test?bogus=1
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

GET {{base}}/self/v1/api/schemas/metadata/test/schemas/string?bundle=1
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"
jsonpath "$.detail" == "This endpoint does not recognise the bundle query parameter"

GET {{base}}/self/v1/api/schemas/health/test/schemas/string?bogus=1
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

GET {{base}}/self/v1/api/schemas/locations/test/schemas/string?bogus=1
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

GET {{base}}/self/v1/api/schemas/positions/test/schemas/string?bogus=1
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

GET {{base}}/self/v1/api/schemas/stats/test/schemas/string?bogus=1
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

GET {{base}}/self/v1/api/schemas/dependencies/test/schemas/string?bogus=1
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

GET {{base}}/self/v1/api/schemas/dependents/test/schemas/string?bogus=1
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

# The endpoints that take a request body read nothing out of the query string
# either, and refuse before the body is read
POST {{base}}/self/v1/api/schemas/evaluate/test/schemas/string?bogus=1
```
"hello"
```
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

POST {{base}}/self/v1/api/schemas/trace/test/schemas/string?bogus=1
```
"hello"
```
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

POST {{base}}/self/v1/api/playground/schemas/trace?bogus=1
```
{ "instance": "hello", "schema": { "$schema": "https://json-schema.org/draft/2020-12/schema", "type": "string" } }
```
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"

# A wrong method is still reported as a wrong method, since what the query
# string carries does not make a method any more valid for the URL
POST {{base}}/self/v1/api/schemas/search?bogus=1
HTTP 405
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:method-not-allowed"

# A preflight asks whether a request may be sent rather than making one, so
# it is answered whatever the query string carries. Refusing it would leave a
# browser unable to ask
OPTIONS {{base}}/self/v1/api/schemas/search?bogus=1
Origin: {{base}}
Access-Control-Request-Method: GET
HTTP 204
[Asserts]
header "Access-Control-Allow-Methods" exists

OPTIONS {{base}}/self/v1/api/list?bogus=1
Origin: {{base}}
Access-Control-Request-Method: GET
HTTP 204

# The registry itself is a content surface rather than a declared one. A
# schema URL is published, pasted and linked, and whatever a browser, a CDN or
# an analytics tool appends to one travels with it, so a name this surface
# does not read is ignored rather than refused
GET {{base}}/test/schemas/string.json?foo=bar&baz=qux
HTTP 200
[Asserts]
jsonpath "$['$id']" exists

# Asset URLs are versioned by appending to the query string, which is what
# makes a cached asset expire
GET {{base}}/self/v1/static/style.min.css?v=12345
HTTP 200

# A liveness probe is answered whatever it appends, since what sits in front
# of an instance is not something the instance dictates
GET {{base}}/self/v1/health?probe=1
HTTP 200
