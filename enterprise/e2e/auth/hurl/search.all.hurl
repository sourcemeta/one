# Search surfaces public schemas to anonymous callers and validates against its
# own output schema
GET {{base}}/self/v1/api/schemas/search?q=string
HTTP 200
Cache-Control: public, max-age=60
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
public_results: body
public_results_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/public/string"
jsonpath "$[0].identifier" == "{{base}}/public/string"

POST {{base}}/self/v1/api/schemas/evaluate{{public_results_schema}}
```
{{public_results}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An un-policied schema is public, so search surfaces it to anonymous callers
GET {{base}}/self/v1/api/schemas/search?q=lonely
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/orphan/lonely"
jsonpath "$[0].identifier" == "{{base}}/orphan/lonely"

# Search must not leak apiKey schemas to anonymous callers
GET {{base}}/self/v1/api/schemas/search?q=secret
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 0

# A schema nested under an apiKey prefix inherits the gate, so it stays hidden
# anonymously and is revealed only to the governing key
GET {{base}}/self/v1/api/schemas/search?q=shared
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 0

GET {{base}}/self/v1/api/schemas/search?q=shared
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/private/open/shared"

# The same query reveals the schema once the caller is authorized for it
GET {{base}}/self/v1/api/schemas/search?q=secret
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/private/secret"

# Results are scoped per policy: the internal schema stays hidden behind its own
# key and is not revealed by another policy's key
GET {{base}}/self/v1/api/schemas/search?q=config
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 0

GET {{base}}/self/v1/api/schemas/search?q=config
Authorization: Bearer internal-secret-key
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/internal/config"
