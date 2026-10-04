import { useContext } from "react";
import { AppContext } from "../contexts/AppContext";
import Logo from "./Logo";

const RegistryBar = ({ onOpenCustomDebugger }: { onOpenCustomDebugger: () => void }) => {
  const { registryUrl, registryHealthy, theme, toggleTheme } = useContext(AppContext);

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
        onClick={toggleTheme}
        title={theme === "light" ? "Switch to dark mode" : "Switch to light mode"}
        aria-label={theme === "light" ? "Switch to dark mode" : "Switch to light mode"}
        className="h-8 w-8 shrink-0 flex items-center justify-center rounded-[var(--radius-sm)] border border-[var(--border-strong)] bg-[var(--bg-inset)] text-[var(--text-secondary)] hover:border-[var(--accent)] hover:text-[var(--accent)] transition-colors"
      >
        {theme === "light" ? (
          <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round">
            <path d="M21 12.8A9 9 0 1 1 11.2 3a7 7 0 0 0 9.8 9.8Z" />
          </svg>
        ) : (
          <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round">
            <circle cx="12" cy="12" r="4" />
            <path d="M12 2v2M12 20v2M4.9 4.9l1.4 1.4M17.7 17.7l1.4 1.4M2 12h2M20 12h2M4.9 19.1l1.4-1.4M17.7 6.3l1.4-1.4" />
          </svg>
        )}
      </button>
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
