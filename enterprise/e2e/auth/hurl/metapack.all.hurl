# Every per-schema API is gated on the schema it describes, not on the public
# /self URL it is reached through, so wrapping a private schema in a /self API
# path does not bypass the gate. Each one denies an anonymous caller.
GET {{base}}/self/v1/api/schemas/health/private/secret
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

GET {{base}}/self/v1/api/schemas/dependencies/private/secret
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

GET {{base}}/self/v1/api/schemas/dependents/private/secret
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

GET {{base}}/self/v1/api/schemas/locations/private/secret
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

GET {{base}}/self/v1/api/schemas/positions/private/secret
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

GET {{base}}/self/v1/api/schemas/stats/private/secret
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

GET {{base}}/self/v1/api/schemas/metadata/private/secret
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# The same API for a public schema is anonymous, and validates against its
# output schema
GET {{base}}/self/v1/api/schemas/health/public/string
HTTP 200
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/health/response>; rel="describedby"
[Captures]
public_health: body
public_health_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.score" exists

POST {{base}}/self/v1/api/schemas/evaluate{{public_health_schema}}
```
{{public_health}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# With the right credential the apiKey schema's health is served and validates
GET {{base}}/self/v1/api/schemas/health/private/secret
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/health/response>; rel="describedby"
[Captures]
private_health: body
private_health_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.score" exists

POST {{base}}/self/v1/api/schemas/evaluate{{private_health_schema}}
```
{{private_health}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Locations of the apiKey schema also require the credential, then validate
GET {{base}}/self/v1/api/schemas/locations/private/secret
Authorization: Bearer primary-secret-key
HTTP 200
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/locations/response>; rel="describedby"
[Captures]
private_locations: body
private_locations_schema: header "Link" regex "<([^>]+)>"

POST {{base}}/self/v1/api/schemas/evaluate{{private_locations_schema}}
```
{{private_locations}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The internal schema's API is denied to a key from another policy
GET {{base}}/self/v1/api/schemas/health/internal/config
Authorization: Bearer primary-secret-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# Evaluation of an apiKey schema is gated: anonymous evaluation is denied
POST {{base}}/self/v1/api/schemas/evaluate/private/secret
```
{ "token": "value" }
```
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# With the credential, evaluation runs against the apiKey schema
POST {{base}}/self/v1/api/schemas/evaluate/private/secret
Authorization: Bearer primary-secret-key
```
{ "token": "value" }
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate/private/secret
Authorization: Bearer primary-secret-key
```
{ "missing": "token" }
```
HTTP 200
[Asserts]
jsonpath "$.valid" == false

# Tracing an apiKey schema is gated as well
POST {{base}}/self/v1/api/schemas/trace/private/secret
```
{ "token": "value" }
```
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# With the credential, tracing runs and validates against its output schema
POST {{base}}/self/v1/api/schemas/trace/private/secret
Authorization: Bearer primary-secret-key
```
{ "token": "value" }
```
HTTP 200
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/trace/response>; rel="describedby"
[Captures]
private_trace: body
private_trace_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.valid" == true

POST {{base}}/self/v1/api/schemas/evaluate{{private_trace_schema}}
```
{{private_trace}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
