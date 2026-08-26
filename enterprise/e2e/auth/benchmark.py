#!/usr/bin/env python3

import http.client
import json
import ssl
import sys
import urllib.parse

from measure import run


# A token comes from the provider through the client_credentials grant, the way
# the JWT tests come by one. The certificate it answers under names the provider
# rather than the port it is published on here, so the chain is checked against
# the authority this sandbox commits while the name it carries is let be
def machine_credential():
    context = ssl.create_default_context(cafile="tls/ca.crt")
    context.check_hostname = False
    connection = http.client.HTTPSConnection("localhost", 8443, context=context)
    connection.request(
        "POST",
        "/realms/main/protocol/openid-connect/token",
        body=urllib.parse.urlencode(
            {
                "grant_type": "client_credentials",
                "client_id": "ci-service",
                "client_secret": "ci-service-secret",
            }
        ),
        headers={"Content-Type": "application/x-www-form-urlencoded"},
    )
    response = connection.getresponse()
    if response.status != 200:
        sys.exit(f"The provider refused to issue a token: HTTP {response.status}")
    token = json.load(response)["access_token"]
    connection.close()
    return {"Authorization": f"Bearer {token}"}


# The same act, fetching one schema, asked for four ways of the same instance,
# so what separates the numbers is what it took to admit the caller and little
# else. Credentials are written out the way the tests write them, as a hashed
# policy stores the digest rather than the key, leaving the key itself with
# nowhere else to come from
run(
    [
        {
            "name": "Schema Anonymous",
            "path": "/public/string.json",
        },
        {
            "name": "Schema API Key Identity",
            "path": "/private/secret.json",
            "headers": {"Authorization": "Bearer primary-secret-key"},
        },
        # An identity policy governs this path as well and is consulted first,
        # so what this measures is a digest compared with a verbatim comparison
        # ahead of it, rather than a digest compared alone
        {
            "name": "Schema API Key SHA256",
            "path": "/mixed/thing.json",
            "headers": {"Authorization": "Bearer mixed-hashed-key"},
        },
        {
            "name": "Schema JWT",
            "path": "/machine/config.json",
            "headers": machine_credential,
        },
    ]
)
