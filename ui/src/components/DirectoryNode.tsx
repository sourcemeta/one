import { useContext, useState } from "react";
import { AppContext } from "../contexts/AppContext";
import { listDirectory } from "../api/one";
import type { DirectoryEntry } from "../types/one";

const DirectoryNode = ({
  entry,
  depth,
}: {
  entry: DirectoryEntry;
  depth: number;
}) => {
  const { registryUrl, selectedSchemaPath, setSelectedSchemaPath } =
    useContext(AppContext);
  const [expanded, setExpanded] = useState(false);
  const [children, setChildren] = useState<DirectoryEntry[] | null>(null);
  const [loading, setLoading] = useState(false);

  const indent = { paddingLeft: `${depth * 14 + 10}px` };

  if (entry.type === "schema") {
    const isSelected = selectedSchemaPath === entry.path;
    return (
      <button
        onClick={() => setSelectedSchemaPath(entry.path)}
        style={indent}
        className={`w-full text-left text-sm py-1 pr-2 flex items-center gap-2 rounded-[var(--radius-sm)] ${
          isSelected
            ? "bg-[var(--accent)]/15 text-[var(--accent)]"
            : "text-[var(--text-nav)] hover:bg-[var(--bg-inset)]"
        }`}
        title={entry.path}
      >
        <span className="truncate">📄 {entry.name}</span>
      </button>
    );
  }

  const toggle = async () => {
    const next = !expanded;
    setExpanded(next);
    if (next && children === null) {
      setLoading(true);
      try {
        const listing = await listDirectory(
          registryUrl,
          entry.path.replace(/^\/|\/$/g, "")
        );
        setChildren(listing.entries);
      } finally {
        setLoading(false);
      }
    }
  };

  return (
    <div>
      <button
        onClick={toggle}
        style={indent}
        className="w-full text-left text-sm py-1 pr-2 text-[var(--text)] hover:bg-[var(--bg-inset)] rounded-[var(--radius-sm)] flex items-center gap-1"
      >
        <span className="w-3 inline-block text-[var(--text-secondary)]">
          {expanded ? "▾" : "▸"}
        </span>
        <span className="truncate">📁 {entry.name}</span>
        <span className="ml-auto flex items-center gap-2 shrink-0">
          <span className="text-[var(--text-secondary)] text-xs">
            {entry.schemas}
          </span>
        </span>
      </button>
      {expanded && loading && (
        <div style={indent} className="text-xs text-[var(--text-secondary)] py-1">
          Loading…
        </div>
      )}
      {expanded &&
        children?.map((child) => (
          <DirectoryNode key={child.path} entry={child} depth={depth + 1} />
        ))}
    </div>
  );
};

export default DirectoryNode;
