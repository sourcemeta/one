import { useContext } from "react";
import { AppContext } from "../contexts/AppContext";
import Logo from "./Logo";

const RegistryBar = ({ onOpenCustomDebugger }: { onOpenCustomDebugger: () => void }) => {
  const { registryUrl, registryHealthy } = useContext(AppContext);

  return (
    <div className="flex items-center gap-2 px-3 py-2 rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] shadow-[var(--shadow-sm)]">
      <Logo />
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
      <span className="flex-1 min-w-0 text-sm text-[var(--text-secondary)] truncate">
        {registryUrl}
      </span>
      <button
        type="button"
        onClick={onOpenCustomDebugger}
        className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--accent)]/50 bg-[var(--accent)]/12 text-[var(--accent)] hover:bg-[var(--accent)]/20 transition-colors whitespace-nowrap"
      >
        Custom Debugger
      </button>
    </div>
  );
};

export default RegistryBar;
