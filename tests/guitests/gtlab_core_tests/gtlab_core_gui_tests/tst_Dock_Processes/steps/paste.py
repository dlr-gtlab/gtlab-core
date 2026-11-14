# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

import builtins as python_builtins


def process_int(value):
    # Squish values read back from context.userData can be wrapped Qt integers.
    return python_builtins.int(python_builtins.str(value))


def process_action_label(action):
    text = python_builtins.str(action.text).split('\t', 1)[0]
    return text.replace('&', '').strip()


def process_item_center(view, index):
    rect = view.visualRect(index)
    return (process_int(rect.x) + process_int(rect.width) // 2,
            process_int(rect.y) + process_int(rect.height) // 2)


def paste_process_view():
    return waitForObject({'type': 'GtProcessView', 'visible': True})


def paste_process_menu():
    return waitForObject({'type': 'QMenu', 'visible': True, 'title': ''})


def visible_process_action(name):
    # Resolve each popup afresh: processContextMenu() creates a temporary QMenu.
    for action in paste_process_menu().actions():
        if action.visible and process_action_label(action) == name:
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
    test.verify(action is not None, 'Process menu contains ' + name +
                '; available: ' + ', '.join(process_action_label(item)
                                            for item in paste_process_menu().actions()))
    if action is not None:
        test.verify(action.enabled, name + ' is enabled')


@Then("the process action '|any|' is disabled")
def verify_process_action_disabled(context, name):
    action = visible_process_action(name)
    test.verify(action is not None, 'Process menu contains ' + name +
                '; available: ' + ', '.join(process_action_label(item)
                                            for item in paste_process_menu().actions()))
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
    context.userData['paste_child_count'] = process_int(view.model().rowCount(target))


@When('the remembered paste target is selected')
def select_paste_target(context):
    view = paste_process_view()
    target = context.userData['paste_target']
    view.scrollTo(target)
    x, y = process_item_center(view, target)
    mouseClick(view.viewport(), x, y, Qt.NoModifier, Qt.LeftButton)


@When('the context menu of the selected process is opened')
def open_selected_process_menu(context):
    view = paste_process_view()
    x, y = process_item_center(view, view.currentIndex())
    openContextMenu(view.viewport(), x, y, Qt.NoModifier)
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
    expected = process_int(context.userData['paste_child_count']) + process_int(count)
    test.verify(waitFor(lambda: process_int(paste_process_view().model().rowCount(
        target)) == expected))


@Then("the pasted component is a '|word|' named '|any|'")
def verify_pasted_component(context, component_type, name):
    view = paste_process_view()
    index = view.currentIndex()
    test.compare(python_builtins.str(index.data(Qt.DisplayRole)), name)
    target = context.userData['paste_target']
    test.compare(index.parent().row(), target.row())
    test.compare(index.parent().data(Qt.DisplayRole),
                 target.data(Qt.DisplayRole))

    # Tasks have an Add action; inspecting it does not require opening or
    # locating the hidden submenu.
    open_selected_process_menu(context)
    has_task_menu = visible_process_action('Add...') is not None
    test.compare(has_task_menu, component_type == 'task')
    dismiss_process_menu(context)


@Then('the remembered paste target is running')
def wait_for_running_paste_target(context):
    target = context.userData['paste_target']
    model = paste_process_view().model()
    state_index = model.index(target.row(), 1, target.parent())

    def is_running():
        # GtProcessComponentModel exposes execution state in column 1's tooltip.
        state = python_builtins.str(state_index.data(Qt.ToolTipRole))
        return state == 'Running' or state.startswith('Progress:')

    test.verify(waitFor(is_running, 10000), 'Paste target has started running')


@When('the Sleep calculator is added using the process wizard')
def add_sleep_calculator(context):
    wizard_query = {'type': 'GtProcessWizard', 'visible': True,
                    'windowTitle': 'New Calculator Wizard'}
    wizard = waitForObject(wizard_query)
    tree = waitForObject({'type': 'GtProcessOverviewTree', 'visible': True,
                          'container': wizard})
    model = tree.model()

    def child_named(parent, name):
        labels = []
        for row in python_builtins.range(process_int(model.rowCount(parent))):
            index = model.index(row, 0, parent)
            label = python_builtins.str(index.data(Qt.DisplayRole))
            if label == name:
                return index
            labels.append(label)
        raise LookupError('Process wizard entry ' + name +
                          ' missing; available: ' + ', '.join(labels))

    category = child_named(tree.rootIndex(), 'Testing')
    entry = child_named(category, 'Sleep Calculator [RELEASE]')
    tree.expand(category)
    tree.scrollTo(entry)
    x, y = process_item_center(tree, entry)
    # GtCalculatorOverviewPage::onDoubleClicked advances to Settings.
    doubleClick(tree.viewport(), x, y, Qt.NoModifier, Qt.LeftButton)
    waitForObject({'type': 'GtCalculatorSettingsPage', 'visible': True,
                   'container': wizard})

    def finish_button(parent):
        for child in object.children(parent):
            if object.type(child) == 'QPushButton':
                label = python_builtins.str(child.text).replace('&', '').strip()
                if label == 'Finish' and child.visible and child.enabled:
                    return child
            found = finish_button(child)
            if found is not None:
                return found
        return None

    button = finish_button(wizard)
    if button is None:
        raise LookupError('Calculator wizard has no enabled Finish button')
    clickButton(button)
    test.verify(waitFor(lambda: not object.exists(wizard_query)),
                'Calculator wizard has closed')
