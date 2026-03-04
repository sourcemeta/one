# The server MUST NOT crash on corrupted indexed data

# Corrupted routes.bin node indices
GET {{base}}/self/v1/api/list
[Options]
retry: 3
retry-interval: 1000
HTTP *

GET {{base}}/
[Options]
retry: 3
retry-interval: 1000
HTTP *

GET {{base}}/self/v1/api/schemas/metadata/test/schemas/string
[Options]
retry: 3
retry-interval: 1000
HTTP *

GET {{base}}/does-not-exist/at/all
[Options]
retry: 3
retry-interval: 1000
HTTP *

# Corrupted directory.metapack metadata with wrong types
GET {{base}}/self/v1/api/list
[Options]
retry: 3
retry-interval: 1000
HTTP *

# Deleted search.metapack
GET {{base}}/self/v1/api/schemas/search?q=test
[Options]
retry: 3
retry-interval: 1000
HTTP *

# Corrupted locations.metapack with empty object
POST {{base}}/self/v1/api/schemas/trace/test/schemas/string
Content-Type: application/json
[Options]
retry: 3
retry-interval: 1000
```
"hello"
```
HTTP *
