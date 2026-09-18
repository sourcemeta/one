import type {
  DependencyEdge,
  DirectoryListing,
  EvaluationResult,
  HealthReport,
  ProblemDetails,
  SchemaLocations,
  SchemaMetadata,
  SchemaPositions,
  SchemaStats,
  SearchResult,
  TraceResult,
} from "../types/one";

export class OneApiError extends Error {
  problem: ProblemDetails;

  constructor(problem: ProblemDetails) {
    super(problem.detail || problem.title);
    this.problem = problem;
  }
}

const normaliseBase = (registryUrl: string): string =>
  registryUrl.replace(/\/+$/, "");

const request = async <T>(
  registryUrl: string,
  path: string,
  init?: RequestInit
): Promise<T> => {
  const response = await fetch(`${normaliseBase(registryUrl)}${path}`, init);
  if (!response.ok) {
    const problem = (await response
      .json()
      .catch(() => null)) as ProblemDetails | null;
    if (problem) throw new OneApiError(problem);
    throw new Error(`Request failed with status ${response.status}`);
  }
  return response.json() as Promise<T>;
};

// GET /self/v1/health has an empty body and only exists to be pinged, so it
// resolves to a bool rather than parsing a response.
export const checkRegistryHealth = async (
  registryUrl: string
): Promise<boolean> => {
  try {
    const response = await fetch(`${normaliseBase(registryUrl)}/self/v1/health`);
    return response.ok;
  } catch {
    return false;
  }
};

export const listDirectory = (
  registryUrl: string,
  path = ""
): Promise<DirectoryListing> =>
  request(registryUrl, `/self/v1/api/list${path ? `/${path}` : ""}`);

export const searchSchemas = (
  registryUrl: string,
  query: string
): Promise<SearchResult[]> =>
  request(
    registryUrl,
    `/self/v1/api/schemas/search?q=${encodeURIComponent(query)}`
  );

export const getSchemaMetadata = (
  registryUrl: string,
  schemaPath: string
): Promise<SchemaMetadata> =>
  request(registryUrl, `/self/v1/api/schemas/metadata${schemaPath}`);

export const getSchemaContent = async (
  registryUrl: string,
  schemaPath: string,
  options?: { bundle?: boolean }
): Promise<string> => {
  const query = options?.bundle ? "?bundle=1" : "";
  const response = await fetch(
    `${normaliseBase(registryUrl)}${schemaPath}.json${query}`
  );
  if (!response.ok) {
    throw new Error(`Request failed with status ${response.status}`);
  }
  // Return the server's exact original text rather than re-serializing via
  // JSON.parse/stringify: /schemas/positions is computed against that exact
  // text (for the unbundled case), and reformatting (e.g. reflowing inline
  // arrays to multiple lines) shifts every line number after the first
  // difference, breaking every trace highlight that follows.
  return response.text();
};

export const getSchemaDependencies = (
  registryUrl: string,
  schemaPath: string
): Promise<DependencyEdge[]> =>
  request(registryUrl, `/self/v1/api/schemas/dependencies${schemaPath}`);

export const getSchemaDependents = (
  registryUrl: string,
  schemaPath: string
): Promise<DependencyEdge[]> =>
  request(registryUrl, `/self/v1/api/schemas/dependents${schemaPath}`);

export const getSchemaHealthReport = (
  registryUrl: string,
  schemaPath: string
): Promise<HealthReport> =>
  request(registryUrl, `/self/v1/api/schemas/health${schemaPath}`);

export const evaluateSchema = (
  registryUrl: string,
  schemaPath: string,
  instance: string
): Promise<EvaluationResult> =>
  request(registryUrl, `/self/v1/api/schemas/evaluate${schemaPath}`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: instance,
  });

export const getSchemaPositions = (
  registryUrl: string,
  schemaPath: string
): Promise<SchemaPositions> =>
  request(registryUrl, `/self/v1/api/schemas/positions${schemaPath}`);

export const getSchemaStats = (
  registryUrl: string,
  schemaPath: string
): Promise<SchemaStats> =>
  request(registryUrl, `/self/v1/api/schemas/stats${schemaPath}`);

export const getSchemaLocations = (
  registryUrl: string,
  schemaPath: string
): Promise<SchemaLocations> =>
  request(registryUrl, `/self/v1/api/schemas/locations${schemaPath}`);

// Unlike evaluate/trace, the request body wraps the instance in an object
// alongside optional JSON-LD flattening/compaction options.
export const promoteToRdf = (
  registryUrl: string,
  schemaPath: string,
  instance: unknown,
  options?: { flatten?: boolean; context?: object }
): Promise<unknown> =>
  request(registryUrl, `/self/v1/api/schemas/rdf${schemaPath}`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      instance,
      flatten: options?.flatten,
      context: options?.context,
    }),
  });

export const traceSchema = (
  registryUrl: string,
  schemaPath: string,
  instance: string
): Promise<TraceResult> =>
  request(registryUrl, `/self/v1/api/schemas/trace${schemaPath}`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: instance,
  });

// Traces a schema supplied inline in the request rather than one already in
// the catalog. References to catalog schemas resolve server-side; this is
// what the Custom Debugger uses instead of compiling client-side.
export const traceCustomSchema = (
  registryUrl: string,
  schema: unknown,
  instance: unknown
): Promise<TraceResult> =>
  request(registryUrl, `/self/v1/api/playground/schemas/trace`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ schema, instance }),
  });
