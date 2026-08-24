# The three places a GitHub deployment departs from what a standard would have,
# asked of the deployment directly.
#
# These are not assertions about the registry. They are what pins the sandbox to
# the behaviour it is standing in for, so that the flow tests beside them prove
# the registry copes with a deployment that really does behave this way rather
# than with one that was quietly made easier to talk to

# The API refuses a request that names no user agent, and says so in prose
# rather than in the representation the rest of the API answers with
GET https://github:9443/api/v3/user
User-Agent:
HTTP 403
Content-Type: text/plain; charset=utf-8
[Asserts]
body contains "User-Agent"

# Naming one gets past that, and the credential is then what is missing
GET https://github:9443/api/v3/user
User-Agent: sourcemeta-one
HTTP 401
Content-Type: application/json; charset=utf-8
[Asserts]
jsonpath "$.message" == "Requires authentication"

# The token endpoint answers every outcome with a 200 and names a failure in
# the body, where RFC 6749 Section 5.2 has a failure carry a 400
POST https://github:9443/login/oauth/access_token
Accept: application/json
[FormParams]
grant_type: authorization_code
code: invented
redirect_uri: http://localhost:8000/self/v1/auth/callback/engineering
client_id: registry
client_secret: registry-client-secret
HTTP 200
Content-Type: application/json; charset=utf-8
[Asserts]
jsonpath "$.error" exists
jsonpath "$.access_token" not exists

# And it answers in a form encoding unless a request asks for JSON, where
# RFC 6749 Section 5.1 mandates JSON
POST https://github:9443/login/oauth/access_token
[FormParams]
grant_type: authorization_code
code: invented
redirect_uri: http://localhost:8000/self/v1/auth/callback/engineering
client_id: registry
client_secret: registry-client-secret
HTTP 200
Content-Type: application/x-www-form-urlencoded; charset=utf-8
[Asserts]
body contains "error="

# There is nothing to discover, which is why a policy names an origin rather
# than an issuer and composes both endpoints from it
GET https://github:9443/.well-known/openid-configuration
HTTP 404

GET https://github:9443/.well-known/oauth-authorization-server
HTTP 404
