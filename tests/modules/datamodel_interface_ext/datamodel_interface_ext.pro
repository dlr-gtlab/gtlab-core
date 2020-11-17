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
    TARGET = testDatamodelInterfaceExt-d
} else {
    TARGET = testDatamodelInterfaceExt
}

QT += core widgets xml svg

TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++11

INCLUDEPATH += . \
    ../datamodel_interface

# Core
INCLUDEPATH += ../../../include/core
LIBS        += -L../../..//lib/core
DEPENDPATH  += ../../../lib/core

LIBS        += -L$${BUILD_DEST}
DEPENDPATH  += $${BUILD_DEST}

DESTDIR = $${BUILD_DEST}

HEADERS += \
    test_datamodel_interface_ext.h  \
    test_dmi_ext_package.h \
    test_dmi_ext_packageui.h


SOURCES += \
    test_datamodel_interface_ext.cpp  \
    test_dmi_ext_package.cpp \
    test_dmi_ext_packageui.cpp

CONFIG(debug, debug|release){
    # GTLAB CORE
    LIBS += -lGTlabLogging-d -lGTlabDatamodel-d -lGTlabNumerics-d -lGTlabCalculators-d
    LIBS += -lGTlabCore-d -lGTlabMdi-d -lGTlabNetwork-d
    LIBS += -ltestDatamodelInterface-d
} else {
    # GTLAB CORE
    LIBS += -lGTlabLogging -lGTlabDatamodel -lGTlabNumerics -lGTlabCalculators
    LIBS += -lGTlabCore -lGTlabMdi -lGTlabNetwork
    LIBS += -ltestDatamodelInterface
}
