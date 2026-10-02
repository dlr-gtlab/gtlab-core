#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

"""Publish the Squish GUI test results of the internal GitLab CI on GitHub.

GitLab remains the single source of execution. This helper only *reads* the
GitLab REST API of the pipeline that produced the commit status which triggered
the ``publish-gui-tests.yml`` workflow, downloads the artifacts of that exact
job and converts them into GitHub Actions artifacts plus a job summary.

Subcommands
-----------
resolve   Locate the GitLab pipeline and the GUI test job of the reported status.
download  Download and extract the artifacts of that exact job.
summary   Build the GitHub job summary from the Squish JUnit report.

The commands communicate through ``$GITHUB_OUTPUT`` / ``$GITHUB_STEP_SUMMARY``
so that they can be used as separate workflow steps.
"""

import argparse
import json
import os
import re
import shutil
import sys
import tempfile
import urllib.error
import urllib.parse
import urllib.request
import xml.etree.ElementTree as ET
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import List, Optional, Tuple

# job name of the GUI tests in .gitlab-ci.yml
DEFAULT_JOB_NAME = "guiTests"

# entries of the job artifact archive, see the ".guiTestTemplate" artifacts
REPORT_DIRS = ("gui_tests_web", "gui_tests_junits")
REPORT_FILES = ("gui_tests_server_stdout.txt", "guitests_badge.svg")
REPORT_PREFIXES = ("gui_tests_stdout",)

# job states that allow a GUI test verdict; every other state (including a
# cancelled or skipped job) means "the GUI tests did not produce a result"
EXECUTED_STATES = ("success", "failed")


class GitLabError(RuntimeError):
    """A GitLab request failed."""


class GitLabNotFound(GitLabError):
    """A GitLab resource does not exist (HTTP 404)."""


@dataclass
class Target:
    """Location of the GitLab resources referenced by a commit status."""

    host: str
    project_path: str
    pipeline_id: Optional[int] = None
    job_id: Optional[int] = None

    @property
    def api_base(self) -> str:
        return self.host + "/api/v4"

    @property
    def encoded_project(self) -> str:
        return urllib.parse.quote(self.project_path, safe="")


def log_error(message: str) -> None:
    print(f"::error::{message}", file=sys.stderr)


def log_warning(message: str) -> None:
    print(f"::warning::{message}")


def log_notice(message: str) -> None:
    print(message)


def set_output(name: str, value) -> None:
    """Export a step output, or print it when running outside of Actions."""
    path = os.environ.get("GITHUB_OUTPUT")
    if not path:
        print(f"[output] {name}={value}")
        return
    with open(path, "a", encoding="utf-8") as stream:
        stream.write(f"{name}={value}\n")


def append_summary(markdown: str) -> None:
    """Append to the GitHub job summary, or print it for local runs."""
    path = os.environ.get("GITHUB_STEP_SUMMARY")
    if not path:
        print(markdown)
        return
    with open(path, "a", encoding="utf-8") as stream:
        stream.write(markdown)


class GitLabClient:
    """Minimal read-only GitLab REST API client."""

    def __init__(self, api_base: str, token: str):
        self.api_base = api_base.rstrip("/")
        self.token = token

    def _open(self, url: str) -> bytes:
        request = urllib.request.Request(
            url, headers={"PRIVATE-TOKEN": self.token, "Accept": "application/json"}
        )
        try:
            with urllib.request.urlopen(request, timeout=60) as response:
                return response.read()
        except urllib.error.HTTPError as error:
            # the token is only part of the request header, never of the url
            details = error.read().decode("utf-8", "replace").strip()[:300]
            message = f"GitLab request failed ({error.code} {url}): {details}"
            if error.code == 404:
                raise GitLabNotFound(message) from None
            raise GitLabError(message) from None
        except urllib.error.URLError as error:
            raise GitLabError(f"Cannot reach GitLab ({url}): {error.reason}") from None

    def get(self, path: str, **params) -> object:
        return json.loads(self._open(self._url(path, **params)).decode("utf-8"))

    def get_bytes(self, path: str, **params) -> bytes:
        return self._open(self._url(path, **params))

    def get_list(self, path: str, **params) -> List[dict]:
        """Fetch all pages of a paginated collection endpoint."""
        items: List[dict] = []
        page = 1
        while True:
            chunk = self.get(path, per_page=100, page=page, **params)
            if not isinstance(chunk, list):
                raise GitLabError(f"Unexpected GitLab response for {self._url(path)}")
            items.extend(chunk)
            if len(chunk) < 100 or page >= 10:
                return items
            page += 1

    def _url(self, path: str, **params) -> str:
        url = self.api_base + path
        if params:
            url += "?" + urllib.parse.urlencode(params)
        return url


