# The root is governed by no policy, so it is public and lists anonymously. This
# is the carve-in shape: a public parent enumerating its children, each carrying
# the names of the policies that gate it, while the public children carry none
GET {{base}}/self/v1/api/list
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
root_listing: body
root_listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/"
jsonpath "$.url" == "{{base}}"
jsonpath "$.breadcrumb" count == 0
jsonpath "$.schemas" == 62
jsonpath "$.policies" count == 0
jsonpath "$.entries" count == 6
jsonpath "$.entries[0].name" == "internal"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/internal/"
jsonpath "$.entries[0].policies" count == 1
jsonpath "$.entries[0].policies[0]" == "internal"
jsonpath "$.entries[1].name" == "orphan"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].path" == "/orphan/"
jsonpath "$.entries[1].policies" count == 0
jsonpath "$.entries[2].name" == "private"
jsonpath "$.entries[2].type" == "directory"
jsonpath "$.entries[2].path" == "/private/"
jsonpath "$.entries[2].policies" count == 1
jsonpath "$.entries[2].policies[0]" == "reports"
jsonpath "$.entries[3].name" == "public"
jsonpath "$.entries[3].type" == "directory"
jsonpath "$.entries[3].path" == "/public/"
jsonpath "$.entries[3].policies" count == 0
jsonpath "$.entries[4].name" == "reports"
jsonpath "$.entries[4].type" == "directory"
jsonpath "$.entries[4].path" == "/reports/"
jsonpath "$.entries[4].policies" count == 1
jsonpath "$.entries[4].policies[0]" == "reports"
jsonpath "$.entries[5].name" == "self"
jsonpath "$.entries[5].type" == "directory"
jsonpath "$.entries[5].path" == "/self/"
jsonpath "$.entries[5].title" == "Self"
jsonpath "$.entries[5].policies" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{root_listing_schema}}
```
{{root_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

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
jsonpath "$.policies" count == 0
jsonpath "$.entries[0].name" == "string"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/public/string"
jsonpath "$.entries[0].identifier" == "{{base}}/public/string"
jsonpath "$.entries[0].policies" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{public_listing_schema}}
```
{{public_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An un-policied directory is public too, so the orphan collection lists
# anonymously
GET {{base}}/self/v1/api/list/orphan
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
orphan_listing: body
orphan_listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.schemas" == 1
jsonpath "$.policies" count == 0
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "lonely"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/orphan/lonely"
jsonpath "$.entries[0].identifier" == "{{base}}/orphan/lonely"
jsonpath "$.entries[0].policies" count == 0

POST {{base}}/self/v1/api/schemas/evaluate{{orphan_listing_schema}}
```
{{orphan_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Listing an apiKey directory is denied without a credential
GET {{base}}/self/v1/api/list/private
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# With a credential the listing is served and validates against its schema. Its
# nested child inherits the same gate, so it carries the governing policy name
GET {{base}}/self/v1/api/list/private
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
private_listing: body
private_listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.entries" count == 2
jsonpath "$.policies" count == 1
jsonpath "$.policies[0]" == "reports"
jsonpath "$.entries[0].name" == "open"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/private/open/"
jsonpath "$.entries[0].policies" count == 1
jsonpath "$.entries[0].policies[0]" == "reports"
jsonpath "$.entries[1].name" == "secret"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].path" == "/private/secret"
jsonpath "$.entries[1].policies" count == 1
jsonpath "$.entries[1].policies[0]" == "reports"

POST {{base}}/self/v1/api/schemas/evaluate{{private_listing_schema}}
```
{{private_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The nested directory inherits the gate, so it is denied anonymously
GET {{base}}/self/v1/api/list/private/open
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# With the governing key it lists, marked private, and validates against schema
GET {{base}}/self/v1/api/list/private/open
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
nested_listing: body
nested_listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.path" == "/private/open"
jsonpath "$.policies" count == 1
jsonpath "$.policies[0]" == "reports"
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "shared"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/private/open/shared"
jsonpath "$.entries[0].identifier" == "{{base}}/private/open/shared"
jsonpath "$.entries[0].policies" count == 1
jsonpath "$.entries[0].policies[0]" == "reports"

POST {{base}}/self/v1/api/schemas/evaluate{{nested_listing_schema}}
```
{{nested_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The HTML rendering of an apiKey directory is gated, rendering the 401 page
GET {{base}}/private/
Accept: text/html
HTTP 401
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This page requires authentication"

GET {{base}}/private/
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
[Asserts]
header "Content-Type" contains "text/html"
# Inside the gated collection nothing adds a new policy, so there is no further
# barrier: the lock sits only at the boundary, never repeated within
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0

# The HTML rendering of the nested directory is gated identically
GET {{base}}/private/open/
Accept: text/html
HTTP 401
Content-Type: text/html; charset=utf-8
WWW-Authenticate: Bearer realm="registry"
[Asserts]
xpath "string(//title)" == "Unauthorized"
xpath "string(//h2[@class='fw-bold'])" == "This page requires authentication"

GET {{base}}/private/open/
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
[Asserts]
header "Content-Type" contains "text/html"

# The HTML rendering of the public root lists anonymously, never the 401 page.
# A child whose governing policies differ from the root is an authentication
# barrier, shown with a lock instead of the plain folder icon, while a child that
# shares the root's public scope keeps the folder
GET {{base}}/
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
[Asserts]
header "Content-Type" contains "text/html"
xpath "string(//title)" != "Unauthorized"
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 3
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 3
xpath "count(//tr[.//a[normalize-space(.) = 'private']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'internal']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'reports']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'public']]//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//tr[.//a[normalize-space(.) = 'public']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'orphan']]//i[contains(@class, 'bi-folder-fill')])" == 1
