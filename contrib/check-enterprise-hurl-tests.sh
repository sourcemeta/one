#!/bin/sh

set -o errexit
set -o nounset

ROOT="$(pwd)"

OFFENDERS="$(find "$ROOT/test/e2e" -type f -name '*.enterprise.hurl' 2>/dev/null || true)"

if [ -n "$OFFENDERS" ]
then
  echo "error: enterprise-only hurl tests must not live under \`test/e2e/\`" 1>&2
  echo "" 1>&2
  echo "The \`test/e2e/\` tree is covered by the community license. Test" 1>&2
  echo "assertions are intellectual property, so enterprise-only ones must" 1>&2
  echo "sit under \`enterprise/e2e/\` to fall under the enterprise license." 1>&2
  echo "" 1>&2
  echo "Move each file to \`enterprise/e2e/<sandbox>/hurl/\` and rename it" 1>&2
  echo "from \`*.enterprise.hurl\` to \`*.all.hurl\` (the entire sandbox is" 1>&2
  echo "already enterprise-gated, so the edition suffix is redundant)." 1>&2
  echo "If no matching enterprise sandbox exists, create one mirroring the" 1>&2
  echo "community shape and wire it into the root \`Makefile\` and CI." 1>&2
  echo "" 1>&2
  echo "Offending files:" 1>&2
  echo "$OFFENDERS" | sed 's/^/  /' 1>&2
  exit 1
fi
