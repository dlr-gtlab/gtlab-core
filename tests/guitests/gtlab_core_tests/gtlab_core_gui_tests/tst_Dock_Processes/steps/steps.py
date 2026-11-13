# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+


@When("empty space in the process view is clicked")
def clickEmptyProcessSpace(context):
    view = waitForObject({"type": "GtProcessView", "visible": True})
    mouseClick(view, 10, view.height - 10, 0, Qt.LeftButton)


@When("the process selection is cleared")
def clearProcessSelection(context):
    view = waitForObject({"type": "GtProcessView", "visible": True})
    context.previousProcessIndex = view.currentIndex()
    view.clearSelection()


@When("the previously selected process index is selected again")
def reselectPreviousProcess(context):
    view = waitForObject({"type": "GtProcessView", "visible": True})
    view.setCurrentIndex(context.previousProcessIndex)


@Then("the process view has no current index")
def verifyNoCurrentProcess(context):
    view = waitForObject({"type": "GtProcessView", "visible": True})
    test.verify(not view.currentIndex().isValid(), "No current process index")
