# The lock marks a directory as private, and it marks it wherever it sits under
# the policy rather than only where the policy is declared. Somebody browsing a
# gated tree is deciding whether to pass a link on, and that question has the
# same answer three levels in as it does at the boundary.
#
# It is drawn from what the listing already carries: an entry names the policies
# governing it, inherited ones included, so the lock is that array being
# non-empty and nothing else. A view is only written what it can reach, so an
# entry a caller cannot see is absent rather than locked, and the lock never
# stands for something being withheld.
#
# This sandbox gates two collections with separate keys and leaves a third
# public, so each key sees one lock and never the other's.

# The anonymous root names only what is open. Nothing is locked, because
# nothing here is private to the caller reading it
GET {{base}}/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 2
xpath "count(//tr[.//a[normalize-space(.) = 'open']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'self']]//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'vault']])" == 0
xpath "count(//tr[.//a[normalize-space(.) = 'ledger']])" == 0

# One key in, and its collection appears with a lock. The open children stay
# plain folders, so the mark separates private from public rather than marking
# everything a credential was needed for
GET {{base}}/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
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
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'ledger']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'vault']])" == 0

# Inside the gated collection, one level under the policy. This is the case the
# old rule got wrong: the policy is unchanged from the parent, so nothing was
# marked, and a reader here saw a private directory drawn as an ordinary one
GET {{base}}/vault/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'inner']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0

# Two levels under it, and it still says so
GET {{base}}/vault/inner/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'deeper']]//i[contains(@class, 'bi-lock-fill')])" == 1
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0

# Three levels under it there are no directories left, and a schema row carries
# its dialect rather than a folder or a lock
GET {{base}}/vault/inner/deeper/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0
xpath "count(//tr[.//a[normalize-space(.) = 'bottom']])" == 1

# A public tree is never marked, at any depth, and holding a key does not
# change that. Nothing about the caller decides this
GET {{base}}/open/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 1
xpath "count(//tr[.//a[normalize-space(.) = 'deep']]//i[contains(@class, 'bi-folder-fill')])" == 1

GET {{base}}/open/
Accept: text/html
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 1

GET {{base}}/open/deep/
Accept: text/html
HTTP 200
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0
xpath "count(//i[contains(@class, 'bi-folder-fill')])" == 0

# A key that does not open this collection reaches nothing, so there is no
# locked door to find here either
GET {{base}}/vault/
Accept: text/html
Authorization: Bearer ledger-secret-key
HTTP 404
Content-Type: text/html; charset=utf-8
[Asserts]
xpath "string(/html/head/title)" == "Not Found"
xpath "count(//i[contains(@class, 'bi-lock-fill')])" == 0

# And the data the lock is drawn from says the same thing. An entry carries the
# policies governing it, inherited ones included, which is why the mark needs
# nothing beyond the listing it is rendered from
GET {{base}}/self/v1/api/list/vault
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: application/json
Link: </self/v1/schemas/api/list/response>; rel="describedby"
[Captures]
vault_listing: body
vault_schema: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.policies" count == 1
jsonpath "$.policies[0].name" == "vault"
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "inner"
jsonpath "$.entries[0].type" == "directory"
jsonpath "$.entries[0].policies" count == 1
jsonpath "$.entries[0].policies[0].name" == "vault"
jsonpath "$.entries[0].policies[0].type" == "apiKey"
jsonpath "$.entries[1].name" == "secret"
jsonpath "$.entries[1].type" == "schema"
jsonpath "$.entries[1].policies" count == 1
jsonpath "$.entries[1].policies[0].name" == "vault"

POST {{base}}/self/v1/api/schemas/evaluate{{vault_schema}}
```
{{vault_listing}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Two levels under the policy the entries still name it, which is the property
# the render reads and the reason it needs no notion of depth
GET {{base}}/self/v1/api/list/vault/inner
Authorization: Bearer vault-secret-key
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.policies" count == 1
jsonpath "$.policies[0].name" == "vault"
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "deeper"
jsonpath "$.entries[0].policies" count == 1
jsonpath "$.entries[0].policies[0].name" == "vault"
jsonpath "$.entries[1].name" == "hidden"
jsonpath "$.entries[1].policies" count == 1
jsonpath "$.entries[1].policies[0].name" == "vault"

# A public tree names none, at any depth, so the render has nothing to mark
GET {{base}}/self/v1/api/list/open
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.policies" count == 0
jsonpath "$.entries" count == 2
jsonpath "$.entries[0].name" == "deep"
jsonpath "$.entries[0].policies" count == 0
jsonpath "$.entries[1].name" == "string"
jsonpath "$.entries[1].policies" count == 0

GET {{base}}/self/v1/api/list/open/deep
HTTP 200
Content-Type: application/json
[Asserts]
jsonpath "$.policies" count == 0
jsonpath "$.entries" count == 1
jsonpath "$.entries[0].name" == "nested"
jsonpath "$.entries[0].policies" count == 0
