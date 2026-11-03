#!/usr/bin/env python3
"""Check that GTlab headers use flat includes for other GTlab headers.

Example:
    python check_header_includes.py
    python check_header_includes.py --format rdjsonl
    python check_header_includes.py --exclude '**/internal/**'

When present next to this script, header-includes.json supplies
"source_root" and "exclude" settings. Relative roots are resolved from the
config file; exclude patterns are relative to that root. CLI root overrides
the config, while CLI excludes are added to it. No external Python packages
required.
"""

from __future__ import annotations

import argparse
import fnmatch
import json
import re
import sys
from pathlib import Path
from typing import Iterator

HEADER_EXTENSIONS = {".h", ".hh", ".hpp", ".hxx"}
DEFAULT_CONFIG = Path(__file__).resolve().with_name("header-includes.json")
INCLUDE_RE = re.compile(
    r'^\s*#\s*include\s*(?:"(?P<quoted>[^"\r\n]+)"|<(?P<angled>[^>\r\n]+)>)'
)


def is_excluded(relative_path: Path, patterns: list[str]) -> bool:
    """Check optional globs relative to the scan root."""
    path = relative_path.as_posix()
    return any(
        fnmatch.fnmatchcase(path, pattern) or fnmatch.fnmatchcase("/" + path, pattern)
        for pattern in patterns
    )


def collect_headers(root: Path, patterns: list[str]) -> list[Path]:
    """All non-excluded headers, for both the index and the files to check."""
    return sorted(
        path
        for path in root.rglob("*")
        if path.is_file()
        and path.suffix.lower() in HEADER_EXTENSIONS
        and not is_excluded(path.relative_to(root), patterns)
    )


def without_comments(line: str, inside_block: bool) -> tuple[str, bool]:
    """Mask comments while preserving character offsets and line length."""
    out = list(line)
    i = 0
    quote = None
    while i < len(line):
        if inside_block:
            if line.startswith("*/", i):
                out[i : i + 2] = "  "
                inside_block = False
                i += 2
            else:
                out[i] = " " if line[i] != "\n" else "\n"
                i += 1
        elif quote:
            if line[i] == "\\":
                i += 2  # Skip escaped character (if any).
            elif line[i] == quote:
                quote = None
                i += 1
            else:
                i += 1
        elif line.startswith("//", i):
            out[i:] = [" " if c != "\n" else "\n" for c in line[i:]]
            break
        elif line.startswith("/*", i):
            out[i : i + 2] = "  "
            inside_block = True
            i += 2
        elif line[i] in ('"', "'"):
            quote = line[i]
            i += 1
        else:
            i += 1
    return "".join(out), inside_block


def find_issues(headers: list[Path]) -> Iterator[tuple[Path, int, int, str, str]]:
    """Yield (file, line, column, invalid_target, basename)."""
    known_names = {header.name for header in headers}
    for header in headers:
        inside_block = False
        with header.open(encoding="utf-8-sig", errors="replace") as stream:
            for line_no, line in enumerate(stream, 1):
                code, inside_block = without_comments(line, inside_block)
                match = INCLUDE_RE.match(code)
                if not match:
                    continue
                group = "quoted" if match.group("quoted") is not None else "angled"
                target = match.group(group)
                normalized = target.replace("\\", "/")
                basename = normalized.rsplit("/", 1)[-1]
                if "/" in normalized and basename in known_names:
                    yield header, line_no, match.start(group) + 1, target, basename


def make_diagnostic(
    path: str, line: int, column: int, target: str, basename: str
) -> dict:
    start = {"line": line, "column": column}
    end = {"line": line, "column": column + len(target)}
    return {
        "message": (
            f"GTlab header include '{target}' contains a directory. "
            f"Use '#include \"{basename}\"' instead: GTlab header paths "
            "are not preserved by the flat installation layout."
        ),
        "location": {"path": path, "range": {"start": start}},
        "severity": "ERROR",
        "source": {"name": "gtlab-header-includes"},
        "code": {"value": "GTLAB001"},
        "suggestions": [{"range": {"start": start, "end": end}, "text": basename}],
    }


def load_config(path: Path | None) -> tuple[Path, list[str]]:
    """Load source_root and exclude settings, with a default config if present."""
    if path is None:
        if not DEFAULT_CONFIG.is_file():
            return Path("src"), []
        path = DEFAULT_CONFIG

    config_path = path.resolve()
    try:
        config = json.loads(config_path.read_text(encoding="utf-8-sig"))
    except (OSError, json.JSONDecodeError) as error:
        raise ValueError(f"cannot read config {config_path}: {error}") from error
    if not isinstance(config, dict):
        raise ValueError("config must contain a JSON object")

    source_root = config.get("source_root", "src")
    excludes = config.get("exclude", [])
    if not isinstance(source_root, str) or not source_root:
        raise ValueError("config field 'source_root' must be a non-empty string")
    if not isinstance(excludes, list) or not all(isinstance(item, str) for item in excludes):
        raise ValueError("config field 'exclude' must be an array of strings")

    root = Path(source_root)
    if not root.is_absolute():
        root = config_path.parent / root
    return root, excludes


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--config", type=Path, help="JSON config file (default: header-includes.json next to this script)."
    )
    parser.add_argument("--root", type=Path, help="Override the configured source tree.")
    parser.add_argument(
        "--exclude", action="append", default=[], metavar="GLOB",
        help="Add an exclude glob to those in the config (repeatable).",
    )
    parser.add_argument(
        "--format", choices=("text", "rdjsonl"), default="text",
        help="Use rdjsonl to pipe findings into reviewdog.",
    )
    args = parser.parse_args(argv)
    try:
        config_root, config_excludes = load_config(args.config)
    except ValueError as error:
        print(f"error: {error}", file=sys.stderr)
        return 2
    root = (args.root or config_root).resolve()
    if not root.is_dir():
        print(f"error: source directory not found: {root}", file=sys.stderr)
        return 2

    headers = collect_headers(root, config_excludes + args.exclude)
    if not headers:
        print(f"error: no headers found below {root}", file=sys.stderr)
        return 2

    issues = 0
    for file, line, column, target, basename in find_issues(headers):
        issues += 1
        try:
            display_path = file.relative_to(Path.cwd().resolve()).as_posix()
        except ValueError:
            display_path = file.as_posix()
        diagnostic = make_diagnostic(display_path, line, column, target, basename)
        if args.format == "rdjsonl":
            print(json.dumps(diagnostic, ensure_ascii=False))
        else:
            print(f"{display_path}:{line}:{column}: error GTLAB001: "
                  f"'{target}' -> '#include \"{basename}\"'")

    # In rdjsonl mode reviewdog should decide the CI status after diff filtering.
    # In text mode, the linter itself is usable as a CI quality gate.
    if args.format == "text":
        print(f"Checked {len(headers)} headers; found {issues} invalid includes.", file=sys.stderr)
    return 1 if issues and args.format == "text" else 0


if __name__ == "__main__":
    sys.exit(main())
