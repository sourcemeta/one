# The self/v1 API and schemas are always available

GET {{base}}/self/v1/health
HTTP 200

GET {{base}}/self/v1/api/list
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.entries[?(@.name == 'self')].type" nth 0 == "directory"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/self
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.entries" count >= 1

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/schemas/configuration/configuration.json
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *

GET {{base}}/self/v1/schemas/api/error.json
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *

GET {{base}}/self/v1/api/schemas/metadata/self/v1/schemas/configuration/configuration
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/self/v1/schemas/configuration/configuration"
jsonpath "$.title" == "The Sourcemeta One configuration file format"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
