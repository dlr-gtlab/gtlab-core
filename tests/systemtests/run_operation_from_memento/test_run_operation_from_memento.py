# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

"""Process tests for GTlabConsole executable-operation invocations."""

import base64
import json
import os
import shutil
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import List, Optional

import pytest


TEST_DATA_DIR = Path(__file__).parent
OPERATION_MEMENTO = TEST_DATA_DIR / "operation.xml"
PROJECT_REQUIRED_OPERATION_MEMENTO = (
    TEST_DATA_DIR / "project_required_operation.xml"
)
FAILURE_OPERATION_MEMENTO = TEST_DATA_DIR / "failure_operation.xml"
CANCELLATION_OPERATION_MEMENTO = TEST_DATA_DIR / "cancellation_operation.xml"
EXCEPTION_OPERATION_MEMENTO = TEST_DATA_DIR / "exception_operation.xml"
DATA_MEMENTO = TEST_DATA_DIR / "data.xml"
PROJECT_MEMENTO = (
    TEST_DATA_DIR.parent / "run_task_from_memento" / "project.xml"
)
PROTOCOL_PREFIX = "@gtlab-operation-v1 "


@pytest.fixture
def console_path() -> Path:
    console = os.environ.get("GTLAB_CONSOLE")
    assert console, "GTLAB_CONSOLE must point to GTlabConsole"

    path = Path(console)
    assert path.is_file(), f"GTlabConsole does not exist: {path}"
    return path


def _command(
    console: Path,
    operation: Path,
    *,
    data: Optional[Path] = None,
    project: Optional[Path] = None,
    events: Optional[Path] = None,
    working_directory: Optional[Path] = None,
) -> List[str]:
    args = [
        str(console),
        "run_operation_from_memento",
        "--operation-memento",
        str(operation),
    ]
    if data:
        args.extend(["--data-memento", str(data)])
    if project:
        args.extend(["--project-memento", str(project)])
    if events:
        args.extend(["--events", str(events)])
    if working_directory:
        args.extend(["--working-directory", str(working_directory)])
    return args


def _invoke(
    console: Path,
    operation: Path,
    *,
    data: Optional[Path] = None,
    project: Optional[Path] = None,
    events: Optional[Path] = None,
    working_directory: Optional[Path] = None,
) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        _command(
            console,
            operation,
            data=data,
            project=project,
            events=events,
            working_directory=working_directory,
        ),
        text=True,
        capture_output=True,
        check=False,
    )


def _terminal_record(stdout: str) -> dict:
    records = [
        json.loads(line[len(PROTOCOL_PREFIX) :])
        for line in stdout.splitlines()
        if line.startswith(PROTOCOL_PREFIX)
    ]
    assert len(records) == 1, f"Expected one terminal record, got {records}"
    return records[0]


def _assert_failure(
    result: subprocess.CompletedProcess[str], expected_code: str
) -> dict:
    record = _terminal_record(result.stdout)
    assert result.returncode != 0
    assert record["kind"] == "failure"
    assert record["errorCode"] == expected_code
    assert record["executionId"]
    return record


def test_command_help_lists_the_contract_without_a_result_file_option(
    console_path: Path,
) -> None:
    result = subprocess.run(
        [console_path, "run_operation_from_memento", "--help"],
        text=True,
        capture_output=True,
        check=False,
    )

    assert result.returncode == 0, result.stderr
    for option in (
        "--operation-memento",
        "--data-memento",
        "--project-memento",
        "--events",
        "--working-directory",
    ):
        assert option in result.stdout
    assert "--result" not in result.stdout
    assert PROTOCOL_PREFIX not in result.stdout


def test_runs_registered_operation_with_detached_data_and_separate_events(
    console_path: Path, tmp_path: Path
) -> None:
    work = tmp_path / "operation working directory"
    work.mkdir()
    event_file = tmp_path / "operation events.ndjson"

    result = _invoke(
        console_path,
        OPERATION_MEMENTO,
        data=DATA_MEMENTO,
        events=event_file,
        working_directory=work,
    )

    assert result.returncode == 0, (
        f"GTlabConsole failed with exit code {result.returncode}\n"
        f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}"
    )
    terminal = _terminal_record(result.stdout)
    assert terminal["kind"] == "result"
    assert terminal["resultEncoding"] == "memento-xml-base64"
    assert terminal["executionId"]
    result_xml = base64.b64decode(terminal["result"]).decode("utf-8")
    assert ET.fromstring(result_xml).get("class") == "GtObjectGroup"
    assert "Test executable operation emitted a normal GTlab log" in (
        result.stdout + result.stderr
    )

    events = [json.loads(line) for line in event_file.read_text().splitlines()]
    assert [event["sequence"] for event in events] == [0, 1]
    assert [event["eventType"] for event in events] == [
        "test.operation.started",
        "test.operation.completed",
    ]
    assert all(event["executionId"] == terminal["executionId"] for event in events)
    assert all(event["payload"]["dataProvided"] for event in events)
    assert all(event["payload"]["executionContextActive"] for event in events)
    assert all(not event["payload"]["projectVisible"] for event in events)
    assert "normal GTlab log" not in event_file.read_text()


