import { useContext } from "react";
import { AppContext } from "../contexts/AppContext";
import HealthBar from "./HealthBar";
import { dialectLabel } from "../utils/dialect";
import { formatBytes } from "../utils/format";

const Row = ({
  label,
  children,
}: {
  label: string;
  children: React.ReactNode;
}) => (
  <div className="flex items-center gap-3 px-3 py-1.5 text-sm border-b border-[var(--border)] last:border-b-0">
    <span className="w-28 shrink-0 text-[var(--text-secondary)]">
      {label}
    </span>
    <div className="min-w-0 flex-1 truncate">{children}</div>
  </div>
);

const MetadataTable = () => {
  const { schemaMetadata } = useContext(AppContext);
  if (!schemaMetadata) return null;

  return (
    <div className="border-b border-[var(--border)] bg-[var(--bg-inset)]/40">
      <Row label="Identifier">
        <a
          href={schemaMetadata.identifier}
          target="_blank"
          rel="noreferrer"
          className="text-[var(--info)] hover:underline"
        >
          {schemaMetadata.identifier}
        </a>
      </Row>
      <Row label="Base Dialect">
        <span className="inline-block text-[10px] font-medium px-1.5 py-0.5 rounded-full bg-[var(--accent)]/15 text-[var(--accent)] border border-[var(--accent)]/30">
          {dialectLabel(schemaMetadata.baseDialect)}
        </span>
      </Row>
      <Row label="Dialect">
        <span className="text-[var(--text-secondary)] font-mono text-xs truncate block">
          {schemaMetadata.dialect}
        </span>
      </Row>
      <Row label="Health">
        <HealthBar health={schemaMetadata.health} />
      </Row>
      <Row label="Size">
        <span className="text-[var(--text-secondary)] text-xs">
          {formatBytes(schemaMetadata.bytes)}
          {schemaMetadata.bytesBundled !== schemaMetadata.bytes && (
            <span className="opacity-70">
              {" "}
              ({formatBytes(schemaMetadata.bytesBundled)} bundled)
            </span>
          )}
        </span>
      </Row>
    </div>
  );
};

export default MetadataTable;
