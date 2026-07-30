# Under a base path, the cookie expirations and the redirect are both scoped
# to the instance rather than the whole host
POST {{base}}/v1/catalog/self/v1/auth/logout
Cookie: sourcemeta_one_session=abc; theme=dark; sourcemeta_one_transaction=xyz
HTTP 303
Location: /v1/catalog
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 3
header "Set-Cookie" contains "sourcemeta_one_session=; Path=/v1/catalog; Max-Age=0; HttpOnly; SameSite=Lax"
header "Set-Cookie" contains "sourcemeta_one_transaction=; Path=/v1/catalog; Max-Age=0; HttpOnly; SameSite=Lax"
header "Set-Cookie" contains "sourcemeta_one_renewal=; Path=/v1/catalog; Max-Age=0; HttpOnly; SameSite=Lax"
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

POST {{base}}/v1/catalog/self/v1/auth/logout
HTTP 303
Location: /v1/catalog
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 3
cookie "sourcemeta_one_session[Max-Age]" == 0
cookie "sourcemeta_one_session[Path]" == "/v1/catalog"
cookie "sourcemeta_one_transaction[Max-Age]" == 0
cookie "sourcemeta_one_transaction[Path]" == "/v1/catalog"
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

# The endpoint does not exist outside the base path
POST {{base}}/self/v1/auth/logout
HTTP 404
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Link: </v1/catalog/self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
error_schema: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "type": "urn:sourcemeta:one:not-found",
  "title": "Not Found",
  "status": 404,
  "detail": "There is nothing at this URL"
}

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{error_schema}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </v1/catalog/self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true

# Signing out is not a safe method, so a navigation cannot reach it. Anything
# that follows or warms a link would otherwise sign somebody out by looking
GET {{base}}/v1/catalog/self/v1/auth/logout
Cookie: sourcemeta_one_session=abc
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Allow: POST, OPTIONS
Link: </v1/catalog/self/v1/schemas/api/error>; rel="describedby"
[Captures]
refused_body: body
refused_schema: header "Link" regex "</v1/catalog([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "type": "urn:sourcemeta:one:method-not-allowed",
  "title": "Method Not Allowed",
  "status": 405,
  "detail": "This HTTP method is invalid for this URL"
}

POST {{base}}/v1/catalog/self/v1/api/schemas/evaluate{{refused_schema}}
```
{{refused_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
