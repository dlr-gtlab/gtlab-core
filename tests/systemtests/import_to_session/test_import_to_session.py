# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

"""System tests for importing a project directory into a session."""

import os
import subprocess
import uuid
from pathlib import Path
from typing import Dict, Iterator, List, Tuple, Union

import pytest


TEST_PROJECT_DIR = Path(__file__).parent / "TestProject"


@pytest.fixture
def console_path() -> Path:
    console = os.environ.get("GTLAB_CONSOLE")
    assert console, "GTLAB_CONSOLE must point to GTlabConsole"

    path = Path(console)
    assert path.is_file(), f"GTlabConsole does not exist: {path}"
    return path


@pytest.fixture
def console_environment(tmp_path: Path) -> Dict[str, str]:
    environment = os.environ.copy()
    environment["QT_QPA_PLATFORM"] = "offscreen"
    environment["HOME"] = str(tmp_path)

    if os.name == "nt":
        environment["APPDATA"] = str(tmp_path / "config")
        environment["LOCALAPPDATA"] = str(tmp_path / "local")
        environment["USERPROFILE"] = str(tmp_path)
    else:
        environment["XDG_CONFIG_HOME"] = str(tmp_path / "config")
        environment["XDG_DATA_HOME"] = str(tmp_path / "data")
        environment["XDG_CACHE_HOME"] = str(tmp_path / "cache")

    return environment


def _run_console_command(
    console_path: Path,
    environment: Dict[str, str],
    working_directory: Path,
    *arguments: Union[str, Path],
) -> subprocess.CompletedProcess:
    command = [str(console_path), *map(str, arguments)]
    return subprocess.run(
        command,
        cwd=working_directory,
        env=environment,
        text=True,
        capture_output=True,
        check=False,
    )


def _format_console_result(
    command: List[str], result: subprocess.CompletedProcess
) -> str:
    return (
        f"GTlabConsole command: {command[1:]}\n"
        f"exit code: {result.returncode}\n"
        f"stdout:\n{result.stdout}\n"
        f"stderr:\n{result.stderr}"
    )


def _run_console(
    console_path: Path,
    environment: Dict[str, str],
    working_directory: Path,
    *arguments: Union[str, Path],
) -> subprocess.CompletedProcess:
    command = [str(console_path), *map(str, arguments)]
    result = _run_console_command(
        console_path, environment, working_directory, *arguments
    )

    assert result.returncode == 0, _format_console_result(command, result)
    return result


def _run_console_expect_failure(
    console_path: Path,
    environment: Dict[str, str],
    working_directory: Path,
    *arguments: Union[str, Path],
) -> subprocess.CompletedProcess:
    command = [str(console_path), *map(str, arguments)]
    result = _run_console_command(
        console_path, environment, working_directory, *arguments
    )

    assert result.returncode != 0, (
        "GTlabConsole unexpectedly succeeded:\n"
        + _format_console_result(command, result)
    )
    return result


@pytest.fixture
def session_id(
    console_path: Path,
    console_environment: Dict[str, str],
    tmp_path: Path,
    request: pytest.FixtureRequest,
) -> Iterator[str]:
    """Create and remove a unique session for each test."""
    identifier = f"import-to-session-{uuid.uuid4().hex}"
    _run_console(
        console_path,
        console_environment,
        tmp_path,
        "--session=default",
        "create_session",
        identifier,
    )

    yield identifier

    cleanup_result = _run_console_command(
        console_path,
        console_environment,
        tmp_path,
        "--session=default",
        "delete_session",
        identifier,
    )
    call_report = getattr(request.node, "rep_call", None)
    if cleanup_result.returncode != 0 and not (
        call_report is not None and call_report.failed
    ):
        cleanup_command = [
            str(console_path),
            "--session=default",
            "delete_session",
            identifier,
        ]
        assert cleanup_result.returncode == 0, _format_console_result(
            cleanup_command, cleanup_result
        )


