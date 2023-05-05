#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

GT_MODULE_ID="Unittests"

include( $${PWD}/../../settings.pri )

TEMPLATE = app

QT += xml core sql widgets network

TARGET = GTlabUnitTest

BUILD_DEST_TEMP = $${BUILD_DEST}

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
    ../../src/dataprocessor/thirdparty \
    ../../src/core \
    ../../src/core/network \
    ../../src/core/provider \
    ../../src/core/settings \
    ../../src/core/process_management \
    ../../src/core/network \
    ../../src/gui \
    ../../src/gui/tools \
    ../../src/gui/dialogs \
    ../../src/gui/dock_widgets \
    ../../src/gui/dock_widgets/process \
    ../../src/gui/dock_widgets/process \
    ../../src/gui/exporter \
    ../../src/gui/process_runner

INCLUDEPATH += .\
    ./datamodel \
    ./core

#### HEADERS
HEADERS += gt_testhelper.h
HEADERS += slotadaptor.h
HEADERS += $$files(datamodel/*.h)
HEADERS += $$files(core/*.h)
HEADERS += $$files(gui/*.h)

#### SOURCES
SOURCES += \
    main.cpp \
    gt_testhelper.cpp \
    slotadaptor.cpp

SOURCES += $$files(datamodel/*.cpp)
SOURCES += $$files(calculators/*.cpp)
SOURCES += $$files(core/*.cpp)
SOURCES += $$files(thirdparty/*.cpp)
SOURCES += $$files(exporter/*.cpp)
SOURCES += $$files(mdi/*.cpp)
#SOURCES += $$files(network/*.cpp)
SOURCES += $$files(gui/*.cpp)

#SOURCES += $${CURRENT_DIR}/mdi/*.cpp

RESOURCES += \
    datamodel/data/data.qrc

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

# Use of pre compiled logging header to reduce compile time
PRECOMPILED_HEADER = $${GTLAB_LOGGING_PATH}/include/logging/gt_logging.h

contains(USE_HDF5, true) {
    message("USE_HDF5 = true")
    DEFINES += GT_H5

    CONFIG(debug, debug|release) {
        win32: LIBS += -lhdf5_D -lhdf5_cpp_D
        unix: LIBS += -lhdf5 -lhdf5_cpp
        LIBS += -lGenH5-d
    } else {
        LIBS += -lhdf5 -lhdf5_cpp
        LIBS += -lGenH5
    }
}

CONFIG(debug, debug|release){
    # Utilities
    LIBS += -lGTlabLogging-d
    # Core
    LIBS += -lGTlabDataProcessor-d -lGTlabCore-d
    # GUI
    LIBS += -lGTlabGui-d
} else {
    # Utilities
    LIBS += -lGTlabLogging
    # Core
    LIBS += -lGTlabDataProcessor -lGTlabCore
    # GUI
    LIBS += -lGTlabGui
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



####################################################

DESTDIR = $${BUILD_DEST_TEMP}

# add search paths to shared libraries
unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/modules\''

######################################################################
