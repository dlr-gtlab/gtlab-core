# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

import os
import shutil

import test_logging as logging

from gt_dockwidgets import gtExplorerDock


@Given("the test project directory '|word|' is removed")
@When("the test project directory '|word|' is removed")
@Then("the test project directory '|word|' is removed")
def removeTestProjectDirectory(context, project):
    testProjectsDir = os.path.abspath(
        os.path.join(os.path.dirname(__file__), "../../../../")
    )
    projectDir = os.path.abspath(os.path.join(testProjectsDir, project))

    if (
        not project.startswith("issue1602_")
        or os.path.dirname(projectDir) != testProjectsDir
    ):
        raise ValueError(
            f"Refusing to remove unexpected test project directory: {projectDir}"
        )

    if os.path.exists(projectDir):
        shutil.rmtree(projectDir)

    assert not os.path.exists(projectDir)


@Then("the project '|any|' is open")
def assertProjectIsOpen(context, project):
    logging.evalFunction(
        True,
        gtExplorerDock().isProjectOpen,
        project,
        message=f"the project '{project}' is open",
    )


@Then("the project '|any|' is closed")
def assertProjectIsClosed(context, project):
    logging.evalFunction(
        False,
        gtExplorerDock().isProjectOpen,
        project,
        message=f"the project '{project}' is closed",
    )


@When("the switch project prompt is answered with |word|")
def answerSwitchProjectPrompt(context, answer):
    waitForObject({"type": "QMessageBox", "windowTitle": "Confirm Switch Project"})
    clickButton(
        waitForObject(
            {
                "type": "QPushButton",
                "text": answer,
                "window": {"type": "QMessageBox", "windowTitle": "Confirm Switch Project"},
            }
        )
    )
