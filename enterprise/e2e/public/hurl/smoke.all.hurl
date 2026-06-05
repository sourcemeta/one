GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "Sourcemeta Schemas"
header "Access-Control-Allow-Origin" not exists

GET {{base}}/jsonschema/2020-12
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "/jsonschema/2020-12"
header "Access-Control-Allow-Origin" not exists
