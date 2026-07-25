include ../../../test/e2e/common.mk

# The identity provider serves real TLS under the committed sandbox authority,
# so the client trusts that authority and dials the certificate's name through
# the locally mapped port
HURL += --cacert tls/ca.crt --connect-to keycloak:8443:localhost:8443
