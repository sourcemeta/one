import { useContext } from "react";
import { AppContext, type DetailTab } from "../contexts/AppContext";

// Strips the registry's own origin (and a trailing .json) off an absolute
// schema URL so it can be used as a router path — mirrors the old UI's
// schemaLink() helper in dependencies.js. A URL that isn't hosted by this
// registry (an external $id) has nothing to navigate to, so it's left
// plain. Matches on an exact origin boundary (not just a shared string
// prefix) so an unrelated host that merely starts with the same characters
// — e.g. https://schemas.sourcemeta.com.evil.example — isn't treated as
// this registry.
const resolveSchemaPath = (url: string, registryOrigin: string): string | null => {
  const base = registryOrigin.replace(/\/+$/, "");
  if (url !== base && !url.startsWith(`${base}/`)) return null;
  const rest = url.slice(base.length);
  const path = rest.endsWith(".json") ? rest.slice(0, -5) : rest;
  return path === "" ? "/" : path;
};

const SchemaLinkCell = ({ url }: { url: string }) => {
  const { registryUrl, schemaMetadata, setSelectedSchemaPath } = useContext(AppContext);
  // The open schema's own identifier names the real registry a dependency
  // URL is hosted on. That's usually the same as registryUrl, but not in
  // dev — the dev proxy makes cross-origin API calls transparent, so
  // registryUrl there is the local Vite origin while every $id it returns
  // still points at the real upstream registry.
  let registryOrigin = registryUrl;
  try {
    if (schemaMetadata) registryOrigin = new URL(schemaMetadata.identifier).origin;
  } catch {
    // Malformed identifier — fall back to registryUrl.
  }
  const path = resolveSchemaPath(url, registryOrigin);
  if (!path) {
    return <span className="break-all">{url}</span>;
  }
  return (
    <button
      type="button"
      onClick={() => setSelectedSchemaPath(path)}
      title={url}
      className="text-left break-all hover:underline hover:text-[var(--accent)]"
    >
      {path}
    </button>
  );
};

const TabButton = ({
  tab,
  label,
  count,
}: {
  tab: DetailTab;
  label: string;
  count: number;
}) => {
  const { detailTab, setDetailTab } = useContext(AppContext);
  const active = detailTab === tab;
  return (
    <button
      onClick={() => setDetailTab(tab)}
      className={`px-3 py-1.5 text-xs flex items-center gap-1.5 border-r border-[var(--border)] ${
        active
          ? "text-[var(--text)] bg-[var(--bg-inset)]"
          : "text-[var(--text-secondary)] hover:text-[var(--text)]"
      }`}
    >
      {label}
      <span
        className={`text-[10px] px-1.5 rounded-full ${
          active
            ? "bg-[var(--border-strong)]"
            : "bg-[var(--bg-inset)] text-[var(--text-secondary)]"
        }`}
      >
        {count}
      </span>
    </button>
  );
};

