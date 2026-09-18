// Shapes verified against a running Sourcemeta One instance's HTTP API.

export type DirectoryEntry =
  | {
      name: string;
      type: "directory";
      health: number;
      schemas: number;
      path: string;
      title?: string;
      description?: string;
      email?: string;
      github?: string;
      website?: string;
      private: boolean;
    }
  | {
      name: string;
      type: "schema";
      path: string;
      identifier: string;
      bytes: number;
      bytesBundled: number;
      baseDialect: string;
      dialect: string;
      health: number;
      dependencies: number;
      priority: number;
      alert: string | null;
      private: boolean;
    };

export type DirectoryListing = {
  health: number;
  schemas: number;
  entries: DirectoryEntry[];
  private: boolean;
  path: string;
  url: string;
  breadcrumb: { name: string; path: string }[];
};

export type SearchResult = {
  path: string;
  identifier: string;
  title: string;
  description: string;
  priority: number;
  health: number;
};

export type SchemaMetadata = {
  bytes: number;
  bytesBundled: number;
  identifier: string;
  path: string;
  baseDialect: string;
  dialect: string;
  health: number;
  priority: number;
  private: boolean;
  title?: string;
  description?: string;
  dependencies?: number;
  alert?: string | null;
  examples: unknown[];
  breadcrumb: { name: string; path: string }[];
};

export type EvaluationError = {
  keywordLocation: string;
  absoluteKeywordLocation: string;
  instanceLocation: string;
  error: string;
};

export type EvaluationResult = {
  valid: boolean;
  errors?: EvaluationError[];
};

export type TraceStepType = "push" | "pass" | "fail";

export type TraceStep = {
  type: TraceStepType;
  name: string;
  evaluatePath: string;
  instanceLocation: string;
  instancePositions: number[];
  keywordLocation: string;
  annotation: unknown;
  message: string | null;
  vocabulary: string;
};

export type TraceResult = {
  valid: boolean;
  steps: TraceStep[];
};

export type DependencyEdge = {
  from: string;
  to: string;
  at: string;
};

export type LintFinding = {
  name: string;
  message: string;
  description: string | null;
  custom: boolean;
  pointers: string[];
};

export type HealthReport = {
  score: number;
  errors: LintFinding[];
};

export type SchemaPositions = Record<string, [number, number, number, number]>;

// Keyword occurrence counts, grouped by vocabulary URI then keyword name.
export type SchemaStats = Record<string, Record<string, number>>;

export type SchemaLocationEntry = {
  parent: string | null;
  type: string;
  root: string;
  base: string;
  pointer: string;
  relativePointer: string;
  position: [number, number, number, number];
  dialect: string;
  baseDialect: string;
  propertyName: boolean;
  orphan: boolean;
};

export type SchemaLocations = {
  static: Record<string, SchemaLocationEntry>;
  dynamic: Record<string, SchemaLocationEntry>;
};

export type ProblemDetails = {
  type: string;
  title: string;
  status: number;
  detail: string;
};
