# A public schema's incoming graph names whoever points at it, and one of those
# is gated. Who is asking therefore decides what the graph says, which is the
# disclosure this precomputes away: the answer is built per view at index time
# rather than filtered when the question arrives.
#
# Two schemas point at /public/string, which anybody may read: /public/wrapper,
# which anybody may read too, and /reports/quarterly, which the reports policy
# gates. So the graph narrows rather than empties, and a view that reported
# nothing at all would fail here as loudly as one that reported everything.

GET {{base}}/self/v1/api/schemas/dependents/public/string
HTTP 200
Content-Type: application/json
Cache-Control: private, max-age=0, must-revalidate
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
anonymous_body: body
anonymous_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" matches /^"[0-9a-f]{64}"$/
[
  {
    "from": "{{base}}/public/wrapper",
    "to": "{{base}}/public/string",
    "at": "/properties/label/$ref"
  }
]

POST {{base}}/self/v1/api/schemas/evaluate{{anonymous_schema}}
```
{{anonymous_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The key that opens the gated referrer reads both edges. Without this half, a
# graph emptied for everybody would look exactly like one that was filtered
GET {{base}}/self/v1/api/schemas/dependents/public/string
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/json
Cache-Control: private, max-age=0, must-revalidate
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
reports_body: body
reports_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" matches /^"[0-9a-f]{64}"$/
[
  {
    "from": "{{base}}/public/wrapper",
    "to": "{{base}}/public/string",
    "at": "/properties/label/$ref"
  },
  {
    "from": "{{base}}/reports/quarterly",
    "to": "{{base}}/public/string",
    "at": "/items/$ref"
  }
]

POST {{base}}/self/v1/api/schemas/evaluate{{reports_schema}}
```
{{reports_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The second key the same policy accepts reads the same graph, since what is
# served follows the policies a credential satisfies rather than the credential
GET {{base}}/self/v1/api/schemas/dependents/public/string
Authorization: Bearer secondary-secret-key
HTTP 200
Content-Type: application/json
Cache-Control: private, max-age=0, must-revalidate
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[
  {
    "from": "{{base}}/public/wrapper",
    "to": "{{base}}/public/string",
    "at": "/properties/label/$ref"
  },
  {
    "from": "{{base}}/reports/quarterly",
    "to": "{{base}}/public/string",
    "at": "/items/$ref"
  }
]

# A key for somewhere else reads what an anonymous caller reads. Holding a
# credential is not what widens the graph, holding the one that opens what
# points at it is
GET {{base}}/self/v1/api/schemas/dependents/public/string
Authorization: Bearer internal-secret-key
HTTP 200
Content-Type: application/json
Cache-Control: private, max-age=0, must-revalidate
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[
  {
    "from": "{{base}}/public/wrapper",
    "to": "{{base}}/public/string",
    "at": "/properties/label/$ref"
  }
]

# And the gated schema's own graph is still refused outright, so nothing here
# turned a denial into an empty answer
GET {{base}}/self/v1/api/schemas/dependents/reports/quarterly
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag, WWW-Authenticate
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
