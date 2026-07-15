# A logout with no cookies is a no-op that still redirects to the root
GET {{base}}/self/v1/auth/logout
HTTP 303
Location: /
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

# Session and login transaction cookies are expired, every other cookie is
# left alone
GET {{base}}/self/v1/auth/logout
Cookie: sourcemeta_one_session_okta=abc; theme=dark; sourcemeta_one_transaction_okta=xyz; sourcemeta_one_bogus=keep
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 2
header "Set-Cookie" contains "sourcemeta_one_session_okta=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Set-Cookie" contains "sourcemeta_one_transaction_okta=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

# Sessions against several policies are all cleared at once
GET {{base}}/self/v1/auth/logout
Cookie: sourcemeta_one_session_okta=abc; sourcemeta_one_session_google=def
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 2
header "Set-Cookie" contains "sourcemeta_one_session_okta=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Set-Cookie" contains "sourcemeta_one_session_google=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

# A HEAD logout answers the same header fields a GET would, including the
# cookie expirations
HEAD {{base}}/self/v1/auth/logout
Cookie: sourcemeta_one_session_okta=abc; theme=dark
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
header "Set-Cookie" == "sourcemeta_one_session_okta=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

HEAD {{base}}/self/v1/auth/logout
HTTP 303
Location: /
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

OPTIONS {{base}}/self/v1/auth/logout
HTTP 204
Cache-Control: no-store
Allow: GET, HEAD, OPTIONS
[Asserts]
header "Set-Cookie" not exists
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

POST {{base}}/self/v1/auth/logout
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Allow: GET, HEAD, OPTIONS
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
error_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "type": "urn:sourcemeta:one:method-not-allowed",
  "title": "Method Not Allowed",
  "status": 405,
  "detail": "This HTTP method is invalid for this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{error_schema}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.valid" == true
