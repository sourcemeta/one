GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "boolean(//footer//small[contains(., 'Community')])" == true