def require_token() -> str:
    """Return the GitLab API token, or explain why the run cannot continue."""
    token = os.environ.get("GITLAB_TOKEN", "").strip()
    if not token:
        raise GitLabError(
            "The 'GITLAB_TOKEN' secret is not available to this workflow run. "
            "It is required to read the GUI test results from the GitLab API."
        )
    return token


def parse_target(url: str) -> Optional[Target]:
    """Split a GitLab pipeline/job web url into host, project path and ids."""
    parts = urllib.parse.urlsplit(url.strip())
    if not parts.netloc or not parts.path.strip("/"):
        return None

    host = f"{parts.scheme or 'https'}://{parts.netloc}"

    project, separator, tail = parts.path.partition("/-/")
    if not separator:
        tail = project

    # drop an accidental "/pipelines/123" style suffix from the project path
    project = re.sub(r"/(?:pipelines|jobs)/\d+.*$", "", project).strip("/")
    if not project:
        return None

    match = re.search(r"pipelines/(\d+)", tail)
    if match:
        return Target(host, project, pipeline_id=int(match.group(1)))

    match = re.search(r"jobs/(\d+)", tail)
    if match:
        return Target(host, project, job_id=int(match.group(1)))

    return Target(host, project)


def resolve_pipeline(client: GitLabClient, target: Target) -> int:
    """Determine the exact pipeline id of the status, without polling."""
    if target.pipeline_id:
        return target.pipeline_id

    if target.job_id:
        job = client.get(f"/projects/{target.encoded_project}/jobs/{target.job_id}")
        pipeline = job.get("pipeline") or {}
        if pipeline.get("id"):
            return int(pipeline["id"])

    # last resort: the pipeline of the commit that carries the status
    sha = os.environ.get("STATUS_SHA", "").strip()
    if sha:
        pipelines = client.get_list(
            f"/projects/{target.encoded_project}/repository/commits/{sha}/pipelines"
        )
        matching = [p for p in pipelines if p.get("sha") == sha]
        if matching:
            return int(max(matching, key=lambda p: p.get("id", 0))["id"])

    raise GitLabError(
        "Could not determine the GitLab pipeline of the reported commit status "
        f"(target url: {os.environ.get('STATUS_TARGET_URL', '<empty>')!r})."
    )


def find_gui_tests_job(client: GitLabClient, target: Target, pipeline_id: int,
                       job_name: str) -> Optional[dict]:
    """Return the GUI test job of the given pipeline, newest match wins."""
    jobs = client.get_list(f"/projects/{target.encoded_project}/pipelines/{pipeline_id}/jobs")
    candidates = [job for job in jobs if job.get("name") == job_name]
    if not candidates:
        return None
    return max(candidates, key=lambda job: job.get("id", 0))


def verdict(job: Optional[dict], job_name: str) -> Tuple[str, str]:
    """Map the GitLab job state to a GUI test verdict.

    A job that never ran (or that was cancelled) does not allow any conclusion
    about the GUI tests - reporting a failure in that case would blame the tests
    for an unrelated problem of the GitLab pipeline.
    """
    if job is None:
        return "not_executed", f"pipeline has no '{job_name}' job"

    status = job.get("status", "unknown")
    if status not in EXECUTED_STATES:
        return "not_executed", f"'{job_name}' job has no test verdict (status '{status}')"

    return ("passed" if status == "success" else "failed"), ""


