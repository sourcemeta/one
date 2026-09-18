import type { editor as MonacoEditor } from "monaco-editor";
import type { PropertyClaim } from "./propertyClaims";

const CLASS_BY_STATUS: Record<PropertyClaim["status"], string> = {
  declared: "one-ui-prop-declared",
  "extra-allowed": "one-ui-prop-extra-allowed",
  "extra-rejected": "one-ui-prop-extra-rejected",
};

const HOVER_MESSAGE_BY_STATUS: Record<PropertyClaim["status"], string> = {
  declared: "Declared: matched by `properties`/`patternProperties`.",
  "extra-allowed":
    "Not declared, but allowed through `additionalProperties`/`unevaluatedProperties`.",
  "extra-rejected":
    "Not declared, and rejected by `additionalProperties`/`unevaluatedProperties`.",
};

let stylesInjected = false;

const injectStyles = () => {
  if (stylesInjected) return;
  stylesInjected = true;
  const style = document.createElement("style");
  style.textContent = `
    .one-ui-prop-declared {
      background-color: color-mix(in srgb, var(--accent) 16%, transparent);
    }
    .one-ui-prop-extra-allowed {
      background-color: var(--warning-soft);
    }
    .one-ui-prop-extra-rejected {
      background-color: var(--danger-soft);
    }
  `;
  document.head.appendChild(style);
};

const decorationIdsByEditor = new WeakMap<
  MonacoEditor.IStandaloneCodeEditor,
  string[]
>();

// Highlights each instance property Trace judged against
// properties/patternProperties/additionalProperties/unevaluatedProperties,
// color-coded by computePropertyClaims' verdict. Call with an empty array to
// clear (e.g. once Trace hasn't run yet, or after an edit invalidates it).
export const applyPropertyClaimDecorations = (
  editorInstance: MonacoEditor.IStandaloneCodeEditor,
  claims: PropertyClaim[]
) => {
  injectStyles();
  const previousIds = decorationIdsByEditor.get(editorInstance) ?? [];
  const nextIds = editorInstance.deltaDecorations(
    previousIds,
    claims.map((claim) => ({
      range: claim.range,
      options: {
        inlineClassName: CLASS_BY_STATUS[claim.status],
        hoverMessage: { value: HOVER_MESSAGE_BY_STATUS[claim.status] },
      },
    }))
  );
  decorationIdsByEditor.set(editorInstance, nextIds);
};
