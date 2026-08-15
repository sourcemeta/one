#!/usr/bin/env node

import { readFileSync, writeFileSync, mkdtempSync, rmSync } from "node:fs";
import { join, resolve, dirname } from "node:path";
import { execSync } from "node:child_process";
import { tmpdir } from "node:os";
import { fileURLToPath } from "node:url";

const root = resolve(dirname(fileURLToPath(import.meta.url)), "../..");

const image = process.argv[2];
const output = process.argv[3];
if (!image || !output) {
  process.stderr.write(`Usage: ${process.argv[1]} <image> <output>\n`);
  process.exit(1);
}

const workdir = mkdtempSync(join(tmpdir(), "sbom-"));
try {
  try {
    execSync(`docker image inspect ${image}`, { stdio: "ignore" });
  } catch {
    execSync(`docker pull ${image}`, { stdio: [ "ignore", "inherit", "inherit" ] });
  }

  const container = execSync(`docker create ${image}`, { encoding: "utf-8" }).trim();
  execSync(`docker cp ${container}:/usr/share/sourcemeta/one/npm-packages.spdx.json ${workdir}/npm.json`);
  execSync(`docker cp ${container}:/usr/share/sourcemeta/one/vendor-packages.spdx.json ${workdir}/vendor.json`);
  execSync(`docker cp ${container}:/usr/share/sourcemeta/one/dpkg-packages ${workdir}/dpkg-packages`);
  execSync(`docker rm ${container}`, { stdio: "ignore" });

  const npm = JSON.parse(readFileSync(join(workdir, "npm.json"), "utf-8"));
  const vendor = JSON.parse(readFileSync(join(workdir, "vendor.json"), "utf-8"));
  const dpkg = readFileSync(join(workdir, "dpkg-packages"), "utf-8");

  const version = vendor.packages
    .find((entry) => entry.SPDXID === "SPDXRef-RootPackage").versionInfo;

  const packages = [{
    name: "sourcemeta-one-enterprise",
    SPDXID: "SPDXRef-RootPackage",
    versionInfo: version,
    downloadLocation: "https://github.com/sourcemeta/one",
    filesAnalyzed: false,
    licenseConcluded: "LicenseRef-Sourcemeta-Commercial",
    licenseDeclared: "LicenseRef-Sourcemeta-Commercial"
  }];

  const relationships = [{
    spdxElementId: "SPDXRef-DOCUMENT",
    relationshipType: "DESCRIBES",
    relatedSpdxElement: "SPDXRef-RootPackage"
  }];

  let index = 0;

  for (const entry of vendor.packages) {
    if (entry.SPDXID === "SPDXRef-RootPackage") continue;
    index += 1;
    const spdxid = `SPDXRef-Vendor-${index}`;
    packages.push({ ...entry, SPDXID: spdxid });
    relationships.push({
      spdxElementId: "SPDXRef-RootPackage",
      relationshipType: "DEPENDS_ON",
      relatedSpdxElement: spdxid
    });
  }

  for (const entry of npm.packages || []) {
    if (entry.SPDXID === "SPDXRef-RootPackage") continue;
    index += 1;
    const spdxid = `SPDXRef-Npm-${index}`;
    packages.push({ ...entry, SPDXID: spdxid });
    relationships.push({
      spdxElementId: "SPDXRef-RootPackage",
      relationshipType: "DEPENDS_ON",
      relatedSpdxElement: spdxid
    });
  }

  for (const line of dpkg.split("\n")) {
    if (!line.trim()) continue;
    const [name, entryVersion, homepage] = line.split("\t");
    index += 1;
    const spdxid = `SPDXRef-Dpkg-${index}`;
    packages.push({
      name,
      SPDXID: spdxid,
      versionInfo: entryVersion,
      downloadLocation: homepage || "NOASSERTION",
      filesAnalyzed: false,
      licenseConcluded: "NOASSERTION",
      licenseDeclared: "NOASSERTION"
    });
    relationships.push({
      spdxElementId: "SPDXRef-RootPackage",
      relationshipType: "DEPENDS_ON",
      relatedSpdxElement: spdxid
    });
  }

  // SPDX 2.3 requires every LicenseRef- identifier a document references to be
  // described here. This document declares one for itself and inherits others
  // from the documents it merges, so both are collected and de-duplicated
  const extractedLicenses = [{
    licenseId: "LicenseRef-Sourcemeta-Commercial",
    name: "Sourcemeta One Commercial License",
    extractedText: readFileSync(join(root, "LICENSE-COMMERCIAL"), "utf-8"),
    seeAlsos: [ "https://one.sourcemeta.com/commercial/" ]
  }];

  for (const entry of [
    ...(vendor.hasExtractedLicensingInfos || []),
    ...(npm.hasExtractedLicensingInfos || [])
  ]) {
    if (extractedLicenses.some((seen) => seen.licenseId === entry.licenseId)) {
      continue;
    }

    extractedLicenses.push(entry);
  }

  writeFileSync(output, JSON.stringify({
    spdxVersion: "SPDX-2.3",
    dataLicense: "CC0-1.0",
    SPDXID: "SPDXRef-DOCUMENT",
    name: "sourcemeta-one-enterprise",
    documentNamespace: `https://one.sourcemeta.com/sbom/${version}`,
    creationInfo: {
      created: new Date().toISOString(),
      creators: [ "Tool: enterprise/scripts/sbom.js" ]
    },
    hasExtractedLicensingInfos: extractedLicenses,
    packages,
    relationships
  }, null, 2) + "\n");
} finally {
  rmSync(workdir, { recursive: true, force: true });
}
