---
hide:
  - navigation
---

# Buy a Commercial License

*Ready to scale your JSON Schema management? Our commercial licensing ensures
you have the tools and support needed for enterprise deployments while
contributing to the continued development of industry-leading JSON Schema
technology.*

Sourcemeta One is [publicly available on
GitHub](https://github.com/sourcemeta/one) with full source code transparency,
enabling comprehensive auditing and community contributions.

## Editions

Sourcemeta One is available in two editions:

- **Community**: Licensed under the [Business Source License
  1.1](https://github.com/sourcemeta/one/blob/main/LICENSE). You may use it as
  if under the terms of AGPL-3.0, provided that you may not use it for a
  hosting solution that competes with Sourcemeta. After four years from each
  release, the code transitions to AGPL-3.0.

- **Enterprise**: Includes the [Standard
  Library](https://github.com/sourcemeta/std), additional features, and supply
  chain security capabilities not available in the Community edition. Requires
  a [commercial
  license](https://github.com/sourcemeta/one/blob/main/LICENSE-COMMERCIAL)
  from Sourcemeta.

## Standard Library

The Enterprise edition includes a growing library of hand-crafted high-quality
schemas maintained by Sourcemeta. The standard library provides ready-to-use
schemas for commonly used standards and specifications such as IETF URIs, email
addresses, JSON Pointers, HTTP problem details, and IEEE POSIX paths, among
others. To use it, add `@sourcemeta/std/v0` to the
[`extends`](configuration.md#extends-optional) property of your configuration
file.

For organizations looking to double down on their JSON Schema governance
initiative, the standard library provides a strong foundation of high-quality
schemas ready on day one, removing the need to invest significant time and
effort into authoring and maintaining a large catalog of schemas from scratch.

Every schema in the standard library is maintained by a member of the JSON
Schema Technical Steering Committee, extensively unit tested, performance tuned
for production use, and checked for compliance against the JSON Schema
specification. As part of the Enterprise plan, Sourcemeta will extend the
standard library with any additional standard your organization requires.

## Supply Chain Security

Starting with v4.2.2, the Enterprise container image ships with built-in
supply chain security and regulatory compliance capabilities:

- **Signed Container Images.** Every Enterprise image is cryptographically
  signed using [Cosign](https://github.com/sigstore/cosign) and the
  [Sigstore](https://www.sigstore.dev/) transparency log, allowing you to
  verify image authenticity and integrity before deployment.

- **Software Bill of Materials (SBOM).** Each release includes an SPDX SBOM
  attached as a signed attestation to the container image, providing full
  visibility into all vendored, npm, and system-level dependencies for
  vulnerability management and audit purposes.

- **FIPS-Ready Cryptography.** The Enterprise image is built with the OpenSSL
  FIPS provider (`openssl-provider-fips`) for all cryptographic operations,
  supporting organizations that require FIPS 140 compliance.

### Verifying Image Signatures

You can verify that an Enterprise container image was built and signed by
Sourcemeta's official GitHub Actions pipeline using
[Cosign](https://github.com/sigstore/cosign). For example:

```sh
cosign verify \
  --certificate-oidc-issuer "https://token.actions.githubusercontent.com" \
  --certificate-identity-regexp "^https://github.com/sourcemeta/one/" \
  ghcr.io/sourcemeta/one-enterprise:v4.2.2
```

### Retrieving the SBOM

The SPDX SBOM is attached as a signed in-toto attestation. You can verify and
extract it using Cosign. For example:

```sh
cosign verify-attestation --type spdx \
  --certificate-oidc-issuer "https://token.actions.githubusercontent.com" \
  --certificate-identity-regexp "^https://github.com/sourcemeta/one/" \
  ghcr.io/sourcemeta/one-enterprise:v4.2.2 \
  | jq -r '.payload' | base64 -d | jq '.predicate'
```

## Our Commitment to Excellence

Sourcemeta is led by a member of the JSON Schema Technical Steering Committee,
ensuring our solutions meet the highest industry standards and remain aligned
with JSON Schema ecosystem developments. As an independent, bootstrapped
company without venture capital backing, we maintain complete focus on
delivering nothing less than exceptional JSON Schema tooling.

## Next Steps

Interested in a commercial license? Contact us at
[hello@sourcemeta.com](mailto:hello@sourcemeta.com) to discuss further.
