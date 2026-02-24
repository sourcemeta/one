#!/usr/bin/env node

// Generates an SPDX 2.3 JSON Software Bill of Materials (SBOM) for the
// vendored C++ and frontend dependencies managed through DEPENDENCIES files

import { readFileSync, readdirSync, existsSync } from "node:fs";
import { join, resolve, dirname } from "node:path";
import { fileURLToPath } from "node:url";

const LICENSES = {
  "core": "AGPL-3.0-or-later OR LicenseRef-Commercial",
  "blaze": "AGPL-3.0-or-later OR LicenseRef-Commercial",
  "hydra": "AGPL-3.0-or-later OR LicenseRef-Commercial",
  "codegen": "AGPL-3.0-or-later OR LicenseRef-Commercial",
  "jsonbinpack": "AGPL-3.0-or-later OR LicenseRef-Commercial",
  "jsonschema": "AGPL-3.0-only",
  "uwebsockets": "Apache-2.0",
  "bootstrap": "MIT",
  "bootstrap-icons": "MIT",
  "pcre2": "BSD-3-Clause",
  "zlib": "Zlib",
  "curl": "curl",
  "nghttp2": "MIT",
  "cpr": "MIT",
  "c-ares": "MIT",
  "libpsl": "MIT",
  "openssl": "Apache-2.0"
};

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
    const license = LICENSES[name];
    if (!license || seenUrls.has(url)) continue;
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
