# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+


def paste_process_view():
    return waitForObject({'type': 'GtProcessView', 'visible': True})


@When('the selected process is remembered as the paste target')
def remember_paste_target(context):
    view = paste_process_view()
    context.paste_target = view.currentIndex()
    test.verify(context.paste_target.isValid())
    context.paste_child_count = view.model().rowCount(context.paste_target)


@When('the remembered paste target is selected')
def select_paste_target(context):
    view = paste_process_view()
    view.scrollTo(context.paste_target)
    rect = view.visualRect(context.paste_target)
    mouseClick(view.viewport(), rect.x + rect.width // 2,
               rect.y + rect.height // 2)


@When('the context menu of the selected process is opened')
def open_selected_process_menu(context):
    view = paste_process_view()
    rect = view.visualRect(view.currentIndex())
    openContextMenu(view.viewport(), rect.x + rect.width // 2,
                    rect.y + rect.height // 2)


@When('Ctrl+V is pressed in the process view')
def keyboard_paste(context):
    type(paste_process_view(), '<Ctrl+V>')


@When('the process context menu is dismissed')
def dismiss_process_menu(context):
    type(waitForObject({'type': 'QMenu', 'visible': True}), '<Escape>')


@Then('the paste target has |integer| additional children')
def verify_paste_child_count(context, count):
    expected = context.paste_child_count + int(count)
    test.verify(waitFor(lambda: paste_process_view().model().rowCount(
        context.paste_target) == expected))


@Then("the pasted component is a '|word|' named '|any|'")
def verify_pasted_component(context, component_type, name):
    view = paste_process_view()
    index = view.currentIndex()
    test.compare(str(index.data(Qt.DisplayRole)), name)
    test.compare(index.parent().row(), context.paste_target.row())
    test.compare(index.parent().data(Qt.DisplayRole),
                 context.paste_target.data(Qt.DisplayRole))

    # The Add submenu is present only for tasks in processContextMenu().
    # Check the selected child's type through its UI, without a test-only API.
    open_selected_process_menu(context)
    menu = waitForObject({'type': 'QMenu', 'visible': True})
    has_task_menu = object.exists({'type': 'QMenu', 'title': 'Add...',
                                   'container': menu})
    test.compare(has_task_menu, component_type == 'task')
    dismiss_process_menu(context)
