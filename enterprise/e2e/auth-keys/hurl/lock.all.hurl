# The lock marks a directory as private, and it marks it wherever it sits under
# the policy rather than only where the policy is declared. Somebody browsing a
# gated tree is deciding whether to pass a link on, and that question has the
# same answer three levels in as it does at the boundary.
#
# It is drawn from what the listing already carries: an entry says whether
# anything governs it, inherited policies included, so the lock is that answer
# and nothing else. A view is only written what it can reach, so an entry a
# caller cannot see is absent rather than locked, and the lock never stands for
# something being withheld.
#
# This sandbox gates two collections with separate keys and leaves a third
# public, so each key sees one lock and never the other's.

# The anonymous root names only what is open. Nothing is locked, because
# nothing here is private to the caller reading it. The answer is the same for
# every anonymous caller, so it is public and revalidated
GET {{base}}/
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
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 2
xpath "count(//tr[.//a[normalize-space(.) = 'open']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'self']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'vault']])" == 0
xpath "count(//tr[.//a[normalize-space(.) = 'ledger']])" == 0

# One key in, and its collection appears with a lock. The open children stay
# plain folders, so the mark separates private from public rather than marking
# everything a credential was needed for. The answer varies by caller now, so
# it is private and no shared cache may keep it
GET {{base}}/
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
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'vault']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 2
xpath "count(//tr[.//a[normalize-space(.) = 'open']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'self']]//i[contains(@class, 'bi-folder-fill')])" == 1
# The other policy's collection is not this caller's to see, so it is absent
# rather than shown locked
xpath "count(//tr[.//a[normalize-space(.) = 'ledger']])" == 0

# The other key sees the mirror image, which is what makes the lock a fact
# about the entry rather than about holding a credential
GET {{base}}/
Accept: text/html
Authorization: Bearer ledger-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'ledger']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 2
xpath "count(//tr[.//a[normalize-space(.) = 'vault']])" == 0

# Inside the gated collection, one level under the policy. This is the case the
# old rule got wrong: the policy is unchanged from the parent, so nothing was
# marked, and a reader here saw a private directory drawn as an ordinary one
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
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'inner']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0

# Two levels under it, and it still says so
GET {{base}}/vault/inner/
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
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'deeper']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0

# Three levels under it there are no directories left, and a schema row carries
# its dialect rather than a folder or a lock
GET {{base}}/vault/inner/deeper/
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
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0
xpath "count(//tr[.//a[normalize-space(.) = 'bottom']])" == 1

# A public tree is never marked, at any depth, and holding a key does not
# change that. Nothing about the caller decides this
GET {{base}}/open/
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
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'deep']]//i[contains(@class, 'bi-folder-fill')])" == 1

# The same public tree read under a key is still unmarked, but it is no longer
# shareable. A view holds its own copy of everything it can reach, so this URL
# answers differently per view, and a shared cache handing this one to the next
# caller would be the disclosure the per-view split closed
GET {{base}}/open/
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
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 1

GET {{base}}/open/deep/
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
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0

# A key that does not open this collection reaches nothing, so there is no
# locked door to find here either
GET {{base}}/vault/
Accept: text/html
Authorization: Bearer ledger-secret-key
HTTP 404
Cache-Control: no-store
Content-Type: text/html; charset=utf-8
Referrer-Policy: strict-origin-when-cross-origin
Content-Security-Policy: frame-ancestors 'none'
X-Frame-Options: DENY
[Asserts]
header "Vary" == "Accept, Accept-Encoding"
header "Access-Control-Allow-Origin" not exists
header "WWW-Authenticate" not exists
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0

# And the data the lock is drawn from says the same thing. An entry says
# whether anything governs it, inherited policies included, which is why the
# mark needs nothing beyond the listing it is rendered from
GET {{base}}/self/v1/api/list/vault
Authorization: Bearer vault-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
vault_listing: body
vault_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/vault"
jsonpath "$.private" == true
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "inner"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].private" == true
jsonpath "$.entries[1].name" == "secret"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].private" == true

POST {{base}}/self/v1/api/schemas/evaluate{{vault_schema}}
```
{{vault_listing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# Two levels under the policy the entries are still marked, which is the
# answer the render reads and the reason it needs no notion of depth
GET {{base}}/self/v1/api/list/vault/inner
Authorization: Bearer vault-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
inner_listing: body
inner_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/vault/inner"
jsonpath "$.private" == true
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "deeper"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].private" == true
jsonpath "$.entries[1].name" == "hidden"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].private" == true

POST {{base}}/self/v1/api/schemas/evaluate{{inner_schema}}
```
{{inner_listing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# A public tree names none, at any depth, so the render has nothing to mark
GET {{base}}/self/v1/api/list/open
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
open_listing: body
open_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/open"
jsonpath "$.private" == false
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "deep"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].private" == false
jsonpath "$.entries[1].name" == "string"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].private" == false

POST {{base}}/self/v1/api/schemas/evaluate{{open_schema}}
```
{{open_listing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/self/v1/api/list/open/deep
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
deep_listing: body
deep_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/open/deep"
jsonpath "$.private" == false
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "nested"
jsonpath "$.entries[0].type" == "schema"
jsonpath "$.entries[0].private" == false

POST {{base}}/self/v1/api/schemas/evaluate{{deep_schema}}
```
{{deep_listing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# The root listing under a key, which is where the two gated collections are
# told apart. One is present and marked, the other is absent, and the absent
# one leaves no trace of itself in the counts either
GET {{base}}/self/v1/api/list
Authorization: Bearer vault-secret-key
HTTP 200
Cache-Control: private, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
root_listing: body
root_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/"
jsonpath "$.private" == false
jsonpath "$.entries" count == 3
jsonpath "$.entries[0].name" == "open"
jsonpath "$.entries[0].private" == false
jsonpath "$.entries[1].name" == "self"
jsonpath "$.entries[1].private" == false
jsonpath "$.entries[2].name" == "vault"
jsonpath "$.entries[2].private" == true

POST {{base}}/self/v1/api/schemas/evaluate{{root_schema}}
```
{{root_listing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true

# And anonymously, where neither is named at all
GET {{base}}/self/v1/api/list
HTTP 200
Cache-Control: public, max-age=0, must-revalidate
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
Access-Control-Allow-Origin: *
Access-Control-Expose-Headers: Link, ETag
[Captures]
anonymous_listing: body
anonymous_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" exists
jsonpath "$.path" == "/"
jsonpath "$.private" == false
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "open"
jsonpath "$.entries[0].private" == false
jsonpath "$.entries[1].name" == "self"
jsonpath "$.entries[1].private" == false

POST {{base}}/self/v1/api/schemas/evaluate{{anonymous_schema}}
```
{{anonymous_listing}}
```
HTTP 200
Cache-Control: no-store
Link: </self/v1/schemas/api/schemas/evaluate/response>; rel="describedby"
[Asserts]
jsonpath "$.valid" == true
