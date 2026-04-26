# Health check endpoint is not available
GET {{base}}/self/v1/health
HTTP 404

# List endpoint is not available
GET {{base}}/self/v1/api/list
HTTP 404

GET {{base}}/self/v1/api/list/test
HTTP 404

# Schema metadata endpoint is not available
GET {{base}}/self/v1/api/schemas/metadata/test/schemas/string
HTTP 404

# Schema search endpoint is not available
GET {{base}}/self/v1/api/schemas/search
HTTP 404

# Schema dependencies endpoint is not available
GET {{base}}/self/v1/api/schemas/dependencies/test/schemas/string
HTTP 404

# Schema dependents endpoint is not available
GET {{base}}/self/v1/api/schemas/dependents/test/schemas/string
HTTP 404

# Schema health endpoint is not available
GET {{base}}/self/v1/api/schemas/health/test/schemas/string
HTTP 404

# Schema locations endpoint is not available
GET {{base}}/self/v1/api/schemas/locations/test/schemas/string
HTTP 404

# Schema positions endpoint is not available
GET {{base}}/self/v1/api/schemas/positions/test/schemas/string
HTTP 404

# Schema stats endpoint is not available
GET {{base}}/self/v1/api/schemas/stats/test/schemas/string
HTTP 404

# Evaluate endpoint is not available
POST {{base}}/self/v1/api/schemas/evaluate/test/schemas/string
```
"hello"
```
HTTP 404

# Trace endpoint is not available
POST {{base}}/self/v1/api/schemas/trace/test/schemas/string
```
"hello"
```
HTTP 404

# Catch-all API path is not available
GET {{base}}/self/v1/api/anything
HTTP 404
