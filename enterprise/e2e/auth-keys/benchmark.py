#!/usr/bin/env python3

import os

from measure import run

# A path a policy governs, so what is measured includes admitting the caller
# rather than only serving them. The credential comes from the same file the
# tests run under, and its absence is loud rather than a refusal to explain
run(
    [
        {
            "name": "Schema Gated",
            "path": "/vault/secret.json",
            "headers": {
                "Authorization": f"Bearer {os.environ['ONE_E2E_VAULT_KEY']}"
            },
        },
    ]
)
