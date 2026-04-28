# Health check endpoint is not available
GET {{base}}/self/v1/health
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# List endpoint is not available
GET {{base}}/self/v1/api/list
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

GET {{base}}/self/v1/api/list/test
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Schema metadata endpoint is not available
GET {{base}}/self/v1/api/schemas/metadata/test/schemas/string
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Schema search endpoint is not available
GET {{base}}/self/v1/api/schemas/search
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Schema dependencies endpoint is not available
GET {{base}}/self/v1/api/schemas/dependencies/test/schemas/string
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Schema dependents endpoint is not available
GET {{base}}/self/v1/api/schemas/dependents/test/schemas/string
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Schema health endpoint is not available
GET {{base}}/self/v1/api/schemas/health/test/schemas/string
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Schema locations endpoint is not available
GET {{base}}/self/v1/api/schemas/locations/test/schemas/string
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Schema positions endpoint is not available
GET {{base}}/self/v1/api/schemas/positions/test/schemas/string
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Schema stats endpoint is not available
GET {{base}}/self/v1/api/schemas/stats/test/schemas/string
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Evaluate endpoint is not available
POST {{base}}/self/v1/api/schemas/evaluate/test/schemas/string
```
"hello"
```
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Trace endpoint is not available
POST {{base}}/self/v1/api/schemas/trace/test/schemas/string
```
"hello"
```
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

# Catch-all API path is not available
GET {{base}}/self/v1/api/anything
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Link" not exists
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"
