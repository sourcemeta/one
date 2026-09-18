import { useContext, useEffect, useRef, useState } from "react";
import type { KeyboardEvent, PointerEvent } from "react";
import Editor from "@monaco-editor/react";
import type { editor as MonacoEditor } from "monaco-editor";
import { AppContext } from "../contexts/AppContext";
import MetadataTable from "./MetadataTable";
import DetailPanel from "./DetailPanel";
import { defineMonacoTheme, ONE_UI_EDITOR_FONT_OPTIONS, ONE_UI_MONACO_THEME } from "../utils/monacoTheme";
import { attachSchemaKeywordLinks } from "../utils/learnJsonSchemaLinks";
import { computePropertyClaims } from "../utils/propertyClaims";
import { applyPropertyClaimDecorations } from "../utils/propertyClaimDecorations";
import IdleState from "./IdleState";
import { getSchemaContent } from "../api/one";

// The editor and the DetailPanel below it share the card's height. The editor
// used to be a fixed 288px, which squeezed the DetailPanel into a thin strip
// on shorter windows, so the split is now user-adjustable.
const EDITOR_HEIGHT_KEY = "one-ui:editor-height";
const DEFAULT_EDITOR_HEIGHT = 288;
const MIN_EDITOR_HEIGHT = 80;
const MIN_DETAIL_HEIGHT = 64;
const KEYBOARD_STEP = 24;

const readStoredEditorHeight = () => {
  try {
    const stored = Number(localStorage.getItem(EDITOR_HEIGHT_KEY));
    return Number.isFinite(stored) && stored > 0 ? stored : DEFAULT_EDITOR_HEIGHT;
  } catch {
    return DEFAULT_EDITOR_HEIGHT;
  }
};

