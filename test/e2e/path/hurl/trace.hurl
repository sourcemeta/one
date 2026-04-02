# Trace a simple schema - keyword locations must include path prefix
POST {{base}}/v1/catalog/self/v1/api/schemas/trace/example/string
Content-Type: application/json
"hello"
HTTP 200
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/trace/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
jsonpath "$.steps" count == 2
jsonpath "$.steps[0].keywordLocation" == "{{base}}/v1/catalog/example/string#/type"
jsonpath "$.steps[0].name" == "AssertionTypeStrict"
jsonpath "$.steps[0].type" == "push"
jsonpath "$.steps[0].vocabulary" == "https://json-schema.org/draft/2020-12/vocab/validation"
jsonpath "$.steps[1].keywordLocation" == "{{base}}/v1/catalog/example/string#/type"
jsonpath "$.steps[1].type" == "pass"
jsonpath "$.steps[1].message" == "The value was expected to be of type string"

# Trace a nested schema - keyword locations must include path prefix
POST {{base}}/v1/catalog/self/v1/api/schemas/trace/example/nested/deep
Content-Type: application/json
3.14
HTTP 200
[Asserts]
jsonpath "$.valid" == true
jsonpath "$.steps[0].keywordLocation" == "{{base}}/v1/catalog/example/nested/deep#/type"

# Trace a rebased schema - keyword locations must include path prefix
POST {{base}}/v1/catalog/self/v1/api/schemas/trace/rebased/with-id
Content-Type: application/json
true
HTTP 200
[Asserts]
jsonpath "$.valid" == true
jsonpath "$.steps[0].keywordLocation" == "{{base}}/v1/catalog/rebased/with-id#/type"

# Trace a schema with $ref to sibling - should include both schemas' URIs
POST {{base}}/v1/catalog/self/v1/api/schemas/trace/example/with-ref
Content-Type: application/json
{"name": "hello"}
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# No body returns 400
POST {{base}}/v1/catalog/self/v1/api/schemas/trace/example/string
HTTP 400
Content-Type: application/problem+json
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/no-instance"
