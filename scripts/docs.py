#!/usr/bin/env python3
"""Generate the source catalog and check this repository's documentation.

Requires Python 3.10+ and Git. No packages, network requests, or code execution.
The link checker covers the inline Markdown links and ATX headings used here.
"""

import argparse
from collections import Counter, defaultdict
from pathlib import Path, PurePosixPath
import re
import subprocess
import sys
import unicodedata
from urllib.parse import quote, unquote, urlsplit


ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / "docs/catalog.md"
REQUIRED = (
    "README.md", "CONTRIBUTING.md", "docs/README.md", "docs/structure.md",
    "docs/catalog.md", "docs/quick-start.md", "docs/conventions.md",
    "docs/tooling.md", "docs/maintenance.md", "docs/pages.md",
    "docs/index.md", "docs/api/index.md", "docs/api/snapshot.md",
    "docs/api/updates.md", "docs/api/models.md", "docs/api/dependencies.md", "docs/api/standards.md",
)
LANGUAGES = {
    ".cpp": "C++", ".cc": "C++", ".cxx": "C++", ".c": "C",
    ".py": "Python", ".java": "Java", ".rs": "Rust", ".go": "Go", ".kt": "Kotlin",
}
INFRASTRUCTURE = {"docs", "scripts", "vendor", "assets", "data"}
SOURCE_MARKER = re.compile(r"<!-- source-commit: ([0-9a-f]{40}) -->")
LINK = re.compile(r"!?\[[^\]\n]*\]\(\s*(?:<([^>\n]+)>|([^\s)]+))(?:\s+\"[^\"]*\")?\s*\)")
HEADING = re.compile(r"^ {0,3}(#{1,6})\s+(.+?)\s*#*\s*$")


def git(*args):
    return subprocess.check_output(["git", *args], cwd=ROOT).decode("utf-8")


def tracked_paths(ref):
    return sorted(filter(None, git("ls-tree", "-r", "--name-only", "-z", ref).split("\0")))


def is_source(value):
    path = PurePosixPath(value)
    return (path.suffix.lower() in LANGUAGES
            and path.parts[0] not in INFRASTRUCTURE
            and not path.parts[0].startswith("."))


def link(value):
    return f"[{value}](../{quote(value, safe='/')})"


def catalog_text(commit):
    paths = tracked_paths(commit)
    sources = [value for value in paths if is_source(value)]
    source_paths = set(sources)
    languages = Counter(LANGUAGES[PurePosixPath(value).suffix.lower()] for value in sources)
    groups = defaultdict(list)
    for value in sources:
        parts = PurePosixPath(value).parts
        groups[parts[0] if len(parts) > 1 else "(root)"].append(value)
    stamp = git("show", "-s", "--format=%cI", commit).strip()
    lines = [
        "# 源码目录清单", "", f"<!-- source-commit: {commit} -->", "",
        "> 自动生成：`python3 scripts/docs.py generate --ref origin/main`。不要手工修改本文件。", "",
        f"来源：[`{commit[:7]}`](https://github.com/xw7qwq/solutions/commit/{commit})；提交时间：`{stamp}`。", "",
        "这里统计该提交的 Git 跟踪文件，支持 C/C++、Python、Java、Rust、Go、Kotlin。隐藏工具目录、docs/scripts/vendor/assets/data 不计入算法源码；Templates 和根目录单列。",
        "源码文件数不等于独立题目数、AC 数或网站收录数；没有自动推断题目完成状态。", "",
        "## 总览", "", "| 范围 | 数量 |", "| --- | ---: |",
        f"| 源提交的全部跟踪文件 | {len(paths):,} |",
        f"| 算法相关源码文件 | {len(sources):,} |",
        f"| 平台目录中的源码 | {sum(len(v) for k, v in groups.items() if k not in {'Templates', '(root)'}):,} |",
        f"| 模板源码 | {len(groups.get('Templates', [])):,} |",
        f"| 根目录独立源码 | {len(groups.get('(root)', [])):,} |", "",
        "语言分布：" + "、".join(f"{name} {count:,}" for name, count in sorted(languages.items())) + "。", "",
        "## 平台目录", "", "| 目录 | 源码数 | 文件示例 |", "| --- | ---: | --- |",
    ]
    for name, files in sorted(groups.items()):
        if name not in {"Templates", "(root)"}:
            lines.append(f"| {link(name + '/')} | {len(files):,} | {link(files[0])} |")
    lines += ["", "## 全部模板源码", "", "| 文件 |", "| --- |"]
    lines += [f"| {link(value)} |" for value in groups.get("Templates", [])]
    lines += ["", "## 根目录独立源码", "", "| 文件 |", "| --- |"]
    lines += [f"| {link(value)} |" for value in groups.get("(root)", [])]
    lines += ["", "## 源提交中的其他跟踪文件", "",
              "列出未计入上述源码的全部文件，以便区分笔记、配置和历史杂项。文档分支新增的文件不冒充源提交已有内容。", "",
              "| 文件 |", "| --- |"]
    lines += [f"| {link(value)} |" for value in paths if value not in source_paths]
    lines += ["", "[项目结构](structure.md) · [更新方法](maintenance.md) · [文档导航](README.md)", ""]
    return "\n".join(lines)


