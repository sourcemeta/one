import type { Monaco } from "@monaco-editor/react";
import type { editor as MonacoEditor } from "monaco-editor";
import type { ThemeMode } from "../contexts/AppContext";

const ONE_UI_MONACO_THEME_DARK = "one-ui-dark";
const ONE_UI_MONACO_THEME_LIGHT = "one-ui-light";

export const monacoThemeName = (mode: ThemeMode): string =>
  mode === "light" ? ONE_UI_MONACO_THEME_LIGHT : ONE_UI_MONACO_THEME_DARK;

// Single source of truth for the font every Monaco editor in the app uses.
// Monaco doesn't read page CSS for its own rendering, so this — not
// index.css — is the place to change it app-wide.
export const ONE_UI_EDITOR_FONT_OPTIONS: MonacoEditor.IEditorOptions = {
  fontFamily: "'Inter Variable', ui-sans-serif, sans-serif",
  fontLigatures: false,
  lineHeight: 24,
};

// Monaco doesn't read CSS custom properties, so both themes' colors are
// literal hex here — kept in step with the umber/sand palettes in
// index.css by hand rather than read from the DOM.
export const defineMonacoTheme = (monaco: Monaco) => {
  monaco.editor.defineTheme(ONE_UI_MONACO_THEME_DARK, {
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

  monaco.editor.defineTheme(ONE_UI_MONACO_THEME_LIGHT, {
    base: "vs",
    inherit: true,
    rules: [],
    colors: {
      "editor.background": "#f9f4e8",
      "editor.lineHighlightBackground": "#efe7d488",
      "editorLineNumber.foreground": "#8b7c5e",
      "editorGutter.background": "#f9f4e8",
      "editor.selectionBackground": "#b8571b30",
      "scrollbarSlider.background": "#c3b28c55",
      "scrollbarSlider.hoverBackground": "#c3b28c88",
    },
  });
};
