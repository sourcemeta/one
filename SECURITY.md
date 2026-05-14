# Security Policy

Sourcemeta takes the security of Sourcemeta One seriously. This document
describes how to privately report vulnerabilities and which versions
receive security patches.

## Reporting a Vulnerability

If you believe you have discovered a security vulnerability in Sourcemeta
One, please report it privately to
[security@sourcemeta.com](mailto:security@sourcemeta.com) rather than
opening a public issue or pull request on GitHub. To help us reproduce
and assess the impact of the issue, please include as much of the
following as you can:

- A description of the vulnerability and its potential impact
- The affected version(s) of Sourcemeta One
- Step-by-step instructions to reproduce the issue
- Proof-of-concept code, configuration, or screenshots, where applicable
- Any suggested remediation or mitigation

We will acknowledge receipt of your report and work with you to validate
and address the issue under a coordinated disclosure process. Please
give us a reasonable period to investigate and ship a fix before any
public disclosure.

## Supported Versions

Unless otherwise stated in a commercial contract, security patches are
only landed on the latest released major version of Sourcemeta One.
Fixes may be backported to earlier major versions on a best-effort
basis, without any guarantee of timing or coverage.

Customers with a [commercial
license](https://github.com/sourcemeta/one/blob/main/LICENSE-COMMERCIAL)
or a support agreement may have additional terms, including extended
maintenance for specific versions. Please refer to your contract or
reach out to [security@sourcemeta.com](mailto:security@sourcemeta.com)
for details.

## Disclosure Process

Once a reported vulnerability is confirmed and a fix is available, we
will:

- Publish a patched release on the latest supported major version
- Document the issue in the release notes and, where appropriate, via a
  [GitHub Security
  Advisory](https://github.com/sourcemeta/one/security/advisories) and
  a CVE identifier
- Notify commercial customers in line with their support agreement
- Credit the reporter, if they wish to be publicly acknowledged

## Scope

This policy covers the official Sourcemeta One source code, container
images, and release artifacts published by Sourcemeta. Vulnerabilities
in third-party dependencies should be reported to their respective
maintainers. We still appreciate being notified when such issues
materially affect Sourcemeta One, so that we can coordinate an upgrade
or mitigation.

## Contact

For any questions about this policy, or to report a vulnerability,
please write to
[security@sourcemeta.com](mailto:security@sourcemeta.com).
