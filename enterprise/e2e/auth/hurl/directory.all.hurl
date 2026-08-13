# The root is governed by no policy, so it lists anonymously. A view is written
# only what it can reach, so this names the ungated children and stops there:
# the gated directories are absent rather than shown as doors, and the counts
# behind them are absent with them. The response is shareable, so it is public
# and revalidated rather than held privately
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
root_etag: header "ETag"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/"
jsonpath "$.url" == "{{base}}"
jsonpath "$.breadcrumb" count == 0
jsonpath "$.schemas" == 64
jsonpath "$.policies" count == 0
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].name" == "orphan"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/orphan/"
jsonpath "$.entries[0].policies" count == 0
jsonpath "$.entries[0].schemas" == 1
jsonpath "$.entries[0].health" == 50
jsonpath "$.entries[1].name" == "public"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].path" == "/public/"
jsonpath "$.entries[1].policies" count == 0
jsonpath "$.entries[1].schemas" == 1
jsonpath "$.entries[1].health" == 0
jsonpath "$.entries[2].name" == "self"
jsonpath "$.entries[2].type" == "directory"
jsonpath "$.entries[2].path" == "/self/"
jsonpath "$.entries[2].policies" count == 0
jsonpath "$.entries[2].schemas" == 62
jsonpath "$.entries[2].health" == 100
POST {{base}}/self/v1/api/schemas/evaluate{{root_listing_schema}}
```
{{root_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The same URL under a credential is answered from the view that credential
# resolves to, which holds the anonymous catalog plus what its policy gates.
# The gated children appear here and nowhere else, they carry the name of the
# policy governing them, and the counts move with them, so how much sits behind
# a gate is told only to a caller that can open it. The answer varies by caller
# and cannot be shared, so it is private, and it is a different body under a
# different validator than the anonymous one
GET {{base}}/self/v1/api/list
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
reports_listing: body
reports_listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
header "ETag" != "{{root_etag}}"
jsonpath "$.path" == "/"
jsonpath "$.url" == "{{base}}"
jsonpath "$.breadcrumb" count == 0
jsonpath "$.schemas" == 67
jsonpath "$.policies" count == 0
jsonpath "$.entries" count == 5
jsonpath "$.entries[0].name" == "orphan"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/orphan/"
jsonpath "$.entries[0].policies" count == 0
jsonpath "$.entries[0].schemas" == 1
jsonpath "$.entries[0].health" == 50
jsonpath "$.entries[1].name" == "private"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].path" == "/private/"
jsonpath "$.entries[1].policies" count == 1
jsonpath "$.entries[1].policies[0].name" == "reports"
jsonpath "$.entries[1].policies[0].title" == "reports"
jsonpath "$.entries[1].policies[0].type" == "apiKey"
jsonpath "$.entries[1].schemas" == 2
jsonpath "$.entries[1].health" == 34
jsonpath "$.entries[2].name" == "public"
jsonpath "$.entries[2].type" == "directory"
jsonpath "$.entries[2].path" == "/public/"
jsonpath "$.entries[2].policies" count == 0
jsonpath "$.entries[2].schemas" == 1
jsonpath "$.entries[2].health" == 0
jsonpath "$.entries[3].name" == "reports"
jsonpath "$.entries[3].type" == "directory"
jsonpath "$.entries[3].path" == "/reports/"
jsonpath "$.entries[3].policies" count == 1
jsonpath "$.entries[3].policies[0].name" == "reports"
jsonpath "$.entries[3].policies[0].title" == "reports"
jsonpath "$.entries[3].policies[0].type" == "apiKey"
jsonpath "$.entries[3].schemas" == 1
jsonpath "$.entries[3].health" == 50
jsonpath "$.entries[4].name" == "self"
jsonpath "$.entries[4].type" == "directory"
jsonpath "$.entries[4].path" == "/self/"
jsonpath "$.entries[4].policies" count == 0
jsonpath "$.entries[4].schemas" == 62
jsonpath "$.entries[4].health" == 100

POST {{base}}/self/v1/api/schemas/evaluate{{reports_listing_schema}}
Authorization: Bearer primary-secret-key
```
{{reports_listing}}
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
jsonpath "$.policies[0].name" == "reports"
jsonpath "$.policies[0].title" == "reports"
jsonpath "$.policies[0].type" == "apiKey"
jsonpath "$.entries[0].name" == "open"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].path" == "/private/open/"
jsonpath "$.entries[0].policies" count == 1
jsonpath "$.entries[0].policies[0].name" == "reports"
jsonpath "$.entries[0].policies[0].title" == "reports"
jsonpath "$.entries[0].policies[0].type" == "apiKey"
jsonpath "$.entries[1].name" == "secret"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].path" == "/private/secret"
jsonpath "$.entries[1].policies" count == 1
jsonpath "$.entries[1].policies[0].name" == "reports"
jsonpath "$.entries[1].policies[0].title" == "reports"
jsonpath "$.entries[1].policies[0].type" == "apiKey"

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
jsonpath "$.policies[0].name" == "reports"
jsonpath "$.policies[0].title" == "reports"
jsonpath "$.policies[0].type" == "apiKey"
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "shared"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/private/open/shared"
jsonpath "$.entries[0].identifier" == "{{base}}/private/open/shared"
jsonpath "$.entries[0].policies" count == 1
jsonpath "$.entries[0].policies[0].name" == "reports"
jsonpath "$.entries[0].policies[0].title" == "reports"
jsonpath "$.entries[0].policies[0].type" == "apiKey"

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
# It renders the same three children the API names, every one of them a plain
# folder: a directory the caller cannot enter is not drawn as a locked door,
# it is not drawn at all
GET {{base}}/
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
[Asserts]
header "Content-Type" contains "text/html"
xpath "string(//title)" != "Unauthorized"
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 3
xpath "count(//tr[.//a[normalize-space(.) = 'orphan']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'public']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'self']]//i[contains(@class, 'bi-folder-fill')])" == 1

# The same rendering under a credential is drawn from that policy's view, so
# the gated children appear, and the lock marks them because their governing
# policies differ from those of the root being listed. The children the
# anonymous rendering also shows stay plain folders
GET {{base}}/
Accept: text/html
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
[Asserts]
header "Content-Type" contains "text/html"
xpath "string(//title)" != "Unauthorized"
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 2
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 3
xpath "count(//tr[.//a[normalize-space(.) = 'private']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'reports']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'orphan']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'public']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'self']]//i[contains(@class, 'bi-folder-fill')])" == 1
