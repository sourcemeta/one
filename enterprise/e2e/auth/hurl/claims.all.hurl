# A client the provider stamps with the group and the grant the policy names
POST https://keycloak:8443/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-platform
client_secret: ci-platform-secret
HTTP 200
Cache-Control: no-store
[Captures]
platform_token: jsonpath "$.access_token"
[Asserts]
header "Content-Type" startsWith "application/json"
jsonpath "$.token_type" == "Bearer"
jsonpath "$.expires_in" > 0
jsonpath "$.access_token" matches /^[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+$/

# A client of the same issuer and audience, carrying neither
POST https://keycloak:8443/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-outsider
client_secret: ci-outsider-secret
HTTP 200
Cache-Control: no-store
[Captures]
outsider_token: jsonpath "$.access_token"
[Asserts]
jsonpath "$.token_type" == "Bearer"

# A client carrying no such claims at all
POST https://keycloak:8443/realms/main/protocol/openid-connect/token
[FormParams]
grant_type: client_credentials
client_id: ci-claimless
client_secret: ci-claimless-secret
HTTP 200
Cache-Control: no-store
[Captures]
claimless_token: jsonpath "$.access_token"
[Asserts]
jsonpath "$.token_type" == "Bearer"

# The gated schema is denied without any credential at all
GET {{base}}/scoped/report.json
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag, WWW-Authenticate
[Captures]
anonymous_denied: body
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{anonymous_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A token carrying every claim the policy names is admitted, and the response
# is marked private exactly as any other gated one is
GET {{base}}/scoped/report.json
Authorization: Bearer {{platform_token}}
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Vary: User-Agent, Accept-Encoding
[Asserts]
header "ETag" matches /^"[0-9a-f]{64}"$/
header "Last-Modified" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/scoped/report",
  "type": "object",
  "required": [ "group" ],
  "properties": {
    "group": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# A token the same provider signed for the same audience, whose group is not
# one the policy names and whose grant merely contains the required one as a
# prefix, is refused. Verifying is not admitting
GET {{base}}/scoped/report.json
Authorization: Bearer {{outsider_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag, WWW-Authenticate
[Captures]
outsider_denied: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{outsider_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A token carrying neither claim is refused for the same reason
GET {{base}}/scoped/report.json
Authorization: Bearer {{claimless_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag, WWW-Authenticate
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The same token reaches a path whose policy names no claim, so the rules
# narrow the one policy that declares them rather than the credential itself
GET {{base}}/machine/config.json
Authorization: Bearer {{claimless_token}}
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Vary: User-Agent, Accept-Encoding
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

# An admitted caller reaches the directory listing of the gated collection too
GET {{base}}/self/v1/api/list/scoped
Authorization: Bearer {{platform_token}}
HTTP 200
Content-Type: application/json
Cache-Control: private, max-age=0, must-revalidate
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
listing: body
listing_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.schemas" == 1
jsonpath "$.entries" count == 1
jsonpath "$.policies" count == 1
jsonpath "$.policies[0].name" == "scoped"
jsonpath "$.policies[0].type" == "jwt"
jsonpath "$.entries[0].name" == "report"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].path" == "/scoped/report"
jsonpath "$.entries[0].identifier" == "{{base}}/scoped/report"
jsonpath "$.entries[0].policies" count == 1
jsonpath "$.entries[0].policies[0].name" == "scoped"

POST {{base}}/self/v1/api/schemas/evaluate{{listing_schema}}
```
{{listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A refused caller learns nothing about the collection either
GET {{base}}/self/v1/api/list/scoped
Authorization: Bearer {{outsider_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag, WWW-Authenticate
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}
