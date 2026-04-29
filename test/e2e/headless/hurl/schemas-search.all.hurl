GET {{base}}/self/v1/api/schemas/search
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/missing-query"
jsonpath "$.detail" == "You must provide a query parameter to search for"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/search?q=
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/missing-query"
jsonpath "$.detail" == "You must provide a query parameter to search for"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/search?q=foo
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
jsonpath "$.detail" == "This HTTP method is invalid for this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A string we know won't give any results
GET {{base}}/self/v1/api/schemas/search?q=xxxxxxxxxxxx
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Results with title/description
GET {{base}}/self/v1/api/schemas/search?q=bundling
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 2
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"
jsonpath "$[1].path" == "/test/schemas/bundling-double"
jsonpath "$[1].title" == ""
jsonpath "$[1].description" == ""

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Test casing
GET {{base}}/self/v1/api/schemas/search?q=bUNdLing
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 2
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"
jsonpath "$[1].path" == "/test/schemas/bundling-double"
jsonpath "$[1].title" == ""
jsonpath "$[1].description" == ""

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# No matter what, we impose a limit on the results
GET {{base}}/self/v1/api/schemas/search?q=e
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count <= 10

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Explicit limit
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=1
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Scope: title only
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=title
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Scope: path and description
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=path,description
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 2
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"
jsonpath "$[1].path" == "/test/schemas/bundling-double"
jsonpath "$[1].title" == ""
jsonpath "$[1].description" == ""

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Scope: description only
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=description
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Limit and scope combined
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=1&scope=path
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: limit=0
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=0
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-limit"
jsonpath "$.detail" == "The limit must be a positive integer between 1 and 100"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: limit=-1
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=-1
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-limit"
jsonpath "$.detail" == "The limit must be a positive integer between 1 and 100"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: limit=abc
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=abc
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-limit"
jsonpath "$.detail" == "The limit must be a positive integer between 1 and 100"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: limit=101
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=101
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-limit"
jsonpath "$.detail" == "The limit must be a positive integer between 1 and 100"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: scope=invalid
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=invalid
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-scope"
jsonpath "$.detail" == "The scope must be a comma-separated list of: path, title, description"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: scope with one valid and one invalid field
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=title,invalid
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-scope"
jsonpath "$.detail" == "The scope must be a comma-separated list of: path, title, description"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: limit=3.5 (not an integer)
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=3.5
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-limit"
jsonpath "$.detail" == "The limit must be a positive integer between 1 and 100"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Empty limit= and scope= use defaults
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=&scope=
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 2
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"
jsonpath "$[1].path" == "/test/schemas/bundling-double"
jsonpath "$[1].title" == ""
jsonpath "$[1].description" == ""

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: scope with uppercase
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=Path
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-scope"
jsonpath "$.detail" == "The scope must be a comma-separated list of: path, title, description"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: scope with whitespace around token
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=path,%20title
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-scope"
jsonpath "$.detail" == "The scope must be a comma-separated list of: path, title, description"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Trailing comma in scope is accepted
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=title,
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 1
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: scope with only commas
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=,
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-scope"
jsonpath "$.detail" == "The scope must be a comma-separated list of: path, title, description"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: scope with multiple commas only
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=,,
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-scope"
jsonpath "$.detail" == "The scope must be a comma-separated list of: path, title, description"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Duplicate scope values are accepted
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=path,path
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 2
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"
jsonpath "$[1].path" == "/test/schemas/bundling-double"
jsonpath "$[1].title" == ""
jsonpath "$[1].description" == ""

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Limit at upper boundary
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=100
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 2
jsonpath "$[0].path" == "/test/schemas/bundling-single"
jsonpath "$[0].title" == "Bundling"
jsonpath "$[0].description" == "A bundling example"
jsonpath "$[1].path" == "/test/schemas/bundling-double"
jsonpath "$[1].title" == ""
jsonpath "$[1].description" == ""

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: limit with leading plus sign
GET {{base}}/self/v1/api/schemas/search?q=bundling&limit=%2B5
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-limit"
jsonpath "$.detail" == "The limit must be a positive integer between 1 and 100"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Error: query too long (over 256 characters)
GET {{base}}/self/v1/api/schemas/search?q=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-query"
jsonpath "$.detail" == "The search query must not exceed 256 characters"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Query with null byte gives no results
GET {{base}}/self/v1/api/schemas/search?q=bundling%00extra
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Query with tab character gives no results
GET {{base}}/self/v1/api/schemas/search?q=bundling%09single
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Query with newline character gives no results
GET {{base}}/self/v1/api/schemas/search?q=bundling%0Asingle
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/search/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Scope with null byte is rejected
GET {{base}}/self/v1/api/schemas/search?q=bundling&scope=path%00extra
HTTP 400
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.status" == 400
jsonpath "$.title" == "sourcemeta:one/invalid-search-scope"
jsonpath "$.detail" == "The scope must be a comma-separated list of: path, title, description"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
