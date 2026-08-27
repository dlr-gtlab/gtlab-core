# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

"""System test for executing a task from project and task mementos.

The test verifies the complete headless workflow: GTlabConsole restores the
exported project and task mementos, executes the task with the restored project
as its execution context, and writes an applicable result diff. It protects
against regressions where task changes are lost or the project root name changes.
"""

import os
import shutil
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path

import pytest


TEST_DATA_DIR = Path(__file__).parent


@pytest.fixture
def console_path() -> Path:
    console = os.environ.get("GTLAB_CONSOLE")
    assert console, "GTLAB_CONSOLE must point to GTlabConsole"

    path = Path(console)
    assert path.is_file(), f"GTlabConsole does not exist: {path}"
    return path


def test_run_task_from_memento(console_path: Path, tmp_path: Path) -> None:
    """Execute the fixture task and assert its expected project-property diff."""
    project_memento = tmp_path / "project.xml"
    task_memento = tmp_path / "change_project_task.xml"
    diff_file = tmp_path / "result.diff.xml"

    shutil.copyfile(TEST_DATA_DIR / "project.xml", project_memento)
    shutil.copyfile(TEST_DATA_DIR / "change_project_task.xml", task_memento)

    result = subprocess.run(
        [
            console_path,
            "run_task_from_memento",
            "-p",
            project_memento,
            "-t",
            task_memento,
            "-o",
            diff_file,
            "-w",
            tmp_path,
        ],
        text=True,
        capture_output=True,
        check=False,
    )

    assert result.returncode == 0, (
        f"GTlabConsole failed with exit code {result.returncode}\n"
        f"stdout:\n{result.stdout}\n"
        f"stderr:\n{result.stderr}"
    )
    assert diff_file.is_file(), "GTlabConsole did not create a result diff"

    root = ET.parse(diff_file).getroot()
    package = next(
        (
            object_memento
            for object_memento in root.iter("object")
            if object_memento.get("name") == "Test DMI Package"
        ),
        None,
    )
    assert package is not None, "The changed Test DMI Package is missing"

    change = next(
        (
            diff_change
            for diff_change in package.findall("diff-property-change")
            if diff_change.get("name") == "mementoTestValue"
        ),
        None,
    )
    assert change is not None, "The memento test value was not changed"
    assert change.findtext("oldVal") == "21"
    assert change.findtext("newVal") == "42"
    assert not any(
        attribute_change.get("id") == "name"
        for attribute_change in root.iter("diff-attribute-change")
    )
