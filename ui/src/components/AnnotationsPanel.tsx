import type { CollectedAnnotation } from "../utils/traceStack";

const formatAnnotation = (value: unknown): string => {
  if (typeof value === "string") return `"${value}"`;
  try {
    return JSON.stringify(value);
  } catch {
    return String(value);
  }
};

const formatAnnotationList = (values: unknown[]): string =>
  `[${values.map(formatAnnotation).join(", ")}]`;

const AnnotationsPanel = ({
  annotations,
  currentInstanceLocation,
}: {
  annotations: CollectedAnnotation[];
  currentInstanceLocation?: string;
}) => (
  <div className="w-72 shrink-0 flex flex-col rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] overflow-hidden">
    <div className="px-3 py-1.5 border-b border-[var(--border)]">
      <div className="text-sm text-[var(--text-secondary)]">Annotations</div>
      <div className="text-xs text-[var(--text-secondary)] opacity-70 mt-0.5">
        Collected so far, in emission order
      </div>
    </div>
    <div className="flex-1 min-h-0 overflow-y-auto p-2 flex flex-col gap-1.5">
      {annotations.length === 0 && (
        <span className="text-sm text-[var(--text-secondary)] p-1">
          No annotations emitted yet
        </span>
      )}
      {annotations.map((entry) => {
        const isCurrentScope = entry.instanceLocation === currentInstanceLocation;
        return (
          <div
            key={entry.stepIndex}
            className={`rounded-[var(--radius-sm)] border px-2 py-1.5 text-sm transition-opacity ${
              isCurrentScope
                ? "border-[var(--info)]/50 bg-[var(--info-soft)]"
                : "border-[var(--border)] bg-[var(--bg-inset)] opacity-60"
            }`}
          >
            <div className="break-words text-[var(--info)]">
              {formatAnnotationList(entry.values)}
            </div>
            <div
              className="text-xs text-[var(--text-secondary)] truncate"
              title={`at ${entry.keywordLocation || "/"} · instance ${entry.instanceLocation || "/"}`}
            >
              at {entry.keywordLocation || "/"} · instance {entry.instanceLocation || "/"}
            </div>
          </div>
        );
      })}
    </div>
  </div>
);

export default AnnotationsPanel;
