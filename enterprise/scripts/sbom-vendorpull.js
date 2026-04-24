#!/usr/bin/env node

import { readFileSync, readdirSync, existsSync } from "node:fs";
import { join, resolve, dirname } from "node:path";
import { fileURLToPath } from "node:url";

const LICENSES = {
  "core": "AGPL-3.0-or-later OR LicenseRef-Commercial",
  "blaze": "AGPL-3.0-or-later OR LicenseRef-Commercial",
  "jsonbinpack": "AGPL-3.0-or-later OR LicenseRef-Commercial",
  "uwebsockets": "Apache-2.0",
  "bootstrap": "MIT",
  "bootstrap-icons": "MIT",
  "pcre2": "BSD-3-Clause",
  "libdeflate": "MIT",
  "cmark-gfm": "BSD-2-Clause"
};

const IGNORED = new Set([
  "vendorpull",
  "jsontestsuite",
  "yaml-test-suite",
  "jsonschema-test-suite",
  "referencing-suite",
  "uritemplate-test",
  "pyca-cryptography",
  "googletest",
  "googlebenchmark",
  "jsonschema-2020-12",
  "jsonschema-2019-09",
  "jsonschema-draft7",
  "jsonschema-draft6",
  "jsonschema-draft4",
  "jsonschema-draft3",
  "jsonschema-draft2",
  "jsonschema-draft1",
  "jsonschema-draft0",
  "openapi",
  "spdx"
]);

const IGNORED_PREFIXES = [
  "public/",
  "collections/"
];

const versionFile = process.argv[2];
if (!versionFile) {
  process.stderr.write(`Usage: ${process.argv[1]} <version-file>\n`);
  process.exit(1);
}

const version = readFileSync(versionFile, "utf-8").trim();
const root = resolve(dirname(fileURLToPath(import.meta.url)), "../..");

const vendorDirectory = join(root, "vendor");
const files = [
  join(root, "DEPENDENCIES"),
  ...existsSync(vendorDirectory)
    ? readdirSync(vendorDirectory, { withFileTypes: true })
        .filter((entry) => entry.isDirectory())
        .map((entry) => join(vendorDirectory, entry.name, "DEPENDENCIES"))
    : []
].filter(existsSync).sort();

const seenUrls = new Set();
const usedLicenses = new Set();
const packages = [{
  name: "sourcemeta-one-enterprise",
  SPDXID: "SPDXRef-RootPackage",
  versionInfo: version,
  downloadLocation: "https://github.com/sourcemeta/one",
  filesAnalyzed: false,
  licenseConcluded: "NOASSERTION",
  licenseDeclared: "NOASSERTION"
}];
const relationships = [{
  spdxElementId: "SPDXRef-DOCUMENT",
  relationshipType: "DESCRIBES",
  relatedSpdxElement: "SPDXRef-RootPackage"
}];

let index = 0;
for (const file of files) {
  for (const line of readFileSync(file, "utf-8").split("\n")) {
    if (!line.trim()) continue;
    const [name, url, entryVersion] = line.split(/\s+/);
    if (IGNORED.has(name) || IGNORED_PREFIXES.some((prefix) => name.startsWith(prefix))
        || seenUrls.has(url)) continue;
    const license = LICENSES[name];
    if (!license) {
      process.stderr.write(
        `ERROR: dependency "${name}" (from ${file}) has no LICENSES entry\n`);
      process.exit(1);
    }
    usedLicenses.add(name);
    seenUrls.add(url);
    index += 1;
    const spdxid = `SPDXRef-Vendor-${index}`;
    packages.push({
      name, SPDXID: spdxid,
      versionInfo: entryVersion,
      downloadLocation: url,
      filesAnalyzed: false,
      licenseConcluded: license,
      licenseDeclared: license
    });
    relationships.push({
      spdxElementId: "SPDXRef-RootPackage",
      relationshipType: "DEPENDS_ON",
      relatedSpdxElement: spdxid
    });
  }
}

const staleLicenses = Object.keys(LICENSES).filter(
  (name) => !usedLicenses.has(name));
if (staleLicenses.length > 0) {
  process.stderr.write(
    `ERROR: stale LICENSES entries: ${staleLicenses.join(", ")}\n`);
  process.exit(1);
}

process.stdout.write(JSON.stringify({
  spdxVersion: "SPDX-2.3",
  dataLicense: "CC0-1.0",
  SPDXID: "SPDXRef-DOCUMENT",
  name: "sourcemeta-one-enterprise",
  documentNamespace: `https://one.sourcemeta.com/sbom/${version}`,
  creationInfo: {
    created: new Date().toISOString(),
    creators: [ "Tool: enterprise/scripts/sbom-vendorpull.js" ]
  },
  packages,
  relationships
}, null, 2) + "\n");
