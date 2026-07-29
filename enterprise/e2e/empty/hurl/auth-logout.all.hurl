# Both cookies are expired whether or not the request carried them, since a
# cookie can be withheld on a navigation while the browser still holds it, and
# clearing only what arrived would leave a session behind
POST {{base}}/self/v1/auth/logout
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 2
header "Set-Cookie" contains "sourcemeta_one_session=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Set-Cookie" contains "sourcemeta_one_transaction=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

# Cookies this instance did not mint are left alone
POST {{base}}/self/v1/auth/logout
Cookie: sourcemeta_one_session=abc; theme=dark; sourcemeta_one_transaction=xyz; sourcemeta_one_bogus=keep
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 2
header "Set-Cookie" contains "sourcemeta_one_session=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Set-Cookie" contains "sourcemeta_one_transaction=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

# Several cookies under the one name are replaced by the single expiry, since
# the browser is told to drop that name rather than each value it holds
POST {{base}}/self/v1/auth/logout
Cookie: sourcemeta_one_session=abc; sourcemeta_one_session=def
HTTP 303
Location: /
Cache-Control: no-store
[Asserts]
header "Set-Cookie" count == 2
header "Set-Cookie" contains "sourcemeta_one_session=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Set-Cookie" contains "sourcemeta_one_transaction=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax"
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

# Signing out is not a safe method, so a navigation cannot reach it. Anything
# that follows or warms a link would otherwise sign somebody out by looking
GET {{base}}/self/v1/auth/logout
Cookie: sourcemeta_one_session=abc
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Allow: POST, OPTIONS
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
refused_body: body
refused_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
{
  "type": "urn:sourcemeta:one:method-not-allowed",
  "title": "Method Not Allowed",
  "status": 405,
  "detail": "This HTTP method is invalid for this URL"
}

POST {{base}}/self/v1/api/schemas/evaluate{{refused_schema}}
```
{{refused_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

HEAD {{base}}/self/v1/auth/logout
Cookie: sourcemeta_one_session=abc
HTTP 405
Cache-Control: no-store
Allow: POST, OPTIONS
[Asserts]
header "Set-Cookie" not exists
bytes count == 0
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

OPTIONS {{base}}/self/v1/auth/logout
HTTP 204
Cache-Control: no-store
Allow: POST, OPTIONS
[Asserts]
header "Set-Cookie" not exists
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Access-Control-Allow-Origin" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
bytes count == 0

PUT {{base}}/self/v1/auth/logout
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Allow: POST, OPTIONS
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
