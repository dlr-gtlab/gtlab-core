#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
# SPDX-License-Identifier: MPL-2.0+

"""Collect GCC/Clang diagnostics and compare compatible Ubuntu build reports."""

from __future__ import annotations

import argparse
import collections
import html
import json
import os
import re
import sys
import urllib.error
import urllib.parse
import urllib.request
import xml.etree.ElementTree as ET
import zipfile
from pathlib import Path


DIAGNOSTIC = re.compile(
    r"^(?P<file>.+?):(?P<line>\d+)(?::(?P<column>\d+))?:\s*warning:\s*"
    r"(?P<message>.*?)(?:\s+\[(?P<option>-W[^\]]+)\])?\s*$"
)
ANSI_ESCAPE = re.compile(r"\x1b\[[0-?]*[ -/]*[@-~]")
WARNING_MARKER = re.compile(r"^(?=\S).*?\bwarning:")
REPORT_NAME = "compiler-warnings-ubuntu"
API = "https://api.github.com"
PLATFORM = "ubuntu-24.04"
SCHEMA_VERSION = 2
BASELINE_ERRORS = (OSError, ValueError, KeyError, zipfile.BadZipFile)


def normalize_path(filename: str, root: Path) -> str:
    path = Path(filename)
    if not path.is_absolute():
        path = root / path
    try:
        return path.resolve().relative_to(root.resolve()).as_posix()
    except ValueError:
        return path.resolve().as_posix()


def diagnostic_key(item: dict) -> tuple[str, str, str]:
    return item["file"], item["option"], " ".join(item["message"].split())


def parse_log(log_path: Path, root: Path) -> list[dict]:
    warnings = []
    unsupported = []
    with log_path.open(encoding="utf-8", errors="replace") as stream:
        for raw_line in stream:
            line = ANSI_ESCAPE.sub("", raw_line.rstrip())
            match = DIAGNOSTIC.match(line)
            if match:
                item = match.groupdict()
                warnings.append({
                    "file": normalize_path(item["file"], root),
                    "line": int(item["line"]),
                    "column": int(item["column"] or 0),
                    "message": " ".join(item["message"].split()).replace(str(root), "<workspace>"),
                    "option": item["option"] or "",
                })
            elif WARNING_MARKER.search(line):
                unsupported.append(line)
    if unsupported:
        sample = "\n".join(unsupported[:5])
        raise ValueError(f"Unrecognized warning diagnostics ({len(unsupported)}); refusing an incomplete report:\n{sample}")
    return warnings


def make_report(log_path: Path, root: Path, revision: str, compiler: str, build_succeeded: bool) -> dict:
    return {
        "schema_version": SCHEMA_VERSION,
        "revision": revision,
        "compiler": compiler,
        "platform": PLATFORM,
        "build_succeeded": build_succeeded,
        "warnings": parse_log(log_path, root),
    }


def valid_warning(item: object) -> bool:
    return (
        isinstance(item, dict)
        and all(isinstance(item.get(key), str) for key in ("file", "message", "option"))
        and bool(item["file"])
        and bool(item["message"].strip())
        and type(item.get("line")) is int
        and item["line"] > 0
        and type(item.get("column")) is int
        and item["column"] >= 0
    )


def load_report(path: Path) -> dict:
    report = json.loads(path.read_text(encoding="utf-8"))
    if (
        not isinstance(report, dict)
        or report.get("schema_version") != SCHEMA_VERSION
        or not isinstance(report.get("revision"), str)
        or not report["revision"]
        or not isinstance(report.get("compiler"), str)
        or not report["compiler"].strip()
        or report["compiler"] == "unknown"
        or report.get("platform") != PLATFORM
        or type(report.get("build_succeeded")) is not bool
        or not isinstance(report.get("warnings"), list)
        or not all(valid_warning(item) for item in report["warnings"])
    ):
        raise ValueError(f"Unsupported or malformed warning report: {path}")
    return report


def compare_reports(baseline: dict, current: dict) -> dict:
    old = collections.defaultdict(list)
    new = collections.defaultdict(list)
    for item in baseline["warnings"]:
        old[diagnostic_key(item)].append(item)
    for item in current["warnings"]:
        new[diagnostic_key(item)].append(item)
    result = {"new": [], "existing": [], "resolved": []}
    for key in sorted(old.keys() | new.keys()):
        before, after = len(old[key]), len(new[key])
        common = min(before, after)
        for category, entries in (
            ("new", new[key][common:]),
            ("existing", new[key][:common]),
            ("resolved", old[key][common:]),
        ):
            result[category].extend(
                {**item, "baseline_count": before, "current_count": after}
                for item in entries
            )
    return result


class NoRedirect(urllib.request.HTTPRedirectHandler):
    def redirect_request(self, request, file_pointer, code, message, headers, new_url):
        return None


