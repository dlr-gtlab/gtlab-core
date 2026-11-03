# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

"""System tests for the repeatable "--set" option of the console run command.

The tests execute a task of a project that was imported into a temporary
session. They cover the user facing property path syntax, the error handling
(errors must be visible on stderr) and the project save behaviour.
"""

import os
import shutil
import subprocess
import uuid
from pathlib import Path
from typing import Dict, Iterator, List, Tuple, Union

import pytest


TEST_PROJECT_DIR = Path(__file__).parent / "TestProject"

PROJECT_NAME = "PropertyOverrideProject"
TASK_NAME = "Override Task"
TASK_GROUP = "SystemTest"

# uuid of the first "Solver" child object of the task fixture
SOLVER_UUID = "{a1000000-0000-4000-8000-000000000001}"

TASK_FILE_NAME = "{6c1a1111-2b2b-4c3c-9d4d-5e6f708192a3}.gttask"


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


@pytest.fixture
def session_id(
    console_path: Path,
    console_environment: Dict[str, str],
    tmp_path: Path,
) -> Iterator[str]:
    """Create and remove a unique session for each test."""
    identifier = f"run-property-overrides-{uuid.uuid4().hex}"
    _run_console(console_path, console_environment, tmp_path, "--session=default",
                 "create_session", identifier)

    yield identifier

    _run_console_command(
        console_path, console_environment, tmp_path,
        "--session=default", "delete_session", identifier)


@pytest.fixture
def project_dir(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    tmp_path: Path,
) -> Path:
    """Copy the fixture project into a temporary session and return its path."""
    assert (TEST_PROJECT_DIR / "project.gtlab").is_file()

    project_dir = tmp_path / "Project"
    shutil.copytree(TEST_PROJECT_DIR, project_dir)

    _run_console(console_path, console_environment, tmp_path,
                 f"--session={session_id}", "import_to_session", project_dir)

    return project_dir


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


def _run_task(
    console_path: Path,
    environment: Dict[str, str],
    tmp_path: Path,
    session_id: str,
    overrides: List[str],
    save: bool = False,
) -> subprocess.CompletedProcess:
    """Run the fixture task with the given raw "--set" arguments."""
    arguments: List[Union[str, Path]] = [
        f"--session={session_id}",
        "run",
        PROJECT_NAME,
        TASK_NAME,
        TASK_GROUP,
    ]

    for override in overrides:
        arguments += ["--set", override]

    if save:
        arguments += ["--save"]

    return _run_console_command(
        console_path, environment, tmp_path, *arguments
    )


def _run_task_success(
    console_path: Path,
    environment: Dict[str, str],
    tmp_path: Path,
    session_id: str,
    overrides: List[str],
    save: bool = False,
) -> subprocess.CompletedProcess:
    command_result = _run_task(
        console_path, environment, tmp_path, session_id, overrides, save
    )

    assert command_result.returncode == 0, _format_console_result(
        [str(console_path), "run"], command_result
    )
    return command_result


def _task_xml(project_dir: Path) -> str:
    return (project_dir / "tasks" / "_custom" / TASK_GROUP / TASK_FILE_NAME
            ).read_text(encoding="utf-8")


def _assert_failed_without_saving(
    result: subprocess.CompletedProcess, project_dir: Path, expected: str
) -> None:
    assert result.returncode != 0, _format_console_result(
        ["GTlabConsole", "run"], result
    )
    assert expected in _task_xml(project_dir)


def test_help_documents_the_set_option(
    console_path: Path, console_environment: Dict[str, str], tmp_path: Path
) -> None:
    """The command help explains the repeatable "--set" option."""
    result = _run_console(console_path, console_environment, tmp_path,
                          "run", "--help")

    assert "--set" in result.stdout
    assert "repeatable" in result.stdout.lower()
    assert '--set "iterations=100"' in result.stdout


