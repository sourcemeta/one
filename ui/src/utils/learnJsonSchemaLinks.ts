import type { Monaco } from "@monaco-editor/react";
import type { editor as MonacoEditor, IRange } from "monaco-editor";
import { LEARN_JSON_SCHEMA_KEYWORDS } from "./learnJsonSchemaKeywords";

// Canonical dialect URIs, as they appear in a schema's own "$schema" value,
// mapped to the dialect slug learnjsonschema.com uses in its URLs.
const DIALECT_SLUGS: Record<string, string> = {
  "https://json-schema.org/draft/2020-12/schema": "2020-12",
  "https://json-schema.org/draft/2019-09/schema": "2019-09",
  "http://json-schema.org/draft-07/schema#": "draft7",
  "http://json-schema.org/draft-06/schema#": "draft6",
  "http://json-schema.org/draft-04/schema#": "draft4",
  "http://json-schema.org/draft-03/schema#": "draft3",
};

const DEFAULT_DIALECT_SLUG = "2020-12";

const dialectSlugFromSchemaText = (text: string): string => {
  const match = text.match(/"\$schema"\s*:\s*"([^"]+)"/);
  if (!match) return DEFAULT_DIALECT_SLUG;
  return DIALECT_SLUGS[match[1]] ?? DEFAULT_DIALECT_SLUG;
};

export const learnJsonSchemaUrl = (
  dialectSlug: string,
  keyword: string
): string | null => {
  const vocab = LEARN_JSON_SCHEMA_KEYWORDS[dialectSlug]?.[keyword];
  if (!vocab) return null;
  const slug = keyword.replace(/^\$/, "").toLowerCase();
  return `https://www.learnjsonschema.com/${dialectSlug}/${vocab}/${slug}/`;
};

// Matches a JSON string key immediately followed by a colon, e.g. "type":
// (not a key inside an array or a plain string value). Handles escapes so a
// key like "a\"b" doesn't terminate the match early.
const KEY_PATTERN = /"((?:\\.|[^"\\])*)"\s*:/g;

const DECORATION_CLASS = "one-ui-keyword-link";

let stylesInjected = false;

// Plain <a>-style underline (Monaco's built-in link provider) reads as noisy
// clutter across a whole schema. Instead we style recognized keywords with a
// hover-only highlight + pointer cursor, closer to how an IDE "go to docs"
// affordance normally feels, and only reveal the underline on hover.
const injectStyles = () => {
  if (stylesInjected) return;
  stylesInjected = true;
  const style = document.createElement("style");
  style.textContent = `
    .${DECORATION_CLASS} {
      cursor: pointer;
    }
    .${DECORATION_CLASS}:hover {
      text-decoration: underline;
      background-color: rgba(88, 166, 255, 0.15);
      border-radius: 2px;
    }
  `;
  document.head.appendChild(style);
};

interface KeywordHit {
  range: IRange;
  url: string;
}

const collectKeywordHits = (model: MonacoEditor.ITextModel): KeywordHit[] => {
  const dialectSlug = dialectSlugFromSchemaText(model.getValue());
  const keywordVocab = LEARN_JSON_SCHEMA_KEYWORDS[dialectSlug];
  if (!keywordVocab) return [];

  const hits: KeywordHit[] = [];
  const lineCount = model.getLineCount();
  for (let lineNumber = 1; lineNumber <= lineCount; lineNumber++) {
    const lineText = model.getLineContent(lineNumber);
    KEY_PATTERN.lastIndex = 0;
    let match: RegExpExecArray | null;
    while ((match = KEY_PATTERN.exec(lineText))) {
      const keyword = match[1];
      const url = learnJsonSchemaUrl(dialectSlug, keyword);
      if (!url) continue;
      const startColumn = match.index + 2;
      hits.push({
        range: {
          startLineNumber: lineNumber,
          startColumn,
          endLineNumber: lineNumber,
          endColumn: startColumn + keyword.length,
        },
        url,
      });
    }
  }
  return hits;
};

// Highlights recognized JSON Schema keywords (as object keys) in a schema
// editor and opens their documentation on learnjsonschema.com on click, using
// the dialect declared by the schema's own $schema. Every editor in the app
// uses the "json" language for both schemas and plain data instances, so
// this is opted into per schema-editor instance (via onMount) rather than
// registered globally — an instance editor's own "type" or "required" field
// should never get a bogus link.
export const attachSchemaKeywordLinks = (
  editorInstance: MonacoEditor.IStandaloneCodeEditor,
  monaco: Monaco
) => {
  injectStyles();

  let hits: KeywordHit[] = [];
  let decorationIds: string[] = [];

  const refresh = () => {
    const model = editorInstance.getModel();
    hits = model ? collectKeywordHits(model) : [];
    decorationIds = editorInstance.deltaDecorations(
      decorationIds,
      hits.map((hit) => ({
        range: hit.range,
        options: { inlineClassName: DECORATION_CLASS },
      }))
    );
  };

  refresh();

  const model = editorInstance.getModel();
  const changeSub = model?.onDidChangeContent(refresh);

  const clickSub = editorInstance.onMouseUp((event) => {
    if (event.target.type !== monaco.editor.MouseTargetType.CONTENT_TEXT) return;
    const position = event.target.position;
    if (!position) return;
    const hit = hits.find((h) => monaco.Range.containsPosition(h.range, position));
    if (hit) window.open(hit.url, "_blank", "noopener,noreferrer");
  });

  editorInstance.onDidDispose(() => {
    changeSub?.dispose();
    clickSub.dispose();
  });
};
