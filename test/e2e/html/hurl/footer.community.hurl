GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html
[Asserts]
xpath "boolean(//footer//small[contains(., 'Community')])" == true
