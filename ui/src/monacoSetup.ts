import { loader } from "@monaco-editor/react";
// The bare "monaco-editor" entry registers every language Monaco ships with,
// which costs ~9 MB of chunks we never load. Importing the editor API on its
// own keeps the bundle to the editor core, and the JSON contribution below is
// the only language this UI needs.
import * as monaco from "monaco-editor/editor/editor.api";
import "monaco-editor/language/json/monaco.contribution";
// Monaco 0.56 maps "./*" onto "./esm/vs/*.js", so these are the ESM worker
// entrypoints. The older "monaco-editor/esm/vs/..." spelling does not resolve.
import editorWorker from "monaco-editor/editor/editor.worker?worker";
import jsonWorker from "monaco-editor/language/json/json.worker?worker";

self.MonacoEnvironment = {
  getWorker: (_workerId: string, label: string) =>
    label === "json" ? new jsonWorker() : new editorWorker(),
};

// Without this, @monaco-editor/react falls back to its AMD loader and pulls
// Monaco from jsdelivr at runtime, which a self-hosted registry cannot rely on.
// It throws once the loader has initialised, so this module has to run before
// any editor mounts.
loader.config({ monaco });
