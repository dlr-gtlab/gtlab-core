#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( $${PWD}/../../settings.pri )

win32 {
    RC_FILE = app.rc
}

TARGET = GTlab

QT += widgets
TEMPLATE = app
CONFIG += silent
CONFIG += c++11

#DEFINES += GT_LICENCE
#CONFIG += GT_LICENCE

#DEFINES += GT_MODELTEST
#CONFIG += GT_MODELTEST

GT_MODELTEST {
QT += testlib
}

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-app
    OBJECTS_DIR = $${BUILD_DEST}/debug-app/obj
    MOC_DIR = $${BUILD_DEST}/debug-app/moc
    RCC_DIR = $${BUILD_DEST}/debug-app/rcc
    UI_DIR = $${BUILD_DEST}/debug-app/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-app
    OBJECTS_DIR = $${BUILD_DEST}/release-app/obj
    MOC_DIR = $${BUILD_DEST}/release-app/moc
    RCC_DIR = $${BUILD_DEST}/release-app/rcc
    UI_DIR = $${BUILD_DEST}/release-app/ui
}
INCLUDEPATH += .\
    ../utilities/logging \
    ../datamodel \
    ../calculators \
    ../core \
    ../core/settings \
    ../mdi \
    ../mdi/tools \
    ../mdi/dialogs \
    ../gui \
    ../gui/dialogs

DESTDIR = $${BUILD_DEST}

HEADERS += \

SOURCES += \
    app.cpp

LIBS += -L$${BUILD_DEST}

CONFIG(debug, debug|release){
LIBS += -lGTlabNumerics-d -lGTlabLogging-d -lGTlabDatamodel-d -lGTlabCalculators-d -lGTlabCore-d -lqwtd -lGTlabMdi-d -lGTlabGui-d -lGTlabNetwork-d
} else {
LIBS += -lGTlabNumerics -lGTlabLogging -lGTlabDatamodel -lGTlabCalculators -lGTlabCore -lqwt -lGTlabMdi -lGTlabGui -lGTlabNetwork
}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/modules\''

######################################################################
