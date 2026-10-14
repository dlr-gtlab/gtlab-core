# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

"""System test for opt-in recording through the real task execution path.

GTlabConsole runs a TestRecordingTask whose execution mode is the
TestRecordingExecutor of the test process module. The executor records the
task iteration and every child that inherits its mode; a child set to
``local`` runs in the task's recording. The executor and the task log to the
file named by GTLAB_TEST_RECORDING_LOG (issue #1600).
"""

import os
import re
import shutil
import subprocess
from pathlib import Path

import pytest


TEST_DATA_DIR = Path(__file__).parent
PROJECT_MEMENTO = TEST_DATA_DIR.parent / "run_task_from_memento" / "project.xml"


@pytest.fixture
def console_path() -> Path:
    console = os.environ.get("GTLAB_CONSOLE")
    assert console, "GTLAB_CONSOLE must point to GTlabConsole"

    path = Path(console)
    assert path.is_file(), f"GTlabConsole does not exist: {path}"
    return path


@pytest.fixture
def recording_log(console_path: Path, tmp_path: Path) -> list:
    """Runs the recorded task and returns the lines of the recording log."""
    project_memento = tmp_path / "project.xml"
    task_memento = tmp_path / "recorded_task.xml"
    log_file = tmp_path / "recording.log"

    shutil.copyfile(PROJECT_MEMENTO, project_memento)
    shutil.copyfile(TEST_DATA_DIR / "recorded_task.xml", task_memento)

    env = dict(os.environ, GTLAB_TEST_RECORDING_LOG=str(log_file))
    result = subprocess.run(
        [
            console_path,
            "run_task_from_memento",
            "-p",
            project_memento,
            "-t",
            task_memento,
            "-o",
            tmp_path / "result.diff.xml",
            "-w",
            tmp_path,
        ],
        text=True,
        capture_output=True,
        check=False,
        env=env,
    )

    assert result.returncode == 0, (
        f"GTlabConsole failed with exit code {result.returncode}\n"
        f"stdout:\n{result.stdout}\n"
        f"stderr:\n{result.stderr}"
    )
    assert log_file.is_file(), "The recording executor wrote no log"
    return log_file.read_text().splitlines()


def _started(log: list, component: str) -> re.Match:
    pattern = re.compile(
        rf"started {re.escape(component)} id=(\S+) parent=(\S+)$")
    matches = [m for m in map(pattern.match, log) if m]
    assert len(matches) == 1, f"{component} not started once:\n{log}"
    return matches[0]


def test_setup_runs_once_outside_the_recording(recording_log: list) -> None:
    setups = [line for line in recording_log if line.startswith("setUp")]
    assert setups == ["setUp recording=0"]
    assert recording_log.index("setUp recording=0") < recording_log.index(
        _started(recording_log, "Recording Task").group(0))


def test_task_iteration_is_recorded(recording_log: list) -> None:
    task = _started(recording_log, "Recording Task")
    assert task.group(2) == "none"
    assert recording_log[-1] == "finished Recording Task success=1"

    # the local child runs in the task's recording
    assert ("written Test DMI Package.mementoTestValue in Recording Task"
            in recording_log)
    # setUp()'s read is not recorded
    assert not any(line.startswith("read Test DMI Package")
                   for line in recording_log)


def test_inheriting_child_gets_nested_recording(recording_log: list) -> None:
    task = _started(recording_log, "Recording Task")
    child = _started(recording_log, "Recorded Child")
    assert child.group(2) == task.group(1)

    start = recording_log.index(child.group(0))
    finish = recording_log.index("finished Recorded Child success=1")
    assert recording_log[start + 1:finish] == [
        "written Test DMI Package.mementoTestValue in Recorded Child"]


def test_local_child_has_no_own_recording(recording_log: list) -> None:
    assert not any("Local Child" in line for line in recording_log)