def test_project_required_operation_uses_the_restored_project(
    console_path: Path, tmp_path: Path
) -> None:
    event_file = tmp_path / "project events.ndjson"

    result = _invoke(
        console_path,
        PROJECT_REQUIRED_OPERATION_MEMENTO,
        project=PROJECT_MEMENTO,
        events=event_file,
        working_directory=tmp_path,
    )

    assert result.returncode == 0, result.stderr
    terminal = _terminal_record(result.stdout)
    assert terminal["kind"] == "result"
    events = [json.loads(line) for line in event_file.read_text().splitlines()]
    assert all(event["payload"]["executionContextActive"] for event in events)
    assert all(event["payload"]["projectVisible"] for event in events)


def test_project_independent_operation_does_not_see_optional_project(
    console_path: Path, tmp_path: Path
) -> None:
    event_file = tmp_path / "independent events.ndjson"

    result = _invoke(
        console_path,
        OPERATION_MEMENTO,
        project=PROJECT_MEMENTO,
        events=event_file,
        working_directory=tmp_path,
    )

    assert result.returncode == 0, result.stderr
    events = [json.loads(line) for line in event_file.read_text().splitlines()]
    assert all(event["payload"]["executionContextActive"] for event in events)
    assert all(not event["payload"]["projectVisible"] for event in events)


def test_required_project_missing_is_a_structured_execution_failure(
    console_path: Path, tmp_path: Path
) -> None:
    result = _invoke(console_path, PROJECT_REQUIRED_OPERATION_MEMENTO)

    _assert_failure(result, "project_required")
    assert result.returncode == 5


def test_operation_failure_preserves_operation_status_and_code(
    console_path: Path,
) -> None:
    result = _invoke(console_path, FAILURE_OPERATION_MEMENTO)

    record = _assert_failure(result, "test_operation_failed")
    assert result.returncode == 5
    assert record["details"] == {
        "status": "failed",
        "operationCode": "test_operation_failed",
    }


def test_operation_cancellation_is_reported_as_a_failure(
    console_path: Path,
) -> None:
    result = _invoke(console_path, CANCELLATION_OPERATION_MEMENTO)

    record = _assert_failure(result, "test_operation_cancelled")
    assert result.returncode == 5
    assert record["details"] == {
        "status": "cancelled",
        "operationCode": "test_operation_cancelled",
    }


def test_operation_exception_is_reported_as_an_execution_failure(
    console_path: Path,
) -> None:
    result = _invoke(console_path, EXCEPTION_OPERATION_MEMENTO)

    record = _assert_failure(result, "unhandled_exception")
    assert result.returncode == 5
    assert record["message"] == "Test operation exception."


@pytest.mark.parametrize(
    ("memento", "error_code"),
    [
        (
            '<object class="MissingOperation" uuid="{85eae9dd-e9d3-4f5d-9af2-83f4502e6511}"/>',
            "invalid_operation_memento",
        ),
        (
            '<object class="GtObjectGroup" uuid="{85eae9dd-e9d3-4f5d-9af2-83f4502e6511}"><objectlist/></object>',
            "not_an_executable_operation",
        ),
    ],
)
def test_rejects_unknown_and_non_operation_mementos(
    console_path: Path, tmp_path: Path, memento: str, error_code: str
) -> None:
    operation = tmp_path / "invalid operation.xml"
    operation.write_text(memento, encoding="utf-8")

    result = _invoke(console_path, operation)

    _assert_failure(result, error_code)
    assert result.returncode == 4


def test_argument_and_event_output_failures_have_terminal_records(
    console_path: Path, tmp_path: Path
) -> None:
    missing_operation = subprocess.run(
        [console_path, "run_operation_from_memento"],
        text=True,
        capture_output=True,
        check=False,
    )
    _assert_failure(missing_operation, "invalid_arguments")
    assert missing_operation.returncode == 2

    result = _invoke(
        console_path,
        OPERATION_MEMENTO,
        events=tmp_path / "missing directory" / "events.ndjson",
    )
    _assert_failure(result, "event_output_error")
    assert result.returncode == 6