def api_json(url: str, token: str) -> dict:
    request = urllib.request.Request(url, headers={
        "Authorization": f"Bearer {token}",
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": "2022-11-28",
    })
    with urllib.request.build_opener(NoRedirect()).open(request, timeout=30) as response:
        return json.load(response)


def api_items(url: str, key: str, token: str):
    """Exhaust pagination without the 1,000-result workflow-search limit."""
    page = 1
    separator = "&" if "?" in url else "?"
    while True:
        items = api_json(f"{url}{separator}per_page=100&page={page}", token)[key]
        yield from items
        if len(items) < 100:
            return
        page += 1


def download_artifact(url: str, token: str, destination: Path) -> None:
    request = urllib.request.Request(url, headers={"Authorization": f"Bearer {token}"})
    try:
        response = urllib.request.build_opener(NoRedirect()).open(request, timeout=30)
    except urllib.error.HTTPError as error:
        if error.code != 302 or not error.headers.get("Location"):
            raise
        location = error.headers["Location"]
        if urllib.parse.urlsplit(location).scheme != "https":
            raise ValueError("Artifact storage redirect must use HTTPS")
        # Fetch the signed storage URL without forwarding the repository token.
        with urllib.request.urlopen(location, timeout=60) as artifact:
            destination.write_bytes(artifact.read())
        return
    with response:
        destination.write_bytes(response.read())


def baseline_problem(baseline: dict, current: dict, base_revision: str) -> str | None:
    if not baseline["build_succeeded"]:
        return "baseline was produced by an incomplete or failed build"
    if not base_revision or baseline["revision"] != base_revision:
        return "baseline does not represent the PR base revision"
    if baseline["compiler"] != current["compiler"]:
        return "baseline compiler identity/version differs from the PR compiler"
    return None


def find_baseline(repository: str, token: str, destination: Path, current: dict, base_revision: str) -> dict | None:
    # Artifact enumeration has no workflow-search cap. Sort explicitly so a
    # rerun or API ordering cannot select an older report before a newer one.
    artifacts = list(api_items(f"{API}/repos/{repository}/actions/artifacts?name={REPORT_NAME}", "artifacts", token))
    artifacts.sort(key=lambda item: (item["created_at"], item["id"]), reverse=True)
    for artifact in artifacts:
        if artifact["name"] != REPORT_NAME or artifact["expired"] or artifact["size_in_bytes"] <= 0:
            continue
        candidate = artifact.get("workflow_run", {})
        if candidate.get("head_branch") != "master" or candidate.get("head_sha") != base_revision:
            continue
        run = api_json(f"{API}/repos/{repository}/actions/runs/{candidate['id']}", token)
        if (
            run.get("head_branch") != "master" or run.get("head_sha") != base_revision
            or run.get("event") != "push" or run.get("status") != "completed"
            or run.get("path") != ".github/workflows/ci.yml"
        ):
            continue
        jobs = api_items(f"{API}/repos/{repository}/actions/runs/{run['id']}/jobs?filter=latest", "jobs", token)
        if not any(
            job.get("conclusion") == "success" and job.get("status") == "completed"
            and job.get("name", "").rsplit(" / ", 1)[-1] == "Build and test (ubuntu-24.04)"
            for job in jobs
        ):
            continue
        try:
            # Construct the API URL instead of authenticating to a URL in data.
            download_artifact(f"{API}/repos/{repository}/actions/artifacts/{artifact['id']}/zip", token, destination)
            with zipfile.ZipFile(destination) as archive:
                reports = [name for name in archive.namelist() if name == "compiler-warnings.json"]
                if len(reports) != 1:
                    continue
                report_bytes = archive.read(reports[0])
            destination.write_bytes(report_bytes)
            report = load_report(destination)
            if baseline_problem(report, current, base_revision) is None:
                return report
        except BASELINE_ERRORS:
            continue
    return None


def warning_details(warnings: list[dict]) -> list[str]:
    lines = []
    for item in warnings[:100]:
        location = html.escape(f"{item['file']}:{item['line']}")
        message = html.escape(item["message"])
        option = f" ({html.escape(item['option'])})" if item["option"] else ""
        lines.append(f"- <code>{location}</code>: {message}{option}")
    if len(warnings) > 100:
        lines.append("- Further warnings are listed in the complete artifact.")
    return lines


def report_summary(current: dict) -> list[str]:
    return [
        "### Compiler warnings", "",
        f"Ubuntu build: **{len(current['warnings'])} warning(s)**. "
        "Download `compiler-warnings-ubuntu` for the complete report.",
        f"Compilation successful: **{current['build_succeeded']}**.", "",
    ]


