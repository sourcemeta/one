const KNOWN_DRAFTS = [
  "2020-12",
  "2019-09",
  "draft-07",
  "draft-06",
  "draft-04",
] as const;

export const dialectLabel = (baseDialectUri: string): string => {
  const match = KNOWN_DRAFTS.find((draft) => baseDialectUri.includes(draft));
  return match ?? baseDialectUri;
};
