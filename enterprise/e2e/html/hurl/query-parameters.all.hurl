# The RDF endpoint declares its parameters like every other endpoint under
# `/self/v1/api`, which is to say it reads none, so a name in the query
# string is refused rather than ignored.

POST {{base}}/self/v1/api/schemas/rdf/test/jsonld/product?bogus=1
Content-Type: application/json
{
  "instance": {
    "sku": "ABC-123",
    "name": "Vacuum Robot"
  }
}
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
jsonpath "$.detail" == "This endpoint does not recognise the bogus query parameter"

# The refusal is itself an error document this instance describes
POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# `flatten` and `context` are read out of the request body rather than the
# query string, so naming one there is refused like any other unknown name
POST {{base}}/self/v1/api/schemas/rdf/test/jsonld/product?flatten=1
Content-Type: application/json
{
  "instance": {
    "sku": "ABC-123",
    "name": "Vacuum Robot"
  }
}
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:unknown-query-parameter"
jsonpath "$.detail" == "This endpoint does not recognise the flatten query parameter"

# A preflight is answered whatever the query string carries, so that a browser
# can still ask whether it may send the request
OPTIONS {{base}}/self/v1/api/schemas/rdf/test/jsonld/product?bogus=1
HTTP 204
Allow: POST, OPTIONS
[Asserts]
header "Access-Control-Allow-Methods" == "POST, OPTIONS"

# The registry stays a content surface, so a name it does not read travels
# with the URL rather than being refused. The one exception is the company a
# conversion keeps, which was already refused before this
GET {{base}}/test/jsonld/product.json?foo=bar
HTTP 200

GET {{base}}/test/jsonld/product.json?as=2020-12&foo=bar
HTTP 400
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:incompatible-query-parameters"
