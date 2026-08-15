# A registry gated in its entirety at the root, which is the shape where every
# single directory is private. Nothing here is ever drawn as an ordinary
# folder, the instance's own `self` tree included, since the root policy
# governs that as much as it governs the catalog.
#
# This is the case the old rule was least able to express. It marked a child
# whose policies differed from its parent's, and under a root policy nothing
# ever differs from anything, so a wholly private registry was drawn exactly
# like a wholly public one.

# The root under the key that opens everything. Both children are locked, and
# the special directories table is not exempt from that
GET {{base}}/
Accept: text/html
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 2
xpath "count(//tr[.//a[normalize-space(.) = 'catalog']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'self']]//i[contains(@class, 'bi-lock-fill')])" == 1

# The page itself says what it is, not only the rows on it. This is the shape
# where that matters most: every row is locked, so a reader with nothing else
# to compare them against learns from the page rather than from the contrast
GET {{base}}/
Accept: text/html
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 1

# One level in, where the policy is inherited rather than declared, and the
# nested directory says so just the same
GET {{base}}/catalog/
Accept: text/html
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'nested']]//i[contains(@class, 'bi-lock-fill')])" == 1

# Two levels in there are no directories left, and the schema row carries its
# dialect rather than a lock
GET {{base}}/catalog/nested/
Accept: text/html
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//tr[.//a[normalize-space(.) = 'deep']])" == 1

# Without the key there is no listing to mark at all, which is what keeps the
# lock from ever standing for something being withheld
GET {{base}}/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
header "WWW-Authenticate" not exists
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0

# The data underneath it marks the root as private, since three policies govern
# it and a caller satisfying any of them is admitted, and every entry inherits
# that
GET {{base}}/self/v1/api/list
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
root_listing: body
root_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/"
jsonpath "$.private" == true
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "catalog"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].private" == true
jsonpath "$.entries[1].name" == "self"
jsonpath "$.entries[1].type" == "directory"
jsonpath "$.entries[1].private" == true

POST {{base}}/self/v1/api/schemas/evaluate{{root_schema}}
Authorization: Bearer full-secret-key
```
{{root_listing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# And one level in, where nothing is declared and everything is inherited
GET {{base}}/self/v1/api/list/catalog
Authorization: Bearer full-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
catalog_listing: body
catalog_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/catalog"
jsonpath "$.private" == true
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].name" == "nested"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].private" == true
jsonpath "$.entries[1].name" == "classified"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].private" == true
jsonpath "$.entries[2].name" == "example"
jsonpath "$.entries[2].type" == "schema"
jsonpath "$.entries[2].private" == true

POST {{base}}/self/v1/api/schemas/evaluate{{catalog_schema}}
Authorization: Bearer full-secret-key
```
{{catalog_listing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
