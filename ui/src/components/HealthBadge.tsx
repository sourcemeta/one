const colorFor = (health: number): string => {
  if (health >= 80) return "border-[var(--success)]/50 text-[var(--success)]";
  if (health >= 40) return "border-[var(--warning)]/50 text-[var(--warning)]";
  return "border-[var(--danger)]/50 text-[var(--danger)]";
};

const HealthBadge = ({ health }: { health: number }) => (
  <span
    className={`text-[10px] font-mono px-1.5 py-0.5 rounded-full border bg-transparent shrink-0 ${colorFor(
      health
    )}`}
  >
    {health}%
  </span>
);

export default HealthBadge;
