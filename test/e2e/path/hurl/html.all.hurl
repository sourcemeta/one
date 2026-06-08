GET {{base}}/v1/catalog
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "Sourcemeta Schemas"
xpath "boolean(//h6[contains(text(), 'Special directories')])" == true
xpath "boolean(//h6[contains(text(), 'Special directories')]/following-sibling::table[1]//a[@href='/v1/catalog/self/'])" == true
xpath "count(//h6[contains(text(), 'Special directories')]/preceding-sibling::table//a[@href='/v1/catalog/self/'])" == 0
