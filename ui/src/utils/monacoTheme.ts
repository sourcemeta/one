import type { Monaco } from "@monaco-editor/react";
import type { editor as MonacoEditor } from "monaco-editor";

export const ONE_UI_MONACO_THEME = "one-ui-dark";

// Single source of truth for the font every Monaco editor in the app uses.
// Monaco doesn't read page CSS for its own rendering, so this — not
// index.css — is the place to change it app-wide.
export const ONE_UI_EDITOR_FONT_OPTIONS: MonacoEditor.IEditorOptions = {
  fontFamily: "'Inter', ui-sans-serif, sans-serif",
  fontLigatures: false,
  lineHeight: 24,
};

export const defineMonacoTheme = (monaco: Monaco) => {
  monaco.editor.defineTheme(ONE_UI_MONACO_THEME, {
    base: "vs-dark",
    inherit: true,
    rules: [],
    colors: {
      "editor.background": "#1d1915",
      "editor.lineHighlightBackground": "#2f282022",
      "editorLineNumber.foreground": "#9c8f77",
      "editorGutter.background": "#1d1915",
      "editor.selectionBackground": "#e8963e33",
      "scrollbarSlider.background": "#4f443355",
      "scrollbarSlider.hoverBackground": "#4f443388",
    },
  });
};
