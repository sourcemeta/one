# An instance where nothing is gated marks nothing, and says so in the data as
# well as on the page. This holds in either edition, and in the Community one
# it can hold no other way, since a policy cannot be declared there at all.

# A schema page carries no mark
GET {{base}}/test/no-blaze/string
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0

# And neither does the page it is reached from
GET {{base}}/test
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" > 0

# The root of an instance where nothing is gated says nothing either, and a
# named collection has no mark beside its name
GET {{base}}/
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0

GET {{base}}/test/doc
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(//h2)" == "A sample schema folder"
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0

# The data underneath both says the same, which is what the render is drawn
# from rather than a second answer computed for the page
GET {{base}}/self/v1/api/schemas/metadata/test/no-blaze/string
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
metadata_body: body
metadata_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.private" == false
jsonpath "$.path" == "/test/no-blaze/string"

POST {{base}}/self/v1/api/schemas/evaluate{{metadata_schema}}
```
{{metadata_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/test/no-blaze
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
listing_body: body
listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.private" == false
jsonpath "$.entries[?(@.name == 'string')].private" == false

POST {{base}}/self/v1/api/schemas/evaluate{{listing_schema}}
```
{{listing_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
