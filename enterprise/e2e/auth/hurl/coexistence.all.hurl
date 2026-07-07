# Two clients of the same issuer, each scoped to a different audience
POST http://localhost:8080/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-service
client_secret: ci-service-secret
HTTP 200
[Captures]
schemas_token: jsonpath "$.access_token"
[Asserts]
jsonpath "$.token_type" == "Bearer"
jsonpath "$.access_token" matches /^[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+$/

POST http://localhost:8080/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-reports
client_secret: ci-reports-secret
HTTP 200
[Captures]
reports_token: jsonpath "$.access_token"
[Asserts]
jsonpath "$.token_type" == "Bearer"
jsonpath "$.access_token" matches /^[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+$/

# The machine policy discovers its key set and admits its audience's token
GET {{base}}/machine/config.json
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
  "$id": "{{base}}/machine/config",
  "type": "object",
  "required": [ "token" ],
  "properties": {
    "token": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# The same token does not cross to the telemetry policy's audience
GET {{base}}/telemetry/summary.json
Authorization: Bearer {{schemas_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
machine_on_telemetry: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{machine_on_telemetry}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The telemetry policy points at its key set explicitly and admits its token
GET {{base}}/telemetry/summary.json
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
  "$id": "{{base}}/telemetry/summary",
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

# The reports token does not cross to the machine policy either
GET {{base}}/machine/config.json
Authorization: Bearer {{reports_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
reports_on_machine: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{reports_on_machine}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The strict policy allows only an algorithm the issuer does not use, so the
# token is denied on the algorithm allow list
GET {{base}}/strict/rule.json
Authorization: Bearer {{schemas_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
strict_denied: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{strict_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An apiKey collection sits alongside the JWT ones, and its key admits its schema
GET {{base}}/internal/config.json
Authorization: Bearer internal-secret-key
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
  "additionalProperties": false
}

# A JWT does not satisfy an apiKey policy
GET {{base}}/internal/config.json
Authorization: Bearer {{schemas_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
jwt_on_apikey: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{jwt_on_apikey}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# And an apiKey does not satisfy a JWT policy
GET {{base}}/machine/config.json
Authorization: Bearer internal-secret-key
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
apikey_on_jwt: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{apikey_on_jwt}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
