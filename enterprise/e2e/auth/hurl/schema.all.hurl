# A public schema is served to everyone, without authentication
GET {{base}}/public/string.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "Vary" == "User-Agent, Accept-Encoding"
header "ETag" exists
header "Last-Modified" exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/public/string",
  "type": "string"
}

# An apiKey schema is denied without a credential
GET {{base}}/private/secret.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The primary key admits the apiKey schema, and the response is marked private
# so that shared caches do not store it
GET {{base}}/private/secret.json
Authorization: Bearer primary-secret-key
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
  "$id": "{{base}}/private/secret",
  "type": "object",
  "required": [ "token" ],
  "properties": {
    "token": {
      "type": "string"
    }
  },
  "additionalProperties": false
}

# Any key listed in the policy admits, so the secondary key works too
GET {{base}}/private/secret.json
Authorization: Bearer secondary-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$['$id']" == "{{base}}/private/secret"

# A wrong key is denied
GET {{base}}/private/secret.json
Authorization: Bearer not-the-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# Keys are scoped to their policy: the internal key does not admit /private
GET {{base}}/private/secret.json
Authorization: Bearer internal-secret-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The same policy governs a second prefix, so the primary key admits /reports
GET {{base}}/reports/quarterly.json
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
    "type": "number"
  }
}

# /reports is denied without a credential
GET {{base}}/reports/quarterly.json
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# The internal policy uses a separate key
GET {{base}}/internal/config.json
Authorization: Bearer internal-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/internal/config",
  "type": "object",
  "additionalProperties": false
}

# A key from another policy does not cross over into /internal
GET {{base}}/internal/config.json
Authorization: Bearer primary-secret-key
HTTP 401
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"

# A schema nested under an apiKey prefix inherits the gate, so it is denied
# without a credential like the rest of the scope
GET {{base}}/private/open/shared.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

# The governing key admits it, marked private so shared caches do not store it
GET {{base}}/private/open/shared.json
Authorization: Bearer primary-secret-key
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
  "$id": "{{base}}/private/open/shared",
  "type": "boolean"
}

# Content governed by no policy is public, so it is served anonymously
GET {{base}}/orphan/lonely.json
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Link: <https://json-schema.org/draft/2020-12/schema>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
header "ETag" exists
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "{{base}}/orphan/lonely",
  "type": "null"
}

# A credential changes nothing for un-policied content: it stays public and
# publicly cacheable rather than being narrowed to the caller
GET {{base}}/orphan/lonely.json
Authorization: Bearer internal-secret-key
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/schema+json
Access-Control-Allow-Origin: *
[Asserts]
jsonpath "$['$id']" == "{{base}}/orphan/lonely"
jsonpath "$.type" == "null"
