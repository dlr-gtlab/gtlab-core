#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

GT_MODULE_ID = "Test Process Interface"

######################################################################
#### DO NOT CHANGE BELOW !
######################################################################

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

DEFINES += GT_MODULE_ID='"\\\"$${GT_MODULE_ID}\\\""'

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

HEADERS += \
    calculator/test_calculator.h \
    test_process_interface.h


SOURCES += \
    calculator/test_calculator.cpp \
    test_process_interface.cpp

CONFIG(debug, debug|release){
    # GTLAB CORE
    LIBS += -lGTlabLogging-d -lGTlabNumerics-d
    LIBS += -lGTlabCore-d -lGTlabDataProcessor-d
    LIBS += -lTestDatamodelInterface-d
} else {
    # GTLAB CORE
    LIBS += -lGTlabLogging -lGTlabNumerics
    LIBS += -lGTlabCore -lGTlabDataProcessor
    LIBS += -lTestDatamodelInterface
}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/..\''
