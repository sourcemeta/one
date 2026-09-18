import sourcemetaMark from "../assets/sourcemeta-mark.svg";

const Logo = () => (
  <div className="flex items-center gap-2 pr-3 mr-1 border-r border-[var(--border)]">
    <img src={sourcemetaMark} alt="" className="w-7 h-7 shrink-0" />
    <div className="leading-tight">
      <div className="text-sm font-semibold text-[var(--text)]">one-ui</div>
      <div className="text-[10px] text-[var(--text-secondary)] -mt-0.5">
        Sourcemeta
      </div>
    </div>
  </div>
);

export default Logo;
