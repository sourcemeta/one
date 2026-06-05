GET {{base}}/test/doc/markdown
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
xpath "boolean(//div[contains(@class, 'text-secondary')]//strong[text()='bold'])" == true
xpath "boolean(//div[contains(@class, 'text-secondary')]//em[text()='italic'])" == true
xpath "boolean(//div[contains(@class, 'text-secondary')]//a[@href='https://example.com'][text()='link'])" == true

# Schema title does NOT render markdown (must stay plain text)
GET {{base}}/test/doc/markdown
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
xpath "string(//h2[contains(@class, 'fw-bold')])" == "A **bold** title that must stay plain"
xpath "boolean(//h2[contains(@class, 'fw-bold')]//strong)" == false

GET {{base}}/test/doc
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
xpath "boolean(//div[contains(@class, 'text-secondary')]//strong[text()='testing'])" == true
xpath "boolean(//div[contains(@class, 'text-secondary')]//a[@href='https://example.com'][text()='link'])" == true

GET {{base}}/test/doc
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
xpath "boolean(//td[contains(@class, 'small')]//strong[text()='bold'])" == true
xpath "boolean(//td[contains(@class, 'small')]//a[@href='https://example.com'])" == true

# File manager table title column does NOT render markdown
GET {{base}}/test/doc
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
xpath "boolean(//td//small[contains(text(), '**bold**')])" == true
xpath "boolean(//td//small//strong)" == false

GET {{base}}/test/markdown-alert/example
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
xpath "boolean(//div[contains(@class, 'alert')]//strong[text()='important'])" == true
xpath "boolean(//div[contains(@class, 'alert')]//a[@href='https://example.com'][text()='docs'])" == true

GET {{base}}/test/v2.0/schema
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
[Asserts]
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
xpath "count(//div[@data-sourcemeta-ui-tab-id='health']//div[contains(@class, 'mb-0')]//p)" > 0

# API returns raw markdown text for description, not HTML
GET {{base}}/self/v1/api/schemas/metadata/test/doc/markdown
Accept: application/json
HTTP 200
Content-Type: application/json
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.description" == "A string with **bold**, *italic*, and a [link](https://example.com)"
jsonpath "$.title" == "A **bold** title that must stay plain"
