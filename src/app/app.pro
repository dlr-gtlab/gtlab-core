#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

GT_MODULE_ID="App"

include( $${PWD}/../../settings.pri )

win32 {
    RC_FILE = app.rc
}

TARGET = GTlab

QT += widgets xml network quickwidgets
TEMPLATE = app
CONFIG += silent
CONFIG += c++14

#DEFINES += GT_LICENCE
#CONFIG += GT_LICENCE

#DEFINES += GT_MODELTEST
#CONFIG += GT_MODELTEST

GT_MODELTEST {
    QT += testlib
}

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-app
    OBJECTS_DIR = $${BUILD_DEST}/debug-app/obj
    MOC_DIR = $${BUILD_DEST}/debug-app/moc
    RCC_DIR = $${BUILD_DEST}/debug-app/rcc
    UI_DIR = $${BUILD_DEST}/debug-app/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-app
    OBJECTS_DIR = $${BUILD_DEST}/release-app/obj
    MOC_DIR = $${BUILD_DEST}/release-app/moc
    RCC_DIR = $${BUILD_DEST}/release-app/rcc
    UI_DIR = $${BUILD_DEST}/release-app/ui
}
INCLUDEPATH += .\
    dialogs \
    widgets \
    preferences \
    preferences/pages \
    project \
    project/pages \
    tools \
    models \
    mdi_items \
    mdi_items/memento_viewer \
    mdi_items/session_viewer \
    mdi_items/state_viewer \
    mdi_items/examples \
    mdi_items/result_viewer \
    mdi_items/label_usages \
    mdi_items/process_env \
    mdi_items/process_env/entities \
    dock_widgets/output \
    dock_widgets/explorer \
    dock_widgets/process \
    dock_widgets/properties \
    dock_widgets/calculators \
    dock_widgets/post \
    dock_widgets/labels \
    ../utilities/logging \
    ../dataprocessor \
    ../dataprocessor/property \
    ../core \
    ../core/process_management \
    ../core/process_management/process_runner \
    ../core/process_management/process_runner/commands \
    ../core/settings \
    ../core/network \
    ../core/provider \
    ../core/states \
    ../gui \
    ../gui/exporter \
    ../gui/importer \
    ../gui/tools \
    ../gui/tools/graphic_items \
    ../gui/post \
    ../gui/dialogs \
    ../gui/object_ui \
    ../gui/mdi_items \
    ../gui/mdi_items/collection_env \
    ../gui/dock_widgets \
    ../gui/dock_widgets/process \
    ../gui/dock_widgets/properties \
    ../gui/dock_widgets/post \
    ../gui/process_runner \
    ../gui/widgets

DESTDIR = $${BUILD_DEST}

