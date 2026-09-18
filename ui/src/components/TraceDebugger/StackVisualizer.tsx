import type { OpenFrame } from "../../utils/traceStack";

const statusStyle: Record<OpenFrame["status"], string> = {
  push: "border-[var(--border-strong)] bg-[var(--bg-inset)] text-[var(--text)]",
  pass: "border-[var(--success)] bg-[var(--success-soft)] text-[var(--success)]",
  fail: "border-[var(--danger)] bg-[var(--danger-soft)] text-[var(--danger)]",
};

const annotationStyle =
  "border-[var(--info)] bg-[var(--info-soft)] text-[var(--info)]";

const StackVisualizer = ({
  frames,
  fontFamily,
}: {
  frames: OpenFrame[];
  fontFamily?: string;
}) => (
  <div
    className="relative flex-1 min-h-0 flex flex-col-reverse items-center justify-start gap-0 py-8 overflow-hidden"
    style={{ perspective: "900px" }}
  >
    {frames.length === 0 && (
      <span className="text-xs text-[var(--text-secondary)]">
        Stack is empty — step forward to begin
      </span>
    )}
    {frames.map((frame, depth) => {
      const distanceFromTop = frames.length - 1 - depth;
      const hasAnnotation = frame.status === "pass" && frame.annotation != null;
      return (
      <div
        key={frame.pushIndex}
        className={`w-[min(92%,28rem)] shrink-0 rounded-[var(--radius-sm)] border px-4 py-3 text-sm shadow-[var(--shadow-md)] transition-all duration-300 ease-out ${hasAnnotation ? annotationStyle : statusStyle[frame.status]}`}
        style={{
          fontFamily,
          transform: `translateZ(${-distanceFromTop * 34}px) translateY(${-distanceFromTop * 10}px) scale(${Math.max(1 - distanceFromTop * 0.035, 0.7)})`,
          opacity: Math.max(1 - distanceFromTop * 0.08, 0.35),
          marginTop: depth === 0 ? 0 : -30,
          zIndex: depth + 1,
        }}
      >
        <div className="flex items-center justify-between gap-2">
          <span className="uppercase text-xs tracking-wide opacity-70">
            {hasAnnotation ? "annotation" : frame.status}
          </span>
          <span className="text-xs opacity-50">#{frame.pushIndex + 1}</span>
        </div>
        <div className="truncate font-semibold text-base mt-0.5">{frame.name}</div>
        <div className="truncate opacity-70">{frame.evaluatePath}</div>
        {hasAnnotation && (
          <div className="truncate mt-1 text-xs opacity-90">
            → {JSON.stringify(frame.annotation)}
          </div>
        )}
      </div>
      );
    })}
  </div>
);

export default StackVisualizer;
