import { useContext, useState } from "react";
import { AppContext } from "../contexts/AppContext";
import Logo from "./Logo";

const RegistryBar = ({ onOpenCustomDebugger }: { onOpenCustomDebugger: () => void }) => {
  const { registryUrl, setRegistryUrl, registryHealthy } = useContext(AppContext);
  const [draft, setDraft] = useState(registryUrl);

  const apply = (e: React.FormEvent) => {
    e.preventDefault();
    if (draft.trim()) setRegistryUrl(draft.trim().replace(/\/+$/, ""));
  };

  return (
    <form
      onSubmit={apply}
      className="flex items-center gap-2 px-3 py-2 rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] shadow-[var(--shadow-sm)]"
    >
      <Logo />
      <span className="text-sm text-[var(--text-secondary)] whitespace-nowrap">
        Registry
      </span>
      <span
        title={
          registryHealthy === null
            ? "Checking registry health…"
            : registryHealthy
              ? "Registry is reachable"
              : "Registry health check failed"
        }
        className={`w-2 h-2 rounded-full shrink-0 ${
          registryHealthy === null
            ? "bg-[var(--border-strong)] animate-pulse"
            : registryHealthy
              ? "bg-[var(--success)]"
              : "bg-[var(--danger)]"
        }`}
      />
      <input
        value={draft}
        onChange={(e) => setDraft(e.target.value)}
        placeholder="http://localhost:8000"
        className="flex-1 min-w-0 h-8 px-2.5 text-sm rounded-[var(--radius-sm)] border border-[var(--border)] bg-[var(--bg-inset)] text-[var(--text)] outline-none focus:border-[var(--accent)] transition-colors"
      />
      <button
        type="submit"
        className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--border-strong)] bg-[var(--bg-inset)] text-[var(--text)] hover:border-[var(--accent)] hover:text-[var(--accent)] transition-colors"
      >
        Connect
      </button>
      <button
        type="button"
        onClick={onOpenCustomDebugger}
        className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--accent)]/50 bg-[var(--accent)]/12 text-[var(--accent)] hover:bg-[var(--accent)]/20 transition-colors whitespace-nowrap"
      >
        Custom Debugger
      </button>
    </form>
  );
};

export default RegistryBar;
