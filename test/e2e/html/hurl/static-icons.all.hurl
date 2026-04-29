HEAD {{base}}/self/v1/static/apple-touch-icon.png
HTTP 200
Content-Type: image/png
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" not exists

HEAD {{base}}/self/v1/static/favicon.ico
HTTP 200
Content-Type: image/vnd.microsoft.icon
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" not exists

HEAD {{base}}/self/v1/static/icon.svg
HTTP 200
Content-Type: image/svg+xml
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" not exists

HEAD {{base}}/self/v1/static/icon_192x192.png
HTTP 200
Content-Type: image/png
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" not exists

HEAD {{base}}/self/v1/static/icon_512x512.png
HTTP 200
Content-Type: image/png
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" not exists

HEAD {{base}}/self/v1/static/manifest.webmanifest
HTTP 200
Content-Type: application/manifest+json
[Asserts]
bytes count == 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/self/v1/static/apple-touch-icon.png
HTTP 200
Content-Type: image/png
[Asserts]
bytes count > 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/self/v1/static/favicon.ico
HTTP 200
Content-Type: image/vnd.microsoft.icon
[Asserts]
bytes count > 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/self/v1/static/icon.svg
HTTP 200
Content-Type: image/svg+xml
[Asserts]
bytes count > 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/self/v1/static/icon_192x192.png
HTTP 200
Content-Type: image/png
[Asserts]
bytes count > 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/self/v1/static/icon_512x512.png
HTTP 200
Content-Type: image/png
[Asserts]
bytes count > 0
header "Access-Control-Allow-Origin" not exists

GET {{base}}/self/v1/static/manifest.webmanifest
HTTP 200
Content-Type: application/manifest+json
[Asserts]
bytes count > 0
header "Access-Control-Allow-Origin" not exists