@pytest.mark.skipif(not Path("/dev/full").exists(), reason="requires /dev/full")
def test_terminal_result_write_failure_returns_protocol_exit_code(
    console_path: Path,
) -> None:
    with Path("/dev/full").open("w", encoding="utf-8") as output:
        result = subprocess.run(
            _command(console_path, OPERATION_MEMENTO),
            stdout=output,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )

    assert result.returncode == 7
    assert "Cannot write the terminal operation result record" in result.stderr


def test_bad_data_memento_is_rejected_before_execution(
    console_path: Path, tmp_path: Path
) -> None:
    data = tmp_path / "bad data.xml"
    data.write_text("not xml", encoding="utf-8")

    result = _invoke(console_path, OPERATION_MEMENTO, data=data)

    _assert_failure(result, "invalid_data_memento")
    assert result.returncode == 4


@pytest.mark.parametrize(
    "extra_args",
    [
        ["--unknown-option"],
        ["--data-memento"],
        ["unexpected-positional-argument"],
        *[[f"--{option}="] for option in (
            "operation-memento", "data-memento", "project-memento",
            "events", "working-directory",
        )],
    ],
)
def test_invalid_arguments_are_rejected_before_execution(
    console_path: Path, extra_args: List[str]
) -> None:
    result = subprocess.run(
        _command(console_path, OPERATION_MEMENTO) + extra_args,
        text=True, capture_output=True, check=False,
    )

    _assert_failure(result, "invalid_arguments")
    assert result.returncode == 2
    assert "Test executable operation emitted" not in (
        result.stdout + result.stderr
    )


@pytest.mark.parametrize("input_kind", ["operation", "data", "project"])
def test_events_cannot_overwrite_any_input_memento(
    console_path: Path, tmp_path: Path, input_kind: str
) -> None:
    inputs = {}
    for name, source in (
        ("operation", OPERATION_MEMENTO),
        ("data", DATA_MEMENTO),
        ("project", PROJECT_MEMENTO),
    ):
        inputs[name] = tmp_path / f"{name}.xml"
        shutil.copyfile(source, inputs[name])
    original = {name: path.read_bytes() for name, path in inputs.items()}

    result = _invoke(
        console_path, inputs["operation"], data=inputs["data"],
        project=inputs["project"], events=inputs[input_kind],
    )

    _assert_failure(result, "invalid_arguments")
    assert result.returncode == 2
    assert {name: path.read_bytes() for name, path in inputs.items()} == original


@pytest.mark.parametrize("input_kind", ["operation", "data", "project"])
def test_missing_input_mementos_are_rejected_before_event_output_is_created(
    console_path: Path, tmp_path: Path, input_kind: str
) -> None:
    missing = tmp_path / "missing.xml"
    event_file = tmp_path / "events.ndjson"
    kwargs = {input_kind: missing} if input_kind != "operation" else {}

    result = _invoke(
        console_path, missing if input_kind == "operation" else OPERATION_MEMENTO,
        events=event_file, **kwargs,
    )

    _assert_failure(result, "input_file_error")
    assert result.returncode == 3
    assert not event_file.exists()


@pytest.mark.parametrize("directory_is_file", [False, True])
def test_invalid_working_directory_is_rejected(
    console_path: Path, tmp_path: Path, directory_is_file: bool
) -> None:
    working_directory = tmp_path / "working directory"
    if directory_is_file:
        working_directory.write_text("not a directory", encoding="utf-8")

    result = _invoke(
        console_path, OPERATION_MEMENTO, working_directory=working_directory,
    )

    _assert_failure(result, "input_file_error")
    assert result.returncode == 3


@pytest.mark.parametrize("input_kind", ["operation", "data", "project"])
@pytest.mark.parametrize("fixture_name", ["malformed.xml", "unknown_object.xml"])
def test_invalid_mementos_are_rejected_for_each_input_role(
    console_path: Path, tmp_path: Path, input_kind: str, fixture_name: str
) -> None:
    invalid = TEST_DATA_DIR / fixture_name
    event_file = tmp_path / "events.ndjson"
    kwargs = {input_kind: invalid} if input_kind != "operation" else {}

    result = _invoke(
        console_path, invalid if input_kind == "operation" else OPERATION_MEMENTO,
        events=event_file, **kwargs,
    )

    _assert_failure(result, f"invalid_{input_kind}_memento")
    assert result.returncode == 4
    assert not event_file.exists()


