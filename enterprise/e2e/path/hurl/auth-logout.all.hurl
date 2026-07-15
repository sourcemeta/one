# Under a base path, the cookie expirations and the redirect are both scoped
# to the instance rather than the whole host
GET {{base}}/v1/catalog/self/v1/auth/logout
Cookie: sourcemeta_one_session_okta=abc; theme=dark; sourcemeta_one_transaction_okta=xyz
HTTP 303
Location: /v1/catalog
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 2
header "Set-Cookie" contains "sourcemeta_one_session_okta=; Path=/v1/catalog; Max-Age=0; HttpOnly; SameSite=Lax"
header "Set-Cookie" contains "sourcemeta_one_transaction_okta=; Path=/v1/catalog; Max-Age=0; HttpOnly; SameSite=Lax"
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

GET {{base}}/v1/catalog/self/v1/auth/logout
HTTP 303
Location: /v1/catalog
Cache-Control: no-store
[Asserts]
header "Set-Cookie" not exists
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

# The endpoint does not exist outside the base path
GET {{base}}/self/v1/auth/logout
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
