# A machine token and a person's identity token are both signed by the same
# provider under the same key, and where a policy names an audience that
# matches an interactive client's identifier, everything else about them agrees
# too. RFC 9068 Section 4 has a resource server refuse a token whose `typ` is
# not the access token profile's, which Section 5 says is what keeps the two
# apart. A provider that stamps no type cannot be told apart that way, so a
# policy says which type it requires rather than one being assumed.
#
# These two clients differ in exactly that: same realm, same key, same
# algorithm, same audience, and one stamps the profile's type while the other
# does not. So a refusal here is attributable to the type alone.

# The client that stamps the type gets a token carrying it
POST https://keycloak:8443/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-typed
client_secret: ci-typed-secret
HTTP 200
[Captures]
typed_token: jsonpath "$.access_token"
[Asserts]
jsonpath "$.token_type" == "Bearer"
jsonpath "$.access_token" matches /^[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+$/

# The client that does not stamp it gets one that is otherwise the same
POST https://keycloak:8443/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-untyped
client_secret: ci-untyped-secret
HTTP 200
[Captures]
untyped_token: jsonpath "$.access_token"
[Asserts]
jsonpath "$.token_type" == "Bearer"

# The policy requires the profile's type, so the token carrying it is admitted
GET {{base}}/typed/thing.json
Authorization: Bearer {{typed_token}}
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/typed/thing",
  "type": "boolean"
}

# And the one without it is refused, though it is valid in every other respect
GET {{base}}/typed/thing.json
Authorization: Bearer {{untyped_token}}
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
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
[Asserts]
jsonpath "$.valid" == true

# Carrying nothing is refused identically, so the untyped token discloses
# nothing by being refused differently
GET {{base}}/typed/thing.json
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
body == {{denied_body}}

# The gated listing is not enumerable without the right token either
GET {{base}}/self/v1/api/list/typed
Authorization: Bearer {{untyped_token}}
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Asserts]
body == {{denied_body}}

# The token that carries the type enumerates it, and the listing marks what
# the policy governs as private
GET {{base}}/self/v1/api/list/typed
Authorization: Bearer {{typed_token}}
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
listing_body: body
listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.path" == "/typed"
jsonpath "$.url" == "{{base}}/typed"
jsonpath "$.schemas" == 1
jsonpath "$.breadcrumb" count == 1
jsonpath "$.breadcrumb[0].name" == "typed"
jsonpath "$.breadcrumb[0].path" == "/typed/"
jsonpath "$.private" == true
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "thing"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/typed/thing"
jsonpath "$.entries[0].identifier" == "{{base}}/typed/thing"
jsonpath "$.entries[0].private" == true

POST {{base}}/self/v1/api/schemas/evaluate{{listing_schema}}
```
{{listing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A policy that requires no type is unaffected, so a provider that stamps none
# keeps working
GET {{base}}/machine/config.json
Authorization: Bearer {{untyped_token}}
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
[Asserts]
body == {{denied_body}}
