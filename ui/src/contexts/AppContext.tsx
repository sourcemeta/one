import { createContext } from "react";
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

export type ResultMode = "evaluate" | "trace" | "rdf";
export type EditorTab = "schema" | "instance";
export type DetailTab = "dependencies" | "dependents" | "lint" | "stats" | "locations";
export type ThemeMode = "light" | "dark";

type AppContextType = {
  registryUrl: string;
  registryHealthy: boolean | null;

  theme: ThemeMode;
  toggleTheme: () => void;

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
  // Maps a schema's own JSON pointers to their line/column range, so a lint
  // finding, dependency origin, or location entry can be resolved to a spot
  // in the (read-only) schema editor when clicked.
  schemaPositions: SchemaPositions | null;
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
};

export const AppContext = createContext<AppContextType>(
  {} as AppContextType
);