def prose_lines(text):
    """Keep line numbers while excluding fenced examples from Markdown checks."""
    fence = None
    for number, line in enumerate(text.splitlines(), 1):
        match = re.match(r"^ {0,3}(`{3,}|~{3,})(.*)$", line)
        if match:
            marker, rest = match.groups()
            if fence is None:
                fence = marker
            elif marker[0] == fence[0] and len(marker) >= len(fence) and not rest.strip():
                fence = None
            continue
        if fence is None:
            yield number, line
    if fence is not None:
        raise ValueError("unclosed code fence")


def heading_anchors(lines):
    anchors = set()
    for _, line in lines:
        match = HEADING.match(line)
        if not match:
            continue
        title = re.sub(r"<[^>]*>", "", match.group(2)).lower()
        slug = "".join(char for char in title if char in " -_" or unicodedata.category(char)[0] in "LMN")
        slug = slug.replace(" ", "-")
        candidate = slug
        suffix = 0
        while candidate in anchors:
            suffix += 1
            candidate = f"{slug}-{suffix}"
        anchors.add(candidate)
    return anchors


def check():
    errors = []
    tracked = set(filter(None, git("ls-files", "-z").split("\0")))
    for relative in REQUIRED:
        if not (ROOT / relative).is_file():
            errors.append(f"missing required document: {relative}")
    documents = sorted(set([ROOT / "README.md", ROOT / "CONTRIBUTING.md", *ROOT.glob("docs/**/*.md")]))
    checked_links = 0
    for document in documents:
        if not document.is_file():
            continue
        relative = document.relative_to(ROOT)
        if relative.as_posix() not in tracked:
            errors.append(f"{relative}: document is not tracked; add new documents to Git before checking")
        raw = document.read_bytes()
        text = raw.decode("utf-8")
        if b"\r" in raw or not raw.endswith(b"\n"):
            errors.append(f"{relative}: use LF and end the file with a newline")
        for number, line in enumerate(text.splitlines(), 1):
            if line.rstrip() != line:
                errors.append(f"{relative}:{number}: trailing whitespace")
        try:
            lines = list(prose_lines(text))
        except ValueError as exc:
            errors.append(f"{relative}: {exc}")
            continue
        if sum(bool(re.match(r"^# ", line)) for _, line in lines) != 1:
            errors.append(f"{relative}: expected one top-level heading")
        for number, line in lines:
            for match in LINK.finditer(line):
                destination = match.group(1) or match.group(2)
                url = urlsplit(destination)
                if url.scheme or url.netloc:
                    continue
                checked_links += 1
                target = (document.parent / unquote(url.path)).resolve() if url.path else document
                if not target.is_relative_to(ROOT):
                    errors.append(f"{relative}:{number}: link leaves repository: {destination}")
                elif not target.exists():
                    errors.append(f"{relative}:{number}: missing link target: {destination}")
                else:
                    target_relative = target.relative_to(ROOT).as_posix()
                    in_git = target_relative in tracked
                    if target.is_dir():
                        prefix = "" if target == ROOT else target_relative + "/"
                        in_git = any(value.startswith(prefix) for value in tracked)
                    if not in_git:
                        errors.append(f"{relative}:{number}: link target is not tracked by Git: {destination}")
                if target.is_file() and target.is_relative_to(ROOT) and url.fragment and target.suffix == ".md":
                    try:
                        anchors = heading_anchors(prose_lines(target.read_text(encoding="utf-8")))
                        if unquote(url.fragment) not in anchors:
                            errors.append(f"{relative}:{number}: missing anchor: {destination}")
                    except ValueError as exc:
                        errors.append(f"{relative}:{number}: target {exc}")
    if CATALOG.is_file():
        actual = CATALOG.read_text(encoding="utf-8")
        marker = SOURCE_MARKER.search(actual)
        if marker is None:
            errors.append("docs/catalog.md: missing source commit marker")
        else:
            commit = marker.group(1)
            try:
                if actual != catalog_text(commit):
                    errors.append("docs/catalog.md: stale or edited generated content; run generate")
                baseline = {value for value in tracked_paths(commit) if is_source(value)}
                current = {value for value in tracked if is_source(value)}
                if baseline != current:
                    errors.append("source paths differ from catalog baseline; sync source and regenerate")
            except subprocess.CalledProcessError:
                errors.append("catalog source commit unavailable; fetch complete repository history")
    if errors:
        print("\n".join(errors), file=sys.stderr)
        return 1
    print(f"Documentation OK: {len(documents)} Markdown files, {checked_links} local links, reproducible catalog.")
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    generate = commands.add_parser("generate", help="refresh docs/catalog.md from a Git commit")
    generate.add_argument("--ref", default="origin/main", help="source branch or commit (default: origin/main)")
    commands.add_parser("check", help="validate documentation and the recorded source inventory")
    args = parser.parse_args()
    if args.command == "generate":
        commit = git("rev-parse", "--verify", "--end-of-options", f"{args.ref}^{{commit}}").strip()
        CATALOG.parent.mkdir(parents=True, exist_ok=True)
        CATALOG.write_text(catalog_text(commit), encoding="utf-8")
        print(f"Generated docs/catalog.md from {commit}.")
        return 0
    return check()


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, UnicodeError, subprocess.CalledProcessError) as error:
        print(f"Documentation check failed: {error}", file=sys.stderr)
        sys.exit(1)
