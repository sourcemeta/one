import { useCallback, useEffect, useState, type ReactNode } from "react";
import {
  AppContext,
  type DetailTab,
  type EditorTab,
  type ResultMode,
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
  SchemaStats,
  TraceResult,
} from "../types/one";

const SESSION_REGISTRY_KEY = "one-ui.registryUrl";
// Sourcemeta's own public instance — works out of the box with zero setup,
// instead of pointing at a localhost registry that likely isn't running.
const DEFAULT_REGISTRY_URL = "https://schemas.sourcemeta.com";

export const AppProvider = ({ children }: { children: ReactNode }) => {
  const [registryUrl, setRegistryUrlState] = useState(
    () => sessionStorage.getItem(SESSION_REGISTRY_KEY) ?? DEFAULT_REGISTRY_URL
  );

  const setRegistryUrl = useCallback((url: string) => {
    sessionStorage.setItem(SESSION_REGISTRY_KEY, url);
    setRegistryUrlState(url);
  }, []);

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

  const [selectedSchemaPath, setSelectedSchemaPath] = useState<string | null>(
    null
  );

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
  const [detailLoading, setDetailLoading] = useState(false);

  const [resultMode, setResultMode] = useState<ResultMode | null>(null);
  const [evaluationResult, setEvaluationResult] =
    useState<EvaluationResult | null>(null);
  const [traceResult, setTraceResult] = useState<TraceResult | null>(null);
  const [rdfResult, setRdfResult] = useState<unknown>(null);
  const [resultLoading, setResultLoading] = useState(false);
  const [resultError, setResultError] = useState<string | null>(null);

  const [debuggerOpen, setDebuggerOpen] = useState(false);
  const openDebugger = useCallback(() => setDebuggerOpen(true), []);
  const closeDebugger = useCallback(() => setDebuggerOpen(false), []);

  const [customDebuggerSeed, setCustomDebuggerSeed] = useState<{
    schema: string;
    instance: string;
  } | null>(null);
  const openCustomDebuggerWithSchema = useCallback(
    (schema: string, instance: string) => {
      setCustomDebuggerSeed({ schema, instance });
      window.location.hash = "#/debugger";
    },
    []
  );
  const consumeCustomDebuggerSeed = useCallback(
    () => setCustomDebuggerSeed(null),
    []
  );

  useEffect(() => {
    if (!selectedSchemaPath) {
      setSchemaMetadata(null);
      setSchemaContent(null);
      return;
    }

    let cancelled = false;
    setMetadataLoading(true);
    setMetadataError(null);
    setEvaluationResult(null);
    setTraceResult(null);
    setRdfResult(null);
    setResultMode(null);
    // Without this the previous schema's failure (e.g. a 422 from RDF) stays
    // in the Result panel next to a schema it has nothing to do with.
    setResultError(null);
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
    Promise.all([
      getSchemaDependencies(registryUrl, selectedSchemaPath),
      getSchemaDependents(registryUrl, selectedSchemaPath),
      getSchemaHealthReport(registryUrl, selectedSchemaPath),
      getSchemaStats(registryUrl, selectedSchemaPath),
      getSchemaLocations(registryUrl, selectedSchemaPath),
    ])
      .then(([deps, dependentsList, health, stats, locations]) => {
        if (cancelled) return;
        setDependencies(deps);
        setDependents(dependentsList);
        setHealthReport(health);
        setSchemaStats(stats);
        setSchemaLocations(locations);
      })
      .catch(() => {
        if (!cancelled) {
          setDependencies([]);
          setDependents([]);
          setHealthReport(null);
          setSchemaStats(null);
          setSchemaLocations(null);
        }
      })
      .finally(() => {
        if (!cancelled) setDetailLoading(false);
      });

    return () => {
      cancelled = true;
    };
  }, [registryUrl, selectedSchemaPath]);

  const runEvaluate = useCallback(() => {
    if (!selectedSchemaPath) return;
    setResultLoading(true);
    setResultError(null);
    setEvaluationResult(null);
    setResultMode("evaluate");
    evaluateSchema(registryUrl, selectedSchemaPath, instanceText)
      .then(setEvaluationResult)
      .catch((error: unknown) =>
        setResultError(error instanceof Error ? error.message : String(error))
      )
      .finally(() => setResultLoading(false));
  }, [registryUrl, selectedSchemaPath, instanceText]);

  const runTrace = useCallback(() => {
    if (!selectedSchemaPath) return;
    setResultLoading(true);
    setResultError(null);
    setTraceResult(null);
    setResultMode("trace");
    traceSchema(registryUrl, selectedSchemaPath, instanceText)
      .then(setTraceResult)
      .catch((error: unknown) =>
        setResultError(error instanceof Error ? error.message : String(error))
      )
      .finally(() => setResultLoading(false));
  }, [registryUrl, selectedSchemaPath, instanceText]);

  const runRdf = useCallback(() => {
    if (!selectedSchemaPath) return;
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
      .then(setRdfResult)
      .catch((error: unknown) =>
        setResultError(error instanceof Error ? error.message : String(error))
      )
      .finally(() => setResultLoading(false));
  }, [registryUrl, selectedSchemaPath, instanceText]);

  const value = {
    registryUrl,
    setRegistryUrl,
    registryHealthy,
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
    customDebuggerSeed,
    openCustomDebuggerWithSchema,
    consumeCustomDebuggerSeed,
  };

  return <AppContext.Provider value={value}>{children}</AppContext.Provider>;
};
