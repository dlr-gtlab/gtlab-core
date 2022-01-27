#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include($${PWD}/../../../settings.pri)

BUILD_DEST = ../../../build/modules

CONFIG(debug, debug|release){
    TARGET = TestMdiInterface-d
} else {
    TARGET = TestMdiInterface
}

QT += core widgets xml svg

TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++11

DEFINES += TEST_MDI_INTERFACE_DLL

INCLUDEPATH += . \
    mdi \
    data

# Core
INCLUDEPATH += ../../../include/core
LIBS        += -L../../../lib/core
DEPENDPATH  += ../../../lib/core

DESTDIR = $${BUILD_DEST}

HEADERS += \
    data/test_mdi_data.h \
    data/test_mdi_package.h \
    mdi/test_mdi_dockwidget.h \
    mdi/test_mdi_packageui.h \
    mdi/test_mdi_viewer.h \
    test_mdi_interface_exports.h  \
    test_mdi_interface.h


SOURCES += \
    data/test_mdi_data.cpp \
    data/test_mdi_package.cpp \
    mdi/test_mdi_dockwidget.cpp \
    mdi/test_mdi_packageui.cpp \
    mdi/test_mdi_viewer.cpp \
    test_mdi_interface.cpp

CONFIG(debug, debug|release){
    # GTLAB CORE
    LIBS += -lGTlabLogging-d -lGTlabNumerics-d
    LIBS += -lGTlabCore-d -lGTlabDataProcessor-d -lGTlabGui-d
} else {
    # GTLAB CORE
    LIBS += -lGTlabLogging -lGTlabNumerics
    LIBS += -lGTlabCore -lGTlabDataProcessor -lGTlabGui
}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/..\''
