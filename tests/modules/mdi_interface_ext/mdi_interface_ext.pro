#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/
######################################################################
#### DO NOT CHANGE BELOW !
######################################################################

GT_MODULE_ID="Test Mdi Interface Ext"

include($${PWD}/../../../settings.pri)

BUILD_DEST = $${BUILD_DEST}/modules

CONFIG(debug, debug|release){
    TARGET = TestMdiInterfaceExt-d
} else {
    TARGET = TestMdiInterfaceExt
}

QT += core widgets xml svg

TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++14

isEmpty(GT_MODULE_ID) {
   error("GT_MODULE_ID undefined. Please define variable GT_MODULE_ID=\"My Module ID\" in project file.")
}

INCLUDEPATH += . \
    data \
    mdi \
    ../mdi_interface \
    ../mdi_interface/data

# Core
INCLUDEPATH += ../../../include/core
LIBS        += -L../../../lib/core
DEPENDPATH  += ../../../lib/core

LIBS        += -L$${BUILD_DEST}
DEPENDPATH  += $${BUILD_DEST}

DESTDIR = $${BUILD_DEST}

# Use of pre compiled logging header to reduce compile time
PRECOMPILED_HEADER = $${GTLAB_LOGGING_PATH}/include/logging/gt_logging.h

HEADERS += \
    mdi/test_mdi_ext_packageui.h \
    data/test_mdi_ext_package.h \
    mdi/test_mdi_ext_viewer.h \
    test_mdi_interface_ext.h


SOURCES += \
    mdi/test_mdi_ext_packageui.cpp \
    data/test_mdi_ext_package.cpp \
    mdi/test_mdi_ext_viewer.cpp \
    test_mdi_interface_ext.cpp \

CONFIG(debug, debug|release){
    # GTLAB CORE
    LIBS += -lGTlabLogging-d
    LIBS += -lGTlabCore-d -lGTlabDataProcessor-d -lGTlabGui-d
    LIBS += -lTestMdiInterface-d
} else {
    # GTLAB CORE
    LIBS += -lGTlabLogging
    LIBS += -lGTlabCore -lGTlabDataProcessor -lGTlabGui
    LIBS += -lTestMdiInterface
}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/..\''
