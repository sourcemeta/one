# A JWT policy under a non-root base path is matched with the base path stripped,
# just like the apiKey policies. A token opens the machine collection.
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

# The machine schema is denied without a credential, the error Link carrying the
# base path
GET {{base}}/registry/machine/config.json
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
denied_body: body
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

# The denial validates against the error schema, addressed base-path-relative
POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A verified token admits the machine schema, whose identity is rebased onto the
# registry URL including the base path, and the response is marked private
GET {{base}}/registry/machine/config.json
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
  "$id": "{{base}}/registry/machine/config",
  "type": "object",
  "required": [ "token" ],
  "properties": {
    "token": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# A token minted for a different audience does not satisfy the policy
POST http://localhost:4444/oauth2/token
[FormParams]
grant_type: client_credentials
client_id: ci-wrong
client_secret: ci-wrong-secret
audience: https://other.example.com
HTTP 200
[Captures]
wrong_audience_token: jsonpath "$.access_token"

GET {{base}}/registry/machine/config.json
Authorization: Bearer {{wrong_audience_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}
