# A directory listing says whether an entry is private, and a schema page says
# the same about itself. Clicking a locked row used to lose the mark, which
# left the reader who is deciding whether to pass a link on with the answer on
# one page and not the next.
#
# Both surfaces read the same field, computed once, so a schema and the row
# that leads to it cannot disagree.

# A public schema carries no mark, for a caller carrying nothing
GET {{base}}/open/string
Accept: text/html
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0

# And carries none for a caller holding a key either, since a credential does
# not make a public schema private
GET {{base}}/open/string
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0

# A gated schema carries the mark, once
GET {{base}}/vault/secret
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 1
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0

# A schema three levels under the policy carries it too, since what governs a
# path is inherited and the mark follows the same answer
GET {{base}}/vault/inner/deeper/bottom
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 1

# A schema under the other key is absent rather than marked, so the badge never
# stands for something being withheld
GET {{base}}/ledger/entry
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 404
Content-Type: text/html; charset=utf-8
[Asserts]
header "WWW-Authenticate" not exists
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0

# A directory says the same about itself, rather than only being marked in the
# listing above it. This collection was given no name to say it under, so the
# mark cannot depend on there being a header to hang it from
GET {{base}}/vault/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 1

# And one that was given a name says it beside the name, which is the other
# shape the header takes
GET {{base}}/ledger/
Accept: text/html
Authorization: Bearer ledger-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(//h2)" == "Ledger"
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 1

# A directory under the policy rather than at it carries the mark too, since
# what governs a path is inherited
GET {{base}}/vault/inner/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 1

# An open collection carries none, so the mark separates private from public
# rather than marking whatever a credential was needed for
GET {{base}}/open/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0

# And neither does the root here, which nothing governs. The locked rows below
# it are about the entries rather than about the page they sit on
GET {{base}}/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//span[contains(@class, 'badge')][normalize-space(.) = 'Private'])" == 0
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 1

# The page is rendered from this, so the two cannot drift
GET {{base}}/self/v1/api/schemas/metadata/vault/secret
Authorization: Bearer vault-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
private_metadata: body
private_metadata_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.private" == true
jsonpath "$.path" == "/vault/secret"

POST {{base}}/self/v1/api/schemas/evaluate{{private_metadata_schema}}
```
{{private_metadata}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/schemas/metadata/open/string
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/schemas/metadata/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
public_metadata: body
public_metadata_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.private" == false
jsonpath "$.path" == "/open/string"

POST {{base}}/self/v1/api/schemas/evaluate{{public_metadata_schema}}
```
{{public_metadata}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A listing row and the schema it leads to agree, which is the whole point of
# there being one computation behind both
GET {{base}}/self/v1/api/list/vault
Authorization: Bearer vault-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Asserts]
jsonpath "$.private" == true
jsonpath "$.entries[?(@.name == 'secret')].private" == true
jsonpath "$.entries[?(@.name == 'inner')].private" == true

# The same answer reaches an agent, through the tool rather than the endpoint
POST {{base}}/self/v1/mcp
Accept: application/json, text/event-stream
Content-Type: application/json
MCP-Protocol-Version: 2025-11-25
Authorization: Bearer vault-secret-key
```
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": {
    "name": "get_schema_metadata",
    "arguments": { "schema": "{{base}}/vault/secret" }
  }
}
```
HTTP 200
Cache-Control: no-store
Content-Type: application/json
[Captures]
tool_metadata: body
[Asserts]
jsonpath "$.error" not exists
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.private" == true

POST {{base}}/self/v1/api/schemas/evaluate/self/v1/schemas/mcp/response
```
{{tool_metadata}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
