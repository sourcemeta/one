import { useContext } from "react";
import { AppContext } from "../contexts/AppContext";
import type { TraceStep } from "../types/one";

const stepStyle =
  "text-[var(--text-secondary)] border-[var(--border)] bg-[var(--bg-inset)]/50";

const badgeStyle: Record<TraceStep["type"], string> = {
  push: "text-[var(--text-secondary)] bg-[var(--bg-inset)] border-[var(--border-strong)]",
  pass: "text-[var(--success)] bg-[var(--success)]/20 border-[var(--success)]/60",
  fail: "text-[var(--danger)] bg-[var(--danger)]/20 border-[var(--danger)]/60",
};

const TraceStepRow = ({ step, index }: { step: TraceStep; index: number }) => (
  <div
    className={`text-xs border rounded-[var(--radius-sm)] px-2 py-1.5 ${stepStyle}`}
  >
    <div className="flex items-center gap-2">
      <span
        className={`uppercase text-xs font-bold tracking-wide px-2.5 py-1 rounded-full border shrink-0 ${badgeStyle[step.type]}`}
      >
        {step.type}
      </span>
      <span className="font-mono truncate">{step.name}</span>
      <span className="text-[10px] opacity-50 ml-auto shrink-0">
        #{index + 1}
      </span>
    </div>
    <div className="font-mono text-[11px] opacity-70 truncate mt-1">
      {step.evaluatePath}
    </div>
    {step.message && <div className="mt-0.5 break-words">{step.message}</div>}
  </div>
);

const ResultPanel = () => {
  const {
    resultMode,
    evaluationResult,
    traceResult,
    rdfResult,
    resultLoading,
    resultError,
    openDebugger,
  } = useContext(AppContext);

  return (
    <div className="w-96 shrink-0 flex flex-col h-full overflow-y-auto rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] shadow-[var(--shadow-sm)]">
      <div className="px-3 py-2.5 border-b border-[var(--border)] text-sm font-medium text-[var(--text)] sticky top-0 bg-[var(--bg-surface)]">
        Result
      </div>

      <div className="p-3 flex flex-col gap-2 overflow-y-auto">
        {resultLoading && (
          <p className="text-sm text-[var(--text-secondary)]">Running…</p>
        )}
        {resultError && (
          <p className="text-sm text-[var(--danger)]">{resultError}</p>
        )}

        {!resultLoading && !resultError && resultMode === null && (
          <p className="text-sm text-[var(--text-secondary)]">
            Click Evaluate, Trace, or RDF to see a result here.
          </p>
        )}

        {!resultLoading && resultMode === "evaluate" && evaluationResult && (
          <>
            <p
              className={`text-sm font-medium ${
                evaluationResult.valid
                  ? "text-[var(--success)]"
                  : "text-[var(--danger)]"
              }`}
            >
              {evaluationResult.valid ? "✓ Valid" : "✗ Invalid"}
            </p>
            {evaluationResult.errors?.map((err, i) => (
              <div
                key={i}
                className="text-xs border border-[var(--danger)]/40 bg-[var(--danger-soft)] text-[var(--text)] rounded-[var(--radius-sm)] px-2 py-1.5"
              >
                <div className="font-mono text-[11px] opacity-70">
                  {err.instanceLocation || "/"}
                </div>
                <div className="mt-0.5 break-words">{err.error}</div>
              </div>
            ))}
          </>
        )}

        {/* Guarded on rdfResult the way the evaluate and trace branches are
            guarded on theirs: a failed request has no output to show, and
            rendering it anyway printed a "JSON-LD" heading over the literal
            text "null" underneath the error message. */}
        {!resultLoading && resultMode === "rdf" && rdfResult !== null && (
          <>
            <p className="text-sm font-medium text-[var(--info)]">JSON-LD</p>
            {Array.isArray(rdfResult) && rdfResult.length === 0 ? (
              <p className="text-sm text-[var(--text-secondary)]">
                No JSON-LD annotations were emitted for this instance.
              </p>
            ) : (
              <pre className="text-xs font-mono border border-[var(--border)] bg-[var(--bg-inset)]/50 rounded-[var(--radius-sm)] px-2 py-1.5 whitespace-pre-wrap break-words">
                {JSON.stringify(rdfResult, null, 2)}
              </pre>
            )}
          </>
        )}

        {!resultLoading && resultMode === "trace" && traceResult && (
          <>
            <p
              className={`text-sm font-medium ${
                traceResult.valid
                  ? "text-[var(--success)]"
                  : "text-[var(--danger)]"
              }`}
            >
              {traceResult.valid ? "✓ Valid" : "✗ Invalid"} ·{" "}
              {traceResult.steps.length} steps
            </p>
            <button
              onClick={openDebugger}
              className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--accent)]/50 bg-[var(--accent)]/12 text-[var(--accent)] hover:bg-[var(--accent)]/20 transition-colors self-start"
            >
              Open Step Debugger →
            </button>
            {traceResult.steps.map((step, i) => (
              <TraceStepRow key={i} step={step} index={i} />
            ))}
          </>
        )}
      </div>
    </div>
  );
};

export default ResultPanel;
