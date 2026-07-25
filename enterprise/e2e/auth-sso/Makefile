include ../../../test/e2e/common.mk

# The identity provider serves real TLS under a sandbox-local authority, so
# the client trusts that authority and dials the certificate's name through
# the locally mapped port
HURL += --cacert tls/ca.crt --connect-to keycloak:8443:localhost:8443

up: tls/keycloak.crt

tls/keycloak.crt:
	mkdir -p tls
	openssl req -x509 -newkey rsa:2048 -nodes -days 36500 \
		-keyout tls/ca.key -out tls/ca.crt -subj "/CN=auth-sso sandbox authority"
	openssl req -newkey rsa:2048 -nodes \
		-keyout tls/keycloak.key -out tls/keycloak.csr -subj "/CN=keycloak"
	printf "subjectAltName=DNS:keycloak\n" > tls/keycloak.ext
	openssl x509 -req -days 36500 -in tls/keycloak.csr \
		-CA tls/ca.crt -CAkey tls/ca.key -CAcreateserial \
		-extfile tls/keycloak.ext -out tls/keycloak.crt