def cmd_resolve(_args: argparse.Namespace) -> int:
    token = require_token()
    job_name = os.environ.get("GITLAB_GUI_TESTS_JOB", DEFAULT_JOB_NAME).strip()

    target = parse_target(os.environ.get("STATUS_TARGET_URL", ""))
    if target is None:
        raise GitLabError(
            "The commit status does not reference a usable GitLab url "
            f"({os.environ.get('STATUS_TARGET_URL', '<empty>')!r}). "
            "Expected a pipeline url such as "
            "'https://gitlab.dlr.de/<project>/-/pipelines/<id>'."
        )

    client = GitLabClient(target.api_base, token)
    pipeline_id = resolve_pipeline(client, target)
    pipeline = client.get(f"/projects/{target.encoded_project}/pipelines/{pipeline_id}")
    job = find_gui_tests_job(client, target, pipeline_id, job_name)

    result, reason = verdict(job, job_name)

    set_output("api_base", target.api_base)
    set_output("project", target.project_path)
    set_output("pipeline_url", pipeline.get("web_url", ""))
    set_output("pipeline_status", pipeline.get("status", ""))
    set_output("job_name", job_name)
    set_output("job_id", job.get("id", "") if job else "")
    set_output("job_url", job.get("web_url", "") if job else "")
    set_output("result", result)
    set_output("reason", reason)

    append_summary(summary_header(pipeline, pipeline_id, job, job_name, result, reason))

    log_notice(
        f"Resolved GitLab pipeline {pipeline_id} of {target.project_path} on {target.host}: "
        f"GUI tests {result}{' (' + reason + ')' if reason else ''}"
    )
    return 0


def summary_header(pipeline: dict, pipeline_id: int, job: Optional[dict], job_name: str,
                   result: str, reason: str) -> str:
    """First part of the job summary: where the results come from."""
    status_state = os.environ.get("STATUS_STATE", "").strip()
    pipeline_url = pipeline.get("web_url", "")
    lines = [
        "## Squish GUI tests\n\n",
        f"- Execution: GitLab pipeline "
        f"[{pipeline.get('id', pipeline_id)}]({pipeline_url})"
        f" (status `{pipeline.get('status', 'unknown')}`"
        + (f", commit status `{status_state}`" if status_state else "")
        + ")\n",
    ]

    if job:
        lines.append(
            f"- Test job: [`{job_name}`]({job.get('web_url', '')})"
            f" (status `{job.get('status', 'unknown')}`)\n"
        )
    else:
        lines.append(f"- Test job: `{job_name}` - not part of this pipeline\n")

    if result == "not_executed":
        lines.append(f"- Verdict: **not executed** - {reason}\n")

    return "".join(lines)


def artifact_root(directory: str) -> Optional[str]:
    """Find the folder that holds the GUI test results inside the archive.

    GitLab stores the artifact paths relative to the build directory, so the
    results normally live at the archive root. As a fallback the shallowest
    directory containing one of the known entries is used.
    """
    matches = []
    for current, dirs, files in os.walk(directory):
        if (any(d in dirs for d in REPORT_DIRS)
                or any(f in REPORT_FILES or f.startswith(REPORT_PREFIXES) for f in files)):
            matches.append(current)

    if not matches:
        return None
    return min(matches, key=lambda path: len(Path(path).parts))


def is_result_entry(name: str) -> bool:
    return (
        name in REPORT_DIRS
        or name in REPORT_FILES
        or name.startswith(REPORT_PREFIXES)
    )


def copy_results(root: Path, dest: Path) -> List[str]:
    """Copy the GUI test results of an extracted archive to the destination."""
    copied = []
    for entry in sorted(root.iterdir()):
        if not is_result_entry(entry.name):
            continue

        target = dest / entry.name
        if target.is_dir():
            shutil.rmtree(target)
        elif target.exists():
            target.unlink()

        if entry.is_dir():
            shutil.copytree(entry, target)
        else:
            shutil.copy2(entry, target)
        copied.append(entry.name)
    return copied


