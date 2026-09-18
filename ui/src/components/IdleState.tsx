import { useEffect, useState } from "react";
import sourcemetaMark from "../assets/sourcemeta-mark.svg";

const INTRO_DURATION_MS = 2000;

const IdleState = () => {
  const [introDone, setIntroDone] = useState(false);

  useEffect(() => {
    const timer = setTimeout(() => setIntroDone(true), INTRO_DURATION_MS);
    return () => clearTimeout(timer);
  }, []);

  return (
    <div className="relative flex flex-col items-center justify-center gap-3 flex-1 min-w-0 rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] shadow-[var(--shadow-sm)] overflow-hidden">
      <div
        className="absolute inset-0 flex flex-col items-center justify-center gap-3"
        style={{
          opacity: introDone ? 0 : 1,
          transition: "opacity 1s ease-in",
          pointerEvents: introDone ? "none" : "auto",
        }}
      >
        <img
          src={sourcemetaMark}
          alt=""
          className="w-30 h-30 animate-logo-pop"
        />
        <span className="text-xs tracking-widest uppercase text-[var(--text-secondary)]">
          Sourcemeta
        </span>
      </div>

      <p
        className="text-sm text-[var(--text-secondary)]"
        style={{
          opacity: introDone ? 1 : 0,
          transition: "opacity 1s ease-in",
        }}
      >
        Select a schema to explore
      </p>
    </div>
  );
};

export default IdleState;
