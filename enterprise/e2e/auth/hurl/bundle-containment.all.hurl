# A bundle can never carry content the schema's own path does not admit.
#
# Bundling inlines whatever a schema references, with no containment check of
# its own, so the property holds only because a reference to something behind a
# stricter policy is refused when the instance is built. This file pins the
# consequence rather than the rule, and pins it in both directions, because a
# containment claim proves nothing without evidence that bundling would carry
# the content across if it were allowed to.

# The evidence: a governed schema references a public one, which is permitted,
# and its bundle really does inline that content. So bundling does cross a
# policy boundary whenever the boundary allows it, and containment is a fact
# about which references exist rather than about what bundling declines to do
GET {{base}}/reports/quarterly.json?bundle=1
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/reports/quarterly",
  "type": "array",
  "items": {
    "$ref": "../public/string"
  },
  "$defs": {
    "{{base}}/public/string": {
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "{{base}}/public/string",
      "type": "string"
    }
  }
}

# The inlined content travels under the referring schema's own gate, so the
# bundle is absent for a caller the referring schema is absent for, whatever
# the inlined part would have answered on its own
GET {{base}}/reports/quarterly.json?bundle=1
HTTP 404
Content-Type: application/problem+json

# The containment itself: the public schema is referenced by a governed one, and
# its own bundle names nothing of the governed schema. The reverse edge is a
# fact about the graph and never becomes content here
GET {{base}}/public/string.json?bundle=1
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/public/string",
  "type": "string"
}

# The same answer for a caller who holds the credential for the governed side,
# so the public bundle is one stored answer rather than something that grows
# when a privileged caller asks for it. It stays publicly cacheable too, since
# presenting a credential for somewhere else does not make a public answer
# specific to whoever asked
GET {{base}}/public/string.json?bundle=1
Authorization: Bearer primary-secret-key
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/public/string",
  "type": "string"
}
