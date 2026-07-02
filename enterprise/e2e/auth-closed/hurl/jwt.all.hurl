# The registry is closed in its entirety by two policies at the root, an apiKey
# and a JWT. A verified token opens it just as the key does, proving the two
# credential types are unioned rather than one shadowing the other.
POST http://localhost:4444/oauth2/token
[FormParams]
grant_type: client_credentials
client_id: ci-service
client_secret: ci-service-secret
audience: https://schemas.example.com
HTTP 200
Cache-Control: no-store
[Captures]
access_token: jsonpath "$.access_token"
[Asserts]
header "Content-Type" startsWith "application/json"
jsonpath "$.token_type" == "bearer"
jsonpath "$.access_token" matches /^[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+$/

# A schema that denies anonymously is served with a token, marked private
GET {{base}}/catalog/example.json
Authorization: Bearer {{access_token}}
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Vary: User-Agent, Accept-Encoding
[Asserts]
header "ETag" matches /^"[0-9a-f]{64}"$/
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/catalog/example",
  "type": "string"
}

# The listing is enumerable with a token, and every node reports both governing
# policy names in canonical order
GET {{base}}/self/v1/api/list/catalog
Authorization: Bearer {{access_token}}
HTTP 200
Content-Type: application/json
Cache-Control: private, max-age=0, must-revalidate
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
listing_body: body
listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.entries" count == 3
jsonpath "$.policies" count == 2
jsonpath "$.policies[0]" == "machine"
jsonpath "$.policies[1]" == "registry"
jsonpath "$.entries[0].policies[0]" == "machine"
jsonpath "$.entries[0].policies[1]" == "registry"

POST {{base}}/self/v1/api/schemas/evaluate{{listing_schema}}
Authorization: Bearer {{access_token}}
```
{{listing_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A token minted for a different audience opens nothing
POST http://localhost:4444/oauth2/token
[FormParams]
grant_type: client_credentials
client_id: ci-wrong
client_secret: ci-wrong-secret
audience: https://other.example.com
HTTP 200
[Captures]
wrong_audience_token: jsonpath "$.access_token"

GET {{base}}/catalog/example.json
Authorization: Bearer {{wrong_audience_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# A tampered token fails verification against the issuer's key set and is denied
GET {{base}}/catalog/example.json
Authorization: Bearer {{access_token}}tampered
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}
