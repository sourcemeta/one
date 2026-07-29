#!/bin/sh

# The configuration anchor records which configuration the output was built
# from, and the incremental build trusts it: an anchor matching the
# configuration at hand means nothing derived from the configuration needs
# regenerating. That anchor is written early, while the artifacts derived from
# it are written later and the build state only once everything has finished.
# So a run that dies in between leaves an anchor ahead of both, and the next run
# reads it as proof of work that never happened.
#
# The authentication artifact is where that matters most, since it is written a
# whole wave after the anchor and a stale one leaves a path the configuration
# says is gated open to everybody, while indexing reports success.

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

mkdir "$TMP/schemas" "$TMP/extra"

cat << 'EOF' > "$TMP/schemas/a.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/a"
}
EOF

cat << 'EOF' > "$TMP/extra/b.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://other.com/b"
}
EOF

# One policy, gating only the first collection
write_one_policy() {
  cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com/",
  "authentication": [
    {
      "type": "apiKey",
      "name": "alpha",
      "paths": [ "/schemas" ],
      "algorithm": "identity",
      "keys": [ { "environmentVariable": "ONE_KEY_ALPHA" } ]
    }
  ],
  "contents": {
    "schemas": { "baseUri": "https://example.com/", "path": "./schemas" },
    "extra": { "baseUri": "https://other.com/", "path": "./extra" }
  }
}
EOF
}

# The same, plus a policy gating the second collection
write_two_policies() {
  cat << 'EOF' > "$TMP/one.json"
{
  "url": "https://example.com/",
  "authentication": [
    {
      "type": "apiKey",
      "name": "alpha",
      "paths": [ "/schemas" ],
      "algorithm": "identity",
      "keys": [ { "environmentVariable": "ONE_KEY_ALPHA" } ]
    },
    {
      "type": "apiKey",
      "name": "beta",
      "paths": [ "/extra" ],
      "algorithm": "identity",
      "keys": [ { "environmentVariable": "ONE_KEY_BETA" } ]
    }
  ],
  "contents": {
    "schemas": { "baseUri": "https://example.com/", "path": "./schemas" },
    "extra": { "baseUri": "https://other.com/", "path": "./extra" }
  }
}
EOF
}

index_into() {
  "$1" --skip-banner "$TMP/one.json" "$2" > /dev/null 2>&1 \
    && CODE="$?" || CODE="$?"
  test "$CODE" = "0" || exit 1
}

# The key set variables a policy names are the artifact's own record of which
# policies it holds
policies_in() {
  strings "$1/authentication.bin" \
    | grep -E '^ONE_KEY_(ALPHA|BETA)$' \
    | LC_ALL=C sort > "$TMP/actual.txt"
}

# What the anchor looks like once the second policy is configured. The anchor
# records the configuration file's own path, so it has to come from a build of
# this very file rather than a copy of another
write_two_policies
index_into "$1" "$TMP/interrupted"

# The output as an earlier, completed build left it
write_one_policy
index_into "$1" "$TMP/output"
policies_in "$TMP/output"
cat << 'EOF' > "$TMP/expected.txt"
ONE_KEY_ALPHA
EOF
diff "$TMP/actual.txt" "$TMP/expected.txt"

# What a run that wrote the anchor and then died leaves behind: an anchor for
# the new configuration, with the build state and every derived artifact still
# describing the old one
cp "$TMP/interrupted/configuration.json" "$TMP/output/configuration.json"

# Re-running is the ordinary response to a build that failed, and it has to
# produce what a build of this configuration produces, not report success over
# the artifact the interrupted run left behind
write_two_policies
index_into "$1" "$TMP/output"
policies_in "$TMP/output"
cat << 'EOF' > "$TMP/expected.txt"
ONE_KEY_ALPHA
ONE_KEY_BETA
EOF
diff "$TMP/actual.txt" "$TMP/expected.txt"
