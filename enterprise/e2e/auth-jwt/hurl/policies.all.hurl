# Three clients of the same issuer, each scoped to a different audience: one the
# `internal` and `strict` policies expect, one the `reports` policy expects, and
# one no policy accepts
POST http://localhost:4444/oauth2/token
[FormParams]
grant_type: client_credentials
client_id: ci-service
client_secret: ci-service-secret
audience: https://schemas.example.com
HTTP 200
[Captures]
schemas_token: jsonpath "$.access_token"
[Asserts]
jsonpath "$.token_type" == "bearer"
jsonpath "$.access_token" matches /^[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+$/

POST http://localhost:4444/oauth2/token
[FormParams]
grant_type: client_credentials
client_id: ci-reports
client_secret: ci-reports-secret
audience: https://reports.example.com
HTTP 200
[Captures]
reports_token: jsonpath "$.access_token"
[Asserts]
jsonpath "$.token_type" == "bearer"
jsonpath "$.access_token" matches /^[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+$/

# The audience token for the internal policy admits the internal schema, whose
# key set the registry discovers from the issuer's metadata
GET {{base}}/internal/config.json
Authorization: Bearer {{schemas_token}}
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
  "$id": "{{base}}/internal/config",
  "type": "object",
  "required": [ "token" ],
  "properties": {
    "token": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# The same token is rejected on the reports policy, whose audience differs, so a
# credential minted for one audience never crosses to another under one issuer
GET {{base}}/reports/summary.json
Authorization: Bearer {{schemas_token}}
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

# The reports audience token admits the reports schema, verified against the key
# set the policy points at explicitly rather than through discovery
GET {{base}}/reports/summary.json
Authorization: Bearer {{reports_token}}
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
  "$id": "{{base}}/reports/summary",
  "type": "object",
  "required": [ "quarter", "total" ],
  "properties": {
    "quarter": {
      "type": "string"
    },
    "total": {
      "type": "number"
    }
  },
  "additionalProperties": false
}

# The reports token does not cross to the internal policy either
GET {{base}}/internal/config.json
Authorization: Bearer {{reports_token}}
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

# The strict policy shares the internal policy's audience but only allows a
# signature algorithm the issuer does not use, so a token that opens the
# internal schema is rejected here purely on the algorithm allow list
GET {{base}}/strict/rule.json
Authorization: Bearer {{schemas_token}}
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

# The legacy policy is an apiKey policy alongside the JWT ones, and its key
# admits its schema
GET {{base}}/legacy/record.json
Authorization: Bearer legacy-secret-key
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
  "$id": "{{base}}/legacy/record",
  "type": "object",
  "required": [ "identifier" ],
  "properties": {
    "identifier": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# A JWT does not satisfy an apiKey policy
GET {{base}}/legacy/record.json
Authorization: Bearer {{schemas_token}}
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

# And an apiKey does not satisfy a JWT policy
GET {{base}}/internal/config.json
Authorization: Bearer legacy-secret-key
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# Every denial across the mixed policy set validates against the same error
# schema it advertises
POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.valid" == true
