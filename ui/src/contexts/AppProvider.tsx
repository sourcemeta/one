import { useCallback, useEffect, useRef, useState, type ReactNode } from "react";
import {
  AppContext,
  type DetailTab,
  type EditorTab,
  type ResultMode,
  type ThemeMode,
} from "./AppContext";
import {
  checkRegistryHealth,
  evaluateSchema,
  getSchemaContent,
  getSchemaDependencies,
  getSchemaDependents,
  getSchemaHealthReport,
  getSchemaLocations,
  getSchemaMetadata,
  getSchemaPositions,
  getSchemaStats,
  promoteToRdf,
  traceSchema,
} from "../api/one";
import type {
  DependencyEdge,
  EvaluationResult,
  HealthReport,
  SchemaLocations,
  SchemaMetadata,
  SchemaPositions,
  SchemaStats,
  TraceResult,
} from "../types/one";

const THEME_KEY = "one-ui:theme";

// index.html applies a stored choice to the <html> element before this ever
// runs (avoids a flash of the wrong theme) — this just reads that same
// choice back into React state so the rest of the app can react to it too.
const readStoredTheme = (): ThemeMode =>
  document.documentElement.dataset.theme === "light" ? "light" : "dark";

export const AppProvider = ({ children }: { children: ReactNode }) => {
  // This UI is served BY the registry it's meant to browse (or, in dev, by
  // a Vite proxy standing in for one — see vite.config.ts), so it's never
  // pointed anywhere else: no separate "which registry" concept to manage.
  const registryUrl = window.location.origin;

  const [theme, setTheme] = useState<ThemeMode>(readStoredTheme);

  useEffect(() => {
    document.documentElement.dataset.theme = theme;
    try {
      localStorage.setItem(THEME_KEY, theme);
    } catch {
      // Storage can be unavailable (private mode); the toggle still works
      // for the rest of the session.
    }
  }, [theme]);

  const toggleTheme = useCallback(
    () => setTheme((current) => (current === "light" ? "dark" : "light")),
    []
  );

  const [registryHealthy, setRegistryHealthy] = useState<boolean | null>(null);

  useEffect(() => {
    let cancelled = false;
    setRegistryHealthy(null);
    checkRegistryHealth(registryUrl).then((healthy) => {
      if (!cancelled) setRegistryHealthy(healthy);
    });
    return () => {
      cancelled = true;
    };
  }, [registryUrl]);

  // The server serves this same app for any path (including one that names
  // a schema), so the path itself is the source of truth for which schema
  // is selected — a reload or a shared link lands back on the same view.
  // In dev, Vite serves that app shell at `base` (see vite.config.ts)
  // instead of "/", so that path counts as root too — only in production
  // does a real page route ever land on the bare "/".
  const isRootPath = (pathname: string) =>
    pathname === "/" ||
    pathname === import.meta.env.BASE_URL ||
    pathname === import.meta.env.BASE_URL.replace(/\/+$/, "");

  const [selectedSchemaPath, setSelectedSchemaPathState] = useState<
    string | null
  >(() => (isRootPath(window.location.pathname) ? null : window.location.pathname));

  const setSelectedSchemaPath = useCallback((path: string | null) => {
    setSelectedSchemaPathState(path);
    const url = path ?? "/";
    if (window.location.pathname !== url) {
      window.history.pushState(null, "", url);
    }
  }, []);

  useEffect(() => {
    const onPopState = () => {
      const path = window.location.pathname;
      setSelectedSchemaPathState(isRootPath(path) ? null : path);
    };
    window.addEventListener("popstate", onPopState);
    return () => window.removeEventListener("popstate", onPopState);
  }, []);

  const [schemaMetadata, setSchemaMetadata] = useState<SchemaMetadata | null>(
    null
  );
  const [metadataLoading, setMetadataLoading] = useState(false);
  const [metadataError, setMetadataError] = useState<string | null>(null);

  const [activeTab, setActiveTab] = useState<EditorTab>("schema");
  const [schemaContent, setSchemaContent] = useState<string | null>(null);
  const [schemaContentLoading, setSchemaContentLoading] = useState(false);
  const [schemaContentError, setSchemaContentError] = useState<string | null>(
    null
  );

  const [instanceText, setInstanceText] = useState("{}");

  const [detailTab, setDetailTab] = useState<DetailTab>("dependencies");
  const [dependencies, setDependencies] = useState<DependencyEdge[] | null>(
    null
  );
  const [dependents, setDependents] = useState<DependencyEdge[] | null>(null);
  const [healthReport, setHealthReport] = useState<HealthReport | null>(null);
  const [schemaStats, setSchemaStats] = useState<SchemaStats | null>(null);
  const [schemaLocations, setSchemaLocations] =
    useState<SchemaLocations | null>(null);
  const [schemaPositions, setSchemaPositions] =
    useState<SchemaPositions | null>(null);
  const [detailLoading, setDetailLoading] = useState(false);

  const [resultMode, setResultMode] = useState<ResultMode | null>(null);
  const [evaluationResult, setEvaluationResult] =
    useState<EvaluationResult | null>(null);
  const [traceResult, setTraceResult] = useState<TraceResult | null>(null);
  const [rdfResult, setRdfResult] = useState<unknown>(null);
  const [resultLoading, setResultLoading] = useState(false);
  const [resultError, setResultError] = useState<string | null>(null);

  // Invalidates in-flight Evaluate/Trace/RDF requests when the schema
  // changes (or a newer of the same kind is fired) so a slow response can't
  // land after the fact and overwrite a different schema's result.
  const resultRequestRef = useRef(0);

  const [debuggerOpen, setDebuggerOpen] = useState(false);
  const openDebugger = useCallback(() => setDebuggerOpen(true), []);
  const closeDebugger = useCallback(() => setDebuggerOpen(false), []);

  useEffect(() => {
    if (!selectedSchemaPath) {
      setSchemaMetadata(null);
      setSchemaContent(null);
      return;
    }

    let cancelled = false;
    resultRequestRef.current += 1;
    setMetadataLoading(true);
    setMetadataError(null);
    setEvaluationResult(null);
    setTraceResult(null);
    setRdfResult(null);
    setResultMode(null);
    // Without this the previous schema's failure (e.g. a 422 from RDF) stays
    // in the Result panel next to a schema it has nothing to do with.
    setResultError(null);
    setResultLoading(false);
    setActiveTab("schema");

    getSchemaMetadata(registryUrl, selectedSchemaPath)
      .then((metadata) => {
        if (cancelled) return;
        setSchemaMetadata(metadata);
        const firstExample = metadata.examples?.[0];
        setInstanceText(
          firstExample !== undefined
            ? JSON.stringify(firstExample, null, 2)
            : "{}"
        );
      })
      .catch((error: unknown) => {
        if (cancelled) return;
        setSchemaMetadata(null);
        setMetadataError(
          error instanceof Error ? error.message : String(error)
        );
      })
      .finally(() => {
        if (!cancelled) setMetadataLoading(false);
      });

    setSchemaContentLoading(true);
    setSchemaContentError(null);
    getSchemaContent(registryUrl, selectedSchemaPath)
      .then((content) => {
        if (!cancelled) setSchemaContent(content);
      })
      .catch((error: unknown) => {
        if (cancelled) return;
        setSchemaContent(null);
        setSchemaContentError(
          error instanceof Error ? error.message : String(error)
        );
      })
      .finally(() => {
        if (!cancelled) setSchemaContentLoading(false);
      });

    setDetailLoading(true);
    setDependencies(null);
    setDependents(null);
    setHealthReport(null);
    setSchemaStats(null);
    setSchemaLocations(null);
    setSchemaPositions(null);
    Promise.allSettled([
      getSchemaDependencies(registryUrl, selectedSchemaPath),
      getSchemaDependents(registryUrl, selectedSchemaPath),
      getSchemaHealthReport(registryUrl, selectedSchemaPath),
      getSchemaStats(registryUrl, selectedSchemaPath),
      getSchemaLocations(registryUrl, selectedSchemaPath),
      getSchemaPositions(registryUrl, selectedSchemaPath),
    ]).then(([deps, dependentsList, health, stats, locations, positions]) => {
      if (cancelled) return;
      // Each panel section fails independently, instead of one bad endpoint
      // (e.g. stats) blanking out sections that loaded fine (e.g. dependencies).
      setDependencies(deps.status === "fulfilled" ? deps.value : []);
      setDependents(dependentsList.status === "fulfilled" ? dependentsList.value : []);
      setHealthReport(health.status === "fulfilled" ? health.value : null);
      setSchemaStats(stats.status === "fulfilled" ? stats.value : null);
      setSchemaLocations(locations.status === "fulfilled" ? locations.value : null);
      setSchemaPositions(positions.status === "fulfilled" ? positions.value : null);
      setDetailLoading(false);
    });

    return () => {
      cancelled = true;
    };
  }, [registryUrl, selectedSchemaPath]);

  const runEvaluate = useCallback(() => {
    if (!selectedSchemaPath) return;
    const generation = ++resultRequestRef.current;
    setResultLoading(true);
    setResultError(null);
    setEvaluationResult(null);
    setResultMode("evaluate");
    evaluateSchema(registryUrl, selectedSchemaPath, instanceText)
      .then((result) => {
        if (resultRequestRef.current === generation) setEvaluationResult(result);
      })
      .catch((error: unknown) => {
        if (resultRequestRef.current === generation) {
          setResultError(error instanceof Error ? error.message : String(error));
        }
      })
      .finally(() => {
        if (resultRequestRef.current === generation) setResultLoading(false);
      });
  }, [registryUrl, selectedSchemaPath, instanceText]);

  const runTrace = useCallback(() => {
    if (!selectedSchemaPath) return;
    const generation = ++resultRequestRef.current;
    setResultLoading(true);
    setResultError(null);
    setTraceResult(null);
    setResultMode("trace");
    traceSchema(registryUrl, selectedSchemaPath, instanceText)
      .then((result) => {
        if (resultRequestRef.current === generation) setTraceResult(result);
      })
      .catch((error: unknown) => {
        if (resultRequestRef.current === generation) {
          setResultError(error instanceof Error ? error.message : String(error));
        }
      })
      .finally(() => {
        if (resultRequestRef.current === generation) setResultLoading(false);
      });
  }, [registryUrl, selectedSchemaPath, instanceText]);

  const runRdf = useCallback(() => {
    if (!selectedSchemaPath) return;
    const generation = ++resultRequestRef.current;
    setResultLoading(true);
    setResultError(null);
    // A failed run must not leave the previous run's output on screen.
    setRdfResult(null);
    setResultMode("rdf");
    let instance: unknown;
    try {
      instance = JSON.parse(instanceText);
    } catch (parseError) {
      setResultError(`Invalid instance JSON: ${(parseError as Error).message}`);
      setResultLoading(false);
      return;
    }
    promoteToRdf(registryUrl, selectedSchemaPath, instance)
      .then((result) => {
        if (resultRequestRef.current === generation) setRdfResult(result);
      })
      .catch((error: unknown) => {
        if (resultRequestRef.current === generation) {
          setResultError(error instanceof Error ? error.message : String(error));
        }
      })
      .finally(() => {
        if (resultRequestRef.current === generation) setResultLoading(false);
      });
  }, [registryUrl, selectedSchemaPath, instanceText]);

  const value = {
    registryUrl,
    registryHealthy,
    theme,
    toggleTheme,
    selectedSchemaPath,
    setSelectedSchemaPath,
    schemaMetadata,
    metadataLoading,
    metadataError,
    activeTab,
    setActiveTab,
    schemaContent,
    schemaContentLoading,
    schemaContentError,
    instanceText,
    setInstanceText,
    detailTab,
    setDetailTab,
    dependencies,
    dependents,
    healthReport,
    schemaStats,
    schemaLocations,
    schemaPositions,
    detailLoading,
    resultMode,
    evaluationResult,
    traceResult,
    rdfResult,
    resultLoading,
    resultError,
    runEvaluate,
    runTrace,
    runRdf,
    debuggerOpen,
    openDebugger,
    closeDebugger,
  };

  return <AppContext.Provider value={value}>{children}</AppContext.Provider>;
};