def cmd_download(args: argparse.Namespace) -> int:
    client = GitLabClient(args.api_base, require_token())
    project = urllib.parse.quote(args.project, safe="")

    if not args.job_id:
        log_notice("No GUI test job in this pipeline - nothing to download.")
        set_output("has_results", "false")
        return 0

    try:
        archive = client.get_bytes(f"/projects/{project}/jobs/{args.job_id}/artifacts")
    except GitLabNotFound:
        # the job was created but never published any artifacts
        log_notice("The GitLab GUI test job did not publish any artifacts.")
        set_output("has_results", "false")
        return 0

    dest = Path(args.dest)
    dest.mkdir(parents=True, exist_ok=True)

    with tempfile.TemporaryDirectory(prefix="guitests-") as tmp:
        package = Path(tmp) / "artifacts.zip"
        package.write_bytes(archive)

        try:
            with zipfile.ZipFile(package) as zip_file:
                zip_file.extractall(tmp)
        except zipfile.BadZipFile as error:
            raise GitLabError(
                f"The artifact archive of the GitLab GUI test job is not a valid zip "
                f"file ({error})."
            )

        root = artifact_root(tmp)
        if root is None:
            raise GitLabError(
                "The artifacts of the GitLab GUI test job do not contain any "
                "Squish test results."
            )

        copied = copy_results(Path(root), dest)

    if not copied:
        raise GitLabError(f"No GUI test result files found in the job artifacts ({root}).")

    set_output("has_results", "true")
    set_output("results_dir", str(dest))
    log_notice(f"Downloaded artifacts of GitLab job {args.job_id}: {', '.join(copied)}")
    return 0


@dataclass
class JUnit:
    """Aggregated JUnit information of a Squish run."""

    suites: List[dict]
    tests: int = 0
    failures: int = 0
    errors: int = 0
    skipped: int = 0
    failed_cases: List[str] = field(default_factory=list)
    problems: List[str] = field(default_factory=list)

    @property
    def passed(self) -> int:
        return max(self.tests - self.failures - self.errors - self.skipped, 0)

    @property
    def empty(self) -> bool:
        return not self.suites


def attribute_int(element: ET.Element, name: str, default: int) -> int:
    value = element.get(name)
    try:
        return int(value) if value is not None else default
    except ValueError:
        return default


def first_line(text: Optional[str], limit: int = 160) -> str:
    line = (text or "").strip().splitlines()[0] if (text or "").strip() else ""
    return line if len(line) <= limit else line[:limit] + "..."


def parse_junit(directory: str) -> JUnit:
    """Read all JUnit reports of the Squish run."""
    base = Path(directory)
    files = sorted(base.glob("gui_tests_junits/*.xml"))
    if not files:
        files = sorted(base.rglob("junit*.xml"))

    junit = JUnit(suites=[])
    for path in files:
        try:
            root = ET.parse(path).getroot()
        except (ET.ParseError, OSError) as error:
            junit.problems.append(f"Cannot parse `{path.name}`: {error}")
            continue

        suites = [root] if root.tag == "testsuite" else list(root.iter("testsuite"))
        for suite in suites:
            cases = suite.findall("testcase")
            failures = sum(1 for case in cases if case.find("failure") is not None)
            errors = sum(1 for case in cases if case.find("error") is not None)
            skipped = sum(1 for case in cases if case.find("skipped") is not None)

            # prefer the reported suite attributes, but never hide a failing case
            entry = {
                "name": suite.get("name") or path.name,
                "tests": max(len(cases), attribute_int(suite, "tests", len(cases))),
                "failures": max(failures, attribute_int(suite, "failures", failures)),
                "errors": max(errors, attribute_int(suite, "errors", errors)),
                "skipped": max(skipped, attribute_int(suite, "skipped", skipped)),
            }
            junit.suites.append(entry)
            junit.tests += entry["tests"]
            junit.failures += entry["failures"]
            junit.errors += entry["errors"]
            junit.skipped += entry["skipped"]

            for case in cases:
                for tag in ("failure", "error"):
                    node = case.find(tag)
                    if node is None:
                        continue
                    name = case.get("name") or "<unknown>"
                    classname = case.get("classname") or entry["name"]
                    message = first_line(node.get("message") or node.text)
                    junit.failed_cases.append(
                        f"`{classname}::{name}` - {message or tag}"
                    )

    return junit


