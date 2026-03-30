GET {{base}}/self/v1/api/schemas/locations/test/schemas/no-id
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/locations/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
jsonpath "$.static['{{base}}/test/schemas/no-id'].base" == "{{base}}/test/schemas/no-id"
jsonpath "$.static['{{base}}/test/schemas/no-id'].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.static['{{base}}/test/schemas/no-id'].dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.static['{{base}}/test/schemas/no-id'].parent" == null
jsonpath "$.static['{{base}}/test/schemas/no-id'].pointer" count == 0
jsonpath "$.static['{{base}}/test/schemas/no-id'].relativePointer" count == 0
jsonpath "$.static['{{base}}/test/schemas/no-id'].root" == "{{base}}/test/schemas/no-id"
jsonpath "$.static['{{base}}/test/schemas/no-id'].type" == "resource"
jsonpath "$.static['{{base}}/test/schemas/no-id'].position" count == 4
jsonpath "$.static['{{base}}/test/schemas/no-id'].position[0]" == 1
jsonpath "$.static['{{base}}/test/schemas/no-id'].position[1]" == 1
jsonpath "$.static['{{base}}/test/schemas/no-id'].position[2]" == 5
jsonpath "$.static['{{base}}/test/schemas/no-id'].position[3]" == 1
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].base" == "{{base}}/test/schemas/no-id"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].parent" count == 0
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].pointer" count == 1
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].pointer[0]" == "$id"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].relativePointer" count == 1
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].relativePointer[0]" == "$id"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].root" == "{{base}}/test/schemas/no-id"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].type" == "pointer"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].position" count == 4
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].position[0]" == 3
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].position[1]" == 3
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].position[2]" == 3
jsonpath "$.static['{{base}}/test/schemas/no-id#/$id'].position[3]" == 51
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].base" == "{{base}}/test/schemas/no-id"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].parent" count == 0
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].pointer" count == 1
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].pointer[0]" == "$schema"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].relativePointer" count == 1
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].relativePointer[0]" == "$schema"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].root" == "{{base}}/test/schemas/no-id"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].type" == "pointer"
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].position" count == 4
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].position[0]" == 2
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].position[1]" == 3
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].position[2]" == 2
jsonpath "$.static['{{base}}/test/schemas/no-id#/$schema'].position[3]" == 54
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].base" == "{{base}}/test/schemas/no-id"
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].baseDialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].dialect" == "http://json-schema.org/draft-07/schema#"
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].parent" count == 0
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].pointer" count == 1
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].pointer[0]" == "type"
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].relativePointer" count == 1
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].relativePointer[0]" == "type"
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].root" == "{{base}}/test/schemas/no-id"
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].type" == "pointer"
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].position" count == 4
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].position[0]" == 4
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].position[1]" == 3
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].position[2]" == 4
jsonpath "$.static['{{base}}/test/schemas/no-id#/type'].position[3]" == 18

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

HEAD {{base}}/self/v1/api/schemas/locations/test/schemas/no-id
Accept: application/json
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/schemas/locations/response>; rel="describedby"
[Asserts]
header "ETag" exists
header "Last-Modified" exists
bytes count == 0

POST {{base}}/self/v1/api/schemas/locations/test/schemas/no-id
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

GET {{base}}/self/v1/api/schemas/locations/xyz/
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
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
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/locations
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
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
jsonpath "$.valid" == true
