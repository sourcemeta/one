# The /self collection is not mounted when API is disabled
GET {{base}}/self/v1/schemas/configuration/configuration.json
HTTP 404

GET {{base}}/self/v1/schemas/api/error.json
HTTP 404

GET {{base}}/self/v1/schemas/api/list/response.json
HTTP 404

GET {{base}}/self/v1/schemas/api/schemas/evaluate/response.json
HTTP 404
