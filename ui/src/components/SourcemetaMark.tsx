// Inlined (rather than an <img src="...svg">) so the mark's fill can follow
// `currentColor` — the source asset is solid white, which disappears
// against the light theme's cream surfaces.
const SourcemetaMark = ({ className }: { className?: string }) => (
  <svg viewBox="0 0 500 500" role="img" className={className} fill="none">
    <title>Sourcemeta mark</title>
    <path
      d="M332.497646,208.327299 L359.443646,225.164299 L360.207354,226.386742 L141.385646,363.120299 L113.079646,345.433299 L332.497646,208.327299 Z"
      fill="currentColor"
    />
    <path
      d="M345.497646,248.327299 L372.443646,265.164299 L373.207354,266.386742 L154.385646,403.120299 L126.079646,385.433299 L345.497646,248.327299 Z"
      transform="translate(249.6435, 325.724299) scale(-1, 1) translate(-249.6435, -325.724299)"
      fill="currentColor"
    />
    <path
      d="M154.099646,128.699299 L373.207354,265.613258 L372.950646,266.021299 L345.099646,283.424299 L125.794646,146.386299 L154.099646,128.699299 Z"
      transform="translate(249.5005, 206.061799) scale(-1, 1) translate(-249.5005, -206.061799)"
      fill="currentColor"
    />
    <path
      d="M141.099646,88.699299 L360.207354,225.613258 L359.950646,226.021299 L332.099646,243.424299 L112.794646,106.386299 L141.099646,88.699299 Z"
      fill="currentColor"
    />
  </svg>
);

export default SourcemetaMark;
