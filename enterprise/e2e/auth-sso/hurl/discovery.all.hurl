# A provider describes itself in a document this instance fetches before it
# trusts anything, and every endpoint in that document is a URL the instance
# will then use. The token endpoint receives the client secret, the
# authorization code and the PKCE verifier, so one advertised as cleartext
# would put all three on the wire in the open, and an identity provider behind
# a misconfigured proxy advertises exactly that while its issuer stays
# encrypted. The sandbox serves two providers that differ in that one scheme
# and nothing else, so a refusal here is attributable to the scheme alone.

# The provider advertising a cleartext token endpoint is refused before a login
# even starts, so no credential is ever sent to it
GET {{base}}/self/v1/auth/login/cleartext
HTTP 502
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
Access-Control-Allow-Origin: *
[Captures]
refused_body: body
refused_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
header "Location" not exists
{
  "type": "urn:sourcemeta:one:auth-provider-unreachable",
  "title": "Bad Gateway",
  "status": 502,
  "detail": "The identity provider could not be reached"
}

POST {{base}}/self/v1/api/schemas/evaluate{{refused_schema}}
```
{{refused_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The callback refuses it too, so a login that somehow began cannot be
# completed against that provider either
GET {{base}}/self/v1/auth/callback/cleartext?state=whatever&code=x
HTTP 400
Cache-Control: no-store
Content-Type: application/problem+json
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
callback_body: body
callback_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "Set-Cookie" not exists
{
  "type": "urn:sourcemeta:one:auth-invalid-callback",
  "title": "Bad Request",
  "status": 400,
  "detail": "The login could not be completed"
}

POST {{base}}/self/v1/api/schemas/evaluate{{callback_schema}}
```
{{callback_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# The same document with an encrypted token endpoint is accepted, and the login
# proceeds to the provider carrying the state and challenge it minted. That is
# what makes the refusal above about the scheme and not about the fixture
GET {{base}}/self/v1/auth/login/encrypted
HTTP 303
Cache-Control: no-store
[Asserts]
header "Location" startsWith "https://keycloak:8443/realms/encrypted/protocol/openid-connect/auth?"
header "Location" contains "client_id=registry"
header "Location" contains "response_type=code"
header "Location" contains "code_challenge_method=S256"
header "Location" contains "state="
header "Location" contains "nonce="
header "Location" contains "code_challenge="
cookie "sourcemeta_one_transaction_encrypted" exists
cookie "sourcemeta_one_transaction_encrypted[HttpOnly]" exists
cookie "sourcemeta_one_transaction_encrypted[SameSite]" == "Lax"
cookie "sourcemeta_one_transaction_encrypted[Max-Age]" == 600

# Neither provider gates anything the other does not, so the collection they
# both govern stays denied to a caller carrying nothing
GET {{base}}/discovery/thing.json
HTTP 401
Cache-Control: no-store
Content-Type: application/problem+json
WWW-Authenticate: Bearer realm="registry"
Link: </self/v1/schemas/api/error>; rel="describedby"
[Captures]
denied_body: body
denied_schema: header "Link" regex "<([^>]+)>"
[Asserts]
header "ETag" not exists
{
  "type": "urn:sourcemeta:one:authentication-required",
  "title": "Unauthorized",
  "status": 401,
  "detail": "This resource requires authentication"
}

POST {{base}}/self/v1/api/schemas/evaluate{{denied_schema}}
```
{{denied_body}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
