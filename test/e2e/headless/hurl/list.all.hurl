GET {{base}}/self/v1/api/list
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/"
jsonpath "$.url" == "{{base}}"
jsonpath "$.breadcrumb" count == 0
jsonpath "$.schemas" == 70
jsonpath "$.entries" count >= 2
jsonpath "$.entries[0].name" == "self"
jsonpath "$.entries[0].title" == "Self"
jsonpath "$.entries[0].description" == "The schemas that define the current version of this instance"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 100
jsonpath "$.entries[0].schemas" == 56
jsonpath "$.entries[0].path" == "/self/"
jsonpath "$.entries[1].name" == "test"
jsonpath "$.entries[1].title" == "Test"
jsonpath "$.entries[1].description" == "A directory full of testing schemas"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].schemas" == 14
jsonpath "$.entries[1].path" == "/test/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/test
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test"
jsonpath "$.url" == "{{base}}/test"
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.description" == "A directory full of testing schemas"
jsonpath "$.title" == "Test"
jsonpath "$.schemas" == 14
jsonpath "$.entries" count == 3

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/test/schemas/v2.0.json
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# A bare `..` collapses to the catalog root.
GET {{base}}/self/v1/api/list/..
[Options]
path-as-is: true
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/"
jsonpath "$.url" == "{{base}}"
jsonpath "$.breadcrumb" count == 0
jsonpath "$.schemas" == 70
jsonpath "$.entries" count >= 2
jsonpath "$.entries[0].name" == "self"
jsonpath "$.entries[0].title" == "Self"
jsonpath "$.entries[0].description" == "The schemas that define the current version of this instance"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].health" == 100
jsonpath "$.entries[0].schemas" == 56
jsonpath "$.entries[0].path" == "/self/"
jsonpath "$.entries[1].name" == "test"
jsonpath "$.entries[1].title" == "Test"
jsonpath "$.entries[1].description" == "A directory full of testing schemas"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].schemas" == 14
jsonpath "$.entries[1].path" == "/test/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# A mid-path `..` that canonicalises back inside the catalog root is served.
GET {{base}}/self/v1/api/list/test/../test
[Options]
path-as-is: true
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test"
jsonpath "$.url" == "{{base}}/test"
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.description" == "A directory full of testing schemas"
jsonpath "$.title" == "Test"
jsonpath "$.schemas" == 14
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].name" == "no-base"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/test/no-base/"
jsonpath "$.entries[1].name" == "no-blaze"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].path" == "/test/no-blaze/"
jsonpath "$.entries[2].name" == "schemas"
jsonpath "$.entries[2].type" == "directory"
jsonpath "$.entries[2].path" == "/test/schemas/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# A trailing slash on the URL path is accepted and serves the same listing.
GET {{base}}/self/v1/api/list/test/
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/test"
jsonpath "$.url" == "{{base}}/test"
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.description" == "A directory full of testing schemas"
jsonpath "$.title" == "Test"
jsonpath "$.schemas" == 14
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].name" == "no-base"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/test/no-base/"
jsonpath "$.entries[1].name" == "no-blaze"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].path" == "/test/no-blaze/"
jsonpath "$.entries[2].name" == "schemas"
jsonpath "$.entries[2].type" == "directory"
jsonpath "$.entries[2].path" == "/test/schemas/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
