import sys
import os

#generate os-compatible path
def generateImportPath(*path):
    joinedPath = path[0]
    for i in range(1, len(path)):
        subPath     = path[i]
        if os.name == 'nt':
            joinedPath += '\\'
        else:
            joinedPath += '/'
        joinedPath += subPath
    return joinedPath

# add path to custom, gloabl object_map
sys.path.append(generateImportPath('..', '..', 'common', 'shared_object_names'))

from objectmaphelper import *
# add custom, global object_map
from object_namespace import *


gtMainWin_GtMainWin = {"name": "GtMainWin", "type": "GtMainWin", "visible": 1}
preferences_GtPreferencesDialog = {"type": "GtPreferencesDialog", "unnamed": 1, "visible": 1, "windowTitle": "Preferences"}
gtMainWin_menuOptions_QMenu = {"name": "menuOptions", "type": "QMenu", "visible": 1, "window": gtMainWin_GtMainWin}
gtMainWin_Processes_Calculators_GtProcessDock = {"name": "Processes/Calculators", "type": "GtProcessDock", "visible": 1, "window": gtMainWin_GtMainWin}
processes_Calculators_GtProcessView = {"container": gtMainWin_Processes_Calculators_GtProcessDock, "type": "GtProcessView", "unnamed": 1, "visible": 1}
turbine_Design_from_manual_data_QModelIndex = {"column": 0, "container": processes_Calculators_GtProcessView, "text": "Turbine-Design from manual data", "type": "QModelIndex"}
hPC_CoMeT_Design_from_Performance_QModelIndex = {"column": 0, "container": processes_Calculators_GtProcessView, "text": "HPC-CoMeT-Design from Performance", "type": "QModelIndex"}
turbine_Design_from_manual_data_HPC_CoMeT_Design_from_Performance_QModelIndex = {"column": 0, "container": turbine_Design_from_manual_data_QModelIndex, "text": "HPC-CoMeT-Design from Performance", "type": "QModelIndex"}
gtMainWin_Explorer_GtExplorerDock = {"name": "Explorer", "type": "GtExplorerDock", "visible": 1, "window": gtMainWin_GtMainWin}
explorer_QComboBox = {"container": gtMainWin_Explorer_GtExplorerDock, "type": "QComboBox", "unnamed": 1, "visible": 1}
gtMainWin_Output_GtOutputDock = {"name": "Output", "type": "GtOutputDock", "visible": 1, "window": gtMainWin_GtMainWin}
output_qt_tabwidget_stackedwidget_QStackedWidget = {"container": gtMainWin_Output_GtOutputDock, "name": "qt_tabwidget_stackedwidget", "type": "QStackedWidget", "visible": 1}
qt_tabwidget_stackedwidget_QWidget = {"container": output_qt_tabwidget_stackedwidget_QStackedWidget, "type": "QWidget", "unnamed": 1, "visible": 1}
o_GtListView = {"container": qt_tabwidget_stackedwidget_QWidget, "type": "GtListView", "unnamed": 1, "visible": 1}
o_QScrollBar = {"container": o_GtListView, "type": "QScrollBar", "unnamed": 1, "visible": 1}
gtMainWin_QMenu = {"type": "QMenu", "unnamed": 1, "visible": 1, "window": gtMainWin_GtMainWin}
gtMainWin_Post_Processing_GtPostDock = {"name": "Post Processing", "type": "GtPostDock", "visible": 1, "window": gtMainWindow}
post_Processing_QPushButton = {"container": gtMainWin_Post_Processing_GtPostDock, "occurrence": 4, "type": "QPushButton", "unnamed": 1, "visible": 1}
post_Processing_GtListView = {"container": gtMainWin_Post_Processing_GtPostDock, "type": "GtListView", "unnamed": 1, "visible": 1}
delete_Item_GtDeleteItemMessageBox = {"type": "GtDeleteItemMessageBox", "unnamed": 1, "visible": 1, "windowTitle": "Delete Item"}
delete_Item_Yes_QPushButton = {"text": "Yes", "type": "QPushButton", "unnamed": 1, "visible": 1, "window": delete_Item_GtDeleteItemMessageBox}
gtMainWin_mdiArea_QMdiArea = {"name": "mdiArea", "type": "QMdiArea", "visible": 1, "window": gtMainWindow}
mdiArea_Add_QPushButton = {"container": gtMainWin_mdiArea_QMdiArea, "text": "Add", "type": "QPushButton", "unnamed": 1, "visible": 1}
o_QMenu = {"type": "QMenu", "unnamed": 1, "visible": 1}
mdiArea_QPushButton = {"container": gtMainWin_mdiArea_QMdiArea, "occurrence": 13, "type": "QPushButton", "unnamed": 1, "visible": 1}
mdiArea_Print_QPushButton = {"container": gtMainWin_mdiArea_QMdiArea, "text": "Print...", "type": "QPushButton", "unnamed": 1, "visible": 1}
print_Preview_QPrintPreviewDialog = {"type": "QPrintPreviewDialog", "unnamed": 1, "visible": 1, "windowTitle": "Print Preview"}
mdiArea_To_PDF_QPushButton = {"container": gtMainWin_mdiArea_QMdiArea, "text": "To PDF", "type": "QPushButton", "unnamed": 1, "visible": 1}
mdiArea_CloseButton = {"container": gtMainWin_mdiArea_QMdiArea, "type": "CloseButton", "unnamed": 1, "visible": 1}
explorer_GtExplorerView = {"container": gtMainWin_Explorer_GtExplorerDock, "type": "GtExplorerView", "unnamed": 1, "visible": 1}
module_Environment_Settings_Save_QPushButton = {"text": "Save", "type": "QPushButton", "unnamed": 1, "visible": 1, "window": gtEnvironmentDialog}
disabled_Plugins_Ok_QPushButton = {"text": "Ok", "type": "QPushButton", "unnamed": 1, "visible": 1, "window": gtDisabledPluginsDialog}
module_Environment_Settings_Exit_QPushButton = {"text": "Exit", "type": "QPushButton", "unnamed": 1, "visible": 1, "window": gtEnvironmentDialog}
