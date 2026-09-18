const colorFor = (health: number): string => {
  if (health >= 80) return "var(--success)";
  if (health >= 40) return "var(--warning)";
  return "var(--danger)";
};

const HealthBar = ({ health }: { health: number }) => (
  <div className="flex items-center gap-2 w-full max-w-xs">
    <div className="flex-1 h-3.5 rounded-full bg-[var(--bg-inset)] overflow-hidden">
      <div
        className="h-full rounded-full transition-all"
        style={{ width: `${health}%`, background: colorFor(health) }}
      />
    </div>
    <span className="text-xs font-mono text-[var(--text-secondary)] w-9 text-right">
      {health}%
    </span>
  </div>
);

export default HealthBar;
