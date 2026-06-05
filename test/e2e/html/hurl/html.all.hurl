OPTIONS {{base}}
HTTP 405
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "Sourcemeta Schemas"
xpath "count(//script[not(contains(@src, '?v='))])" == 0
xpath "count(//link[@rel='stylesheet' and not(contains(@href, '?v='))])" == 0
xpath "boolean(//h6[contains(text(), 'Special directories')])" == true
xpath "boolean(//h6[contains(text(), 'Special directories')]/following-sibling::table[1]//a[@href='/self/'])" == true
xpath "count(//h6[contains(text(), 'Special directories')]/preceding-sibling::table//a[@href='/self/'])" == 0
header "Access-Control-Allow-Origin" not exists

# This is a Safari Accept header
GET {{base}}/test/doc
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "A sample schema folder"
xpath "count(//script[not(contains(@src, '?v='))])" == 0
xpath "count(//link[@rel='stylesheet' and not(contains(@href, '?v='))])" == 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/doc
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "A sample schema folder"
xpath "count(//script[not(contains(@src, '?v='))])" == 0
xpath "count(//link[@rel='stylesheet' and not(contains(@href, '?v='))])" == 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}
HTTP 404
Content-Type: application/problem+json
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
GET {{base}}/test/doc
HTTP 404
Content-Type: application/problem+json
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

GET {{base}}/test/doc.json
Accept: text/html
HTTP 404
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 404
jsonpath "$.title" == "sourcemeta:one/not-found"
jsonpath "$.detail" == "There is nothing at this URL"

GET {{base}}/test/doc/string-1
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "Non-blank string"
xpath "count(//script[not(contains(@src, '?v='))])" == 0
xpath "count(//link[@rel='stylesheet' and not(contains(@href, '?v='))])" == 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/doc/string-1.html
Accept: text/html
HTTP 404
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//script[not(contains(@src, '?v='))])" == 0
xpath "count(//link[@rel='stylesheet' and not(contains(@href, '?v='))])" == 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/v2.0
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "/test/v2.0"
xpath "count(//script[not(contains(@src, '?v='))])" == 0
xpath "count(//link[@rel='stylesheet' and not(contains(@href, '?v='))])" == 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/xxxxxxx
Accept: text/html
HTTP 404
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//script[not(contains(@src, '?v='))])" == 0
xpath "count(//link[@rel='stylesheet' and not(contains(@href, '?v='))])" == 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/v2.0?locations=1
Accept: text/html
HTTP 200
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/v2.0?health=1
Accept: text/html

HTTP 200
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/v2.0?positions=1
Accept: text/html
HTTP 200
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/v2.0?positions=1
Accept: text/html
HTTP 200
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/schemas/%25
Accept: text/html
HTTP 200
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/schemas/%25/test
Accept: text/html
HTTP 200
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "count(//script[not(contains(@src, '?v='))])" == 0
xpath "count(//link[@rel='stylesheet' and not(contains(@href, '?v='))])" == 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/foobar/sourcemeta:one/not-found
Accept: text/html
HTTP 404
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "count(//script[not(contains(@src, '?v='))])" == 0
xpath "count(//link[@rel='stylesheet' and not(contains(@href, '?v='))])" == 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/schemas/clash
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "/test/schemas/clash"
xpath "count(//table/tbody/tr)" == 2
xpath "string(//table/tbody/tr[1]/td[2]//a)" == "foo"
xpath "string(//table/tbody/tr[1]/td[2]//a/@href)" == "/test/schemas/clash/foo/"
xpath "string(//table/tbody/tr[2]/td[2]//a)" == "foo"
xpath "string(//table/tbody/tr[2]/td[2]//a/@href)" == "/test/schemas/clash/foo"
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/schemas/clash/foo
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "/test/schemas/clash/foo"
xpath "count(//code)" > 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/test/schemas/clash/foo/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(/html/head/title)" == "/test/schemas/clash/foo"
xpath "count(//code)" == 0
xpath "count(//table/tbody/tr)" == 1
xpath "string(//table/tbody/tr[1]/td[2]//a)" == "bar"
header "Access-Control-Allow-Origin" not exists

# Test that the v= query parameter is consistent across different pages to
# ensure the cache stamp is generated once at build/startup time, not per request
GET {{base}}
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Captures]
page1_script_v: xpath "substring-after(//script/@src, '?v=')"
page1_style_v: xpath "substring-after(//link[@rel='stylesheet']/@href, '?v=')"
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
GET {{base}}/test/doc
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Captures]
page2_script_v: xpath "substring-after(//script/@src, '?v=')"
page2_style_v: xpath "substring-after(//link[@rel='stylesheet']/@href, '?v=')"
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
variable "page1_script_v" == "{{page2_script_v}}"
variable "page1_style_v" == "{{page2_style_v}}"
