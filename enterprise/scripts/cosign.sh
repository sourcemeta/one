#!/bin/sh

set -o errexit
set -o nounset

if [ "$#" -ne 4 ]
then
  echo "Usage: $0 <image> <version> <certificate-oidc-issuer> <certificate-identity>" 1>&2
  exit 1
fi

IMAGE="$1"
VERSION="$2"
CERTIFICATE_OIDC_ISSUER="$3"
CERTIFICATE_IDENTITY="$4"

echo "Cosign: Extracting manifest digest for ${IMAGE}:${VERSION}" 1>&2
DIGEST=$(docker buildx imagetools inspect "${IMAGE}:${VERSION}" \
  --format '{{json .Manifest}}' | jq --raw-output '.digest')
if ! echo "$DIGEST" | grep -qE '^sha256:[a-f0-9]{64}$'
then
  echo "Cosign: Invalid manifest digest: $DIGEST" 1>&2
  exit 1
fi
echo "Cosign: Manifest digest is ${DIGEST}" 1>&2

echo "Cosign: Signing ${IMAGE}@${DIGEST}" 1>&2
cosign sign --yes "${IMAGE}@${DIGEST}"

echo "Cosign: Verifying signature for ${IMAGE}@${DIGEST}" 1>&2
echo "Cosign: OIDC issuer: ${CERTIFICATE_OIDC_ISSUER}" 1>&2
echo "Cosign: Certificate identity: ${CERTIFICATE_IDENTITY}" 1>&2
cosign verify \
  --certificate-oidc-issuer "$CERTIFICATE_OIDC_ISSUER" \
  --certificate-identity "$CERTIFICATE_IDENTITY" \
  "${IMAGE}@${DIGEST}"

echo "Cosign: Signature verified successfully" 1>&2
