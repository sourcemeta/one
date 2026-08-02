include ../../../test/e2e/common.mk

# The identity provider serves real TLS under this sandbox's committed
# authority, so the client trusts that authority and dials the certificate's
# name through the locally mapped port
BASE = https://registry
HURL_FLAGS = --cacert tls/ca.crt --connect-to keycloak:8443:localhost:8443 \
  --connect-to registry:$(PORT):localhost:$(PORT)
