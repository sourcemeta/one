#!/bin/sh

# Regenerates the TLS material the end-to-end sandboxes serve over. The output
# is committed, so this only runs when a certificate expires or a new name is
# needed. One authority signs every name, so a sandbox trusting it reaches the
# identity provider and the registry alike through a single `--cacert`.
#
# The registry needs a certificate at all because RFC 9728 defines a protected
# resource identifier as an https URL, with no exception for loopback, so an
# instance publishing that metadata has to be served over TLS even in a test.

set -o errexit
set -o nounset

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DAYS=3650

WORK="$(mktemp -d)"
clean() { rm -rf "$WORK"; }
trap clean EXIT

openssl req -x509 -newkey rsa:2048 -nodes \
  -keyout "$WORK/ca.key" -out "$WORK/ca.crt" -days "$DAYS" \
  -subj "/CN=Sourcemeta One End-to-End Authority" \
  -addext "basicConstraints=critical,CA:TRUE" \
  -addext "keyUsage=critical,keyCertSign,cRLSign" 2>/dev/null

issue() {
  NAME="$1"
  printf 'subjectAltName=DNS:%s\n' "$NAME" > "$WORK/$NAME.ext"
  printf 'keyUsage=critical,digitalSignature,keyEncipherment\n' >> "$WORK/$NAME.ext"
  printf 'extendedKeyUsage=serverAuth\n' >> "$WORK/$NAME.ext"
  openssl req -newkey rsa:2048 -nodes \
    -keyout "$WORK/$NAME.key" -out "$WORK/$NAME.csr" \
    -subj "/CN=$NAME" 2>/dev/null
  openssl x509 -req -in "$WORK/$NAME.csr" \
    -CA "$WORK/ca.crt" -CAkey "$WORK/ca.key" -CAcreateserial \
    -out "$WORK/$NAME.crt" -days "$DAYS" -extfile "$WORK/$NAME.ext" 2>/dev/null
}

issue keycloak
issue registry

install_into() {
  SANDBOX="$1"
  TARGET="$ROOT/enterprise/e2e/$SANDBOX/tls"
  mkdir -p "$TARGET"
  cp "$WORK/ca.crt" "$TARGET/ca.crt"
  shift
  for NAME in "$@"
  do
    cp "$WORK/$NAME.crt" "$TARGET/$NAME.crt"
    cp "$WORK/$NAME.key" "$TARGET/$NAME.key"
  done
  echo "  enterprise/e2e/$SANDBOX/tls"
}

# Only the sandboxes that publish protected resource metadata serve the
# registry itself over TLS. The rest need the provider's name alone
install_into auth keycloak
install_into auth-path keycloak
install_into auth-sso keycloak
install_into auth-closed keycloak registry
install_into auth-mcp-path registry