def _project_names(
    output: str, session_id: str
) -> List[str]:
    lines = output.splitlines()
    header_index = next(
        (
            index
            for index, line in enumerate(lines)
            if line.startswith("Projects in the current session ")
        ),
        None,
    )
    assert header_index is not None, "GTlabConsole did not list the active session"
    assert session_id in lines[header_index]
    return [
        line.strip()
        for line in lines[header_index + 1 :]
        if line.startswith("\t")
    ]


def _list_session_projects(
    console_path: Path,
    console_environment: Dict[str, str],
    tmp_path: Path,
    session_id: str,
) -> List[str]:
    result = _run_console(
        console_path,
        console_environment,
        tmp_path,
        f"--session={session_id}",
        "list",
        "--project",
    )
    return _project_names(result.stdout, session_id)


def test_import_project_to_temporary_session(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    tmp_path: Path,
) -> None:
    """Import a project into an empty session and remove that session again."""
    assert (TEST_PROJECT_DIR / "project.gtlab").is_file()
    assert _list_session_projects(
        console_path, console_environment, tmp_path, session_id
    ) == []

    _run_console(
        console_path,
        console_environment,
        tmp_path,
        f"--session={session_id}",
        "import_to_session",
        TEST_PROJECT_DIR,
    )

    assert _list_session_projects(
        console_path, console_environment, tmp_path, session_id
    ) == ["TestProject"]


@pytest.mark.parametrize(
    "create_directory",
    [False, True],
    ids=["missing-directory", "empty-directory"],
)
def test_import_missing_project_leaves_session_empty(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    tmp_path: Path,
    create_directory: bool,
) -> None:
    """A missing project.gtlab file is rejected without changing the session."""
    missing_project_dir = tmp_path / "missing-project"
    if create_directory:
        missing_project_dir.mkdir()

    result = _run_console_expect_failure(
        console_path,
        console_environment,
        tmp_path,
        f"--session={session_id}",
        "import_to_session",
        missing_project_dir,
    )

    assert "project file" in result.stderr.lower()
    assert "not found" in result.stderr.lower()
    assert _list_session_projects(
        console_path, console_environment, tmp_path, session_id
    ) == []


def test_import_invalid_project_leaves_session_empty(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    tmp_path: Path,
) -> None:
    """A project file without required project metadata is rejected."""
    invalid_project_dir = tmp_path / "invalid-project"
    invalid_project_dir.mkdir()
    (invalid_project_dir / "project.gtlab").write_text("<GTLAB/>")

    result = _run_console_expect_failure(
        console_path,
        console_environment,
        tmp_path,
        f"--session={session_id}",
        "import_to_session",
        invalid_project_dir,
    )

    assert "could not be imported" in result.stderr.lower()
    assert _list_session_projects(
        console_path, console_environment, tmp_path, session_id
    ) == []


@pytest.mark.parametrize(
    "project_arguments",
    [(), (TEST_PROJECT_DIR, TEST_PROJECT_DIR)],
    ids=["missing-path", "extra-path"],
)
def test_import_invalid_argument_count_leaves_session_empty(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    tmp_path: Path,
    project_arguments: Tuple[Path, ...],
) -> None:
    """The command rejects calls without exactly one project directory."""
    result = _run_console_expect_failure(
        console_path,
        console_environment,
        tmp_path,
        f"--session={session_id}",
        "import_to_session",
        *project_arguments,
    )

    assert "invalid arguments" in result.stdout.lower()
    assert "import_to_session project_directory" in result.stdout.lower()
    assert _list_session_projects(
        console_path, console_environment, tmp_path, session_id
    ) == []


def test_importing_same_project_twice_keeps_single_session_entry(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    tmp_path: Path,
) -> None:
    """Importing an existing project again does not add a duplicate."""
    for _ in range(2):
        _run_console(
            console_path,
            console_environment,
            tmp_path,
            f"--session={session_id}",
            "import_to_session",
            TEST_PROJECT_DIR,
        )

    assert _list_session_projects(
        console_path, console_environment, tmp_path, session_id
    ) == ["TestProject"]
