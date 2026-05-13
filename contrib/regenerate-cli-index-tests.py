#!/usr/bin/env python3
"""
Usage
-----

    # Regenerate all failing tests for the indexer (default):
    contrib/regenerate-cli-index-tests.py --all

    # Regenerate specific tests by qualified name:
    contrib/regenerate-cli-index-tests.py common.fail-bundle-ref-no-fragment \\
                                          enterprise.no-options

    # Read failing test names from stdin:
    make 2>&1 | grep -oE 'one\\.index\\.\\S+\\s\\(Failed\\)' \\
        | sed -E 's/^one\\.index\\.//; s/ \\(Failed\\)//' \\
        | contrib/regenerate-cli-index-tests.py --stdin

The script auto-discovers the project root and ONE_PREFIX from the working
directory by walking up to the closest CMakeLists.txt. Override either with
`--root` / `--prefix` if needed.
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

DIFF_LINE_RE = re.compile(
    r'^diff "\$TMP/([A-Za-z0-9._-]+)" "\$TMP/([A-Za-z0-9._-]+)"', re.MULTILINE
)
HEREDOC_RE_TEMPLATE = (
    r'(cat << (\'EOF\'|EOF) > "\$TMP/{name}"\n)(.*?)(^EOF\b)'
)
TMP_PATH_RE = re.compile(
    r"(?:/private)?/(?:var/folders/[^/]+/[^/]+/T|tmp)/tmp\.[A-Za-z0-9]+"
)


def discover_root(start: Path) -> Path:
    current = start.resolve()
    while True:
        if (current / "CMakeLists.txt").is_file() and (current / "src").is_dir():
            return current
        if current.parent == current:
            raise RuntimeError(
                "Could not auto-detect project root from "
                f"{start}. Pass --root explicitly."
            )
        current = current.parent


def find_test_path(root: Path, qualified_name: str) -> Path:
    bucket, name = qualified_name.split(".", 1)
    return root / "test" / "cli" / "index" / bucket / f"{name}.sh"


def parse_diff_pairs(text: str):
    return [
        (match.start(), match.group(1), match.group(2))
        for match in DIFF_LINE_RE.finditer(text)
    ]


def find_heredoc_before(text: str, position: int, expected_name: str):
    pattern = re.compile(
        HEREDOC_RE_TEMPLATE.format(name=re.escape(expected_name)),
        re.DOTALL | re.MULTILINE,
    )
    last = None
    for match in pattern.finditer(text):
        if match.end() < position:
            last = match
        else:
            break
    return last


def make_runnable_copy(script_text: str, tmp_override: Path) -> str:
    text = script_text
    text = re.sub(
        r'^TMP="\$\(mktemp -d\)"',
        f'TMP="{tmp_override}"\nmkdir -p "$TMP"',
        text,
        count=1,
        flags=re.MULTILINE,
    )
    text = re.sub(r'^trap clean EXIT', "# trap clean EXIT", text, flags=re.MULTILINE)

    snap_index = [0]

    def diff_replacement(match):
        snap_index[0] += 1
        actual = match.group(1)
        # `cp || true` so a missing source file doesn't kill the script
        return (
            f'cp "$TMP/{actual}" "$TMP/{actual}.snap{snap_index[0]}" 2>/dev/null '
            f'|| true'
        )

    text = re.sub(DIFF_LINE_RE, diff_replacement, text)
    return text


def normalise_for_expansion(actual: str, tmp_path: Path, prefix: Path) -> str:
    # Order matters: handle the `/private`-prefixed forms first so we don't
    # replace the inner part and leave a stray `/private` prefix.
    actual = actual.replace("/private" + str(prefix), "$ONE_PREFIX")
    actual = actual.replace(str(prefix), "$ONE_PREFIX")
    actual = actual.replace("/private" + str(tmp_path), '$(realpath "$TMP")')
    actual = actual.replace(str(tmp_path), '$(realpath "$TMP")')
    actual = TMP_PATH_RE.sub('$(realpath "$TMP")', actual)
    return actual


def escape_dollars_outside_substitutions(text: str) -> str:
    """
    For a `<< EOF` heredoc (with shell expansion enabled), escape every $
    that is not part of a substitution we deliberately introduced
    (`$ONE_PREFIX`, `$(...)`, `$TMP`). Also escape backticks to prevent
    accidental command substitution on inputs like `\\$ref`. Round-trip
    via sentinel markers so we don't double-escape our own substitutions.
    """
    sentinel_open = "\x00OPEN\x00"
    sentinel_close = "\x00CLOSE\x00"

    def protect(match):
        return sentinel_open + match.group(0)[1:] + sentinel_close

    protected = re.sub(r"\$\([^)]*\)", protect, text)
    protected = re.sub(r"\$ONE_PREFIX\b", protect, protected)
    protected = re.sub(r"\$TMP\b", protect, protected)

    protected = protected.replace("$", r"\$")
    protected = protected.replace("`", r"\`")

    protected = protected.replace(sentinel_open, "$").replace(sentinel_close, "")
    return protected


def regenerate_one(script_path: Path, root: Path, prefix: Path,
                   indexer_bin: Path, edition: str, version: str) -> tuple[bool, str]:
    """Returns (changed, message)."""
    if not script_path.exists():
        return False, "script not found"

    original = script_path.read_text()

    with tempfile.TemporaryDirectory(prefix="regen-cli-") as tmp_root:
        tmp_root_path = Path(tmp_root)
        tmp_override = tmp_root_path / "tmp"
        modified = make_runnable_copy(original, tmp_override)

        snapshots = parse_diff_pairs(modified) and []  # placeholder
        snapshots = []
        snap_index = 0
        for actual_name, expected_name in [
            (m.group(1), m.group(2))
            for m in DIFF_LINE_RE.finditer(original)
        ]:
            snap_index += 1
            snapshots.append((snap_index, actual_name, expected_name))
        if not snapshots:
            return False, "no diff pairs"

        script_copy = tmp_root_path / "test.sh"
        script_copy.write_text(modified)
        script_copy.chmod(0o755)

        env = os.environ.copy()
        env["ONE_PREFIX"] = str(prefix)
        try:
            result = subprocess.run(
                ["sh", str(script_copy), str(indexer_bin), edition, version],
                cwd=str(root),
                env=env,
                capture_output=True,
                text=True,
                timeout=180,
            )
        except subprocess.TimeoutExpired:
            return False, "timeout"

        patched = original
        for snap_idx, actual_name, expected_name in reversed(snapshots):
            snap_file = tmp_override / f"{actual_name}.snap{snap_idx}"
            if not snap_file.exists():
                return False, (
                    f"snapshot {snap_idx} ({actual_name}) missing "
                    f"(rc={result.returncode})"
                )
            actual_body = snap_file.read_text()

            diff_positions = parse_diff_pairs(patched)
            if snap_idx > len(diff_positions):
                return False, f"diff {snap_idx} not found"
            pos = diff_positions[snap_idx - 1][0]
            heredoc_match = find_heredoc_before(patched, pos, expected_name)
            if not heredoc_match:
                return False, f"heredoc for {expected_name} not found"
            literal = "'EOF'" in heredoc_match.group(2)
            if not literal:
                actual_body = normalise_for_expansion(
                    actual_body, tmp_override, prefix
                )
                actual_body = escape_dollars_outside_substitutions(actual_body)
            if not actual_body.endswith("\n"):
                actual_body += "\n"

            opening = heredoc_match.group(1)
            closing = heredoc_match.group(4)
            replacement = opening + actual_body + closing
            patched = (
                patched[: heredoc_match.start()]
                + replacement
                + patched[heredoc_match.end() :]
            )

        if patched == original:
            return False, "no change"

        script_path.write_text(patched)
        return True, "updated"


def collect_failing_from_make(root: Path) -> list[str]:
    result = subprocess.run(
        ["make", "test"],
        cwd=str(root),
        capture_output=True,
        text=True,
    )
    pattern = re.compile(r"one\.index\.([A-Za-z0-9_.-]+) \(Failed\)")
    seen = set()
    out = []
    for match in pattern.finditer(result.stdout + result.stderr):
        name = match.group(1)
        if name not in seen:
            seen.add(name)
            out.append(name)
    return out


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument(
        "--all",
        action="store_true",
        help="Run `make test` and regenerate every failing CLI index test.",
    )
    source.add_argument(
        "--stdin",
        action="store_true",
        help="Read qualified test names (e.g. common.fail-bundle-ref-no-fragment) "
        "from stdin, one per line.",
    )
    source.add_argument(
        "tests",
        nargs="*",
        default=[],
        help="Qualified test names to regenerate.",
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=None,
        help="Project root. Defaults to the closest ancestor of CWD with "
        "a CMakeLists.txt and src/.",
    )
    parser.add_argument(
        "--prefix",
        type=Path,
        default=None,
        help="ONE_PREFIX (the resolved install prefix used by tests). "
        "Defaults to <root>/build/dist.",
    )
    parser.add_argument(
        "--indexer",
        type=Path,
        default=None,
        help="Path to the sourcemeta-one-index binary. "
        "Defaults to <prefix>/bin/sourcemeta-one-index.",
    )
    parser.add_argument(
        "--edition",
        default="enterprise",
        help="Edition string passed to each test (default: enterprise).",
    )
    parser.add_argument(
        "--version",
        default="0.0.0",
        help="Version string passed to each test (default: 0.0.0).",
    )

    args = parser.parse_args()

    root = (args.root or discover_root(Path.cwd())).resolve()
    prefix = (args.prefix or (root / "build" / "dist")).resolve()
    indexer = (args.indexer or (prefix / "bin" / "sourcemeta-one-index")).resolve()

    if not indexer.exists():
        print(f"error: indexer not found at {indexer}", file=sys.stderr)
        print("       Build the project first: make compile", file=sys.stderr)
        return 1

    if args.all:
        names = collect_failing_from_make(root)
        if not names:
            print("No failing CLI index tests detected.")
            return 0
    elif args.stdin:
        names = [line.strip() for line in sys.stdin if line.strip()]
    else:
        names = list(args.tests)

    if not names:
        print("No tests to regenerate.")
        return 0

    print(f"Regenerating {len(names)} tests")
    updated = []
    skipped = []
    failed = []
    for name in names:
        script_path = find_test_path(root, name)
        changed, message = regenerate_one(
            script_path, root, prefix, indexer, args.edition, args.version
        )
        if changed:
            updated.append(name)
        elif message in ("no change", "no diff pairs"):
            skipped.append((name, message))
        else:
            failed.append((name, message))

    print(f"Updated: {len(updated)}")
    if skipped:
        print(f"Skipped: {len(skipped)}")
        for name, reason in skipped:
            print(f"  - {name}: {reason}")
    if failed:
        print(f"Failed:  {len(failed)}")
        for name, reason in failed:
            print(f"  - {name}: {reason}")
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