HEADERS += \
    dialogs/gt_logerrormessagebox.h \
    dialogs/gt_moduledetailsdialog.h \
    dialogs/gt_markdowneditor.h \
    dialogs/gt_refusedpluginsdialog.h \
    dock_widgets/process/gt_taskgroupmodel.h \
    dock_widgets/properties/gt_propertycontainerwidget.h \
    gt_mainwin.h \
    preferences/gt_preferencesdialog.h \
    preferences/pages/gt_preferencesapp.h \
    preferences/pages/gt_preferenceslanguage.h \
    preferences/pages/gt_preferencessession.h \
    preferences/pages/gt_preferencesshortcuts.h \
    tools/gt_toolbarhandler.h \
    widgets/gt_sessionlistwidget.h \
    preferences/pages/gt_preferencesperspective.h \
    widgets/gt_perspectiveslistwidget.h \
    project/gt_projectwizard.h \
    project/pages/gt_projectwizardstart.h \
    project/pages/gt_projectwizardspec.h \
    project/pages/gt_projectwizardmodules.h \
    project/pages/gt_projectwizardimport.h \
    project/pages/gt_abstractprojectwizardpage.h \
    widgets/gt_perspectiveswitchwidget.h \
    preferences/pages/gt_preferencesaccess.h \
    dialogs/gt_aboutdialog.h \
    tools/gt_accessgroupmodel.h \
    dialogs/gt_accessdatadialog.h \
    widgets/gt_splashscreen.h \
    widgets/gt_startuppage.h \
    dialogs/gt_checkforupdatesdialog.h \
    widgets/gt_cornerwidget.h \
    widgets/gt_simpleloadingwidget.h \
    preferences/pages/gt_preferencespathsettings.h \
    models/gt_processqueuemodel.h \
    widgets/gt_processqueuewidget.h \
    dialogs/gt_environmentdialog.h \
    mdi_items/session_viewer/gt_sessionviewer.h \
    mdi_items/state_viewer/gt_stateviewer.h \
    mdi_items/state_viewer/gt_statemodel.h \
    mdi_items/memento_viewer/gt_mementoviewer.h \
    mdi_items/memento_viewer/gt_mementoeditor.h \
    mdi_items/examples/gt_examplesmdiwidget.h \
    mdi_items/examples/gt_examplesentry.h \
    mdi_items/examples/gt_examplegraphicalitem.h \
    mdi_items/examples/gt_examplestabwidget.h \
    mdi_items/label_usages/gt_labelusageviewer.h \
    mdi_items/label_usages/gt_labelcontainer.h \
    mdi_items/label_usages/gt_labelusagemodel.h \
    mdi_items/label_usages/gt_labelusagedelegate.h \
    mdi_items/process_env/gt_processeditor.h \
    mdi_items/process_env/entities/gt_taskentity.h \
    mdi_items/process_env/entities/gt_calculatorentity.h \
    mdi_items/process_env/entities/gt_calculatorpropertyentity.h \
    mdi_items/process_env/entities/gt_propertyportentity.h \
    mdi_items/process_env/entities/gt_taskarrowentity.h \
    mdi_items/process_env/entities/gt_taskentityshadow.h \
    mdi_items/process_env/entities/gt_datamodelportentity.h \
    mdi_items/process_env/gt_processeditorview.h \
    mdi_items/process_env/entities/gt_addtaskglobalbutton.h \
    mdi_items/process_env/entities/gt_taskconnectionentity.h \
    mdi_items/process_env/entities/gt_taskpipeentity.h \
    mdi_items/process_env/entities/gt_tasklinkentity.h \
    mdi_items/process_env/entities/gt_taskarrowlabelentity.h \
    mdi_items/process_env/gt_processeditorscene.h \
    mdi_items/process_env/gt_processconnectionmodel.h \
    mdi_items/process_env/entities/gt_processconnectionview.h \
    mdi_items/process_env/gt_processconnectiongraphicsview.h \
    mdi_items/process_env/entities/gt_processpropertyportentity.h \
    mdi_items/process_env/gt_processconnectionitem.h \
    mdi_items/process_env/entities/gt_processpropertyconnectionentity.h \
    mdi_items/process_env/gt_processconnectionscene.h \
    dock_widgets/output/gt_outputdock.h \
    dock_widgets/output/gt_styledlogmodel.h \
    dock_widgets/output/gt_filteredlogmodel.h \
    dock_widgets/output/gt_outputtester.h \
    dock_widgets/output/gt_taskhistorymodel.h \
    dock_widgets/explorer/gt_explorerdock.h \
    dock_widgets/explorer/gt_explorerview.h \
    dock_widgets/explorer/gt_openwithmenu.h \
    dock_widgets/process/gt_processdock.h \
    dock_widgets/process/gt_processview.h \
    dock_widgets/process/gt_processconnectioneditor.h \
    dock_widgets/process/gt_processcomponentsettingsbutton.h \
    dock_widgets/calculators/gt_calculatorsdock.h \
    dock_widgets/post/gt_postdelegate.h \
    dock_widgets/post/gt_postdock.h \
    dock_widgets/properties/gt_propertiesdock.h \
    dock_widgets/labels/gt_labelsdock.h \
    dock_widgets/labels/gt_labeldelegate.h

FORMS += ui/gt_mainwin.ui

