#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( $${PWD}/../../settings.pri )

CONFIG(debug, debug|release){
    TARGET = GTlabGui-d
} else {
    TARGET = GTlabGui
}

QT += core widgets network xml
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++11

DEFINES += GT_GUI_DLL

QTPLUGIN += gif

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-gui
    OBJECTS_DIR = $${BUILD_DEST}/debug-gui/obj
    MOC_DIR = $${BUILD_DEST}/debug-gui/moc
    RCC_DIR = $${BUILD_DEST}/debug-gui/rcc
    UI_DIR = $${BUILD_DEST}/debug-gui/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-gui
    OBJECTS_DIR = $${BUILD_DEST}/release-gui/obj
    MOC_DIR = $${BUILD_DEST}/release-gui/moc
    RCC_DIR = $${BUILD_DEST}/release-gui/rcc
    UI_DIR = $${BUILD_DEST}/release-gui/ui
}
INCLUDEPATH += .\
    widgets \
    preferences \
    preferences/pages \
    project \
    project/pages \
    dialogs \
    tools \
    models \
    ../datamodel \
    ../calculators \
    ../core \
    ../core/settings \
    ../core/provider \
    ../mdi \
    ../mdi/tools \
    ../mdi/object_ui \
    ../mdi/mdi_items/memento_viewer \
    ../utilities/logging \
    ../datamodel/property \
    ../network \
    ../mdi/mdi_items/collection_env \

DESTDIR = $${BUILD_DEST}

HEADERS += \
    dialogs/gt_refusedpluginsdialog.h \
    gt_mainwin.h \
    gt_gui_exports.h \
    preferences/gt_preferencesdialog.h \
    preferences/pages/gt_preferencespage.h \
    preferences/pages/gt_preferencesapp.h \
    preferences/pages/gt_preferencesplugins.h \
    preferences/pages/gt_preferenceslanguage.h \
    preferences/pages/gt_preferencessession.h \
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
    dialogs/gt_changelogdialog.h \
    tools/gt_accessgroupmodel.h \
    dialogs/gt_accessdatadialog.h \
    widgets/gt_startuppage.h \
    dialogs/gt_checkforupdatesdialog.h \
    widgets/gt_cornerwidget.h \
    widgets/gt_simpleloadingwidget.h \
    preferences/pages/gt_preferencespathsettings.h \
    models/gt_processqueuemodel.h \
    widgets/gt_processqueuewidget.h

FORMS += ui/gt_mainwin.ui

SOURCES += \
    dialogs/gt_refusedpluginsdialog.cpp \
    gt_mainwin.cpp \
    preferences/gt_preferencesdialog.cpp \
    preferences/pages/gt_preferencespage.cpp \
    preferences/pages/gt_preferencesapp.cpp \
    preferences/pages/gt_preferencesplugins.cpp \
    preferences/pages/gt_preferenceslanguage.cpp \
    preferences/pages/gt_preferencessession.cpp \
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
    dialogs/gt_changelogdialog.cpp \
    tools/gt_accessgroupmodel.cpp \
    dialogs/gt_accessdatadialog.cpp \
    widgets/gt_startuppage.cpp \
    dialogs/gt_checkforupdatesdialog.cpp \
    widgets/gt_cornerwidget.cpp \
    widgets/gt_simpleloadingwidget.cpp \
    preferences/pages/gt_preferencespathsettings.cpp \
    models/gt_processqueuemodel.cpp \
    widgets/gt_processqueuewidget.cpp

RESOURCES += ../resources/icons/icons.qrc \
    ../resources/pixmaps/pixmaps.qrc

LIBS += -L$${BUILD_DEST}

CONFIG(debug, debug|release){
    LIBS += -lGTlabNumerics-d -lGTlabLogging-d -lGTlabDatamodel-d -lGTlabCalculators-d -lGTlabNetwork-d -lGTlabCore-d -lqwtd -lGTlabMdi-d
} else {
    LIBS += -lGTlabNumerics -lGTlabLogging -lGTlabDatamodel -lGTlabCalculators -lGTlabNetwork -lGTlabCore -lqwt -lGTlabMdi
}

######################################################################