@pytest.mark.parametrize(
    "project", [OPERATION_MEMENTO, TEST_DATA_DIR / "invalid_project_root.xml"]
)
def test_project_requires_an_object_group_containing_only_packages(
    console_path: Path, project: Path
) -> None:
    result = _invoke(console_path, OPERATION_MEMENTO, project=project)

    _assert_failure(result, "invalid_project_memento")
    assert result.returncode == 4


def test_relative_paths_are_resolved_before_changing_working_directory(
    console_path: Path, tmp_path: Path
) -> None:
    shutil.copyfile(OPERATION_MEMENTO, tmp_path / "operation.xml")
    shutil.copyfile(DATA_MEMENTO, tmp_path / "data.xml")
    work = tmp_path / "work"
    work.mkdir()

    result = subprocess.run(
        _command(
            console_path, Path("operation.xml"), data=Path("data.xml"),
            events=Path("events.ndjson"), working_directory=Path("work"),
        ),
        cwd=tmp_path, text=True, capture_output=True, check=False,
    )

    assert result.returncode == 0, result.stderr
    terminal = _terminal_record(result.stdout)
    events = [
        json.loads(line)
        for line in (tmp_path / "events.ndjson").read_text().splitlines()
    ]
    assert len(events) == 2
    assert all(
        event["executionId"] == terminal["executionId"] for event in events
    )
    assert all(event["payload"]["dataProvided"] for event in events)
    assert all(
        Path(event["payload"]["workingDirectory"]) == work for event in events
    )
    assert not (work / "events.ndjson").exists()


def test_project_directory_is_the_default_working_directory(
    console_path: Path, tmp_path: Path
) -> None:
    project_dir = tmp_path / "project directory"
    project_dir.mkdir()
    project = project_dir / "project.xml"
    shutil.copyfile(PROJECT_MEMENTO, project)
    event_file = tmp_path / "events.ndjson"

    result = _invoke(
        console_path, PROJECT_REQUIRED_OPERATION_MEMENTO,
        project=project, events=event_file,
    )

    assert result.returncode == 0, result.stderr
    assert _terminal_record(result.stdout)["kind"] == "result"
    events = [json.loads(line) for line in event_file.read_text().splitlines()]
    assert len(events) == 2
    assert all(event["payload"]["projectVisible"] for event in events)
    assert all(
        Path(event["payload"]["workingDirectory"]) == project_dir
        for event in events
    )


@pytest.mark.skipif(
    os.name != "posix" or os.geteuid() == 0,
    reason="requires POSIX permissions enforced for a non-root user",
)
def test_existing_but_unreadable_operation_memento_is_rejected(
    console_path: Path, tmp_path: Path
) -> None:
    operation = tmp_path / "unreadable.xml"
    shutil.copyfile(OPERATION_MEMENTO, operation)
    operation.chmod(0)
    try:
        result = _invoke(console_path, operation)
    finally:
        operation.chmod(0o600)

    record = _assert_failure(result, "invalid_operation_memento")
    assert result.returncode == 4
    assert "Cannot read operation Memento" in record["message"]


@pytest.mark.skipif(
    os.name != "posix" or os.geteuid() == 0,
    reason="requires POSIX permissions enforced for a non-root user",
)
def test_existing_but_inaccessible_working_directory_is_rejected(
    console_path: Path, tmp_path: Path
) -> None:
    work = tmp_path / "inaccessible"
    work.mkdir()
    work.chmod(0)
    try:
        result = _invoke(console_path, OPERATION_MEMENTO, working_directory=work)
    finally:
        work.chmod(0o700)

    _assert_failure(result, "working_directory_error")
    assert result.returncode == 3


@pytest.mark.skipif(not Path("/dev/full").exists(), reason="requires /dev/full")
def test_event_write_failure_is_reported_instead_of_a_successful_result(
    console_path: Path,
) -> None:
    result = _invoke(console_path, OPERATION_MEMENTO, events=Path("/dev/full"))

    _assert_failure(result, "event_output_error")
    assert result.returncode == 6


@pytest.mark.skipif(not Path("/dev/full").exists(), reason="requires /dev/full")
@pytest.mark.parametrize("operation", [None, FAILURE_OPERATION_MEMENTO])
def test_terminal_failure_write_errors_return_the_protocol_exit_code(
    console_path: Path, operation: Optional[Path]
) -> None:
    command = _command(console_path, operation) if operation else [
        str(console_path), "run_operation_from_memento",
    ]
    with Path("/dev/full").open("w", encoding="utf-8") as output:
        result = subprocess.run(
            command, stdout=output, stderr=subprocess.PIPE,
            text=True, check=False,
        )

    assert result.returncode == 7
    assert "Cannot write the terminal operation failure record" in result.stderr
