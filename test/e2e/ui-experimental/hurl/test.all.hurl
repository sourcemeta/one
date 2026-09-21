# The application is one page whatever location is asked for, so the root is
# where its name is read out. What it points at is named after its own
# contents, which is why the name is captured here rather than written down
GET {{base}}
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "ETag" exists
header "Last-Modified" exists
xpath "boolean(//div[@id='root'])" == true
[Captures]
bundle: xpath "string(//script[@type='module']/@src)"

# A location that holds a schema is answered with the same page, since what
# tells one location from another is what the application reads from the API
# once it is running rather than what the index wrote out per location
GET {{base}}/test/example
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "boolean(//div[@id='root'])" == true

# A location that holds nothing is still answered with the page, as the
# application has a screen for that too. Only the status says it is not here
GET {{base}}/does-not-exist
Accept: text/html
HTTP 404
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "boolean(//div[@id='root'])" == true

# The assets carry their build in their own name, which is what lets a browser
# keep them without ever asking again
GET {{base}}{{bundle}}
HTTP 200
Content-Type: text/javascript
Cache-Control: public, max-age=31536000, immutable
[Asserts]
header "Vary" == "Accept-Encoding"
header "ETag" exists

# Nothing the old pages were built from is served any more, since opting in
# means none of them are produced
GET {{base}}/self/v1/static/style.min.css
HTTP 404

GET {{base}}/self/v1/static/main.min.js
HTTP 404

# What a caller asks for as a schema is unchanged, so whatever reads this
# instance as a registry cannot tell which interface it is serving
GET {{base}}/test/example.json
HTTP 200
Content-Type: application/schema+json
[Asserts]
# An ingested schema is given an identifier under this instance, so what comes
# back names where it now lives rather than where it was written
jsonpath "$['$id']" == "{{base}}/test/example"
jsonpath "$.title" == "Example"

# The API the application is written against answers as it always did
GET {{base}}/self/v1/api/list
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.path" == "/"
jsonpath "$.entries" count == 2

GET {{base}}/self/v1/api/schemas/metadata/test/example
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.title" == "Example"
jsonpath "$.identifier" == "{{base}}/test/example"

GET {{base}}/self/v1/health
HTTP 200
