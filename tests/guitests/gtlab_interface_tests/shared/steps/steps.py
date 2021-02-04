from common import *
from qt_icon import qIcon

import test_logging as logging
import test
import squish
from qt_menu import qMenu

@Step("Icon Test")
def step(context):
    cwd  = context.userData['application_context'].cwd
    menu = qMenu(title="File")
    # object has an icon
    act  = menu.action("New Project")
    icon = qIcon(act)
    icon.isValid()
    icon.saveIcon(cwd)
    # object does not have an icon
    act  = menu.action("Save As...")
    icon = qIcon(act)
    icon.isValid()
    icon.saveIcon(cwd)