const DetailPanel = ({
  onFocusPointers,
  onFocusPosition,
}: {
  onFocusPointers: (pointers: string[]) => void;
  onFocusPosition: (position: [number, number, number, number]) => void;
}) => {
  const {
    detailTab,
    dependencies,
    dependents,
    healthReport,
    schemaStats,
    schemaLocations,
    schemaMetadata,
    detailLoading,
  } = useContext(AppContext);

  const statsRows = Object.entries(schemaStats ?? {}).flatMap(
    ([vocabulary, keywords]) =>
      Object.entries(keywords).map(([keyword, count]) => ({
        vocabulary,
        keyword,
        count,
      }))
  );
  statsRows.sort((a, b) => b.count - a.count);
  const maxCount = Math.max(1, ...statsRows.map((row) => row.count));

  const staticLocations = Object.entries(schemaLocations?.static ?? {});
  const dynamicLocations = Object.entries(schemaLocations?.dynamic ?? {});

  return (
    <div className="flex flex-col flex-1 min-h-0">
      <div className="flex border-b border-[var(--border)] overflow-x-auto shrink-0">
        <TabButton
          tab="dependencies"
          label="Dependencies"
          count={dependencies?.length ?? 0}
        />
        <TabButton
          tab="dependents"
          label="Dependents"
          count={dependents?.length ?? 0}
        />
        <TabButton
          tab="lint"
          label="Lint"
          count={healthReport?.errors.length ?? 0}
        />
        <TabButton tab="stats" label="Stats" count={statsRows.length} />
        <TabButton
          tab="locations"
          label="Locations"
          count={staticLocations.length + dynamicLocations.length}
        />
      </div>

      <div className="p-2 flex-1 min-h-0 overflow-y-auto">
        {detailLoading && (
          <p className="text-xs text-[var(--text-secondary)] px-1">
            Loading…
          </p>
        )}

        {!detailLoading && detailTab === "dependencies" && (
          <table className="w-full text-xs">
            <thead>
              <tr className="text-left text-[var(--text-secondary)]">
                <th className="font-medium pb-1 pr-2">Origin</th>
                <th className="font-medium pb-1">Dependency</th>
              </tr>
            </thead>
            <tbody>
              {dependencies?.length === 0 && (
                <tr>
                  <td colSpan={2} className="text-[var(--text-secondary)] opacity-60 py-1">
                    No dependencies
                  </td>
                </tr>
              )}
              {dependencies?.map((edge, i) => {
                // /at only points into this schema for a direct edge — an
                // indirect one names a spot in whichever intermediate schema
                // it actually came from, which isn't open here to highlight.
                const local = edge.from === schemaMetadata?.identifier;
                return (
                  <tr key={i} className="border-t border-[var(--border)]">
                    <td className="py-1 pr-2 text-[var(--info)] align-top">
                      {local ? (
                        <button
                          type="button"
                          onClick={() => onFocusPointers([edge.at])}
                          className="text-left hover:underline"
                        >
                          {edge.at}
                        </button>
                      ) : (
                        // edge.at is a pointer into edge.from, not this
                        // schema — showing it here would look like a local
                        // origin when it isn't one.
                        <span className="text-[var(--text-secondary)] opacity-70">
                          Indirect
                        </span>
                      )}
                    </td>
                    <td className="py-1 text-[var(--text-nav)] break-all">
                      <SchemaLinkCell url={edge.to} />
                    </td>
                  </tr>
                );
              })}
            </tbody>
          </table>
        )}

        {!detailLoading && detailTab === "dependents" && (
          <table className="w-full text-xs">
            <thead>
              <tr className="text-left text-[var(--text-secondary)]">
                <th className="font-medium pb-1 pr-2">Dependent</th>
                <th className="font-medium pb-1">At</th>
              </tr>
            </thead>
            <tbody>
              {dependents?.length === 0 && (
                <tr>
                  <td colSpan={2} className="text-[var(--text-secondary)] opacity-60 py-1">
                    No dependents
                  </td>
                </tr>
              )}
              {dependents?.map((edge, i) => (
                <tr key={i} className="border-t border-[var(--border)]">
                  <td className="py-1 pr-2 text-[var(--text-nav)] break-all align-top">
                    <SchemaLinkCell url={edge.from} />
                  </td>
                  <td className="py-1 text-[var(--info)]">
                    {edge.at}
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        )}

        {!detailLoading && detailTab === "lint" && (
          <div className="flex flex-col gap-1.5">
            {healthReport?.errors.length === 0 && (
              <p className="text-xs text-[var(--text-secondary)] opacity-60">
                No lint findings
              </p>
            )}
            {healthReport?.errors.map((finding, i) => (
              <div
                key={i}
                className="text-xs border border-[var(--border)] rounded-[var(--radius-sm)] px-2 py-1.5 bg-[var(--bg-inset)]/50"
              >
                <div className="text-[var(--text-nav)]">
                  {finding.name}
                </div>
                <div className="text-[var(--text-secondary)] mt-0.5">
                  {finding.message}
                </div>
                {finding.pointers.length > 0 && finding.pointers[0] !== "" && (
                  <button
                    type="button"
                    onClick={() => onFocusPointers(finding.pointers)}
                    className="text-left text-[var(--text-secondary)] opacity-70 mt-0.5 hover:underline hover:opacity-100 hover:text-[var(--accent)]"
                  >
                    {finding.pointers.join(", ")}
                  </button>
                )}
              </div>
            ))}
          </div>
        )}

        {!detailLoading && detailTab === "stats" && (
          <div className="flex flex-col gap-1.5">
            {statsRows.length === 0 && (
              <p className="text-xs text-[var(--text-secondary)] opacity-60">
                No keyword usage recorded
              </p>
            )}
            {statsRows.map((row, i) => (
              <div key={i} className="text-sm">
                <div className="flex items-center justify-between gap-2">
                  <span className="text-[var(--text-nav)] truncate">
                    {row.keyword}
                  </span>
                  <span className="text-[var(--text-secondary)] shrink-0">
                    {row.count}
                  </span>
                </div>
                <div className="h-1 mt-0.5 rounded-full bg-[var(--bg-inset)] overflow-hidden">
                  <div
                    className="h-full bg-[var(--info)]"
                    style={{ width: `${(row.count / maxCount) * 100}%` }}
                  />
                </div>
                <div
                  className="text-xs text-[var(--text-secondary)] opacity-60 truncate mt-0.5"
                  title={row.vocabulary}
                >
                  {row.vocabulary}
                </div>
              </div>
            ))}
          </div>
        )}

        {!detailLoading && detailTab === "locations" && (
          <div className="flex flex-col gap-3">
            {staticLocations.length === 0 && dynamicLocations.length === 0 && (
              <p className="text-xs text-[var(--text-secondary)] opacity-60">
                No locations found
              </p>
            )}
            {staticLocations.length > 0 && (
              <div>
                <div className="text-xs text-[var(--text-secondary)] opacity-70 mb-1">
                  Static ({staticLocations.length})
                </div>
                <div className="flex flex-col gap-1">
                  {staticLocations.map(([uri, entry]) => (
                    <div
                      key={uri}
                      className="text-xs border border-[var(--border)] rounded-[var(--radius-sm)] px-2 py-1 bg-[var(--bg-inset)]/50"
                      title={uri}
                    >
                      <button
                        type="button"
                        onClick={() => onFocusPosition(entry.position)}
                        className="flex items-center justify-between gap-2 w-full text-left hover:underline"
                      >
                        <span className="text-[var(--text-nav)] truncate">
                          {entry.pointer || "/"}
                        </span>
                        <span className="text-[var(--text-secondary)] shrink-0">
                          {entry.type}
                        </span>
                      </button>
                      {entry.orphan && (
                        <div className="text-[10px] text-[var(--warning)] mt-0.5">
                          orphan (inside a definitions container)
                        </div>
                      )}
                    </div>
                  ))}
                </div>
              </div>
            )}
            {dynamicLocations.length > 0 && (
              <div>
                <div className="text-xs text-[var(--text-secondary)] opacity-70 mb-1">
                  Dynamic ({dynamicLocations.length})
                </div>
                <div className="flex flex-col gap-1">
                  {dynamicLocations.map(([uri, entry]) => (
                    <div
                      key={uri}
                      className="text-xs border border-[var(--border)] rounded-[var(--radius-sm)] px-2 py-1 bg-[var(--bg-inset)]/50"
                      title={uri}
                    >
                      <button
                        type="button"
                        onClick={() => onFocusPosition(entry.position)}
                        className="flex items-center justify-between gap-2 w-full text-left hover:underline"
                      >
                        <span className="text-[var(--text-nav)] truncate">
                          {entry.pointer || "/"}
                        </span>
                        <span className="text-[var(--text-secondary)] shrink-0">
                          {entry.type}
                        </span>
                      </button>
                    </div>
                  ))}
                </div>
              </div>
            )}
          </div>
        )}
      </div>
    </div>
  );
};

export default DetailPanel;
