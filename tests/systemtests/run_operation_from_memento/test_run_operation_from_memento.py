import base64
import json
import os
import subprocess
from pathlib import Path
import xml.etree.ElementTree as ET

PREFIX = "@gtlab-operation-v1 "

def invoke(console: Path, *args: str) -> subprocess.CompletedProcess:
    return subprocess.run([str(console), "run_operation_from_memento", *args],
                          text=True, capture_output=True, check=False,
                          env={**os.environ, "QT_QPA_PLATFORM": "offscreen"})

def terminal_records(stdout: str) -> list[dict]:
    return [json.loads(line[len(PREFIX):]) for line in stdout.splitlines()
            if line.startswith(PREFIX) and json.loads(line[len(PREFIX):]).get("kind") in ("result", "failure")]

def test_help_lists_contract_options():
    console = Path(os.environ["GTLAB_CONSOLE"])
    result = invoke(console, "--help")
    assert result.returncode == 0
    for option in ("--operation-memento", "--data-memento", "--project-memento", "--events", "--working-directory"):
        assert option in result.stdout

def test_executes_operation_with_data_and_writes_events(tmp_path: Path):
    console = Path(os.environ["GTLAB_CONSOLE"])
    operation = tmp_path / "operation.xml"
    data = tmp_path / "data.xml"
    events = tmp_path / "events.ndjson"
    operation.write_text('<object class="TestOperation" name="operation" uuid="-"/>')
    data.write_text('<object class="GtObjectGroup" name="detached-input" uuid="-"/>')
    result = invoke(console, "--operation-memento", str(operation),
                    "--data-memento", str(data), "--events", str(events),
                    "--working-directory", str(tmp_path))
    assert result.returncode == 0, result.stderr + result.stdout
    records = terminal_records(result.stdout)
    assert len(records) == 1 and records[0]["kind"] == "result"
    serialized = base64.b64decode(records[0]["result"])
    assert ET.fromstring(serialized).attrib["name"] == "detached-input"
    event_records = [json.loads(line) for line in events.read_text().splitlines()]
    assert [event["sequence"] for event in event_records] == [0, 1]
    assert [event["eventType"] for event in event_records] == ["test.started", "test.finished"]
    assert all(event["executionId"] == records[0]["executionId"] for event in event_records)
    assert all("DEBUG" not in line and "INFO" not in line for line in events.read_text().splitlines())

def test_non_operation_and_restore_errors_emit_one_terminal_failure(tmp_path: Path):
    console = Path(os.environ["GTLAB_CONSOLE"])
    not_operation = tmp_path / "not-operation.xml"
    not_operation.write_text('<object class="GtObjectGroup" name="group" uuid="-"/>')
    result = invoke(console, "--operation-memento", str(not_operation))
    assert result.returncode != 0
    records = terminal_records(result.stdout)
    assert len(records) == 1 and records[0]["kind"] == "failure"
    malformed = tmp_path / "malformed.xml"
    malformed.write_text("<invalid")
    result = invoke(console, "--operation-memento", str(malformed))
    assert result.returncode != 0
    records = terminal_records(result.stdout)
    assert len(records) == 1 and records[0]["kind"] == "failure"


def test_project_required_operation_and_missing_project(tmp_path: Path):
    console = Path(os.environ["GTLAB_CONSOLE"])
    operation = tmp_path / "project-operation.xml"
    operation.write_text('<object class="TestOperation" name="requires-project" uuid="-"/>')
    missing = invoke(console, "--operation-memento", str(operation))
    assert missing.returncode != 0
    records = terminal_records(missing.stdout)
    assert len(records) == 1 and records[0]["kind"] == "failure"

    project = tmp_path / "project.xml"
    project.write_bytes((Path(__file__).parents[1] / "run_task_from_memento" / "project.xml").read_bytes())
    result = invoke(console, "--operation-memento", str(operation),
                    "--project-memento", str(project), "--working-directory", str(tmp_path))
    assert result.returncode == 0, result.stderr + result.stdout
    records = terminal_records(result.stdout)
    assert len(records) == 1 and records[0]["kind"] == "result"
    serialized = base64.b64decode(records[0]["result"])
    assert ET.fromstring(serialized).attrib["name"] == "project-context-present"

def test_execution_failure_is_one_terminal_failure(tmp_path: Path):
    console = Path(os.environ["GTLAB_CONSOLE"])
    operation = tmp_path / "failing-operation.xml"
    operation.write_text('<object class="TestOperation" name="fails" uuid="-"/>')
    result = invoke(console, "--operation-memento", str(operation))
    assert result.returncode != 0
    records = terminal_records(result.stdout)
    assert len(records) == 1 and records[0]["kind"] == "failure"
