# A public schema's incoming graph names whoever points at it, and one of those
# is gated. Who is asking therefore decides what the graph says, which is the
# disclosure this precomputes away: the answer is built per view at index time
# rather than filtered when the question arrives.
#
# Two schemas point at /public/string, which anybody may read: /orphan/lonely,
# which anybody may read too, and /reports/quarterly, which the reports policy
# gates. So the graph narrows rather than empties, and a view that reported
# nothing at all would fail here as loudly as one that reported everything.

# Anonymous, so the answer holds only the referrer anybody may read.
#
# It is marked public because a shared cache keys on the URL alone, and this is
# the answer every anonymous caller receives, so handing a stored copy to the
# next of them serves what they would have been given anyway. That is true of
# this view alone: the same URL answers differently below.
GET {{base}}/self/v1/api/schemas/dependents/public/string
HTTP 200
Content-Type: application/json
Cache-Control: public, max-age=0, must-revalidate
Vary: Accept-Encoding
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
anonymous_body: body
anonymous_etag: header "ETag"
anonymous_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" matches /^"[0-9a-f]{64}"$/
[
  {
    "from": "{{base}}/orphan/lonely",
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
# graph emptied for everybody would look exactly like one that was filtered.
#
# The same URL, and a different answer, so this one must never be stored for
# reuse: a shared cache holding it would hand the gated referrer to whoever
# asked next, which is the disclosure above closed by another route. Marking it
# private is what says so, and it is the only header here that turns on who
# asked.
#
# The validator differs too, but nothing arranged that. It is a checksum of the
# artifact, and the artifacts differ, so a conditional request carrying one
# view's validator misses in another rather than matching something it was
# never served.
GET {{base}}/self/v1/api/schemas/dependents/public/string
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/json
Cache-Control: private, max-age=0, must-revalidate
Vary: Accept-Encoding
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
reports_body: body
reports_etag: header "ETag"
reports_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" matches /^"[0-9a-f]{64}"$/
header "ETag" != {{anonymous_etag}}
[
  {
    "from": "{{base}}/orphan/lonely",
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

# The second key the same policy accepts reads the same graph and carries the
# same validator, since what is served follows the policies a credential
# satisfies rather than the credential itself
GET {{base}}/self/v1/api/schemas/dependents/public/string
Authorization: Bearer secondary-secret-key
HTTP 200
Content-Type: application/json
Cache-Control: private, max-age=0, must-revalidate
Vary: Accept-Encoding
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" != {{anonymous_etag}}
header "ETag" == {{reports_etag}}
[
  {
    "from": "{{base}}/orphan/lonely",
    "to": "{{base}}/public/string",
    "at": "/properties/label/$ref"
  },
  {
    "from": "{{base}}/reports/quarterly",
    "to": "{{base}}/public/string",
    "at": "/items/$ref"
  }
]

# A key for somewhere else reads what an anonymous caller reads, down to the
# validator, since it is placed in the view holding whatever it opens and that
# is nothing here. Holding a credential is not what widens the graph, holding
# the one that opens what points at it is.
#
# It stays private even so. What decides that is the view the answer was built
# for rather than the bytes that came out of it, so an answer that happens to
# match the anonymous one is not offered for reuse on the strength of a
# coincidence
GET {{base}}/self/v1/api/schemas/dependents/public/string
Authorization: Bearer internal-secret-key
HTTP 200
Content-Type: application/json
Cache-Control: private, max-age=0, must-revalidate
Vary: Accept-Encoding
Link: </self/v1/schemas/api/schemas/dependents/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" == {{anonymous_etag}}
[
  {
    "from": "{{base}}/orphan/lonely",
    "to": "{{base}}/public/string",
    "at": "/properties/label/$ref"
  }
]

# And the gated schema's own graph is still withheld outright, so nothing here
# turned a path the caller cannot reach into an empty answer
GET {{base}}/self/v1/api/schemas/dependents/reports/quarterly
HTTP 404
Content-Type: application/problem+json
Cache-Control: no-store
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
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
