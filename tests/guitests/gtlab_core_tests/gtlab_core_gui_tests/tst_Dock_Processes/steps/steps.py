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
    view.clearSelection()


@When("the current process index is selected again")
def reselectCurrentProcess(context):
    view = waitForObject({"type": "GtProcessView", "visible": True})
    view.setCurrentIndex(view.currentIndex())
