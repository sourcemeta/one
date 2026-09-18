import { createContext } from "react";
import type {
  DependencyEdge,
  EvaluationResult,
  HealthReport,
  SchemaLocations,
  SchemaMetadata,
  SchemaStats,
  TraceResult,
} from "../types/one";

export type ResultMode = "evaluate" | "trace" | "rdf";
export type EditorTab = "schema" | "instance";
export type DetailTab = "dependencies" | "dependents" | "lint" | "stats" | "locations";

type AppContextType = {
  registryUrl: string;
  setRegistryUrl: (url: string) => void;
  registryHealthy: boolean | null;

  selectedSchemaPath: string | null;
  setSelectedSchemaPath: (path: string | null) => void;

  schemaMetadata: SchemaMetadata | null;
  metadataLoading: boolean;
  metadataError: string | null;

  activeTab: EditorTab;
  setActiveTab: (tab: EditorTab) => void;

  schemaContent: string | null;
  schemaContentLoading: boolean;
  schemaContentError: string | null;

  instanceText: string;
  setInstanceText: (text: string) => void;

  detailTab: DetailTab;
  setDetailTab: (tab: DetailTab) => void;
  dependencies: DependencyEdge[] | null;
  dependents: DependencyEdge[] | null;
  healthReport: HealthReport | null;
  schemaStats: SchemaStats | null;
  schemaLocations: SchemaLocations | null;
  detailLoading: boolean;

  resultMode: ResultMode | null;
  evaluationResult: EvaluationResult | null;
  traceResult: TraceResult | null;
  rdfResult: unknown;
  resultLoading: boolean;
  resultError: string | null;

  runEvaluate: () => void;
  runTrace: () => void;
  runRdf: () => void;

  debuggerOpen: boolean;
  openDebugger: () => void;
  closeDebugger: () => void;

  // Lets InstanceEditor hand an edited (unsaved) schema + instance off to
  // the Custom Debugger, which is the only place able to trace a schema
  // that isn't the one actually stored on the registry (the registry's
  // own /schemas/trace endpoint always uses the stored version by path).
  customDebuggerSeed: { schema: string; instance: string } | null;
  openCustomDebuggerWithSchema: (schema: string, instance: string) => void;
  consumeCustomDebuggerSeed: () => void;
};

export const AppContext = createContext<AppContextType>(
  {} as AppContextType
);
