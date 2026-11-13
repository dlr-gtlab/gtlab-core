# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+


def paste_process_view():
    return waitForObject({'type': 'GtProcessView', 'visible': True})


def paste_process_menu():
    return waitForObject({'type': 'QMenu', 'visible': True})


def visible_process_action(name):
    # Resolve each popup afresh: processContextMenu() creates a temporary QMenu.
    for action in paste_process_menu().actions():
        if action.visible and str(action.text).replace('&', '') == name:
            return action
    return None


@When("the process action '|any|' is activated")
def activate_process_action(context, path):
    menu = paste_process_menu()
    labels = path.split(' > ')
    for position, label in enumerate(labels):
        activateItem(menu, label)
        if position < len(labels) - 1:
            menu = waitForObject({'type': 'QMenu', 'visible': True,
                                  'title': label})


@Then("the process action '|any|' is enabled")
def verify_process_action_enabled(context, name):
    action = visible_process_action(name)
    test.verify(action is not None, 'Process menu contains ' + name)
    if action is not None:
        test.verify(action.enabled, name + ' is enabled')


@Then("the process action '|any|' is disabled")
def verify_process_action_disabled(context, name):
    action = visible_process_action(name)
    test.verify(action is not None, 'Process menu contains ' + name)
    if action is not None:
        test.verify(not action.enabled, name + ' is disabled')


@Then("the process action '|any|' does not exist")
def verify_process_action_absent(context, name):
    test.verify(visible_process_action(name) is None,
                'Process menu does not contain ' + name)


@When('the selected process is remembered as the paste target')
def remember_paste_target(context):
    view = paste_process_view()
    target = view.currentIndex()
    test.verify(target.isValid())
    context.userData['paste_target'] = target
    context.userData['paste_child_count'] = view.model().rowCount(target)


@When('the remembered paste target is selected')
def select_paste_target(context):
    view = paste_process_view()
    target = context.userData['paste_target']
    view.scrollTo(target)
    rect = view.visualRect(target)
    mouseClick(view.viewport(), rect.x + rect.width // 2,
               rect.y + rect.height // 2, Qt.NoModifier, Qt.LeftButton)


@When('the context menu of the selected process is opened')
def open_selected_process_menu(context):
    view = paste_process_view()
    rect = view.visualRect(view.currentIndex())
    openContextMenu(view.viewport(), rect.x + rect.width // 2,
                    rect.y + rect.height // 2, Qt.NoModifier)
    paste_process_menu()


@When('Ctrl+V is pressed in the process view')
def keyboard_paste(context):
    type(paste_process_view(), '<Ctrl+V>')


@When('the process context menu is dismissed')
def dismiss_process_menu(context):
    type(paste_process_menu(), '<Escape>')


@Then('the paste target has |integer| additional children')
def verify_paste_child_count(context, count):
    target = context.userData['paste_target']
    expected = context.userData['paste_child_count'] + int(count)
    test.verify(waitFor(lambda: paste_process_view().model().rowCount(
        target) == expected))


@Then("the pasted component is a '|word|' named '|any|'")
def verify_pasted_component(context, component_type, name):
    view = paste_process_view()
    index = view.currentIndex()
    test.compare(str(index.data(Qt.DisplayRole)), name)
    target = context.userData['paste_target']
    test.compare(index.parent().row(), target.row())
    test.compare(index.parent().data(Qt.DisplayRole),
                 target.data(Qt.DisplayRole))

    # The Add submenu is present only for tasks in processContextMenu().
    # Check the selected child's type through its UI, without a test-only API.
    open_selected_process_menu(context)
    menu = paste_process_menu()
    has_task_menu = object.exists({'type': 'QMenu', 'title': 'Add...',
                                   'container': menu})
    test.compare(has_task_menu, component_type == 'task')
    dismiss_process_menu(context)
