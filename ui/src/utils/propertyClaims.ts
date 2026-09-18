import type { TraceStep } from "../types/one";

// Whether an instance object property was matched by "properties" /
// "patternProperties" (declared), or fell through to
// additionalProperties/unevaluatedProperties — and if so, whether that
// keyword accepted or rejected it. additionalProperties/unevaluatedProperties
// being "true" (or simply absent) leaves nothing to report: there's no
// keyword actually constraining that bucket, so nothing gets flagged.
export type PropertyClaimStatus = "declared" | "extra-allowed" | "extra-rejected";

export interface PropertyClaim {
  instanceLocation: string;
  status: PropertyClaimStatus;
  range: {
    startLineNumber: number;
    startColumn: number;
    endLineNumber: number;
    endColumn: number;
  };
}

const toRange = (positions: number[]) => ({
  startLineNumber: positions[0],
  startColumn: positions[1],
  endLineNumber: positions[2],
  endColumn: positions[3],
});

const DECLARED_PATTERN = /\/(properties|patternProperties)\/[^/]+/;
const EXTRA_PATTERN = /\/(additionalProperties|unevaluatedProperties)(\/|$)/;

// A Trace step's own instanceLocation already pinpoints exactly which
// property it's judging, and instancePositions already gives its on-screen
// range (same 1-indexed positions the schema side uses) — so this is a pure
// readout of the API's own evaluation, not a re-implementation of it.
export const computePropertyClaims = (steps: TraceStep[]): PropertyClaim[] => {
  const claims = new Map<string, PropertyClaim>();

  for (const step of steps) {
    if (step.type === "push" || !step.instanceLocation) continue;
    if (step.instancePositions.length !== 4) continue;
    const range = toRange(step.instancePositions);

    if (DECLARED_PATTERN.test(step.keywordLocation)) {
      claims.set(step.instanceLocation, {
        instanceLocation: step.instanceLocation,
        status: "declared",
        range,
      });
    } else if (EXTRA_PATTERN.test(step.keywordLocation)) {
      claims.set(step.instanceLocation, {
        instanceLocation: step.instanceLocation,
        status: step.type === "fail" ? "extra-rejected" : "extra-allowed",
        range,
      });
    }
  }

  return [...claims.values()];
};