def write_checkstyle(warnings: list[dict], destination: Path) -> None:
    root = ET.Element("checkstyle", version="4.3")
    files = {}
    for item in warnings:
        if item["file"] not in files:
            files[item["file"]] = ET.SubElement(root, "file", name=item["file"])
        ET.SubElement(files[item["file"]], "error", line=str(item["line"]), column=str(item["column"]),
                      message=item["message"], severity="warning", source=item["option"])
    destination.write_bytes(ET.tostring(root, encoding="utf-8", xml_declaration=True))


def compare_command(args) -> int:
    current = load_report(args.current)  # Invalid current analysis must fail.
    baseline = None
    reason = None
    if args.target_branch != "master":
        reason = f"unsupported target: {args.target_branch}; enforcement supports master PRs only"
    elif not current["build_succeeded"]:
        reason = "PR compilation was incomplete or failed"
    elif not args.base_revision:
        reason = "PR base revision is unavailable"
    elif args.baseline:
        try:
            baseline = load_report(args.baseline)
            reason = baseline_problem(baseline, current, args.base_revision)
        except BASELINE_ERRORS as error:
            reason = str(error)
    elif args.repository and args.token:
        temporary = args.current.with_name("master-warnings-download.tmp")
        try:
            baseline = find_baseline(args.repository, args.token, temporary, current, args.base_revision)
        except (OSError, urllib.error.URLError) as error:
            reason = f"baseline API lookup unavailable: {error}"
        finally:
            temporary.unlink(missing_ok=True)
    if baseline is None and reason is None:
        reason = "no valid successful master artifact matches the PR base revision and compiler"

    lines = report_summary(current)
    new_warnings = []
    if reason:
        lines.extend([f"Comparison **not verified**: {html.escape(reason)}. Please review the report manually.", ""])
        lines.extend(warning_details(current["warnings"]))
    else:
        changes = compare_reports(baseline, current)
        new_warnings = changes["new"]
        lines.extend([
            f"Baseline <code>{html.escape(baseline['revision'])}</code>; compiler <code>{html.escape(current['compiler'])}</code>.",
            f"New: **{len(new_warnings)}** · Existing: **{len(changes['existing'])}** · Resolved: **{len(changes['resolved'])}**", "",
        ])
        if new_warnings and args.head_revision and args.repository and args.token:
            try:
                head = urllib.parse.quote(args.head_revision, safe="")
                comparison = api_json(f"{API}/repos/{args.repository}/compare/master...{head}", args.token)
                if comparison["behind_by"] > 0:
                    lines.extend(["Please rebase onto the latest master and rerun the checks.", ""])
            except (OSError, urllib.error.URLError):
                pass
        for category, entries in changes.items():
            if entries:
                lines.extend([f"#### {category.title()} warnings", "", *warning_details(entries), ""])
    args.summary.write_text("\n".join(lines) + "\n", encoding="utf-8")
    write_checkstyle(new_warnings, args.checkstyle)
    if os.environ.get("GITHUB_OUTPUT"):
        with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as output:
            output.write(f"new_warnings={len(new_warnings)}\n")
    print(f"Comparison {'not verified' if reason else 'verified'}; {len(new_warnings)} new warning(s).")
    return 1 if new_warnings else 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    analyze = commands.add_parser("analyze")
    analyze.add_argument("--log", type=Path, required=True)
    analyze.add_argument("--root", type=Path, required=True)
    analyze.add_argument("--output", type=Path, required=True)
    analyze.add_argument("--revision", required=True)
    analyze.add_argument("--compiler", required=True)
    analyze.add_argument("--build-succeeded", choices=("true", "false"), required=True)
    summary = commands.add_parser("summary")
    summary.add_argument("--current", type=Path, required=True)
    summary.add_argument("--summary", type=Path, required=True)
    compare = commands.add_parser("compare")
    compare.add_argument("--current", type=Path, required=True)
    compare.add_argument("--baseline", type=Path)
    compare.add_argument("--repository", default=os.environ.get("GITHUB_REPOSITORY", ""))
    compare.add_argument("--token", default=os.environ.get("GITHUB_TOKEN", ""))
    compare.add_argument("--summary", type=Path, required=True)
    compare.add_argument("--checkstyle", type=Path, required=True)
    compare.add_argument("--target-branch", default="master")
    compare.add_argument("--base-revision", default="")
    compare.add_argument("--head-revision", default="")
    args = parser.parse_args()
    if args.command == "analyze":
        report = make_report(args.log, args.root, args.revision, args.compiler, args.build_succeeded == "true")
        args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(f"Captured {len(report['warnings'])} compiler warning(s).")
        return 0
    if args.command == "summary":
        report = load_report(args.current)
        args.summary.write_text("\n".join(report_summary(report) + warning_details(report["warnings"])) + "\n", encoding="utf-8")
        return 0
    return compare_command(args)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError) as error:
        print(f"Warning analysis failed: {error}", file=sys.stderr)
        sys.exit(2)
