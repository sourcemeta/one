# Something for a scrape to have counted, so that the assertions below read a
# series that exists rather than one that happens not to yet
GET {{base}}/self/v1/health
HTTP 200

GET {{base}}/self/v1/api/list
HTTP 200

GET {{base}}/self/v1/metrics
HTTP 200
Content-Type: text/plain; version=0.0.4; charset=utf-8

# Whether a scrape succeeded is the scraper's own verdict on what it read, and
# it is zero for a target it could not reach or could not parse. Nothing else
# here proves as much in a single number
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

# The scraper reports on its own targets, which says the endpoint answered
# rather than merely that something was listening
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
jsonpath "$.data.activeTargets[0].labels.job" == "sourcemeta-one"

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
# addressable by the questions an operator actually asks
GET http://localhost:9099/api/v1/query
[Options]
retry: 30
retry-interval: 1000
[QueryStringParams]
query: sourcemeta_one_http_requests_total{action="health_check_v1",code="200"}
HTTP 200
[Asserts]
jsonpath "$.status" == "success"
jsonpath "$.data.result" count == 1
jsonpath "$.data.result[0].metric.action" == "health_check_v1"
jsonpath "$.data.result[0].metric.code" == "200"

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
