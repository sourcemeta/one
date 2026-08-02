include ../../../test/e2e/common.mk

# The registry answers over TLS under the name its resource identifier
# advertises, so the client trusts this sandbox's authority and dials that
# name through the locally mapped port
BASE = https://registry
HURL_FLAGS = --cacert tls/ca.crt --connect-to registry:8000:localhost:$(PORT)
