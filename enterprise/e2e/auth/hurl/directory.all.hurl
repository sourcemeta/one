# Listing a public directory needs no credential, and the response validates
# against its own output schema
GET {{base}}/self/v1/api/list/public
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
public_listing: body
public_listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.schemas" == 1
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "string"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/public/string"
jsonpath "$.entries[0].identifier" == "{{base}}/public/string"

POST {{base}}/self/v1/api/schemas/evaluate{{public_listing_schema}}
```
{{public_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Listing an apiKey directory is denied without a credential
GET {{base}}/self/v1/api/list/private
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# With a credential the listing is served and validates against its schema. It
# exposes the nested public carve-out as a child directory
GET {{base}}/self/v1/api/list/private
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
private_listing: body
private_listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "open"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/private/open/"
jsonpath "$.entries[1].name" == "secret"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].path" == "/private/secret"

POST {{base}}/self/v1/api/schemas/evaluate{{private_listing_schema}}
```
{{private_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The nested public carve-out can be listed anonymously
GET {{base}}/self/v1/api/list/private/open
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "shared"
jsonpath "$.entries[0].path" == "/private/open/shared"

# The root listing is governed by the (absent) root policy, so it is denied
GET {{base}}/self/v1/api/list
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# The HTML rendering of an apiKey directory is gated just like the JSON API
GET {{base}}/private/
Accept: text/html
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

GET {{base}}/private/
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
[Asserts]
header "Content-Type" contains "text/html"

# The HTML rendering of the public carve-out directory is anonymous
GET {{base}}/private/open/
Accept: text/html
HTTP 200
[Asserts]
header "Content-Type" contains "text/html"
