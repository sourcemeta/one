#!/bin/sh

set -o errexit
set -o nounset

if [ "$#" -ne 5 ]
then
  echo "Usage: $0 <image> <version> <certificate-oidc-issuer> <certificate-identity> <sbom-file>" 1>&2
  exit 1
fi

IMAGE="$1"
VERSION="$2"
CERTIFICATE_OIDC_ISSUER="$3"
CERTIFICATE_IDENTITY="$4"
SBOM_FILE="$5"

echo "Cosign: Extracting manifest digest for ${IMAGE}:${VERSION}" 1>&2
MANIFEST=$(docker buildx imagetools inspect "${IMAGE}:${VERSION}" \
  --format '{{json .Manifest}}')
DIGEST=$(printf '%s\n' "$MANIFEST" | jq --raw-output '.digest')
if ! printf '%s\n' "$DIGEST" | grep -qE '^sha256:[a-f0-9]{64}$'
then
  echo "Cosign: Invalid manifest digest: $DIGEST" 1>&2
  exit 1
fi
echo "Cosign: Manifest digest is ${DIGEST}" 1>&2

echo "Cosign: Signing ${IMAGE}@${DIGEST}" 1>&2
cosign sign --yes "${IMAGE}@${DIGEST}"

echo "Cosign: Attaching SBOM attestation to ${IMAGE}@${DIGEST}" 1>&2
cosign attest --yes --predicate "$SBOM_FILE" --type spdx "${IMAGE}@${DIGEST}"

echo "Cosign: Verifying signature for ${IMAGE}@${DIGEST}" 1>&2
echo "Cosign: OIDC issuer: ${CERTIFICATE_OIDC_ISSUER}" 1>&2
echo "Cosign: Certificate identity: ${CERTIFICATE_IDENTITY}" 1>&2
cosign verify \
  --certificate-oidc-issuer "$CERTIFICATE_OIDC_ISSUER" \
  --certificate-identity "$CERTIFICATE_IDENTITY" \
  "${IMAGE}@${DIGEST}"

echo "Cosign: Signature verified successfully" 1>&2

echo "Cosign: Verifying SBOM attestation for ${IMAGE}@${DIGEST}" 1>&2
cosign verify-attestation --type spdx \
  --certificate-oidc-issuer "$CERTIFICATE_OIDC_ISSUER" \
  --certificate-identity "$CERTIFICATE_IDENTITY" \
  "${IMAGE}@${DIGEST}"

echo "Cosign: SBOM attestation verified successfully" 1>&2
