import { useContext, useEffect, useState } from "react";
import { AppContext } from "../contexts/AppContext";
import { listDirectory, searchSchemas } from "../api/one";
import type { DirectoryEntry, SearchResult } from "../types/one";
import DirectoryNode from "./DirectoryNode";

const describeError = (err: unknown, registryUrl: string): string => {
  const message = err instanceof Error ? err.message : String(err);
  // A raw fetch failure (unreachable host, CORS, offline) surfaces as the
  // browser's generic "NetworkError"/"Failed to fetch" — not useful on its
  // own, especially with the default localhost registry on a hosted deploy.
  if (/networkerror|failed to fetch/i.test(message)) {
    return `Couldn't reach the registry at ${registryUrl}. Make sure it's running and reachable, or click "Connect" to point at a different URL.`;
  }
  return message;
};

const SchemaPicker = () => {
  const { registryUrl, selectedSchemaPath, setSelectedSchemaPath } =
    useContext(AppContext);

  const [query, setQuery] = useState("");
  const [searchResults, setSearchResults] = useState<SearchResult[] | null>(
    null
  );
  const [rootEntries, setRootEntries] = useState<DirectoryEntry[] | null>(
    null
  );
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    let cancelled = false;
    setError(null);
    listDirectory(registryUrl)
      .then((listing) => {
        if (!cancelled) {
          setRootEntries(listing.entries);
          setError(null);
        }
      })
      .catch((err: unknown) => {
        if (!cancelled) setError(describeError(err, registryUrl));
      });
    return () => {
      cancelled = true;
    };
  }, [registryUrl]);

  useEffect(() => {
    if (!query.trim()) {
      setSearchResults(null);
      return;
    }
    const handle = setTimeout(() => {
      searchSchemas(registryUrl, query.trim())
        .then(setSearchResults)
        .catch(() => setSearchResults([]));
    }, 250);
    return () => clearTimeout(handle);
  }, [registryUrl, query]);

  return (
    <div className="flex flex-col h-full rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] shadow-[var(--shadow-sm)] w-72 shrink-0 overflow-hidden">
      <div className="p-2 border-b border-[var(--border)]">
        <input
          value={query}
          onChange={(e) => setQuery(e.target.value)}
          placeholder="Search schemas…"
          className="w-full h-8 px-2.5 text-sm rounded-[var(--radius-sm)] border border-[var(--border)] bg-[var(--bg-inset)] text-[var(--text)] outline-none focus:border-[var(--accent)] transition-colors"
        />
      </div>

      <div className="flex-1 overflow-y-auto p-1.5">
        {error && <p className="text-xs text-[var(--danger)] p-2">{error}</p>}

        {searchResults !== null ? (
          searchResults.length === 0 ? (
            <p className="text-xs text-[var(--text-secondary)] p-2">
              No matches
            </p>
          ) : (
            searchResults.map((result) => (
              <button
                key={result.path}
                onClick={() => setSelectedSchemaPath(result.path)}
                className={`w-full text-left text-sm py-1.5 px-2 truncate rounded-[var(--radius-sm)] ${
                  selectedSchemaPath === result.path
                    ? "bg-[var(--accent)]/15 text-[var(--accent)]"
                    : "text-[var(--text-nav)] hover:bg-[var(--bg-inset)]"
                }`}
                title={result.path}
              >
                📄 {result.title || result.path}
              </button>
            ))
          )
        ) : (
          rootEntries?.map((entry) => (
            <DirectoryNode key={entry.path} entry={entry} depth={0} />
          ))
        )}
      </div>
    </div>
  );
};

export default SchemaPicker;