def headline(result: str, junit: JUnit, reason: str) -> str:
    if result == "not_executed":
        return (":white_circle: **GUI tests were not executed** - "
                f"{reason or 'no test verdict available'}")
    if junit.empty:
        if result == "failed":
            return (":cross_mark: **GUI tests failed** - the GitLab job did not publish "
                    "a JUnit report")
        return ":white_check_mark: **GUI tests passed** - no JUnit report was found"

    bad = junit.failures + junit.errors
    if bad:
        return (f":cross_mark: **{bad} of {junit.tests} GUI test(s) failed** "
                f"({junit.failures} failure(s), {junit.errors} error(s))")
    return f":white_check_mark: **all {junit.tests} GUI test(s) passed**"


def cmd_summary(args: argparse.Namespace) -> int:
    results = args.dir if args.dir and os.path.isdir(args.dir) else ""
    junit = parse_junit(results) if results else JUnit(suites=[])

    set_output("tests", junit.tests)
    set_output("failed", junit.failures + junit.errors)
    set_output("passed", junit.passed)
    set_output("skipped", junit.skipped)

    result = args.result
    # a green job with failing reported tests must not be published as success
    if result == "passed" and junit.failures + junit.errors > 0:
        result = "failed"
        set_output("result", result)

    lines = [f"\n{headline(result, junit, args.reason)}\n"]

    if junit.suites:
        lines.append("\n| Test suite | Tests | Passed | Failed | Skipped |\n"
                     "| --- | ---: | ---: | ---: | ---: |\n")
        for suite in junit.suites:
            bad = suite["failures"] + suite["errors"]
            passed = max(suite["tests"] - bad - suite["skipped"], 0)
            lines.append(
                f"| {suite['name']} | {suite['tests']} | {passed} | {bad} | {suite['skipped']} |\n"
            )

    if junit.failed_cases:
        lines.append(
            "\n<details>\n<summary>Failed tests</summary>\n\n"
            + "".join(f"- {case}\n" for case in junit.failed_cases[:50])
            + (f"- ... and {len(junit.failed_cases) - 50} more\n"
               if len(junit.failed_cases) > 50 else "")
            + "\n</details>\n"
        )

    for problem in junit.problems:
        log_warning(problem)
        lines.append(f"\n> [!WARNING]\n> {problem}\n")

    if junit.empty:
        lines.append("\nNo JUnit report (`gui_tests_junits/*.xml`) was found "
                     "in the artifacts of the GitLab GUI test job.\n")

    lines.append("\n### Reports\n\n")
    if args.has_results == "true":
        lines.append(
            "- Squish HTML report: `gui_tests_web` artifact of this workflow run\n"
            "- JUnit reports: `gui_tests_junits` artifact\n"
            "- Console logs: `gui_tests_stdout` and `gui_tests_server_stdout` artifacts\n"
            "- Status badge: `guitests_badge` artifact (`guitests_badge.svg`)\n"
        )
    else:
        lines.append("- The GitLab GUI test job did not publish any test report.\n")

    if args.pipeline_url:
        lines.append(f"- GitLab pipeline: [{args.pipeline_url}]({args.pipeline_url})\n")
    if args.job_url:
        lines.append(f"- GitLab GUI test job: [{args.job_url}]({args.job_url})\n")

    append_summary("".join(lines))

    log_notice(
        f"GUI tests: {junit.tests} test(s), {junit.failures} failure(s), "
        f"{junit.errors} error(s), {junit.skipped} skipped -> {result}"
    )
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    commands = parser.add_subparsers(dest="command", required=True)

    commands.add_parser("resolve", help="locate pipeline and GUI test job")

    download = commands.add_parser("download", help="download the GUI test job artifacts")
    download.add_argument("--api-base", required=True)
    download.add_argument("--project", required=True)
    download.add_argument("--job-id", default="")
    download.add_argument("--dest", required=True)

    summary = commands.add_parser("summary", help="build the job summary from JUnit")
    summary.add_argument("--dir", default="")
    summary.add_argument("--result", default="not_executed")
    summary.add_argument("--reason", default="")
    summary.add_argument("--job-url", default="")
    summary.add_argument("--pipeline-url", default="")
    summary.add_argument("--has-results", default="false")

    args = parser.parse_args()
    handlers = {"resolve": cmd_resolve, "download": cmd_download, "summary": cmd_summary}

    try:
        return handlers[args.command](args)
    except GitLabError as error:
        log_error(str(error))
        return 1


if __name__ == "__main__":
    sys.exit(main())
