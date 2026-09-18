import { useContext, useMemo, useRef, useState, useEffect } from "react";
import Editor, { type Monaco } from "@monaco-editor/react";
import type { editor as MonacoEditor } from "monaco-editor";
import { AppContext } from "../../contexts/AppContext";
import { traceCustomSchema } from "../../api/one";
import { defineMonacoTheme, ONE_UI_EDITOR_FONT_OPTIONS, ONE_UI_MONACO_THEME } from "../../utils/monacoTheme";
import { attachSchemaKeywordLinks } from "../../utils/learnJsonSchemaLinks";
import { computePropertyClaims } from "../../utils/propertyClaims";
import { applyPropertyClaimDecorations } from "../../utils/propertyClaimDecorations";
import { getCollectedAnnotations, getDynamicScope, getOpenFrames } from "../../utils/traceStack";
import { computeJsonPositions } from "../../utils/jsonPointerPositions";
import type { SchemaPositions, TraceResult } from "../../types/one";
import StackVisualizer from "../TraceDebugger/StackVisualizer";
import AnnotationsPanel from "../AnnotationsPanel";

const PLAY_INTERVAL_MS = 700;
const API_URL_KEY = "one-ui.customDebuggerApiUrl";
// Sourcemeta's own public instance — works standalone for any pasted
// schema, no setup required, and still resolves $refs correctly if a
// schema happens to reference something hosted there. Independent of
// whatever "Registry" the rest of the app is connected to (or isn't), so
// the debugger keeps working with zero setup either way.
const DEFAULT_API_URL = "https://schemas.sourcemeta.com";

const DEFAULT_SCHEMA = `{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "properties": {
    "engineer": { "type": "string", "minLength": 1 },
    "project": { "type": "string", "enum": ["one-ui", "studio-json-schema", "blaze"] },
    "task": { "type": "string" },
    "date": { "type": "string", "format": "date" },
    "planet": { "const": "Earth" },
    "company": { "type": "string" },
    "tags": {
      "type": "array",
      "items": { "type": "string" },
      "minItems": 1
    }
  },
  "required": ["engineer", "project", "task", "date", "planet", "company"]
}`;

const DEFAULT_INSTANCE = `{
  "engineer": "Sumit",
  "project": "one-ui",
  "task": "Blaze integration",
  "date": "2026-09-04",
  "planet": "Earth",
  "company": "Sourcemeta",
  "tags": ["wasm", "browser", "debugger"]
}`;

const toMonacoRange = (position: [number, number, number, number]) => ({
  startLineNumber: position[0] + 1,
  startColumn: position[1] + 1,
  endLineNumber: position[2] + 1,
  endColumn: position[3] + 1,
});

const humanize = (name: string) => name.replace(/([a-z0-9])([A-Z])/g, "$1 $2");

const highlightClass = (type: "push" | "pass" | "fail") =>
  type === "fail"
    ? "trace-highlight-fail"
    : type === "pass"
      ? "trace-highlight-pass"
      : "trace-highlight-push";

// keywordLocation is "#/json/pointer" for a keyword within the schema we
// posted (including one reached through a same-document $ref — the server
// resolves those to their real location), or "<resource>#/json/pointer" for
// a $ref into another registry schema. resource is null for the former.
const splitKeywordLocation = (
  keywordLocation: string
): { resource: string | null; pointer: string } => {
  if (keywordLocation.startsWith("#")) {
    return { resource: null, pointer: keywordLocation.slice(1) };
  }
  const hashIndex = keywordLocation.indexOf("#");
  if (hashIndex === -1) return { resource: keywordLocation, pointer: "" };
  return {
    resource: keywordLocation.slice(0, hashIndex),
    pointer: keywordLocation.slice(hashIndex + 1) || "",
  };
};

// A resource URL's own $id doubles as a fetchable document URL on Sourcemeta
// One — no ".json" suffix needed, unlike registryUrl-relative schema paths.
type RefSchemaState =
  | { status: "loading" }
  | { status: "error"; message: string }
  | { status: "ready"; text: string; positions: SchemaPositions };

const shortResourceLabel = (resource: string): string => {
  try {
    const url = new URL(resource);
    const segments = url.pathname.split("/").filter(Boolean);
    return segments.slice(-2).join("/") || url.hostname;
  } catch {
    return resource;
  }
};

