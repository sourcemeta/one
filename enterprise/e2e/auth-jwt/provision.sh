#!/bin/sh

set -o errexit
set -o nounset

# Registers the OAuth2 clients the end-to-end tests authenticate as, against the
# identity provider's admin API. The in-memory store starts empty on every run,
# so both clients are created deterministically with fixed credentials: one
# whose audience matches the policy, and one for a different audience that the
# policy must reject.

ADMIN="http://hydra:4445/admin/clients"

register() {
  wget --quiet --output-document - \
    --header "Content-Type: application/json" \
    --post-data "$1" \
    "$ADMIN"
}

CI_SERVICE='{"client_id":"ci-service","client_secret":"ci-service-secret","grant_types":["client_credentials"],"response_types":[],"token_endpoint_auth_method":"client_secret_post","audience":["https://schemas.example.com"]}'
CI_WRONG='{"client_id":"ci-wrong","client_secret":"ci-wrong-secret","grant_types":["client_credentials"],"response_types":[],"token_endpoint_auth_method":"client_secret_post","audience":["https://other.example.com"]}'

# The admin API is not ready the instant the container starts, so retry the
# first registration until it succeeds before creating the rest.
attempt=0
until register "$CI_SERVICE"
do
  attempt=$((attempt + 1))
  if [ "$attempt" -ge 60 ]
  then
    echo "identity provider admin API did not become ready" 1>&2
    exit 1
  fi
  sleep 1
done

register "$CI_WRONG"
