#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( $${PWD}/../../settings.pri )

TEMPLATE = app

QT += xml core sql widgets

TARGET = GTlabUnitTest

BUILD_DEST_TEMP = $${BUILD_DEST}/../build

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST_TEMP}/debug-unittests
    OBJECTS_DIR = $${BUILD_DEST_TEMP}/debug-unittests/obj
    MOC_DIR = $${BUILD_DEST_TEMP}/debug-unittests/moc
    RCC_DIR = $${BUILD_DEST_TEMP}/debug-unittests/rcc
    UI_DIR = $${BUILD_DEST_TEMP}/debug-unittests/ui
} else {
    DESTDIR = $${BUILD_DEST_TEMP}/release-unittests
    OBJECTS_DIR = $${BUILD_DEST_TEMP}/release-unittests/obj
    MOC_DIR = $${BUILD_DEST_TEMP}/release-unittests/moc
    RCC_DIR = $${BUILD_DEST_TEMP}/release-unittests/rcc
    UI_DIR = $${BUILD_DEST_TEMP}/release-unittests/ui
}

CONFIG += silent
CONFIG += c++11
CONFIG += console

#CURRENT_DIR=$$project_src_dir/tests/unittests

INCLUDEPATH += .\
    ../../src/datamodel \
    ../../src/datamodel/property \
    ../../src/calculators \
    ../../src/core \
    ../../src/core/provider \
    ../../src/core/settings \
    ../../src/mdi \
    ../../src/mdi/tools \
    ../../src/mdi/dialogs \
    ../../src/mdi/dock_widgets \
    ../../src/mdi/dock_widgets/process \
    ../../src/gui \
    ../../src/gui/dialogs

#### HEADERS
HEADERS += \
    gt_testhelper.h
HEADERS += datamodel/*.h
HEADERS += mdi/*.h

#### SOURCES
SOURCES += \
    main.cpp \
    gt_testhelper.cpp
SOURCES += datamodel/*.cpp
SOURCES += calculators/*.cpp
SOURCES += core/*.cpp
SOURCES += mdi/*.cpp

#SOURCES += $${CURRENT_DIR}/mdi/*.cpp

#### THIRD PARTY LIBRARIES
equals(QT_MAJOR_VERSION, 5):!lessThan(QT_MINOR_VERSION, 12) {
    message(Qt Version 5.12 or newer)

        # Google Test
        INCLUDEPATH += $${GOOGLE_TEST_PATH}/include
        LIBS        += -L$${GOOGLE_TEST_PATH}/lib
        DEPENDPATH  += $${GOOGLE_TEST_PATH}/lib

} else {
    message(Qt Version older than 5.12)
        # Google Test
        INCLUDEPATH += $${GOOGLE_TEST_PATH}/include
        win32 {
                CONFIG(debug, debug|release){
                        LIBS        += -L$${GOOGLE_TEST_PATH}/libDebug
                        DEPENDPATH  += $${GOOGLE_TEST_PATH}/libDebug
                } else {
                        LIBS        += -L$${GOOGLE_TEST_PATH}/lib
                        DEPENDPATH  += $${GOOGLE_TEST_PATH}/lib
                }
        }
        unix {
                LIBS        += -L$${GOOGLE_TEST_PATH}/lib
                DEPENDPATH  += $${GOOGLE_TEST_PATH}/lib
        }

}

LIBS += -L$${BUILD_DEST_TEMP}


CONFIG(debug, debug|release){
    #Utilities
    LIBS += -lGTlabLogging-d -lGTlabNumerics-d
    # Core
    LIBS += -lGTlabMdi-d -lGTlabDatamodel-d -lGTlabNetwork-d
    LIBS += -lGTlabCalculators-d -lGTlabCore-d
} else {
    #Utilities
    LIBS += -lGTlabLogging -lGTlabNumerics
   # Core
    LIBS += -lGTlabMdi -lGTlabDatamodel -lGTlabNetwork
    LIBS += -lGTlabCalculators -lGTlabCore
}

# third party
equals(QT_MAJOR_VERSION, 5):!lessThan(QT_MINOR_VERSION, 12) {
    message(Qt Version 5.12 or newer)

        CONFIG(debug, debug|release){
                LIBS += -lgtestd
        } else {
                LIBS += -lgtest
        }

} else {
    message(Qt Version older than 5.12)

        LIBS += -lgtest
}


RESOURCES += resources/unittests-resources.qrc

####################################################

DESTDIR = $${BUILD_DEST_TEMP}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/modules\''

######################################################################