#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( ../../settings.pri )

win32 {
    RC_FILE = batch.rc
}

TARGET = GTlabConsole

QT += core xml gui widgets
TEMPLATE = app
CONFIG += console
CONFIG += silent
CONFIG += c++11

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-batch
    OBJECTS_DIR = $${BUILD_DEST}/debug-batch/obj
    MOC_DIR = $${BUILD_DEST}/debug-batch/moc
    RCC_DIR = $${BUILD_DEST}/debug-batch/rcc
    UI_DIR = $${BUILD_DEST}/debug-batch/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-batch
    OBJECTS_DIR = $${BUILD_DEST}/release-batch/obj
    MOC_DIR = $${BUILD_DEST}/release-batch/moc
    RCC_DIR = $${BUILD_DEST}/release-batch/rcc
    UI_DIR = $${BUILD_DEST}/release-batch/ui
}
INCLUDEPATH += .\
    ../datamodel \
    ../calculators \
    ../core \
    ../core/provider \
    ../mdi \
    ../datamodel/property

DESTDIR = $${BUILD_DEST}

HEADERS += \
    batchremote.h \

SOURCES += \
    batch.cpp \
    batchremote.cpp \

LIBS += -L$${BUILD_DEST}

CONFIG(debug, debug|release){
    # UTILITIES
    LIBS += -lGTlabNumerics-d -lGTlabLogging-d
    # CORE
    LIBS += -lGTlabDatamodel-d -lGTlabCalculators-d -lGTlabCore-d
    LIBS += -lGTlabMdi-d -lGTlabNetwork-d
    # THIRD PARTY
    win32 {
    }
    unix {
        LIBS += -lcminpack
    }
} else {
    # UTILITIES
    LIBS += -lGTlabNumerics -lGTlabLogging
    # CORE
    LIBS += -lGTlabDatamodel -lGTlabCalculators -lGTlabCore
    LIBS += -lGTlabMdi -lGTlabNetwork
    # THIRD PARTY
    unix {
        LIBS += -lcminpack
    }
}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/modules\''

######################################################################
