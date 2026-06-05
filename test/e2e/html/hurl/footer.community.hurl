GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "boolean(//footer//small[contains(., 'Community')])" == true
