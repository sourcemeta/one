# The provider issues a signed JWT via the client_credentials grant
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
jsonpath "$.scope" == ""
jsonpath "$.expires_in" > 0
jsonpath "$.access_token" matches /^[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+$/

# A public schema is served to everyone and stays publicly cacheable
GET {{base}}/public/string.json
HTTP 200
Content-Type: application/schema+json
Cache-Control: public, max-age=0, must-revalidate
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
  "$id": "{{base}}/public/string",
  "type": "string"
}

# A token on a public path is ignored, and the response stays publicly cacheable
GET {{base}}/public/string.json
Authorization: Bearer {{access_token}}
HTTP 200
Content-Type: application/schema+json
Cache-Control: public, max-age=0, must-revalidate
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Vary: User-Agent, Accept-Encoding
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/public/string",
  "type": "string"
}

# The machine schema is denied without a credential
GET {{base}}/machine/config.json
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
denied_body: body
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
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A verified token admits the machine schema, marked private
GET {{base}}/machine/config.json
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
header "Last-Modified" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
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

# The bearer scheme is matched case-insensitively per RFC 6750
GET {{base}}/machine/config.json
Authorization: bearer {{access_token}}
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

# A verified token admits a HEAD request too, carrying no body
HEAD {{base}}/machine/config.json
Authorization: Bearer {{access_token}}
HTTP 200
Content-Type: application/schema+json
Cache-Control: private, max-age=0, must-revalidate
Access-Control-Allow-Origin: *
[Asserts]
bytes count == 0

# A bearer value that is not a token is denied
GET {{base}}/machine/config.json
Authorization: Bearer not-a-token
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
bogus_denied: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{bogus_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A credential presented without the bearer scheme is denied
GET {{base}}/machine/config.json
Authorization: {{access_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
no_scheme_denied: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{no_scheme_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A valid token under a non-bearer scheme is denied
GET {{base}}/machine/config.json
Authorization: Basic {{access_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
basic_denied: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{basic_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A tampered signature fails verification and is denied
GET {{base}}/machine/config.json
Authorization: Bearer {{access_token}}tampered
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
tampered_denied: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{tampered_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# An unsigned alg:none token is denied, never admitted on the empty signature
GET {{base}}/machine/config.json
Authorization: Bearer eyJhbGciOiJub25lIiwidHlwIjoiSldUIn0.eyJpc3MiOiJodHRwOi8vaHlkcmE6NDQ0NCIsImF1ZCI6Imh0dHBzOi8vc2NoZW1hcy5leGFtcGxlLmNvbSIsImV4cCI6OTk5OTk5OTk5OSwiaWF0IjoxMDAwMDAwMDAwfQ.
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
none_denied: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{none_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A correctly signed token minted for a different audience is denied
POST http://localhost:4444/oauth2/token
[FormParams]
grant_type: client_credentials
client_id: ci-wrong
client_secret: ci-wrong-secret
audience: https://other.example.com
HTTP 200
[Captures]
wrong_audience_token: jsonpath "$.access_token"

GET {{base}}/machine/config.json
Authorization: Bearer {{wrong_audience_token}}
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
wrong_audience_denied: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{wrong_audience_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A verified token discloses that a resource does not exist, as a not found
GET {{base}}/machine/ghost.json
Authorization: Bearer {{access_token}}
HTTP 404
Content-Type: application/problem+json
Cache-Control: no-store
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
not_found_body: body
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{not_found_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Without a credential the missing path denies identically, so existence is not
# probeable
GET {{base}}/machine/ghost.json
HTTP 401
Content-Type: application/problem+json
Cache-Control: no-store
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
ghost_denied: body
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/api/error
```
{{ghost_denied}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
