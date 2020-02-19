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
    TARGET = GTlabNetwork-d
} else {
    TARGET = GTlabNetwork
}

QT += core xml network
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent

DEFINES += GT_NETWORK_DLL

# qstring restrictions
DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_NO_URL_CAST_FROM_STRING

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-network
    OBJECTS_DIR = $${BUILD_DEST}/debug-network/obj
    MOC_DIR = $${BUILD_DEST}/debug-network/moc
    RCC_DIR = $${BUILD_DEST}/debug-network/rcc
    UI_DIR = $${BUILD_DEST}/debug-network/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-network
    OBJECTS_DIR = $${BUILD_DEST}/release-network/obj
    MOC_DIR = $${BUILD_DEST}/release-network/moc
    RCC_DIR = $${BUILD_DEST}/release-network/rcc
    UI_DIR = $${BUILD_DEST}/release-network/ui
}
INCLUDEPATH += .\
    ../datamodel \
    ../datamodel/property \
    ../utilities/logging

DESTDIR = $${BUILD_DEST}

HEADERS += \
    gt_network_exports.h \
    gt_accessdata.h \
    gt_accessmanager.h \
    gt_accessgroup.h \
    gt_networkinterface.h \
    gt_accessdataconnection.h \
    gt_abstractaccessdataconnection.h \
    gt_accessselectionproperty.h \
    gt_downloaditem.h \
    gt_downloader.h

SOURCES += \
    gt_accessdata.cpp \
    gt_accessmanager.cpp \
    gt_accessgroup.cpp \
    gt_accessdataconnection.cpp \
    gt_abstractaccessdataconnection.cpp \
    gt_accessselectionproperty.cpp \
    gt_downloaditem.cpp \
    gt_downloader.cpp

LIBS += -L$${BUILD_DEST}

CONFIG(debug, debug|release){
    LIBS += -lGTlabNumerics-d -lGTlabLogging-d -lGTlabDatamodel-d
} else {
    LIBS += -lGTlabNumerics -lGTlabLogging -lGTlabDatamodel
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