const CustomDebugger = ({ onClose }: { onClose: () => void }) => {
  const { customDebuggerSeed, consumeCustomDebuggerSeed } = useContext(AppContext);

  const [apiUrl, setApiUrl] = useState(
    () => localStorage.getItem(API_URL_KEY) ?? DEFAULT_API_URL
  );

  const [schemaText, setSchemaText] = useState(
    () => customDebuggerSeed?.schema ?? DEFAULT_SCHEMA
  );
  const [instanceText, setInstanceText] = useState(
    () => customDebuggerSeed?.instance ?? DEFAULT_INSTANCE
  );

  // Consume once on mount so a later, unrelated open of the Custom Debugger
  // (e.g. via the header button) doesn't reuse a stale seed.
  useEffect(() => {
    if (customDebuggerSeed) consumeCustomDebuggerSeed();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);
  const [traceResult, setTraceResult] = useState<TraceResult | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const [stepIndex, setStepIndex] = useState(0);
  const [isPlaying, setIsPlaying] = useState(false);

  // "local" is the pasted schema; any other key is a resource URL fetched
  // on demand when a step's keywordLocation points into it, so a $ref into
  // another registry schema can be viewed and highlighted like the local one.
  const [activeSchemaTab, setActiveSchemaTab] = useState<string>("local");
  const [refSchemas, setRefSchemas] = useState<Record<string, RefSchemaState>>({});

  // If the pasted schema declares its own top-level $id, every keywordLocation
  // in it — even for keywords reached via a same-document $ref — carries that
  // $id as its resource instead of a bare "#...". Without this, such a schema
  // would look like an external ref to itself and get (uselessly) fetched.
  const localSchemaId = useMemo(() => {
    try {
      const parsed: unknown = JSON.parse(schemaText);
      const id = (parsed as { $id?: unknown } | null)?.$id;
      return typeof id === "string" ? id : null;
    } catch {
      return null;
    }
  }, [schemaText]);

  const instanceEditorRef = useRef<MonacoEditor.IStandaloneCodeEditor | null>(null);
  const instanceDecorationsRef = useRef<MonacoEditor.IEditorDecorationsCollection | null>(null);
  const schemaEditorRef = useRef<MonacoEditor.IStandaloneCodeEditor | null>(null);
  const schemaDecorationsRef = useRef<MonacoEditor.IEditorDecorationsCollection | null>(null);

  const steps = useMemo(() => traceResult?.steps ?? [], [traceResult]);
  const currentStep = steps[stepIndex];
  const openFrames = useMemo(
    () => getOpenFrames(steps, stepIndex),
    [steps, stepIndex]
  );
  const collectedAnnotations = useMemo(
    () => getCollectedAnnotations(steps, stepIndex),
    [steps, stepIndex]
  );
  const dynamicScope = useMemo(() => getDynamicScope(openFrames), [openFrames]);

  const schemaPositions = useMemo(() => {
    try {
      return computeJsonPositions(schemaText);
    } catch {
      return {};
    }
  }, [schemaText]);

  const instancePositions = useMemo(() => {
    try {
      return computeJsonPositions(instanceText);
    } catch {
      return {};
    }
  }, [instanceText]);

  const runTrace = async () => {
    setLoading(true);
    setError(null);
    setStepIndex(0);
    setIsPlaying(false);
    try {
      let schema: unknown;
      let instance: unknown;
      try {
        schema = JSON.parse(schemaText);
      } catch (parseError) {
        throw new Error(`Invalid schema JSON: ${(parseError as Error).message}`);
      }
      try {
        instance = JSON.parse(instanceText);
      } catch (parseError) {
        throw new Error(`Invalid instance JSON: ${(parseError as Error).message}`);
      }
      setTraceResult(await traceCustomSchema(apiUrl, schema, instance));
    } catch (err) {
      setTraceResult(null);
      setError(err instanceof Error ? err.message : String(err));
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    if (!isPlaying) return;
    if (stepIndex >= steps.length - 1) {
      const timer = setTimeout(() => setIsPlaying(false), 0);
      return () => clearTimeout(timer);
    }
    const timer = setTimeout(() => setStepIndex((i) => i + 1), PLAY_INTERVAL_MS);
    return () => clearTimeout(timer);
  }, [isPlaying, stepIndex, steps.length]);

  useEffect(() => {
    if (!instanceEditorRef.current) return;
    applyPropertyClaimDecorations(instanceEditorRef.current, computePropertyClaims(steps));
  }, [steps]);

  useEffect(() => {
    const editorInstance = instanceEditorRef.current;
    if (!editorInstance || !currentStep) return;

    const position = instancePositions[currentStep.instanceLocation];
    instanceDecorationsRef.current?.clear();
    if (!position) return;

    const range = toMonacoRange(position);
    instanceDecorationsRef.current = editorInstance.createDecorationsCollection([
      {
        range,
        options: {
          className: highlightClass(currentStep.type),
          isWholeLine: false,
          hoverMessage: currentStep.message ? { value: currentStep.message } : undefined,
        },
      },
    ]);
    editorInstance.revealRangeInCenterIfOutsideViewport(range);
  }, [currentStep, instancePositions]);

  // Follows the current step across schema documents: switches to the ref's
  // tab and fetches its text (once per resource) so it can be viewed and
  // highlighted the same way as the locally pasted schema.
  useEffect(() => {
    if (!currentStep) return;
    const { resource: rawResource } = splitKeywordLocation(currentStep.keywordLocation);
    const resource = rawResource === localSchemaId ? null : rawResource;
    setActiveSchemaTab(resource ?? "local");
    if (resource === null || refSchemas[resource]) return;

    setRefSchemas((prev) => ({ ...prev, [resource]: { status: "loading" } }));
    fetch(resource)
      .then((res) => {
        if (!res.ok) throw new Error(`Request failed with status ${res.status}`);
        return res.json();
      })
      .then((schema) => {
        const text = JSON.stringify(schema, null, 2);
        setRefSchemas((prev) => ({
          ...prev,
          [resource]: { status: "ready", text, positions: computeJsonPositions(text) },
        }));
      })
      .catch((err: unknown) => {
        setRefSchemas((prev) => ({
          ...prev,
          [resource]: {
            status: "error",
            message: err instanceof Error ? err.message : String(err),
          },
        }));
      });
    // refSchemas is read, not a dependency, so an already-fetched (or
    // in-flight) resource isn't re-fetched on every render.
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [currentStep, localSchemaId]);

  const activeRefSchema = activeSchemaTab !== "local" ? refSchemas[activeSchemaTab] : null;
  const activeSchemaPositions = useMemo(
    () =>
      activeSchemaTab === "local"
        ? schemaPositions
        : activeRefSchema?.status === "ready"
          ? activeRefSchema.positions
          : {},
    [activeSchemaTab, schemaPositions, activeRefSchema]
  );

  const schemaHighlightNote = useMemo(() => {
    if (!currentStep) return null;
    const { resource: rawResource, pointer } = splitKeywordLocation(currentStep.keywordLocation);
    const resource = rawResource === localSchemaId ? null : rawResource;
    if (resource !== null) {
      const state = refSchemas[resource];
      if (!state || state.status === "loading") return `Fetching ${resource}…`;
      if (state.status === "error") return `Could not fetch ${resource}: ${state.message}`;
      return state.positions[pointer]
        ? null
        : "Could not locate this keyword in the referenced schema.";
    }
    return schemaPositions[pointer]
      ? null
      : "Could not locate this keyword in the schema text.";
  }, [currentStep, schemaPositions, refSchemas, localSchemaId]);

  useEffect(() => {
    const editorInstance = schemaEditorRef.current;
    if (!editorInstance || !currentStep) return;

    const { pointer } = splitKeywordLocation(currentStep.keywordLocation);
    const position = activeSchemaPositions[pointer];
    schemaDecorationsRef.current?.clear();

    if (!position) return;

    const range = toMonacoRange(position);
    schemaDecorationsRef.current = editorInstance.createDecorationsCollection([
      {
        range,
        options: {
          className: highlightClass(currentStep.type),
          isWholeLine: false,
          hoverMessage: currentStep.message ? { value: currentStep.message } : undefined,
        },
      },
    ]);
    editorInstance.revealRangeInCenterIfOutsideViewport(range);
  }, [currentStep, activeSchemaPositions]);

  const beforeMount = (monaco: Monaco) => defineMonacoTheme(monaco);

  const handleApiUrlChange = (url: string) => {
    setApiUrl(url);
    localStorage.setItem(API_URL_KEY, url);
  };

  const describeStep = (step: NonNullable<typeof currentStep>): string => {
    if (step.message) return step.message;
    if (step.type === "push")
      return `Now checking rule "${humanize(step.name)}" at ${step.evaluatePath || "the root"}.`;
    if (step.type === "pass") return "This check passed.";
    return "This check failed.";
  };

  const next = () => setStepIndex((i) => Math.min(i + 1, steps.length - 1));
  const prev = () => setStepIndex((i) => Math.max(i - 1, 0));
  const nextFailure = () => {
    const found = steps.findIndex((s, i) => i > stepIndex && s.type === "fail");
    if (found !== -1) setStepIndex(found);
  };

  return (
    <div className="fixed inset-0 z-50 flex flex-col bg-[var(--bg-canvas)] text-[var(--text)]">
      <style>{`
        .trace-highlight-pass { background: color-mix(in srgb, var(--success) 28%, transparent); border-bottom: 2px solid var(--success); }
        .trace-highlight-fail { background: color-mix(in srgb, var(--danger) 32%, transparent); border-bottom: 2px solid var(--danger); }
        .trace-highlight-push { background: color-mix(in srgb, var(--accent) 20%, transparent); border-bottom: 2px solid var(--accent); }
      `}</style>

      <div className="flex items-center justify-between gap-3 px-4 py-2.5 border-b border-[var(--border)] shrink-0">
        <div className="flex items-center gap-3 min-w-0">
          <button
            onClick={onClose}
            className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--border-strong)] bg-[var(--bg-inset)] hover:border-[var(--accent)] hover:text-[var(--accent)] transition-colors"
          >
            ← Close
          </button>
          <span className="text-sm font-medium truncate">Custom Debugger</span>
          <span className="text-[10px] text-[var(--text-secondary)] hidden md:inline">
            Paste any schema + instance and step through the real Blaze evaluation
          </span>
        </div>
        <div className="flex items-center gap-2 shrink-0">
          <input
            value={apiUrl}
            onChange={(e) => handleApiUrlChange(e.target.value)}
            placeholder={DEFAULT_API_URL}
            title="Sourcemeta One instance to send the schema + instance to for tracing"
            className="h-8 w-56 px-2 text-xs rounded-[var(--radius-sm)] border border-[var(--border-strong)] bg-[var(--bg-inset)] text-[var(--text)] font-mono focus:outline-none focus:border-[var(--accent)]"
          />
          <button
            onClick={runTrace}
            disabled={loading}
            className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--accent)]/50 bg-[var(--accent)]/12 text-[var(--accent)] hover:bg-[var(--accent)]/20 transition-colors disabled:opacity-40"
          >
            {loading ? "Compiling…" : "Compile & Trace"}
          </button>
          {traceResult && (
            <span
              className={`text-xs font-mono px-2 py-1 rounded-full border ${
                traceResult.valid
                  ? "border-[var(--success)]/40 text-[var(--success)]"
                  : "border-[var(--danger)]/40 text-[var(--danger)]"
              }`}
            >
              {traceResult.valid ? "Valid" : "Invalid"} · {steps.length} steps
            </span>
          )}
        </div>
      </div>

      {error && (
        <div className="px-4 py-2 text-xs text-[var(--danger)] bg-[var(--danger-soft)] border-b border-[var(--danger)]/40">
          {error}
        </div>
      )}

      <div className="flex flex-1 min-h-0 gap-3 p-3">
        <div className="flex-1 min-w-0 flex flex-col rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] overflow-hidden">
          <div className="flex items-center border-b border-[var(--border)] overflow-x-auto shrink-0">
            <button
              onClick={() => setActiveSchemaTab("local")}
              className={`px-3 py-1.5 text-xs shrink-0 border-r border-[var(--border)] ${
                activeSchemaTab === "local"
                  ? "text-[var(--text)] bg-[var(--bg-inset)]"
                  : "text-[var(--text-secondary)] hover:text-[var(--text)]"
              }`}
            >
              Schema (editable)
            </button>
            {Object.keys(refSchemas).map((resource) => (
              <div
                key={resource}
                title={resource}
                className={`flex items-center shrink-0 border-r border-[var(--border)] ${
                  activeSchemaTab === resource
                    ? "text-[var(--text)] bg-[var(--bg-inset)]"
                    : "text-[var(--text-secondary)]"
                }`}
              >
                <button
                  onClick={() => setActiveSchemaTab(resource)}
                  className="pl-3 pr-1.5 py-1.5 text-xs truncate max-w-32 hover:text-[var(--text)]"
                >
                  {shortResourceLabel(resource)}
                  {refSchemas[resource].status === "loading" && " …"}
                  {refSchemas[resource].status === "error" && " ⚠"}
                </button>
                <button
                  onClick={() => {
                    setRefSchemas((prev) => {
                      const next = { ...prev };
                      delete next[resource];
                      return next;
                    });
                    if (activeSchemaTab === resource) setActiveSchemaTab("local");
                  }}
                  title={`Close ${resource}`}
                  className="pr-2 pl-0.5 py-1.5 text-xs opacity-60 hover:opacity-100 hover:text-[var(--danger)]"
                >
                  ×
                </button>
              </div>
            ))}
            <span className="ml-auto px-2 flex items-center gap-2 min-w-0">
              {currentStep && (
                <span className="text-xs text-[var(--accent)] font-mono truncate">
                  {currentStep.keywordLocation || "#"}
                </span>
              )}
              {schemaHighlightNote && (
                <span className="text-[10px] text-[var(--accent)] truncate shrink-0">
                  {schemaHighlightNote}
                </span>
              )}
            </span>
          </div>
          <div className="flex-1 min-h-0">
            <Editor
              key={activeSchemaTab === "local" ? "local" : "ref"}
              language="json"
              theme={ONE_UI_MONACO_THEME}
              beforeMount={beforeMount}
              value={
                activeSchemaTab === "local"
                  ? schemaText
                  : activeRefSchema?.status === "ready"
                    ? activeRefSchema.text
                    : ""
              }
              onChange={
                activeSchemaTab === "local"
                  ? (value) => setSchemaText(value ?? "")
                  : undefined
              }
              onMount={(editorInstance, monaco) => {
                schemaEditorRef.current = editorInstance;
                attachSchemaKeywordLinks(editorInstance, monaco);
              }}
              options={{
                ...ONE_UI_EDITOR_FONT_OPTIONS,
                readOnly: activeSchemaTab !== "local",
                minimap: { enabled: false },
                fontSize: 13.5,
                stickyScroll: { enabled: false },
              }}
            />
          </div>
        </div>

        <div className="flex-1 min-w-0 flex flex-col rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] overflow-hidden">
          <div className="px-3 py-1.5 text-xs text-[var(--text-secondary)] border-b border-[var(--border)] truncate">
            Instance (editable)
            {currentStep && (
              <span className="ml-2 text-[var(--accent)] font-mono">
                {currentStep.instanceLocation || "/"}
              </span>
            )}
          </div>
          <div className="flex-1 min-h-0">
            <Editor
              language="json"
              theme={ONE_UI_MONACO_THEME}
              beforeMount={beforeMount}
              value={instanceText}
              onChange={(value) => setInstanceText(value ?? "")}
              onMount={(editorInstance) => {
                instanceEditorRef.current = editorInstance;
                applyPropertyClaimDecorations(editorInstance, computePropertyClaims(steps));
              }}
              options={{ ...ONE_UI_EDITOR_FONT_OPTIONS, minimap: { enabled: false }, fontSize: 13.5, stickyScroll: { enabled: false } }}
            />
          </div>
        </div>

        <div className="w-80 shrink-0 flex flex-col rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] overflow-hidden">
          <div className="px-3 py-1.5 border-b border-[var(--border)]">
            <div className="text-sm text-[var(--text-secondary)]">Call Stack</div>
            <div className="text-xs text-[var(--text-secondary)] opacity-70 mt-0.5">
              Rules currently being checked, deepest on top
            </div>
            <div className="flex items-center gap-3 mt-1.5 text-xs">
              <span className="flex items-center gap-1">
                <span className="w-2 h-2 rounded-full bg-[var(--border-strong)] inline-block" />
                checking
              </span>
              <span className="flex items-center gap-1 text-[var(--success)]">
                <span className="w-2 h-2 rounded-full bg-[var(--success)] inline-block" />
                passed
              </span>
              <span className="flex items-center gap-1 text-[var(--danger)]">
                <span className="w-2 h-2 rounded-full bg-[var(--danger)] inline-block" />
                failed
              </span>
              <span className="flex items-center gap-1 text-[var(--info)]">
                <span className="w-2 h-2 rounded-full bg-[var(--info)] inline-block" />
                annotation
              </span>
            </div>
            {dynamicScope.length > 1 && (
              <div className="mt-2 pt-2 border-t border-[var(--border)]">
                <div className="text-xs text-[var(--text-secondary)] opacity-70">
                  Dynamic scope
                </div>
                <div className="flex flex-col gap-0.5 mt-1">
                  {dynamicScope.map((resource, i) => (
                    <span
                      key={i}
                      className="text-xs truncate text-[var(--accent)]"
                    >
                      {resource || "(this schema)"}
                    </span>
                  ))}
                </div>
              </div>
            )}
          </div>
          {traceResult ? (
            <StackVisualizer frames={openFrames} />
          ) : (
            <div className="flex-1 flex items-center justify-center p-4 text-center text-xs text-[var(--text-secondary)]">
              Click "Compile & Trace" to run the real Blaze evaluator on your pasted schema and instance.
            </div>
          )}
        </div>

        <AnnotationsPanel
          annotations={collectedAnnotations}
          currentInstanceLocation={currentStep?.instanceLocation}
        />
      </div>

      {traceResult && (
        <div className="shrink-0 border-t border-[var(--border)] px-4 py-3 flex flex-col gap-3">
          {currentStep && (
            <div
              className={`rounded-[var(--radius-sm)] border px-3 py-2.5 flex items-start gap-3 ${
                currentStep.annotation != null
                  ? "border-[var(--info)]/40 bg-[var(--info-soft)]"
                  : currentStep.type === "fail"
                    ? "border-[var(--danger)]/40 bg-[var(--danger-soft)]"
                    : currentStep.type === "pass"
                      ? "border-[var(--success)]/40 bg-[var(--success-soft)]"
                      : "border-[var(--accent)]/40 bg-[var(--accent)]/10"
              }`}
            >
              <span
                className={`text-lg leading-none shrink-0 ${
                  currentStep.annotation != null
                    ? "text-[var(--info)]"
                    : currentStep.type === "fail"
                      ? "text-[var(--danger)]"
                      : currentStep.type === "pass"
                        ? "text-[var(--success)]"
                        : "text-[var(--accent)]"
                }`}
              >
                {currentStep.annotation != null
                  ? "𝒾"
                  : currentStep.type === "fail"
                    ? "✗"
                    : currentStep.type === "pass"
                      ? "✓"
                      : "▶"}
              </span>
              <div className="min-w-0">
                <div className="text-sm font-medium">
                  {humanize(currentStep.name)}{" "}
                  <span className="text-[var(--text-secondary)] font-mono text-xs">
                    {currentStep.evaluatePath || "/"}
                  </span>
                </div>
                <div className="text-sm text-[var(--text-secondary)] mt-0.5">
                  {describeStep(currentStep)}
                </div>
              </div>
            </div>
          )}

          <div className="flex items-center gap-2">
            <button
              onClick={prev}
              disabled={stepIndex === 0}
              className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--border-strong)] bg-[var(--bg-inset)] disabled:opacity-40 disabled:cursor-not-allowed"
            >
              ◀ Step Back
            </button>
            <button
              onClick={() => setIsPlaying((p) => !p)}
              className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--accent)]/50 bg-[var(--accent)]/12 text-[var(--accent)]"
            >
              {isPlaying ? "Pause" : "Play"}
            </button>
            <button
              onClick={next}
              disabled={stepIndex >= steps.length - 1}
              className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--border-strong)] bg-[var(--bg-inset)] disabled:opacity-40 disabled:cursor-not-allowed"
            >
              Step Forward ▶
            </button>
            <button
              onClick={nextFailure}
              className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--danger)]/40 bg-[var(--danger-soft)] text-[var(--danger)]"
            >
              Next Failure ⏭
            </button>

            <input
              type="range"
              min={0}
              max={Math.max(steps.length - 1, 0)}
              value={stepIndex}
              onChange={(e) => setStepIndex(Number(e.target.value))}
              className="flex-1 accent-[var(--accent)]"
            />
            <span className="text-xs font-mono text-[var(--text-secondary)] w-16 text-right">
              {stepIndex + 1} / {steps.length}
            </span>
          </div>
        </div>
      )}
    </div>
  );
};

export default CustomDebugger;
