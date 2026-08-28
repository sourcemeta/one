include ../../../test/e2e/common.mk

# The deployment serves real TLS under this sandbox's committed authority, so
# the client trusts that authority and dials the certificate's name through the
# locally mapped port
HURL_FLAGS = --cacert tls/ca.crt --connect-to github:9443:localhost:9443
