GET {{base}}/v1/catalog/self/v1/health
HTTP 200
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
body == ""
header "Link" not exists

HEAD {{base}}/v1/catalog/self/v1/health
HTTP 200
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
header "Link" not exists

POST {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

PUT {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

DELETE {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

PATCH {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"

OPTIONS {{base}}/v1/catalog/self/v1/health
HTTP 405
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
[Asserts]
header "Referrer-Policy" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-3][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] GMT$/
jsonpath "$.status" == 405
jsonpath "$.title" == "sourcemeta:one/method-not-allowed"
