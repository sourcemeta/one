# Unsafe HTML in schema description is filtered
GET {{base}}/test/doc/unsafe
Accept: text/html
HTTP 200
Content-Type: text/html
[Asserts]
xpath "count(//div[contains(@class, 'text-secondary')]//script)" == 0
xpath "count(//div[contains(@class, 'text-secondary')]//iframe)" == 0

# Unsafe HTML in alert is filtered
GET {{base}}/test/unsafe-alert/example
Accept: text/html
HTTP 200
Content-Type: text/html
[Asserts]
xpath "count(//div[contains(@class, 'alert')]//script)" == 0
xpath "count(//div[contains(@class, 'alert')]//iframe)" == 0

# API still returns raw text for unsafe description
GET {{base}}/self/v1/api/schemas/metadata/test/doc/unsafe
Accept: application/json
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.description" contains "<script>"
jsonpath "$.description" contains "<iframe"
