import base64
import json
import os
import subprocess
from pathlib import Path
import xml.etree.ElementTree as ET


TEST_DIR = Path(__file__).parent
FIXTURES = TEST_DIR / "fixtures"
PROJECT_MEMENTO = TEST_DIR.parent / "run_task_from_memento" / "project.xml"
PREFIX = "@gtlab-operation-v1 "


def invoke(console: Path, *args: str) -> subprocess.CompletedProcess:
    return subprocess.run(
        [str(console), "run_operation_from_memento", *args],
        text=True,
        capture_output=True,
        check=False,
        env={**os.environ, "QT_QPA_PLATFORM": "offscreen"},
    )


def terminal_records(stdout: str) -> list[dict]:
    records = []
    for line in stdout.splitlines():
        if not line.startswith(PREFIX):
            continue
        record = json.loads(line[len(PREFIX) :])
        if record.get("kind") in ("result", "failure"):
            records.append(record)
    return records


def test_help_lists_contract_options():
    result = invoke(Path(os.environ["GTLAB_CONSOLE"]), "--help")

    assert result.returncode == 0
    for option in (
        "--operation-memento",
        "--data-memento",
        "--project-memento",
        "--events",
        "--working-directory",
    ):
        assert option in result.stdout


def test_executes_operation_with_data_and_writes_events(tmp_path: Path):
    events = tmp_path / "events.ndjson"
    result = invoke(
        Path(os.environ["GTLAB_CONSOLE"]),
        "--operation-memento",
        str(FIXTURES / "operation.xml"),
        "--data-memento",
        str(FIXTURES / "data.xml"),
        "--events",
        str(events),
        "--working-directory",
        str(tmp_path),
    )

    assert result.returncode == 0, result.stderr + result.stdout
    records = terminal_records(result.stdout)
    assert len(records) == 1
    assert records[0]["kind"] == "result"
    result_memento = base64.b64decode(records[0]["result"])
    assert ET.fromstring(result_memento).attrib["name"] == "detached-input"

    event_lines = events.read_text().splitlines()
    event_records = [json.loads(line) for line in event_lines]
    assert [event["sequence"] for event in event_records] == [0, 1]
    assert [event["eventType"] for event in event_records] == [
        "test.started",
        "test.finished",
    ]
    assert all(
        event["executionId"] == records[0]["executionId"]
        for event in event_records
    )
    assert all("DEBUG" not in line and "INFO" not in line for line in event_lines)


def test_non_operation_and_invalid_memento_emit_one_failure():
    console = Path(os.environ["GTLAB_CONSOLE"])
    for fixture in ("not_operation.xml", "malformed.xml"):
        result = invoke(
            console,
            "--operation-memento",
            str(FIXTURES / fixture),
        )

        assert result.returncode != 0
        records = terminal_records(result.stdout)
        assert len(records) == 1
        assert records[0]["kind"] == "failure"


def test_project_required_operation_fails_without_project():
    result = invoke(
        Path(os.environ["GTLAB_CONSOLE"]),
        "--operation-memento",
        str(FIXTURES / "operation_requires_project.xml"),
    )

    assert result.returncode != 0
    records = terminal_records(result.stdout)
    assert len(records) == 1
    assert records[0]["kind"] == "failure"


def test_project_required_operation_runs_with_project(tmp_path: Path):
    result = invoke(
        Path(os.environ["GTLAB_CONSOLE"]),
        "--operation-memento",
        str(FIXTURES / "operation_requires_project.xml"),
        "--project-memento",
        str(PROJECT_MEMENTO),
        "--working-directory",
        str(tmp_path),
    )

    assert result.returncode == 0, result.stderr + result.stdout
    records = terminal_records(result.stdout)
    assert len(records) == 1
    assert records[0]["kind"] == "result"
    result_memento = base64.b64decode(records[0]["result"])
    assert ET.fromstring(result_memento).attrib["name"] == "project-context-present"


def test_operation_failure_emits_one_terminal_failure():
    result = invoke(
        Path(os.environ["GTLAB_CONSOLE"]),
        "--operation-memento",
        str(FIXTURES / "operation_fails.xml"),
    )

    assert result.returncode != 0
    records = terminal_records(result.stdout)
    assert len(records) == 1
    assert records[0]["kind"] == "failure"
