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


# Additional issue-1524 acceptance coverage through the real console executable.
def _run(console, project, task, output, working_directory=None):
    args = [console, "run_task_from_memento", "-p", project, "-t", task,
            "-o", output]
    if working_directory:
        args += ["-w", working_directory]
    return subprocess.run(args, text=True, capture_output=True, check=False)


def _fixtures(directory):
    directory.mkdir()
    project = directory / "project memento.xml"
    task = directory / "task memento.xml"
    shutil.copyfile(TEST_DATA_DIR / "project.xml", project)
    shutil.copyfile(TEST_DATA_DIR / "change_project_task.xml", task)
    return project, task


def _assert_failed_without_output(result, output):
    assert result.returncode != 0
    assert not output.exists()


def test_explicit_working_directory_paths_logs_and_side_effect(console_path, tmp_path):
    """Covers spaced paths, explicit -w, logs, and untracked side effects."""
    project, task = _fixtures(tmp_path / "input files")
    output = tmp_path / "output files" / "result diff.xml"
    output.parent.mkdir()
    working_directory = tmp_path / "explicit working directory"
    working_directory.mkdir()
    result = _run(console_path, project, task, output, working_directory)
    assert result.returncode == 0, result.stderr
    assert "Memento system test calculator completed" in result.stdout + result.stderr
    assert (working_directory / "memento_side_effect.txt").read_text() == "side effect\n"
    assert "memento_side_effect.txt" not in output.read_text()


def test_default_working_directory(console_path, tmp_path):
    """Without -w, the project-Memento directory is used deterministically."""
    project, task = _fixtures(tmp_path / "default working directory")
    output = tmp_path / "result.xml"
    assert _run(console_path, project, task, output).returncode == 0
    assert (project.parent / "memento_side_effect.txt").is_file()


def test_empty_diff(console_path, tmp_path):
    """A successful task without calculators produces a valid empty diff."""
    project, _ = _fixtures(tmp_path / "empty input")
    task = project.parent / "empty task.xml"
    task.write_text("<object class=\"GtTask\" uuid=\"{0996195d-ae13-4061-8778-5b406a852814}\" name=\"Empty\"><property type=\"bool\" name=\"skip\">false</property><property type=\"QString\" name=\"processRunner\"/><objectlist/></object>")
    output = tmp_path / "empty.xml"
    result = _run(console_path, project, task, output)
    assert result.returncode == 0, result.stderr
    assert output.read_bytes() == b""


@pytest.mark.parametrize("project_data,task_data", [(None, None), ("invalid", None), ("fixture", "invalid"), ("fixture", (TEST_DATA_DIR / "project.xml").read_text())])
def test_invalid_inputs_publish_no_output(console_path, tmp_path, project_data, task_data):
    """Missing, invalid, and unsupported Mementos discard stale output."""
    project, task, output = tmp_path / "project.xml", tmp_path / "task.xml", tmp_path / "result.xml"
    if project_data == "fixture":
        shutil.copyfile(TEST_DATA_DIR / "project.xml", project)
    elif project_data is not None:
        project.write_text(project_data)
    if task_data is not None:
        task.write_text(task_data)
    elif project_data is not None:
        shutil.copyfile(TEST_DATA_DIR / "change_project_task.xml", task)
    output.write_text("stale")
    _assert_failed_without_output(_run(console_path, project, task, output), output)


def test_task_failure_and_unwritable_output_publish_no_diff(console_path, tmp_path):
    """Task failure and failed atomic output publication leave no result file."""
    project, task = _fixtures(tmp_path / "task failure")
    task.write_text(task.read_text().replace("{2f96bc74-bdec-407c-9cec-ece2a13da5c2}", "{00000000-0000-0000-0000-000000000000}"))
    output = tmp_path / "failure.xml"
    _assert_failed_without_output(_run(console_path, project, task, output), output)
    project, task = _fixtures(tmp_path / "output failure")
    output = tmp_path / "missing directory" / "result.xml"
    _assert_failed_without_output(_run(console_path, project, task, output), output)
