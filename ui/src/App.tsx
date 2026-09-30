import { useContext, useEffect, useState } from "react";
import { AppProvider } from "./contexts/AppProvider";
import { AppContext } from "./contexts/AppContext";
import RegistryBar from "./components/RegistryBar";
import SchemaPicker from "./components/SchemaPicker";
import InstanceEditor from "./components/InstanceEditor";
import ResultPanel from "./components/ResultPanel";
import TraceDebugger from "./components/TraceDebugger/TraceDebugger";
import CustomDebugger from "./components/CustomDebugger/CustomDebugger";
import SourcemetaMark from "./components/SourcemetaMark";

// A real (bookmarkable/shareable) route for the debugger. Hash-based
// because this is a static GitHub Pages deploy with no server-side routing
// or SPA fallback — a path like /one-ui/debugger would 404 on direct load.
const CUSTOM_DEBUGGER_HASH = "#/debugger";

const isCustomDebuggerRoute = () => window.location.hash === CUSTOM_DEBUGGER_HASH;

// Since the URL (and thus the selected schema) now survives a reload, most
// loads land directly on a schema rather than the empty idle state — this
// intro plays regardless, as a brief splash over whatever's underneath,
// instead of only showing up when there happens to be nothing selected yet.
const INTRO_DURATION_MS = 2000;

const AppShell = () => {
  const { debuggerOpen } = useContext(AppContext);
  const [customDebuggerOpen, setCustomDebuggerOpen] = useState(isCustomDebuggerRoute);
  const [introDone, setIntroDone] = useState(false);

  useEffect(() => {
    const onHashChange = () => setCustomDebuggerOpen(isCustomDebuggerRoute());
    window.addEventListener("hashchange", onHashChange);
    return () => window.removeEventListener("hashchange", onHashChange);
  }, []);

  useEffect(() => {
    const timer = setTimeout(() => setIntroDone(true), INTRO_DURATION_MS);
    return () => clearTimeout(timer);
  }, []);

  const openCustomDebugger = () => {
    window.location.hash = CUSTOM_DEBUGGER_HASH;
  };
  const closeCustomDebugger = () => {
    window.location.hash = "";
  };

  const content = debuggerOpen ? (
    <TraceDebugger />
  ) : customDebuggerOpen ? (
    <CustomDebugger onClose={closeCustomDebugger} />
  ) : (
    <div className="flex flex-col h-full bg-[var(--bg-canvas)] p-3 gap-3">
      <RegistryBar onOpenCustomDebugger={openCustomDebugger} />
      <div className="flex flex-1 min-h-0 gap-3">
        <SchemaPicker />
        <InstanceEditor />
        <ResultPanel />
      </div>
    </div>
  );

  return (
    <>
      {/* inert while the splash covers it — otherwise Tab/Enter can still
          reach and activate controls hidden underneath the opaque overlay */}
      <div inert={!introDone} className="h-full">
        {content}
      </div>
      <div
        aria-hidden="true"
        className="fixed inset-0 z-50 flex flex-col items-center justify-center gap-3 bg-[var(--bg-canvas)]"
        style={{
          opacity: introDone ? 0 : 1,
          transition: "opacity 1s ease-in",
          pointerEvents: introDone ? "none" : "auto",
        }}
      >
        <SourcemetaMark className="w-30 h-30 animate-logo-pop text-[var(--text)]" />
        <span className="text-xs tracking-widest uppercase text-[var(--text-secondary)]">
          Sourcemeta
        </span>
      </div>
    </>
  );
};

function App() {
  return (
    <AppProvider>
      <AppShell />
    </AppProvider>
  );
}

export default App;
