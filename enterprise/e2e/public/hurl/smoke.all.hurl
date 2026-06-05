GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(/html/head/title)" == "Sourcemeta Schemas"
header "Access-Control-Allow-Origin" not exists

GET {{base}}/jsonschema/2020-12
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(/html/head/title)" == "/jsonschema/2020-12"
header "Access-Control-Allow-Origin" not exists
