#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/
######################################################################
#### DO NOT CHANGE!
######################################################################

GT_MODULE_ID="Test Module Interface"

include($${PWD}/../../../settings.pri)

BUILD_DEST = $${BUILD_DEST}/modules

CONFIG(debug, debug|release){
    TARGET = TestModuleInterface-d
} else {
    TARGET = TestModuleInterface
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

# Core
INCLUDEPATH += ../../../include/core
LIBS        += -L../../../lib/core
DEPENDPATH  += ../../../lib/core

DESTDIR = $${BUILD_DEST}

# Use of pre compiled logging header to reduce compile time
PRECOMPILED_HEADER = $${GTLAB_LOGGING_PATH}/include/logging/gt_logging.h

HEADERS += \
    test_module_interface.h \
    test_preferencepage.h


SOURCES += \
    test_module_interface.cpp  \
    test_preferencepage.cpp

CONFIG(debug, debug|release){
    # GTLAB CORE
    LIBS += -lGTlabLogging-d
    LIBS += -lGTlabGui-d -lGTlabCore-d -lGTlabDataProcessor-d
} else {
    # GTLAB CORE
    LIBS += -lGTlabLogging
    LIBS += -lGTlabGui -lGTlabCore -lGTlabDataProcessor
}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/..\''
