#!/usr/bin/env python3
"""Generate an amalgamated single-include header for jsonpp.

The script recursively expands local includes from an entry header, deduplicates
system includes, and writes one usable header file.
"""

from __future__ import annotations

import argparse
from pathlib import Path
import re
import sys


INCLUDE_RE = re.compile(r'^\s*#\s*include\s*([<"])([^">]+)[>"]\s*(?://.*)?$')
IFNDEF_RE = re.compile(r'^\s*#\s*ifndef\s+([A-Za-z_]\w*)\s*$')
DEFINE_RE = re.compile(r'^\s*#\s*define\s+([A-Za-z_]\w*)(?:\s+.*)?$')
ENDIF_RE = re.compile(r'^\s*#\s*endif\b')


class Amalgamator:
    def __init__(self, src_root: Path) -> None:
        self.src_root = src_root.resolve()
        self.repo_root = self.src_root.parent
        self.visited: set[Path] = set()
        self.system_includes: dict[str, None] = {}
        self.chunks: list[str] = []

    def build(self, entry_header: Path, include_all_headers: bool) -> str:
        self._process_header(entry_header.resolve())

        if include_all_headers:
            for header in sorted(self.src_root.rglob("*.hpp")):
                self._process_header(header.resolve())

        return self._compose_output()

    def _compose_output(self) -> str:
        out: list[str] = []

        for header in self.system_includes:
            out.append(f"#include <{header}>\n")

        if self.system_includes:
            out.append("\n")

        out.extend(self.chunks)
        return "".join(out)

    def _process_header(self, header_path: Path) -> None:
        header_path = header_path.resolve()
        if header_path in self.visited:
            return

        if not header_path.is_file():
            raise FileNotFoundError(f"Header does not exist: {header_path}")

        self.visited.add(header_path)

        lines = header_path.read_text(encoding="utf-8").splitlines(keepends=True)
        skip_lines = self._detect_include_guard_lines(lines)
        body: list[str] = []

        for idx, line in enumerate(lines):
            if idx in skip_lines:
                continue

            include_match = INCLUDE_RE.match(line)
            if include_match:
                quote_type = include_match.group(1)
                include_target = include_match.group(2).strip()

                if quote_type == "<":
                    self.system_includes.setdefault(include_target, None)
                    continue

                resolved = self._resolve_local_include(header_path, include_target)
                if resolved is None:
                    # Keep unresolved quoted includes untouched.
                    body.append(line)
                    continue

                if self._is_inside_src_root(resolved):
                    self._process_header(resolved)
                    continue

                # For local includes outside src/, keep the include line.
                body.append(line)
                continue

            body.append(line)

        relative = self._relative_to_repo(header_path)
        self.chunks.append(f"// ---- begin: {relative} ----\n")
        self.chunks.extend(body)
        if not body or not body[-1].endswith("\n"):
            self.chunks.append("\n")
        self.chunks.append(f"// ---- end: {relative} ----\n\n")

    def _resolve_local_include(self, current_file: Path, include_target: str) -> Path | None:
        candidate_paths = [
            (current_file.parent / include_target),
            (self.src_root / include_target),
        ]

        for candidate in candidate_paths:
            resolved = candidate.resolve()
            if resolved.is_file():
                return resolved
        return None

    def _is_inside_src_root(self, path: Path) -> bool:
        try:
            path.relative_to(self.src_root)
            return True
        except ValueError:
            return False

    def _relative_to_repo(self, path: Path) -> str:
        try:
            return path.relative_to(self.repo_root).as_posix()
        except ValueError:
            return path.as_posix()

    @staticmethod
    def _detect_include_guard_lines(lines: list[str]) -> set[int]:
        """Detect and remove simple file-level include guards.

        This expects the common pattern:
            #ifndef SOME_GUARD
            #define SOME_GUARD
            ...
            #endif
        """
        skip: set[int] = set()

        ifndef_idx = None
        define_idx = None
        guard_name = None

        for idx, line in enumerate(lines[:120]):
            match = IFNDEF_RE.match(line)
            if match is None:
                continue

            guard_name = match.group(1)
            j = idx + 1
            while j < len(lines) and not lines[j].strip():
                j += 1

            if j < len(lines):
                define_match = DEFINE_RE.match(lines[j])
                if define_match and define_match.group(1) == guard_name:
                    ifndef_idx = idx
                    define_idx = j
            break

        if ifndef_idx is None or define_idx is None or guard_name is None:
            return skip

        last_non_blank = len(lines) - 1
        while last_non_blank >= 0 and not lines[last_non_blank].strip():
            last_non_blank -= 1

        if last_non_blank >= 0 and ENDIF_RE.match(lines[last_non_blank]):
            skip.update({ifndef_idx, define_idx, last_non_blank})

        return skip


def parse_args() -> argparse.Namespace:
    repo_root = Path(__file__).resolve().parents[1]
    default_src_root = repo_root / "src"

    parser = argparse.ArgumentParser(
        description="Generate jsonpp single_include header by merging .hpp files."
    )
    parser.add_argument(
        "--src-root",
        type=Path,
        default=default_src_root,
        help="Path to source headers root (default: ./src).",
    )
    parser.add_argument(
        "--entry",
        type=Path,
        default=default_src_root / "jsonpp.hpp",
        help="Entry header to expand first (default: ./src/jsonpp.hpp).",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=repo_root / "single_include" / "jsonpp.hpp",
        help="Output single-include file path (default: ./single_include/jsonpp.hpp).",
    )
    parser.add_argument(
        "--guard",
        type=str,
        default="JSONPP_SINGLE_INCLUDE_JSONPP_HPP",
        help="Include guard macro for generated output.",
    )
    parser.add_argument(
        "--entry-only",
        action="store_true",
        help="Only merge headers reachable from the entry header.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    src_root = args.src_root.resolve()
    entry = args.entry.resolve()
    output = args.output.resolve()

    if not src_root.is_dir():
        print(f"error: src root does not exist or is not a directory: {src_root}", file=sys.stderr)
        return 1

    if not entry.is_file():
        print(f"error: entry header does not exist: {entry}", file=sys.stderr)
        return 1

    amalgamator = Amalgamator(src_root=src_root)
    merged_body = amalgamator.build(entry_header=entry, include_all_headers=not args.entry_only)

    include_guard = args.guard.strip()
    if not include_guard:
        print("error: include guard macro cannot be empty", file=sys.stderr)
        return 1

    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(
        "/*\n"
        "jsonpp - JSON for Modern C++\n"
        "https://github.com/mikami-w/jsonpp\n\n"
        "This file is auto-generated by scripts/generate_single_include.py\n"
        "*/\n\n"
        f"#ifndef {include_guard}\n"
        f"#define {include_guard}\n\n"
        f"{merged_body}"
        f"#endif // {include_guard}\n",
        encoding="utf-8",
    )

    print(f"generated: {output}")
    print(f"merged headers: {len(amalgamator.visited)}")
    print(f"unique system includes: {len(amalgamator.system_includes)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())