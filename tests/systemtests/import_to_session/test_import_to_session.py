# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

"""System test for importing a project directory into a session."""

import os
import subprocess
import uuid
from pathlib import Path

import pytest


TEST_PROJECT_DIR = Path(__file__).parent / "TestProject"


@pytest.fixture
def console_path() -> Path:
    console = os.environ.get("GTLAB_CONSOLE")
    assert console, "GTLAB_CONSOLE must point to GTlabConsole"

    path = Path(console)
    assert path.is_file(), f"GTlabConsole does not exist: {path}"
    return path


def _console_environment(tmp_path: Path):
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


def _run_console(
    console_path: Path, environment, working_directory: Path, *arguments
) -> subprocess.CompletedProcess:
    command = [str(console_path), *map(str, arguments)]
    result = subprocess.run(
        command,
        cwd=working_directory,
        env=environment,
        text=True,
        capture_output=True,
        check=False,
    )

    assert result.returncode == 0, (
        f"GTlabConsole command failed: {command[1:]}\n"
        f"exit code: {result.returncode}\n"
        f"stdout:\n{result.stdout}\n"
        f"stderr:\n{result.stderr}"
    )
    return result


def _project_names(output: str, session_id: str):
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
    return [line.strip() for line in lines[header_index + 1 :] if line.startswith("\t")]


def test_import_project_to_temporary_session(
    console_path: Path, tmp_path: Path
) -> None:
    """Import a project into an empty session and remove that session again."""
    assert (TEST_PROJECT_DIR / "project.gtlab").is_file()

    environment = _console_environment(tmp_path)
    session_id = f"import-to-session-{uuid.uuid4().hex}"
    session_created = False

    try:
        _run_console(
            console_path,
            environment,
            tmp_path,
            "--session",
            "default",
            "create_session",
            session_id,
        )
        session_created = True

        before_import = _run_console(
            console_path,
            environment,
            tmp_path,
            "--session",
            session_id,
            "list",
            "--project",
        )
        assert "Projects in the current session" in before_import.stdout
        assert _project_names(before_import.stdout, session_id) == []

        _run_console(
            console_path,
            environment,
            tmp_path,
            "--session",
            session_id,
            "import_to_session",
            TEST_PROJECT_DIR,
        )

        after_import = _run_console(
            console_path,
            environment,
            tmp_path,
            "--session",
            session_id,
            "list",
            "--project",
        )
        assert _project_names(after_import.stdout, session_id) == ["TestProject"]
    finally:
        if session_created:
            _run_console(
                console_path,
                environment,
                tmp_path,
                "--session",
                "default",
                "delete_session",
                session_id,
            )
