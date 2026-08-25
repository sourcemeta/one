# A policy on /self/v1/metrics gates the scrape surface at dispatch: denied
# without a credential, with the canonical 401 shape
GET {{base}}/self/v1/metrics
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag, WWW-Authenticate
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.title" == "Unauthorized"
jsonpath "$.status" == 401
jsonpath "$.detail" == "This resource requires authentication"

# The problem document validates against the error schema named by its Link
POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# A key that opens another surface does not open this one
GET {{base}}/self/v1/metrics
Authorization: Bearer surface-secret-key
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
crossover_body: body
crossover_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.status" == 401

POST {{base}}/self/v1/api/schemas/evaluate{{crossover_schema}}
```
{{crossover_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The metrics key admits, and the exposition answers
GET {{base}}/self/v1/metrics
Authorization: Bearer metrics-secret-key
HTTP 200
Cache-Control: no-store
Content-Type: text/plain; version=0.0.4; charset=utf-8
[Asserts]
header "Vary" not exists
header "Link" not exists
body matches /# TYPE sourcemeta_one_build_info gauge\nsourcemeta_one_build_info\{version="[^"]+",edition="enterprise"\} 1\n/
body matches /# TYPE process_start_time_seconds gauge\nprocess_start_time_seconds [0-9.]+\n/
body matches /# TYPE process_resident_memory_bytes gauge\nprocess_resident_memory_bytes [0-9]+\n/
body matches /# TYPE process_open_fds gauge\nprocess_open_fds [0-9]+\n/
body matches /# TYPE sourcemeta_one_http_requests_in_flight gauge\n/
body matches /# TYPE sourcemeta_one_metrics_dropped_total counter\nsourcemeta_one_metrics_dropped_total 0\n/
body matches /# TYPE sourcemeta_one_http_requests_total counter\n/
body matches /sourcemeta_one_http_requests_total\{action="metrics_v1",code="401"\} [0-9]+\n/
body not matches /le="1e-04"/

# A CORS preflight carries no credentials and is never gated
OPTIONS {{base}}/self/v1/metrics
Origin: http://example.com
Access-Control-Request-Method: GET
HTTP 204
Cache-Control: no-store
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, HEAD, OPTIONS
Access-Control-Allow-Headers: Accept, Accept-Encoding
Access-Control-Max-Age: 3600
Allow: GET, HEAD, OPTIONS

# The gate precedes method validation: an unsupported method denies without the
# key rather than revealing the method is wrong
POST {{base}}/self/v1/metrics
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
method_denied_body: body
method_denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.status" == 401

POST {{base}}/self/v1/api/schemas/evaluate{{method_denied_schema}}
```
{{method_denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# With the key the surface admits and the method check then answers 405
POST {{base}}/self/v1/metrics
Authorization: Bearer metrics-secret-key
HTTP 405
Cache-Control: no-store
Content-Type: application/problem+json
Allow: GET, HEAD, OPTIONS
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
method_allowed_body: body
method_allowed_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:method-not-allowed"
jsonpath "$.title" == "Method Not Allowed"
jsonpath "$.status" == 405

POST {{base}}/self/v1/api/schemas/evaluate{{method_allowed_schema}}
```
{{method_allowed_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Whether a scrape succeeded is the scraper's own verdict on what it read, and
# it is zero for a target it could not reach, could not authenticate against,
# or could not parse. Nothing else here proves as much in a single number
GET http://localhost:9099/api/v1/query
[Options]
retry: 30
retry-interval: 1000
[QueryStringParams]
query: up{job="sourcemeta-one"}
HTTP 200
[Asserts]
jsonpath "$.status" == "success"
jsonpath "$.data.resultType" == "vector"
jsonpath "$.data.result" count == 1
jsonpath "$.data.result[0].value[1]" == "1"

# The scraper reports on its own target, which says the gate admitted it rather
# than merely that something was listening
GET http://localhost:9099/api/v1/targets
[Options]
retry: 30
retry-interval: 1000
[QueryStringParams]
state: active
HTTP 200
[Asserts]
jsonpath "$.status" == "success"
jsonpath "$.data.activeTargets" count == 1
jsonpath "$.data.activeTargets[0].health" == "up"
jsonpath "$.data.activeTargets[0].lastError" == ""
jsonpath "$.data.activeTargets[0].scrapePool" == "sourcemeta-one"

# An info metric survives the round trip with its labels intact, which is the
# whole reason it is shaped as a gauge whose value says nothing
GET http://localhost:9099/api/v1/query
[Options]
retry: 30
retry-interval: 1000
[QueryStringParams]
query: sourcemeta_one_build_info
HTTP 200
[Asserts]
jsonpath "$.status" == "success"
jsonpath "$.data.result" count == 1
jsonpath "$.data.result[0].metric.edition" == "enterprise"
jsonpath "$.data.result[0].metric.__name__" == "sourcemeta_one_build_info"
jsonpath "$.data.result[0].value[1]" == "1"

# A counter carrying both of its labels, which is what makes the series
# addressable by the questions an operator actually asks. The refusals above
# are counted like anything else
GET http://localhost:9099/api/v1/query
[Options]
retry: 30
retry-interval: 1000
[QueryStringParams]
query: sourcemeta_one_http_requests_total{action="metrics_v1",code="401"}
HTTP 200
[Asserts]
jsonpath "$.status" == "success"
jsonpath "$.data.result" count == 1
jsonpath "$.data.result[0].metric.action" == "metrics_v1"
jsonpath "$.data.result[0].metric.code" == "401"

# Asking for a percentile is what proves the buckets are cumulative, ordered,
# and closed by an infinite bound. A malformed histogram answers nothing here
# even though it parses perfectly well as text
GET http://localhost:9099/api/v1/query
[Options]
retry: 30
retry-interval: 1000
[QueryStringParams]
query: histogram_quantile(0.95, sum by (le) (rate(sourcemeta_one_http_request_duration_seconds_bucket[1m])))
HTTP 200
[Asserts]
jsonpath "$.status" == "success"
jsonpath "$.data.result" count == 1
jsonpath "$.data.result[0].value[1]" exists

# The count a histogram carries and the number of requests counted separately
# are two ways of saying the same thing, and they disagree if either is wrong
GET http://localhost:9099/api/v1/query
[Options]
retry: 30
retry-interval: 1000
[QueryStringParams]
query: sum(sourcemeta_one_http_request_duration_seconds_count) - sum(sourcemeta_one_http_requests_total)
HTTP 200
[Asserts]
jsonpath "$.status" == "success"
jsonpath "$.data.result" count == 1
jsonpath "$.data.result[0].value[1]" == "0"

# Nothing was lost on the way to being counted
GET http://localhost:9099/api/v1/query
[Options]
retry: 30
retry-interval: 1000
[QueryStringParams]
query: sourcemeta_one_metrics_dropped_total
HTTP 200
[Asserts]
jsonpath "$.status" == "success"
jsonpath "$.data.result" count == 1
jsonpath "$.data.result[0].value[1]" == "0"

# The process metrics use names shared across every language's client library,
# so a dashboard written against any of them finds these
GET http://localhost:9099/api/v1/query
[Options]
retry: 30
retry-interval: 1000
[QueryStringParams]
query: process_resident_memory_bytes > 0 and process_max_fds > 0 and process_start_time_seconds > 0
HTTP 200
[Asserts]
jsonpath "$.status" == "success"
jsonpath "$.data.result" count == 1
