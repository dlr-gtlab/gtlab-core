#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/
######################################################################
#### DO NOT CHANGE BELOW !
######################################################################

GT_MODULE_ID="Test Process Interface"

include($${PWD}/../../../settings.pri)

BUILD_DEST = $${BUILD_DEST}/modules

CONFIG(debug, debug|release){
    TARGET = TestProcessInterface-d
} else {
    TARGET = TestProcessInterface
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
               ./calculator \
               ../datamodel_interface \
               ../datamodel_interface/data

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
    calculator/test_calculator.h \
    calculator/test_crashonexecutecalculator.h \
    calculator/test_processstatescalculator.h \
    calculator/test_sleepcalculator.h \
    test_process_interface.h


SOURCES += \
    calculator/test_calculator.cpp \
    calculator/test_crashonexecutecalculator.cpp \
    calculator/test_processstatescalculator.cpp \
    calculator/test_sleepcalculator.cpp \
    test_process_interface.cpp

CONFIG(debug, debug|release){
    # GTLAB CORE
    LIBS += -lGTlabLogging-d
    LIBS += -lGTlabCore-d -lGTlabDataProcessor-d
    LIBS += -lTestDatamodelInterface-d
} else {
    # GTLAB CORE
    LIBS += -lGTlabLogging
    LIBS += -lGTlabCore -lGTlabDataProcessor
    LIBS += -lTestDatamodelInterface
}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/..\''
