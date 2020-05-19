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
    TARGET = GTlabCore-d
} else {
    TARGET = GTlabCore
}

QT += core xml network
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent

DEFINES += GT_CORE_DLL

# qstring restrictions
DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_NO_URL_CAST_FROM_STRING

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-core
    OBJECTS_DIR = $${BUILD_DEST}/debug-core/obj
    MOC_DIR = $${BUILD_DEST}/debug-core/moc
    RCC_DIR = $${BUILD_DEST}/debug-core/rcc
    UI_DIR = $${BUILD_DEST}/debug-core/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-core
    OBJECTS_DIR = $${BUILD_DEST}/release-core/obj
    MOC_DIR = $${BUILD_DEST}/release-core/moc
    RCC_DIR = $${BUILD_DEST}/release-core/rcc
    UI_DIR = $${BUILD_DEST}/release-core/ui
}
INCLUDEPATH += .\
    settings \
    provider \
    ../datamodel \
    ../calculators \
    ../network \
    ../datamodel/property \
    ../utilities/numerics \
    ../utilities/logging \
    ../versioncontrol

DESTDIR = $${BUILD_DEST}

HEADERS += \
    gt_coreapplication.h \
    gt_core_exports.h \
    gt_footprint.h \
    gt_session.h \
    gt_project.h \
    settings/gt_settingsitem.h \
    settings/gt_abstractsettings.h \
    settings/gt_settings.h \
    gt_coreprocessexecutor.h \
    gt_explorermodel.h \
    provider/gt_projectprovider.h \
    gt_treefiltermodel.h \
    gt_checkablestringlistmodel.h \
    gt_coremoduleloader.h \
    gt_runnable.h \
    gt_objectfiltermodel.h \
    gt_projectdiffpatch.h \
    gt_coredatamodel.h \
    gt_loadprojecthelper.h \
    gt_updatechecker.h \
    gt_abstractloadinghelper.h \
    gt_command.h

SOURCES += \
    gt_coreapplication.cpp \
    gt_footprint.cpp \
    gt_session.cpp \
    gt_project.cpp \
    settings/gt_settingsitem.cpp \
    settings/gt_abstractsettings.cpp \
    settings/gt_settings.cpp \
    gt_coreprocessexecutor.cpp \
    gt_explorermodel.cpp \
    provider/gt_projectprovider.cpp \
    gt_treefiltermodel.cpp \
    gt_checkablestringlistmodel.cpp \
    gt_coremoduleloader.cpp \
    gt_runnable.cpp \
    gt_objectfiltermodel.cpp \
    gt_projectdiffpatch.cpp \
    gt_coredatamodel.cpp \
    gt_loadprojecthelper.cpp \
    gt_updatechecker.cpp \
    gt_abstractloadinghelper.cpp \
    gt_command.cpp

RESOURCES += ../resources/templates/templates.qrc

LIBS += -L$${BUILD_DEST}

CONFIG(debug, debug|release){
    LIBS += -lGTlabNumerics-d -lGTlabLogging-d -lGTlabDatamodel-d -lGTlabCalculators-d -lGTlabNetwork-d
} else {
    LIBS += -lGTlabNumerics -lGTlabLogging -lGTlabDatamodel -lGTlabCalculators -lGTlabNetwork
}

unix {
    QMAKE_CXXFLAGS += -std=c++11
}

######################################################################

contains(BUILD_DEPLOY, true) {

    copyHeaders($$HEADERS)
    copyToEnvironmentPath()
}

######################################################################
