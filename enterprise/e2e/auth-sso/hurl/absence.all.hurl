# A browser navigating to a gated page is told there is nothing there, in that
# view's own missing page. An interactive policy governs the private tree, and
# none of that shows here: the page names no provider and offers no way in,
# since signing in is somewhere a caller goes rather than something a dead end
# hands them
GET {{base}}/private/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
Vary: Accept, Accept-Encoding
[Captures]
missing_etag: header "ETag"
[Asserts]
header "Location" not exists
header "Set-Cookie" not exists
header "WWW-Authenticate" not exists
xpath "string(//title)" == "Not Found"
xpath "string(//h2[@class='fw-bold'])" == "Oops! What you are looking for is not here"
xpath "string(//p[@class='lead'])" == "Are you sure the link you got is correct?"
xpath "count(//a[@data-sourcemeta-ui-login])" == 0

# HEAD is a navigation too: the same answer, with no body
HEAD {{base}}/private/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists

# An existing schema page under the policy is answered by the byte-identical
# page, so nothing reveals that the schema asked for is really there. The ETag
# is the content hash, so an equal ETag proves an equal body
GET {{base}}/private/secret
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
header "ETag" == {{missing_etag}}

# A path that resolves to no schema at all returns the same page, so what is
# behind the gate and what was never there read alike
GET {{base}}/private/does-not-exist
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
header "ETag" == {{missing_etag}}

# A nested path that does not exist answers with the identical page too
GET {{base}}/private/nope/deeper/still
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
header "ETag" == {{missing_etag}}

# The query string changes nothing about it
GET {{base}}/private/?view=raw&page=2
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
header "ETag" == {{missing_etag}}

# And a path no policy governs at all, which this instance simply does not
# have, is answered by that very same page. This is the whole point: from
# outside, a collection behind a gate and a name nobody ever used are one
GET {{base}}/no-such-collection/
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
header "WWW-Authenticate" not exists
header "ETag" == {{missing_etag}}

# A page that is there is served, so this guards only what is really absent
GET {{base}}/public/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
header "Location" not exists
xpath "string(//title)" != "Not Found"

# A client asking for JSON is told the same thing in its own terms
GET {{base}}/private/secret.json
Accept: application/json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
machine_missing: body
error_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Location" not exists
header "WWW-Authenticate" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{machine_missing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# An explicit ".json" asks for the schema rather than the page, so even a
# browser is answered in the schema's terms
GET {{base}}/private/secret.json
Accept: text/html
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
browser_json_missing: body
[Asserts]
header "Location" not exists
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{browser_json_missing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
