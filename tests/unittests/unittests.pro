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
CONFIG += c++14
CONFIG += console

#CURRENT_DIR=$$project_src_dir/tests/unittests

INCLUDEPATH += .\
    ../../src/dataprocessor \
    ../../src/dataprocessor/property \
    ../../src/core \
    ../../src/core/provider \
    ../../src/core/settings \
    ../../src/core/process_management \
    ../../src/gui \
    ../../src/gui/tools \
    ../../src/gui/dialogs \
    ../../src/gui/dock_widgets \
    ../../src/gui/dock_widgets/process \
    ../../src/gui/dock_widgets/process \
    ../../src/gui/exporter \
    ../../src/gui/exporter/dataZoneExporter

INCLUDEPATH += .\
    ./datamodel \
    ./core

#### HEADERS
HEADERS += gt_testhelper.h
HEADERS += datamodel/*.h
HEADERS += core/*.h

#### SOURCES
SOURCES += \
    main.cpp \
    gt_testhelper.cpp
SOURCES += datamodel/*.cpp
SOURCES += calculators/*.cpp
SOURCES += core/*.cpp
SOURCES += exporter/*.cpp
#SOURCES += mdi/*.cpp

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
    # Utilities
    LIBS += -lGTlabLogging-d -lGTlabNumerics-d
    # Core
    LIBS += -lGTlabDataProcessor-d -lGTlabCore-d
    # GUI
    LIBS += -lGTlabGui-d

    # THIRD PARTY
    win32 {

    }
    unix {
        LIBS += -lcminpack
        contains(USE_HDF5, true) {
            LIBS += -lhdf5 -lhdf5_cpp
            LIBS += -lGTlabH5-d
        }
    }
} else {
    # Utilities
    LIBS += -lGTlabLogging -lGTlabNumerics
    # Core
    LIBS += -lGTlabDataProcessor -lGTlabCore
    # GUI
    LIBS += -lGTlabGui

    # THIRD PARTY
    unix {
        LIBS += -lcminpack
        contains(USE_HDF5, true) {
            LIBS += -lhdf5 -lhdf5_cpp
            LIBS += -lGTlabH5
        }
    }
}

# third party
equals(QT_MAJOR_VERSION, 5):!lessThan(QT_MINOR_VERSION, 12) {
    message(Qt Version 5.12 or newer)

        CONFIG(debug, debug|release){
        win32 {
                LIBS += -lgtestd
        }
        unix {
                LIBS += -lgtest
        }
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