def test_overrides_are_applied_and_task_is_executed(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """Properties of the task and of its child objects can be overridden."""
    result = _run_task_success(
        console_path,
        console_environment,
        tmp_path,
        session_id,
        [
            "iterations=100",
            "Solver[1].tolerance=1e-6",
            f"{SOLVER_UUID}.tolerance=2e-6",
            "Solver[1].points[2].pressure=420000",
            "Solver[1].boundaries[{inlet}].pressure=99000",
        ],
    )

    for override in ["iterations=100", "Solver[1].tolerance=1e-6",
                     "Solver[1].points[2].pressure=420000",
                     "Solver[1].boundaries[{inlet}].pressure=99000"]:
        path, _, value = override.partition("=")
        assert f"Property override applied: {path} = {value}" in result.stdout

    assert "Running Task" in result.stdout


def test_file_option_forwards_property_overrides(
    console_path: Path,
    console_environment: Dict[str, str],
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """The --file execution path applies the same repeated --set values."""
    result = _run_console(
        console_path,
        console_environment,
        tmp_path,
        "run",
        "--file",
        project_dir / "project.gtlab",
        TASK_NAME,
        TASK_GROUP,
        "--set",
        "iterations=64",
    )

    assert "Property override applied: iterations = 64" in result.stdout
    assert "Running Task" in result.stdout


def test_overrides_without_save_are_temporary(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """Without "--save" the project on disk keeps its original values."""
    original = _task_xml(project_dir)

    _run_task_success(
        console_path, console_environment, tmp_path, session_id,
        ["iterations=100"], save=False)

    assert _task_xml(project_dir) == original


def test_save_persists_overridden_values(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """With "--save" the overridden input values are written to the project."""
    _run_task_success(
        console_path, console_environment, tmp_path, session_id,
        ["iterations=128", "Solver[1].boundaries[{inlet}].pressure=99000"],
        save=True)

    saved = _task_xml(project_dir)
    assert '>128<' in saved
    assert '>99000<' in saved


def test_repeated_set_option_last_value_wins(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """Repeated overrides of one property are applied in command line order."""
    _run_task_success(
        console_path, console_environment, tmp_path, session_id,
        ["iterations=1", "iterations=2", "iterations=3"], save=True)

    assert '>3<' in _task_xml(project_dir)


def test_duplicate_object_names_need_an_index(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """Unambiguous names may be used without index, duplicates must not."""
    result = _run_task(console_path, console_environment, tmp_path, session_id,
                       ["Solver.tolerance=1e-6"])

    assert result.returncode != 0
    assert "ambiguous" in result.stderr

    result = _run_task_success(
        console_path, console_environment, tmp_path, session_id,
        ["Solver[1].tolerance=1e-6"])
    assert "Solver[1].tolerance" in result.stdout


def test_object_index_out_of_range_is_rejected(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """An index larger than the number of children is an error."""
    result = _run_task(console_path, console_environment, tmp_path, session_id,
                       ["Solver[3].tolerance=1e-6"])

    assert result.returncode != 0
    assert "out of range" in result.stderr

    result = _run_task(console_path, console_environment, tmp_path, session_id,
                       ["Solver[0].tolerance=1e-6"])
    assert result.returncode != 0
    assert "Invalid object index" in result.stderr


def test_property_container_selectors_are_validated(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """Container selectors must match the container type and must exist."""
    for override, message in [
        ("Solver[1].points[0].pressure=1", "indices start at 1"),
        ("Solver[1].points[4].pressure=1", "out of range"),
        ("Solver[1].points[{inlet}].pressure=1", "sequential"),
        ("Solver[1].boundaries[1].pressure=1", "associative"),
        ("Solver[1].boundaries[{outlet}].pressure=1", "No entry with id"),
    ]:
        result = _run_task(console_path, console_environment, tmp_path,
                           session_id, [override])

        assert result.returncode != 0, (
            f"'{override}' was unexpectedly accepted:\n"
            + _format_console_result(["GTlabConsole", "run"], result)
        )
        assert message in result.stderr


def test_unresolved_paths_are_rejected(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """Paths that cannot be resolved are reported on stderr."""
    for override in [
        "doesNotExist=1",
        "doesNotExist.tolerance=1",
        "Solver[1].doesNotExist=1",
        "Solver[1].points[1]=1",
    ]:
        result = _run_task(console_path, console_environment, tmp_path,
                           session_id, [override])

        assert result.returncode != 0, (
            f"'{override}' was unexpectedly accepted:\n"
            + _format_console_result(["GTlabConsole", "run"], result)
        )
        assert result.stderr.strip(), f"'{override}' reported no error"


def test_invalid_values_are_rejected(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """Values are converted by the GTlab property conversion mechanism."""
    result = _run_task(console_path, console_environment, tmp_path, session_id,
                       ["iterations=many"])

    assert result.returncode != 0
    assert "could not be converted" in result.stderr

    result = _run_task(console_path, console_environment, tmp_path, session_id,
                       ["iterations"])

    assert result.returncode != 0
    assert "expected \"<path>=<value>\"" in result.stderr


def test_read_only_and_monitoring_properties_are_rejected(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """Read only and monitoring properties must not be writable."""
    result = _run_task(console_path, console_environment, tmp_path, session_id,
                       ["readOnlyValue=42"])

    assert result.returncode != 0
    assert "read only" in result.stderr

    result = _run_task(console_path, console_environment, tmp_path, session_id,
                       ["result=42"])

    assert result.returncode != 0
    assert "monitoring" in result.stderr


def test_failing_override_does_not_execute_or_save(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """If one override fails, the task is not executed and not saved."""
    result = _run_task(console_path, console_environment, tmp_path, session_id,
                       ["iterations=77", "readOnlyValue=1"], save=True)

    _assert_failed_without_saving(result, project_dir, '>0<')
    assert "Running Task" not in result.stdout


def test_failing_execution_does_not_save(
    console_path: Path,
    console_environment: Dict[str, str],
    session_id: str,
    project_dir: Path,
    tmp_path: Path,
) -> None:
    """A failed task execution does not write the overrides to the project."""
    result = _run_task(console_path, console_environment, tmp_path, session_id,
                       ["iterations=-1"], save=True)

    _assert_failed_without_saving(result, project_dir, '>0<')
