GET {{base}}/../explorer/index.json
HTTP 404
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

GET {{base}}/test/schemas/string/../../../explorer/index.json
HTTP 404
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

# Percent-encoded `..` segments must be rejected the same way literal
# `..` segments are. A naive URL decoder that decodes before the path
# normalizer would let a request escape the catalog root through
# `%2E%2E` and read host-filesystem files outside the index directory.
GET {{base}}/%2E%2E/explorer/index.json
HTTP 404
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

# Same shape on a path under a real schema namespace, with mixed
# percent-encoding to defeat a string-equality `..` blocklist.
GET {{base}}/test/schemas/string/%2E%2E/%2E%2E/%2E%2E/explorer/index.json
HTTP 404
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

# Common host-file targets through percent-encoded traversal must also
# fall to 404, not 500 (which would suggest the parser blew up) and
# not a file leak.
GET {{base}}/%2E%2E/%2E%2E/%2E%2E/etc/passwd
HTTP 404
[Asserts]
header "Referrer-Policy" not exists
header "Content-Security-Policy" not exists
header "X-Frame-Options" not exists
header "Date" matches /^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), (0[1-9]|[12][0-9]|3[01]) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [0-9]{4} ([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9] GMT$/

# Server identification inventory lock. Neither `Server` nor the
# legacy `X-Powered-By` may appear on the wire on a representative
# success response. Locks against an accidental future emission.
GET {{base}}/test/schemas/string.json
HTTP 200
[Asserts]
header "Server" not exists
header "X-Powered-By" not exists

# Same inventory lock on an error response, which travels a separate
# wire path from the success response and therefore needs its own
# absence assertion.
GET {{base}}/this-path-does-not-exist
HTTP 404
[Asserts]
header "Server" not exists
header "X-Powered-By" not exists
