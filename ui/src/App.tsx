import { useContext, useEffect, useState } from "react";
import { AppProvider } from "./contexts/AppProvider";
import { AppContext } from "./contexts/AppContext";
import RegistryBar from "./components/RegistryBar";
import SchemaPicker from "./components/SchemaPicker";
import InstanceEditor from "./components/InstanceEditor";
import ResultPanel from "./components/ResultPanel";
import TraceDebugger from "./components/TraceDebugger/TraceDebugger";
import CustomDebugger from "./components/CustomDebugger/CustomDebugger";

// A real (bookmarkable/shareable) route for the debugger. Hash-based
// because this is a static GitHub Pages deploy with no server-side routing
// or SPA fallback — a path like /one-ui/debugger would 404 on direct load.
const CUSTOM_DEBUGGER_HASH = "#/debugger";

const isCustomDebuggerRoute = () => window.location.hash === CUSTOM_DEBUGGER_HASH;

const AppShell = () => {
  const { debuggerOpen } = useContext(AppContext);
  const [customDebuggerOpen, setCustomDebuggerOpen] = useState(isCustomDebuggerRoute);

  useEffect(() => {
    const onHashChange = () => setCustomDebuggerOpen(isCustomDebuggerRoute());
    window.addEventListener("hashchange", onHashChange);
    return () => window.removeEventListener("hashchange", onHashChange);
  }, []);

  const openCustomDebugger = () => {
    window.location.hash = CUSTOM_DEBUGGER_HASH;
  };
  const closeCustomDebugger = () => {
    window.location.hash = "";
  };

  if (debuggerOpen) return <TraceDebugger />;
  if (customDebuggerOpen) return <CustomDebugger onClose={closeCustomDebugger} />;

  return (
    <div className="flex flex-col h-full bg-[var(--bg-canvas)] p-3 gap-3">
      <RegistryBar onOpenCustomDebugger={openCustomDebugger} />
      <div className="flex flex-1 min-h-0 gap-3">
        <SchemaPicker />
        <InstanceEditor />
        <ResultPanel />
      </div>
    </div>
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
