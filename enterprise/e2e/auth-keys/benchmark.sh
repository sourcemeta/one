#!/bin/sh

set -o errexit
set -o nounset

BASE="$1"
HERE="$(cd "$(dirname "$0")" && pwd)"
MEASURE="$HERE/../../../benchmark/measure.py"

# A path a policy governs, so what is measured includes admitting the caller
# rather than only serving them
"$MEASURE" --name "Schema Fetch (gated)" \
  --header "Authorization: Bearer $ONE_E2E_VAULT_KEY" \
  --url "$BASE/vault/secret.json"
