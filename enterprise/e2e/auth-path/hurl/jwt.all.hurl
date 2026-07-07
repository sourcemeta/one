# A JWT policy under a base path is matched with the base path stripped
POST http://localhost:8080/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-service
client_secret: ci-service-secret
HTTP 200
Cache-Control: no-store
[Captures]
access_token: jsonpath "$.access_token"
[Asserts]
header "Content-Type" startsWith "application/json"
jsonpath "$.token_type" == "Bearer"
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

# A verified token admits the machine schema, rebased under the base path
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
POST http://localhost:8080/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-wrong
client_secret: ci-wrong-secret
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
[Captures]
wrong_audience_denied: body
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.status" == 401

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{wrong_audience_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The foreign policy expects a different issuer, so a valid-signature token is
# denied on the issuer claim
GET {{base}}/registry/foreign/record.json
Authorization: Bearer {{access_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
issuer_mismatch_denied: body
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.status" == 401

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{issuer_mismatch_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The unreachable policy has no explicit key set, so its issuer's discovery
# endpoint is fetched and fails, denying rather than admitting on the failure
GET {{base}}/registry/unreachable/record.json
Authorization: Bearer {{access_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </registry/self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
discovery_failure_denied: body
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.status" == 401

POST {{base}}/registry/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{discovery_failure_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