SOURCES += \
    app.cpp \
    dialogs/gt_logerrormessagebox.cpp \
    dialogs/gt_moduledetailsdialog.cpp \
    dialogs/gt_markdowneditor.cpp \
    dialogs/gt_refusedpluginsdialog.cpp \
    dock_widgets/process/gt_taskgroupmodel.cpp \
    dock_widgets/properties/gt_propertycontainerwidget.cpp \
    gt_mainwin.cpp \
    preferences/gt_preferencesdialog.cpp \
    preferences/pages/gt_preferencesapp.cpp \
    preferences/pages/gt_preferenceslanguage.cpp \
    preferences/pages/gt_preferencessession.cpp \
    preferences/pages/gt_preferencesshortcuts.cpp \
    tools/gt_toolbarhandler.cpp \
    widgets/gt_sessionlistwidget.cpp \
    preferences/pages/gt_preferencesperspective.cpp \
    widgets/gt_perspectiveslistwidget.cpp \
    project/gt_projectwizard.cpp \
    project/pages/gt_projectwizardstart.cpp \
    project/pages/gt_projectwizardspec.cpp \
    project/pages/gt_projectwizardmodules.cpp \
    project/pages/gt_projectwizardimport.cpp \
    project/pages/gt_abstractprojectwizardpage.cpp \
    widgets/gt_perspectiveswitchwidget.cpp \
    preferences/pages/gt_preferencesaccess.cpp \
    dialogs/gt_aboutdialog.cpp \
    tools/gt_accessgroupmodel.cpp \
    dialogs/gt_accessdatadialog.cpp \
    widgets/gt_splashscreen.cpp \
    widgets/gt_startuppage.cpp \
    dialogs/gt_checkforupdatesdialog.cpp \
    widgets/gt_cornerwidget.cpp \
    widgets/gt_simpleloadingwidget.cpp \
    preferences/pages/gt_preferencespathsettings.cpp \
    models/gt_processqueuemodel.cpp \
    widgets/gt_processqueuewidget.cpp \
    dialogs/gt_environmentdialog.cpp \
    mdi_items/session_viewer/gt_sessionviewer.cpp \
    mdi_items/state_viewer/gt_stateviewer.cpp \
    mdi_items/state_viewer/gt_statemodel.cpp \
    mdi_items/memento_viewer/gt_mementoviewer.cpp \
    mdi_items/memento_viewer/gt_mementoeditor.cpp \
    mdi_items/examples/gt_examplesmdiwidget.cpp \
    mdi_items/examples/gt_examplesentry.cpp \
    mdi_items/examples/gt_examplegraphicalitem.cpp \
    mdi_items/examples/gt_examplestabwidget.cpp \
    mdi_items/label_usages/gt_labelusageviewer.cpp \
    mdi_items/label_usages/gt_labelcontainer.cpp \
    mdi_items/label_usages/gt_labelusagemodel.cpp \
    mdi_items/label_usages/gt_labelusagedelegate.cpp \
    mdi_items/process_env/gt_processeditor.cpp \
    mdi_items/process_env/entities/gt_taskentity.cpp \
    mdi_items/process_env/entities/gt_calculatorentity.cpp \
    mdi_items/process_env/entities/gt_calculatorpropertyentity.cpp \
    mdi_items/process_env/entities/gt_propertyportentity.cpp \
    mdi_items/process_env/entities/gt_taskarrowentity.cpp \
    mdi_items/process_env/entities/gt_taskentityshadow.cpp \
    mdi_items/process_env/entities/gt_datamodelportentity.cpp \
    mdi_items/process_env/gt_processeditorview.cpp \
    mdi_items/process_env/entities/gt_addtaskglobalbutton.cpp \
    mdi_items/process_env/entities/gt_taskconnectionentity.cpp \
    mdi_items/process_env/entities/gt_taskpipeentity.cpp \
    mdi_items/process_env/entities/gt_tasklinkentity.cpp \
    mdi_items/process_env/entities/gt_taskarrowlabelentity.cpp \
    mdi_items/process_env/gt_processeditorscene.cpp \
    mdi_items/process_env/gt_processconnectionmodel.cpp \
    mdi_items/process_env/entities/gt_processconnectionview.cpp \
    mdi_items/process_env/gt_processconnectiongraphicsview.cpp \
    mdi_items/process_env/entities/gt_processpropertyportentity.cpp \
    mdi_items/process_env/gt_processconnectionitem.cpp \
    mdi_items/process_env/entities/gt_processpropertyconnectionentity.cpp \
    mdi_items/process_env/gt_processconnectionscene.cpp \
    dock_widgets/output/gt_outputdock.cpp \
    dock_widgets/output/gt_styledlogmodel.cpp \
    dock_widgets/output/gt_filteredlogmodel.cpp \
    dock_widgets/output/gt_outputtester.cpp \
    dock_widgets/output/gt_taskhistorymodel.cpp \
    dock_widgets/explorer/gt_explorerdock.cpp \
    dock_widgets/explorer/gt_explorerview.cpp \
    dock_widgets/explorer/gt_openwithmenu.cpp \
    dock_widgets/process/gt_processdock.cpp \
    dock_widgets/process/gt_processview.cpp \
    dock_widgets/process/gt_processconnectioneditor.cpp \
    dock_widgets/process/gt_processcomponentsettingsbutton.cpp \
    dock_widgets/calculators/gt_calculatorsdock.cpp \
    dock_widgets/post/gt_postdelegate.cpp \
    dock_widgets/post/gt_postdock.cpp \
    dock_widgets/properties/gt_propertiesdock.cpp \
    dock_widgets/labels/gt_labelsdock.cpp \
    dock_widgets/labels/gt_labeldelegate.cpp


LIBS += -L$${BUILD_DEST}

# Use of pre compiled logging header to reduce compile time
PRECOMPILED_HEADER = $${GTLAB_LOGGING_PATH}/include/logging/gt_logging.h

CONFIG(debug, debug|release){
    # UTILITIES
    LIBS += -lGTlabLogging-d
    # CORE
    LIBS += -lGTlabDataProcessor-d -lGTlabCore-d
    LIBS += -lGTlabGui-d
    # THIRD PARTY
    win32 {
    }
    unix {
        contains(USE_HDF5, true) {
            LIBS += -lhdf5 -lhdf5_cpp
            LIBS += -lGenH5-d
        }
    }
} else {
    # UTILITIES
    LIBS += -lGTlabLogging
    # CORE
    LIBS += -lGTlabDataProcessor -lGTlabCore
    LIBS += -lGTlabGui
    # THIRD PARTY
    unix {
        contains(USE_HDF5, true) {
            LIBS += -lhdf5 -lhdf5_cpp
            LIBS += -lGenH5
        }
    }
}



# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/modules\''

######################################################################