const InstanceEditor = () => {
  const {
    registryUrl,
    selectedSchemaPath,
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
    runEvaluate,
    runTrace,
    runRdf,
    resultLoading,
    openCustomDebuggerWithSchema,
    traceResult,
  } = useContext(AppContext);

  const instanceEditorRef = useRef<MonacoEditor.IStandaloneCodeEditor | null>(null);

  // Bundled view is fetched separately from the plain schemaContent used
  // elsewhere (e.g. the Trace Debugger's highlighting, which relies on
  // /positions being computed against the unbundled text) so toggling it
  // here can't desync anything else that reads schemaContent.
  const [bundled, setBundled] = useState(false);
  const [bundledContent, setBundledContent] = useState<string | null>(null);
  const [bundledLoading, setBundledLoading] = useState(false);
  const [bundledError, setBundledError] = useState<string | null>(null);

  // A local, editable copy of whichever schema view (plain or bundled) is
  // showing. Evaluate/Trace/RDF validate against the schema already stored
  // at selectedSchemaPath on the registry, not this draft — editing here is
  // for exploration only, same as pasting into the Custom Debugger.
  const [schemaDraft, setSchemaDraft] = useState<string | null>(null);

  const cardRef = useRef<HTMLDivElement>(null);
  const editorRef = useRef<HTMLDivElement>(null);
  const separatorRef = useRef<HTMLDivElement>(null);
  // Distance between the pointer and the editor's bottom edge at drag start,
  // so the split doesn't jump to the pointer when the handle is grabbed.
  const grabOffsetRef = useRef(0);
  const [editorHeight, setEditorHeight] = useState(readStoredEditorHeight);
  const [resizing, setResizing] = useState(false);

  // Keep at least MIN_DETAIL_HEIGHT of the card for the DetailPanel.
  const clampEditorHeight = (height: number) => {
    const card = cardRef.current;
    const editor = editorRef.current;
    if (!card || !editor) return Math.max(MIN_EDITOR_HEIGHT, height);
    const cardInnerBottom =
      card.getBoundingClientRect().top + card.clientTop + card.clientHeight;
    const available =
      cardInnerBottom -
      editor.getBoundingClientRect().top -
      (separatorRef.current?.offsetHeight ?? 0) -
      MIN_DETAIL_HEIGHT;
    return Math.round(
      Math.max(MIN_EDITOR_HEIGHT, Math.min(height, available))
    );
  };

  const commitEditorHeight = (height: number) => {
    const next = clampEditorHeight(height);
    setEditorHeight(next);
    try {
      localStorage.setItem(EDITOR_HEIGHT_KEY, String(next));
    } catch {
      // Storage can be unavailable (private mode); the split still works.
    }
  };

  const handleResizeStart = (event: PointerEvent<HTMLDivElement>) => {
    if (event.button !== 0) return;
    event.preventDefault();
    event.currentTarget.setPointerCapture(event.pointerId);
    if (editorRef.current) {
      grabOffsetRef.current =
        event.clientY - editorRef.current.getBoundingClientRect().bottom;
    }
    setResizing(true);
  };

  const handleResizeMove = (event: PointerEvent<HTMLDivElement>) => {
    if (!resizing || !editorRef.current) return;
    const top = editorRef.current.getBoundingClientRect().top;
    setEditorHeight(
      clampEditorHeight(event.clientY - grabOffsetRef.current - top)
    );
  };

  const handleResizeEnd = (event: PointerEvent<HTMLDivElement>) => {
    if (!resizing) return;
    event.currentTarget.releasePointerCapture(event.pointerId);
    setResizing(false);
    commitEditorHeight(editorHeight);
  };

  const handleResizeKey = (event: KeyboardEvent<HTMLDivElement>) => {
    if (event.key === "ArrowUp") {
      event.preventDefault();
      commitEditorHeight(editorHeight - KEYBOARD_STEP);
    } else if (event.key === "ArrowDown") {
      event.preventDefault();
      commitEditorHeight(editorHeight + KEYBOARD_STEP);
    }
  };

  // A stored height from a taller window can overflow a shorter one.
  useEffect(() => {
    const card = cardRef.current;
    if (!card) return;
    const observer = new ResizeObserver(() =>
      setEditorHeight((height) => clampEditorHeight(height))
    );
    observer.observe(card);
    return () => observer.disconnect();
  }, [selectedSchemaPath]);

  useEffect(() => {
    setBundled(false);
    setBundledContent(null);
    setBundledError(null);
    setSchemaDraft(null);
  }, [selectedSchemaPath]);

  useEffect(() => {
    setSchemaDraft(bundled ? bundledContent : schemaContent);
  }, [bundled, bundledContent, schemaContent]);

  useEffect(() => {
    if (!bundled || !selectedSchemaPath) return;
    let cancelled = false;
    setBundledLoading(true);
    setBundledError(null);
    getSchemaContent(registryUrl, selectedSchemaPath, { bundle: true })
      .then((content) => {
        if (!cancelled) setBundledContent(content);
      })
      .catch((error: unknown) => {
        if (!cancelled) {
          setBundledError(error instanceof Error ? error.message : String(error));
        }
      })
      .finally(() => {
        if (!cancelled) setBundledLoading(false);
      });
    return () => {
      cancelled = true;
    };
  }, [bundled, registryUrl, selectedSchemaPath]);

  // Re-paint the additionalProperties/unevaluatedProperties highlight
  // whenever a fresh Trace result comes in while the Instance tab is
  // already mounted (switching tabs remounts the editor and re-runs
  // onMount instead, which applies it from scratch).
  useEffect(() => {
    if (activeTab !== "instance" || !instanceEditorRef.current) return;
    applyPropertyClaimDecorations(
      instanceEditorRef.current,
      traceResult ? computePropertyClaims(traceResult.steps) : []
    );
  }, [activeTab, traceResult]);

  const originalSchema = bundled ? bundledContent : schemaContent;
  const schemaEdited =
    schemaDraft !== null && originalSchema !== null && schemaDraft !== originalSchema;

  const handleTrace = () => {
    if (schemaEdited && schemaDraft) {
      openCustomDebuggerWithSchema(schemaDraft, instanceText);
      return;
    }
    runTrace();
  };

  if (!selectedSchemaPath) {
    return <IdleState />;
  }

  return (
    <div ref={cardRef} className="flex flex-col h-full flex-1 min-w-0 rounded-lg border border-[var(--border)] bg-[var(--bg-surface)] shadow-[var(--shadow-sm)] overflow-hidden">
      <div className="flex items-start justify-between gap-2 px-3 py-2.5 border-b border-[var(--border)] sticky top-0 z-10 bg-[var(--bg-surface)]">
        <div className="min-w-0 flex flex-col gap-0.5">
          {metadataLoading ? (
            <span className="text-sm text-[var(--text-secondary)]">
              Loading…
            </span>
          ) : metadataError ? (
            <span className="text-sm text-[var(--danger)]">
              {metadataError}
            </span>
          ) : (
            <span className="text-sm font-medium text-[var(--text)] truncate">
              {schemaMetadata?.title || selectedSchemaPath}
            </span>
          )}
          {schemaMetadata?.description && (
            <p className="text-xs text-[var(--text-secondary)] truncate max-w-xl">
              {schemaMetadata.description}
            </p>
          )}
        </div>
        <div className="flex gap-2 shrink-0">
          <button
            onClick={runEvaluate}
            disabled={resultLoading}
            className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--border-strong)] bg-[var(--bg-inset)] text-[var(--text)] hover:border-[var(--accent)] hover:text-[var(--accent)] transition-colors disabled:opacity-40 disabled:cursor-not-allowed"
          >
            Evaluate
          </button>
          <button
            onClick={handleTrace}
            disabled={resultLoading}
            title={
              schemaEdited
                ? "Opens the Custom Debugger, tracing your edited schema instead of the one stored on the registry"
                : undefined
            }
            className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--accent)]/50 bg-[var(--accent)]/12 text-[var(--accent)] hover:bg-[var(--accent)]/20 transition-colors disabled:opacity-40 disabled:cursor-not-allowed"
          >
            {schemaEdited ? "Trace edited schema →" : "Trace"}
          </button>
          <button
            onClick={runRdf}
            disabled={resultLoading}
            className="h-8 px-3 text-sm rounded-[var(--radius-sm)] border border-[var(--info)]/50 bg-[var(--info)]/12 text-[var(--info)] hover:bg-[var(--info)]/20 transition-colors disabled:opacity-40 disabled:cursor-not-allowed"
          >
            RDF
          </button>
        </div>
      </div>

      <MetadataTable />

      <div className="flex items-center border-b border-[var(--border)]">
        <button
          onClick={() => setActiveTab("schema")}
          className={`px-3 py-1.5 text-xs border-r border-[var(--border)] ${
            activeTab === "schema"
              ? "text-[var(--text)] bg-[var(--bg-inset)]"
              : "text-[var(--text-secondary)] hover:text-[var(--text)]"
          }`}
        >
          Schema
        </button>
        <button
          onClick={() => setActiveTab("instance")}
          className={`px-3 py-1.5 text-xs border-r border-[var(--border)] ${
            activeTab === "instance"
              ? "text-[var(--text)] bg-[var(--bg-inset)]"
              : "text-[var(--text-secondary)] hover:text-[var(--text)]"
          }`}
        >
          Instance
        </button>
        {activeTab === "instance" && traceResult && (
          <span
            className="ml-auto mr-2 flex items-center gap-2.5 text-[10px] text-[var(--text-secondary)]"
            title="From the last Trace run: which properties matched properties/patternProperties, versus which fell to additionalProperties/unevaluatedProperties"
          >
            <span className="flex items-center gap-1">
              <span className="w-2 h-2 rounded-sm" style={{ backgroundColor: "color-mix(in srgb, var(--accent) 45%, transparent)" }} />
              declared
            </span>
            <span className="flex items-center gap-1">
              <span className="w-2 h-2 rounded-sm bg-[var(--warning)]" />
              extra (allowed)
            </span>
            <span className="flex items-center gap-1">
              <span className="w-2 h-2 rounded-sm bg-[var(--danger)]" />
              extra (rejected)
            </span>
          </span>
        )}
        {activeTab === "schema" && (
          <span className="ml-auto mr-2 flex items-center gap-3">
            <span
              className={`text-[10px] ${
                schemaEdited ? "text-[var(--accent)]" : "text-[var(--text-secondary)] opacity-60"
              }`}
              title={
                schemaEdited
                  ? "Trace will use these edits (opens the Custom Debugger); Evaluate and RDF still validate against the version stored on the registry, since the registry has no equivalent endpoint for those"
                  : "Editing here doesn't change what Evaluate/Trace/RDF validate against — they use the schema already stored on the registry"
              }
            >
              {schemaEdited
                ? "edited — Trace uses this, Evaluate/RDF don't"
                : "edits here don't affect Evaluate/Trace/RDF"}
            </span>
            <label
              title="Show the schema with $ref keywords inlined via JSON Schema Bundling"
              className="flex items-center gap-1.5 text-xs text-[var(--text-secondary)] cursor-pointer select-none"
            >
              <input
                type="checkbox"
                checked={bundled}
                onChange={(e) => setBundled(e.target.checked)}
                className="accent-[var(--accent)]"
              />
              Bundled
            </label>
          </span>
        )}
      </div>

      <div
        ref={editorRef}
        style={{ height: editorHeight }}
        className={`shrink-0 ${resizing ? "pointer-events-none" : ""}`}
      >
        {activeTab === "schema" && bundled && bundledLoading ? (
          <p className="text-sm text-[var(--text-secondary)] p-3">
            Bundling schema…
          </p>
        ) : activeTab === "schema" && bundled && bundledError ? (
          <p className="text-sm text-[var(--danger)] p-3">{bundledError}</p>
        ) : activeTab === "schema" && schemaContentLoading ? (
          <p className="text-sm text-[var(--text-secondary)] p-3">
            Loading schema…
          </p>
        ) : activeTab === "schema" && schemaContentError ? (
          <p className="text-sm text-[var(--danger)] p-3">
            {schemaContentError}
          </p>
        ) : (
          <Editor
            key={activeTab === "schema" && bundled ? "schema-bundled" : activeTab}
            language="json"
            theme={ONE_UI_MONACO_THEME}
            beforeMount={defineMonacoTheme}
            value={activeTab === "schema" ? schemaDraft ?? "" : instanceText}
            onChange={
              activeTab === "schema"
                ? (value) => setSchemaDraft(value ?? "")
                : (value) => setInstanceText(value ?? "")
            }
            onMount={
              activeTab === "schema"
                ? attachSchemaKeywordLinks
                : (editorInstance) => {
                    instanceEditorRef.current = editorInstance;
                    applyPropertyClaimDecorations(
                      editorInstance,
                      traceResult ? computePropertyClaims(traceResult.steps) : []
                    );
                  }
            }
            options={{
              ...ONE_UI_EDITOR_FONT_OPTIONS,
              minimap: { enabled: false },
              fontSize: 14,
              scrollBeyondLastLine: false,
              stickyScroll: { enabled: false },
            }}
          />
        )}
      </div>

      <div
        ref={separatorRef}
        role="separator"
        aria-orientation="horizontal"
        aria-label="Resize editor and detail panel"
        aria-valuenow={editorHeight}
        aria-valuemin={MIN_EDITOR_HEIGHT}
        tabIndex={0}
        title="Drag to resize · double-click to reset"
        onPointerDown={handleResizeStart}
        onPointerMove={handleResizeMove}
        onPointerUp={handleResizeEnd}
        onPointerCancel={handleResizeEnd}
        onDoubleClick={() => commitEditorHeight(DEFAULT_EDITOR_HEIGHT)}
        onKeyDown={handleResizeKey}
        className={`group relative h-1.5 shrink-0 cursor-row-resize touch-none border-y border-[var(--border)] outline-none transition-colors focus-visible:bg-[var(--accent)]/60 ${
          resizing ? "bg-[var(--accent)]/60" : "bg-[var(--bg-inset)] hover:bg-[var(--accent)]/40"
        }`}
      >
        <span className="absolute left-1/2 top-1/2 h-0.5 w-8 -translate-x-1/2 -translate-y-1/2 rounded-full bg-[var(--border-strong)] group-hover:bg-[var(--accent)]" />
      </div>

      <DetailPanel />
    </div>
  );
};

export default InstanceEditor;
