#!/usr/bin/env python3
"""Collect and compare GCC/Clang warning diagnostics from a build log."""

from __future__ import annotations

import argparse
import collections
import json
import os
import re
import sys
import urllib.error
import urllib.request
import zipfile
from pathlib import Path


DIAGNOSTIC = re.compile(
    r"^(?P<file>.+?):(?P<line>\d+):(?P<column>\d+):\s+warning:\s+(?P<message>.*?)(?:\s+\[(?P<option>-W[^\]]+)\])?\s*$"
)
REPORT_NAME = "compiler-warnings-ubuntu"
API = "https://api.github.com"


def normalize_path(filename: str, root: Path) -> str:
    path = Path(filename)
    if not path.is_absolute():
        path = root / path
    try:
        return path.resolve().relative_to(root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def diagnostic_key(item: dict) -> tuple[str, str, str]:
    message = " ".join(item["message"].split())
    return item["file"], item.get("option") or "", message


def parse_log(log_path: Path, root: Path) -> list[dict]:
    warnings = []
    with log_path.open(encoding="utf-8", errors="replace") as stream:
        for line in stream:
            match = DIAGNOSTIC.match(line.rstrip())
            if not match:
                continue
            warning = match.groupdict()
            warnings.append(
                {
                    "file": normalize_path(warning["file"], root),
                    "line": int(warning["line"]),
                    "column": int(warning["column"]),
                    "message": " ".join(warning["message"].split()),
                    "option": warning["option"] or "",
                }
            )
    return warnings


def make_report(log_path: Path, root: Path, revision: str, compiler: str) -> dict:
    return {
        "schema_version": 1,
        "revision": revision,
        "compiler": compiler,
        "platform": "ubuntu-24.04",
        "warnings": parse_log(log_path, root),
    }


def load_report(path: Path) -> dict:
    report = json.loads(path.read_text(encoding="utf-8"))
    if (
        not isinstance(report, dict)
        or report.get("schema_version") != 1
        or not isinstance(report.get("revision"), str)
        or not isinstance(report.get("compiler"), str)
        or report.get("platform") != "ubuntu-24.04"
        or not isinstance(report.get("warnings"), list)
        or any(not isinstance(item, dict) or not {"file", "line", "column", "message", "option"} <= item.keys() for item in report["warnings"])
    ):
        raise ValueError(f"Unsupported or malformed warning report: {path}")
    return report


def compare_reports(baseline: dict, current: dict) -> dict:
    old = collections.Counter(map(diagnostic_key, baseline["warnings"]))
    new = collections.Counter(map(diagnostic_key, current["warnings"]))
    warnings_by_key = {diagnostic_key(item): item for item in current["warnings"] + baseline["warnings"]}
    result = {"new": [], "existing": [], "resolved": []}
    for key in sorted(old.keys() | new.keys()):
        item = warnings_by_key[key]
        before, after = old[key], new[key]
        if after > before:
            item_with_counts = {**item, "baseline_count": before, "current_count": after}
            result["new"].extend([item_with_counts] * (after - before))
        if min(before, after):
            result["existing"].append({**item, "baseline_count": before, "current_count": after})
        if before > after:
            result["resolved"].extend([{**item, "baseline_count": before, "current_count": after}] * (before - after))
    return result


def api_json(url: str, token: str) -> dict:
    request = urllib.request.Request(url, headers={"Authorization": f"Bearer {token}", "Accept": "application/vnd.github+json", "X-GitHub-Api-Version": "2022-11-28"})
    with urllib.request.urlopen(request, timeout=30) as response:
        return json.load(response)


def download_artifact(url: str, token: str, destination: Path) -> None:
    request = urllib.request.Request(url, headers={"Authorization": f"Bearer {token}", "Accept": "application/vnd.github+json"})
    class NoRedirect(urllib.request.HTTPRedirectHandler):
        def redirect_request(self, request, file_pointer, code, message, headers, new_url):
            return None

    opener = urllib.request.build_opener(NoRedirect())
    try:
        response = opener.open(request, timeout=30)
    except urllib.error.HTTPError as error:
        if error.code != 302 or not error.headers.get("Location"):
            raise
        # Fetch the signed storage URL without forwarding the repository token.
        with urllib.request.urlopen(error.headers["Location"], timeout=60) as artifact:
            destination.write_bytes(artifact.read())
        return
    with response:
        destination.write_bytes(response.read())


def find_baseline(repository: str, token: str, destination: Path) -> dict | None:
    page = 1
    while page <= 10:
        runs = api_json(
            f"{API}/repos/{repository}/actions/workflows/ci.yml/runs?branch=master&status=completed&per_page=100&page={page}",
            token,
        ).get("workflow_runs", [])
        if not runs:
            return None
        for run in runs:
            if run.get("head_branch") != "master" or run.get("event") != "push" or run.get("status") != "completed":
                continue
            jobs = api_json(f"{API}/repos/{repository}/actions/runs/{run['id']}/jobs?filter=latest&per_page=100", token).get("jobs", [])
            successful_ubuntu_build = any(
                job.get("conclusion") == "success"
                and job.get("name", "").startswith("Build and test")
                and ("ubuntu-24.04" in job.get("name", "") or "ubuntu-24.04" in job.get("labels", []))
                for job in jobs
            )
            if not successful_ubuntu_build:
                continue
            artifacts = api_json(f"{API}/repos/{repository}/actions/runs/{run['id']}/artifacts?per_page=100", token).get("artifacts", [])
            for artifact in artifacts:
                if artifact.get("name") != REPORT_NAME or artifact.get("expired") or artifact.get("size_in_bytes", 0) <= 0:
                    continue
                try:
                    download_artifact(artifact["archive_download_url"], token, destination)
                    with zipfile.ZipFile(destination) as archive:
                        reports = [name for name in archive.namelist() if name.endswith("warnings.json")]
                        if len(reports) != 1:
                            continue
                        destination.write_bytes(archive.read(reports[0]))
                    report = load_report(destination)
                    if report["revision"] != run.get("head_sha") or report["compiler"] == "unknown":
                        continue
                    return report
                except (OSError, ValueError, KeyError, zipfile.BadZipFile, urllib.error.URLError, json.JSONDecodeError):
                    continue
        if len(runs) < 100:
            return None
        page += 1
    return None


def main() -> int:
    parser = argparse.ArgumentParser()
    commands = parser.add_subparsers(dest="command", required=True)
    analyze = commands.add_parser("analyze")
    analyze.add_argument("--log", type=Path, required=True)
    analyze.add_argument("--root", type=Path, required=True)
    analyze.add_argument("--output", type=Path, required=True)
    analyze.add_argument("--revision", required=True)
    analyze.add_argument("--compiler", required=True)
    compare = commands.add_parser("compare")
    compare.add_argument("--current", type=Path, required=True)
    compare.add_argument("--baseline", type=Path)
    compare.add_argument("--repository", default=os.environ.get("GITHUB_REPOSITORY", ""))
    compare.add_argument("--token", default=os.environ.get("GITHUB_TOKEN", ""))
    compare.add_argument("--summary", type=Path, required=True)
    compare.add_argument("--checkstyle", type=Path, required=True)
    compare.add_argument("--base-revision", default="")
    args = parser.parse_args()

    if args.command == "analyze":
        report = make_report(args.log, args.root, args.revision, args.compiler)
        args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(f"Captured {len(report['warnings'])} compiler warning(s).")
        return 0

    current = load_report(args.current)
    baseline = None
    baseline_error = None
    if args.baseline and args.baseline.exists():
        try:
            baseline = load_report(args.baseline)
        except (OSError, ValueError, json.JSONDecodeError) as error:
            baseline_error = str(error)
    elif args.repository and args.token:
        try:
            temporary = args.current.with_name("master-warnings-download.tmp")
            baseline = find_baseline(args.repository, args.token, temporary)
            temporary.unlink(missing_ok=True)
        except Exception as error:  # API/permissions issues make comparison unavailable, not the analysis broken.
            baseline_error = str(error)
    if baseline is None:
        reason = f" ({baseline_error})" if baseline_error else ""
        warning_details = current["warnings"][:100]
        markdown_lines = [
            "### Compiler warnings",
            "",
            f"PR build: **{len(current['warnings'])} warning(s)**. Comparison **not verified**: no valid successful `master` warning artifact is available{reason}. Please review the report manually.",
        ]
        if warning_details:
            markdown_lines.extend(["", f"<details><summary>Warning details (first {len(warning_details)} of {len(current['warnings'])})</summary>", ""])
            for warning in warning_details:
                option = f" ({warning['option']})" if warning["option"] else ""
                markdown_lines.append(f"- `{warning['file']}:{warning['line']}`: {warning['message']}{option}")
            markdown_lines.extend(["", "</details>"])
        markdown = "\n".join(markdown_lines) + "\n"
        args.summary.write_text(markdown, encoding="utf-8")
        args.checkstyle.write_text('<?xml version="1.0" encoding="utf-8"?>\n<checkstyle version="4.3" />\n', encoding="utf-8")
        if os.environ.get("GITHUB_OUTPUT"):
            with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as output:
                output.write("new_warnings=0\n")
        print(markdown, end="")
        return 0

    changes = compare_reports(baseline, current)
    counts = {key: len(value) for key, value in changes.items()}
    lines = [
        "### Compiler warnings",
        "",
        f"Baseline `{baseline['revision'][:12]}` ({baseline['compiler']}); PR `{current['revision'][:12]}` ({current['compiler']}).",
        f"New: **{counts['new']}** · Existing: **{counts['existing']}** · Resolved: **{counts['resolved']}** · Total: **{len(current['warnings'])}**",
        "",
    ]
    behind_master = False
    if changes["new"] and args.base_revision and args.repository and args.token:
        try:
            master_revision = api_json(f"{API}/repos/{args.repository}/commits/master", args.token)["sha"]
            behind_master = master_revision != args.base_revision
        except Exception:
            pass
    if changes["new"] and behind_master:
        lines.extend(["Please rebase onto the latest master and rerun the checks.", ""])
    for category in ("new", "resolved", "existing"):
        entries = changes[category]
        if entries:
            lines.extend([f"#### {category.title()} warnings", ""])
            for item in entries:
                lines.append(f"- `{item['file']}:{item['line']}`: {item['message']} ({item['baseline_count']} → {item['current_count']})")
            lines.append("")
    args.summary.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines))
    suites = {}
    for item in changes["new"]:
        suite = suites.setdefault(item["file"], {"errors": 0, "warnings": []})
        suite["warnings"].append({"line": item["line"], "column": item["column"], "message": item["message"], "severity": "warning"})
    import xml.etree.ElementTree as ET
    root = ET.Element("checkstyle", version="4.3")
    for filename, contents in sorted(suites.items()):
        file_element = ET.SubElement(root, "file", name=filename)
        for warning in contents["warnings"]:
            ET.SubElement(file_element, "error", **warning)
    args.checkstyle.write_bytes(ET.tostring(root, encoding="utf-8", xml_declaration=True))
    if os.environ.get("GITHUB_OUTPUT"):
        with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as output:
            output.write(f"new_warnings={counts['new']}\n")
    return 1 if changes["new"] else 0


if __name__ == "__main__":
    sys.exit(main())
