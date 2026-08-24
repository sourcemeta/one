# A scrape reports on the requests that came before it, so this one gives it
# something to have counted
GET {{base}}/self/v1/health
HTTP 200

GET {{base}}/self/v1/metrics
HTTP 200
Cache-Control: no-store
Content-Type: text/plain; version=0.0.4; charset=utf-8
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/
body matches /# TYPE sourcemeta_one_build_info gauge\nsourcemeta_one_build_info\{version="[^"]+",edition="enterprise"\} 1\n/
body matches /# TYPE process_start_time_seconds gauge\nprocess_start_time_seconds [0-9.]+\n/
body matches /# TYPE process_cpu_seconds_total counter\nprocess_cpu_seconds_total [0-9.e-]+\n/
body matches /# TYPE process_resident_memory_bytes gauge\nprocess_resident_memory_bytes [0-9]+\n/
body matches /# TYPE process_virtual_memory_bytes gauge\nprocess_virtual_memory_bytes [0-9]+\n/
body matches /# TYPE process_open_fds gauge\nprocess_open_fds [0-9]+\n/
body matches /# TYPE process_max_fds gauge\nprocess_max_fds [0-9]+\n/
body matches /# TYPE sourcemeta_one_http_requests_in_flight gauge\nsourcemeta_one_http_requests_in_flight [0-9]+\n/
body matches /# TYPE sourcemeta_one_metrics_dropped_total counter\nsourcemeta_one_metrics_dropped_total 0\n/
body matches /# TYPE sourcemeta_one_http_requests_total counter\n/
body matches /sourcemeta_one_http_requests_total\{action="health_check_v1",code="200"\} [0-9]+\n/
body matches /# TYPE sourcemeta_one_http_request_duration_seconds histogram\n/
body matches /sourcemeta_one_http_request_duration_seconds_bucket\{action="health_check_v1",le="0.0001"\} [0-9]+\n/
body matches /sourcemeta_one_http_request_duration_seconds_bucket\{action="health_check_v1",le="1.0"\} [0-9]+\n/
body matches /sourcemeta_one_http_request_duration_seconds_bucket\{action="health_check_v1",le="\+Inf"\} [0-9]+\n/
body matches /sourcemeta_one_http_request_duration_seconds_sum\{action="health_check_v1"\} [0-9.e-]+\n/
body matches /sourcemeta_one_http_request_duration_seconds_count\{action="health_check_v1"\} [0-9]+\n/
body not matches /le="1e-04"/

# The endpoint counts itself like anything else, one scrape behind
GET {{base}}/self/v1/metrics
HTTP 200
[Asserts]
body matches /sourcemeta_one_http_requests_total\{action="metrics_v1",code="200"\} [0-9]+\n/

POST {{base}}/self/v1/metrics
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Allow: GET, HEAD, OPTIONS
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
header "Vary" not exists
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
jsonpath "$.status" == 405
jsonpath "$.type" == "urn:sourcemeta:one:method-not-allowed"
jsonpath "$.title" == "Method Not Allowed"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

OPTIONS {{base}}/self/v1/metrics
HTTP 204
Cache-Control: no-store
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
Access-Control-Allow-Methods: GET, HEAD, OPTIONS
Access-Control-Allow-Headers: Accept, Accept-Encoding
Access-Control-Max-Age: 3600
Allow: GET, HEAD, OPTIONS
