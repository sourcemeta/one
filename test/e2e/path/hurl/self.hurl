# The self/v1 API and schemas are always available

GET {{base}}/v1/catalog/self/v1/health
HTTP 200

GET {{base}}/v1/catalog/self/v1/api/list
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.entries[?(@.name == 'self')].type" nth 0 == "directory"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/list/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/api/list/self
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.entries" count >= 1

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/list/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/v1/catalog/self/v1/schemas/configuration/configuration.json
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *

GET {{base}}/v1/catalog/self/v1/schemas/api/error.json
HTTP 200
Content-Type: application/schema+json
Access-Control-Allow-Origin: *

GET {{base}}/v1/catalog/self/v1/api/schemas/metadata/self/v1/schemas/configuration/configuration
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </v1/catalog/self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
[Captures]
last_response: body
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.path" == "/v1/catalog/self/v1/schemas/configuration/configuration"
jsonpath "$.title" == "The Sourcemeta One configuration file format"

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate/self/v1/schemas/api/schemas/metadata/response
```
{{last_response}}
```
HTTP 200
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
