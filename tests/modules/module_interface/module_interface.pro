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
    TARGET = testModuleInterface-d
} else {
    TARGET = testModuleInterface
}

QT += core widgets xml svg

TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++11

INCLUDEPATH += . \

# Core
INCLUDEPATH += ../../../include/core
LIBS        += -L../../..//lib/core
DEPENDPATH  += ../../../lib/core

DESTDIR = $${BUILD_DEST}

HEADERS += \
    test_module_interface.h


SOURCES += \
    test_module_interface.cpp 

CONFIG(debug, debug|release){
    # GTLAB CORE
    LIBS += -lGTlabLogging-d -lGTlabDatamodel-d -lGTlabNumerics-d -lGTlabCalculators-d
    LIBS += -lGTlabCore-d -lGTlabMdi-d -lGTlabNetwork-d
} else {
    # GTLAB CORE
    LIBS += -lGTlabLogging -lGTlabDatamodel -lGTlabNumerics -lGTlabCalculators
    LIBS += -lGTlabCore -lGTlabMdi -lGTlabNetwork
}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/..\''
