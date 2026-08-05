function pluralize(count, singular, plural) {
  return count === 1 ? singular : plural;
}

function schemaLink(url, origin) {
  if (url.startsWith(origin)) {
    let path = url.substring(origin.length);
    if (path.endsWith(".json")) {
      path = path.substring(0, path.length - 5);
    }

    const anchor = document.createElement("a");
    anchor.href = path;
    anchor.textContent = path;
    const code = document.createElement("code");
    code.appendChild(anchor);
    return code;
  }

  const code = document.createElement("code");
  code.textContent = url;
  return code;
}

document.querySelectorAll("[data-sourcemeta-ui-dependencies]").forEach(async (element) => {
  const path = element.getAttribute("data-sourcemeta-ui-dependencies");
  const origin = window.location.origin;
  const identifier = element.getAttribute("data-sourcemeta-ui-identifier");
  const response = await window.fetch(`/self/v1/api/schemas/dependencies${path}`);
  if (!response.ok) {
    element.textContent = "Failed to load dependencies.";
    return;
  }

  const dependencies = await response.json();
  const direct = dependencies.filter((dependency) => dependency.from === identifier);
  const indirect = dependencies.filter((dependency) => dependency.from !== identifier);

  element.innerHTML = "";
  const summary = document.createElement("p");
  summary.textContent = "This schema has " +
    direct.length + " direct " + pluralize(direct.length, "dependency", "dependencies") +
    " and " + indirect.length + " indirect " +
    pluralize(indirect.length, "dependency", "dependencies") + ".";
  element.appendChild(summary);

  if (dependencies.length > 0) {
    const table = document.createElement("table");
    table.className = "table table-bordered";

    const thead = document.createElement("thead");
    const headerRow = document.createElement("tr");
    const originHeader = document.createElement("th");
    originHeader.scope = "col";
    originHeader.textContent = "Origin";
    const depHeader = document.createElement("th");
    depHeader.scope = "col";
    depHeader.textContent = "Dependency";
    headerRow.appendChild(originHeader);
    headerRow.appendChild(depHeader);
    thead.appendChild(headerRow);
    table.appendChild(thead);

    const tbody = document.createElement("tbody");
    for (const dependency of dependencies) {
      const row = document.createElement("tr");
      const originCell = document.createElement("td");
      if (dependency.from === identifier) {
        const anchor = document.createElement("a");
        anchor.href = "#";
        anchor.setAttribute("data-sourcemeta-ui-editor-highlight", path);
        anchor.setAttribute("data-sourcemeta-ui-editor-highlight-pointers",
          JSON.stringify(dependency.at));
        const code = document.createElement("code");
        code.textContent = dependency.at;
        anchor.appendChild(code);
        originCell.appendChild(anchor);
      } else {
        const badge = document.createElement("span");
        badge.className = "badge text-bg-dark";
        badge.textContent = "Indirect";
        originCell.appendChild(badge);
      }

      const depCell = document.createElement("td");
      depCell.appendChild(schemaLink(dependency.to, origin));
      row.appendChild(originCell);
      row.appendChild(depCell);
      tbody.appendChild(row);
    }

    table.appendChild(tbody);
    element.appendChild(table);
  }

  const badge = document.querySelector(
    "[data-sourcemeta-ui-dependencies-count]");
  if (badge) {
    badge.textContent = String(dependencies.length);
  }
});

document.querySelectorAll("[data-sourcemeta-ui-dependents]").forEach(async (element) => {
  const path = element.getAttribute("data-sourcemeta-ui-dependents");
  const origin = window.location.origin;
  const identifier = element.getAttribute("data-sourcemeta-ui-identifier");
  const response = await window.fetch(`/self/v1/api/schemas/dependents${path}`);
  if (!response.ok) {
    element.textContent = "Failed to load dependents.";
    return;
  }

  const dependents = await response.json();
  const directSchemas = new Set();
  const indirectSchemas = new Set();
  for (const dependent of dependents) {
    if (dependent.to === identifier) {
      directSchemas.add(dependent.from);
    } else {
      indirectSchemas.add(dependent.from);
    }
  }

  for (const schema of directSchemas) {
    indirectSchemas.delete(schema);
  }

  element.innerHTML = "";
  const summary = document.createElement("p");
  summary.textContent = "This schema has " +
    directSchemas.size + " direct " +
    pluralize(directSchemas.size, "dependent", "dependents") +
    " and " + indirectSchemas.size + " indirect " +
    pluralize(indirectSchemas.size, "dependent", "dependents") + ".";
  element.appendChild(summary);

  if (directSchemas.size > 0 || indirectSchemas.size > 0) {
    const table = document.createElement("table");
    table.className = "table table-bordered";

    const thead = document.createElement("thead");
    const headerRow = document.createElement("tr");
    const typeHeader = document.createElement("th");
    typeHeader.scope = "col";
    typeHeader.textContent = "Type";
    const depHeader = document.createElement("th");
    depHeader.scope = "col";
    depHeader.textContent = "Dependent";
    headerRow.appendChild(typeHeader);
    headerRow.appendChild(depHeader);
    thead.appendChild(headerRow);
    table.appendChild(thead);

    const tbody = document.createElement("tbody");

    const renderRow = (schema, isDirect) => {
      const row = document.createElement("tr");
      const typeCell = document.createElement("td");
      const badge = document.createElement("span");
      badge.className = isDirect ? "badge text-bg-primary" : "badge text-bg-dark";
      badge.textContent = isDirect ? "Direct" : "Indirect";
      typeCell.appendChild(badge);
      const depCell = document.createElement("td");
      depCell.appendChild(schemaLink(schema, origin));
      row.appendChild(typeCell);
      row.appendChild(depCell);
      tbody.appendChild(row);
    };

    for (const schema of Array.from(directSchemas).sort()) {
      renderRow(schema, true);
    }

    for (const schema of Array.from(indirectSchemas).sort()) {
      renderRow(schema, false);
    }

    table.appendChild(tbody);
    element.appendChild(table);
  }

  const badge = document.querySelector(
    "[data-sourcemeta-ui-dependents-count]");
  if (badge) {
    badge.textContent = String(directSchemas.size + indirectSchemas.size);
  }
});
