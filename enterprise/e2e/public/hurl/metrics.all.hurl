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

# The digest the instance holds is not itself a credential: presenting it is
# refused exactly as any other wrong value is
GET {{base}}/self/v1/metrics
Authorization: Bearer ed20191044553dac8f9c45e62062dd18e7dc1f898a897240b4179fb84fea3db4
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
[Asserts]
jsonpath "$.type" == "urn:sourcemeta:one:authentication-required"
jsonpath "$.status" == 401

# The key admits, and the exposition answers
GET {{base}}/self/v1/metrics
Authorization: Bearer local-development-only
HTTP 200
Cache-Control: no-store
Content-Type: text/plain; version=0.0.4; charset=utf-8
[Asserts]
header "Link" not exists
body matches /# TYPE sourcemeta_one_build_info gauge\nsourcemeta_one_build_info\{version="[^"]+",edition="enterprise"\} 1\n/
body matches /# TYPE sourcemeta_one_http_requests_total counter\n/
body matches /sourcemeta_one_http_requests_total\{action="metrics_v1",code="401"\} [0-9]+\n/
body matches /# TYPE sourcemeta_one_http_request_duration_seconds histogram\n/
body not matches /le="1e-04"/

# Everything else on this instance stays public, so the gate has not leaked
# past the path it names
GET {{base}}/self/v1/health
HTTP 200
Cache-Control: no-store

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
