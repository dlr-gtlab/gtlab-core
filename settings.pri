#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

#### LOCAL SETTINGS
include( local_settings.pri )
#include( src/batch-script/python.prf )

#### CODE COVERAGE ####
CONFIG(debug, debug|release) {
    contains(ENABLE_COVERAGE, true) {
        CONFIG += COVERAGE
    }
}
TARGET_DIR_NAME = core
LIB_BUILD_DEST = ../../lib/$${TARGET_DIR_NAME}

#### THIRD PARTY LIBRARIES
# Python
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/Python/Python_373/include
#LIBS += $${DEV_TOOLS}/ThirdPartyLibraries/Python/Python_373/libs/python37.lib
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/Python/Python_373
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/Python/Python_373
# PythonQt
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/PythonQt/PythonQt_373/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/PythonQt/PythonQt_373/lib
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/PythonQt/PythonQt_373/lib
# Exprtk
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/Exprtk/include
# minpack
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/minpack/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/minpack/lib
## Qwt
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/Qwt/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/Qwt/lib
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/Qwt/lib
win32 {
    CONFIG(debug, debug|release){
        LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/Qwt/libDebug
        DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/Qwt/libDebug
    } else {
        LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/Qwt/lib
        DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/Qwt/lib
    }
}
unix {
    LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/Qwt/lib
    DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/Qwt/lib
}
## RCERemote
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/RCERemote/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/RCERemote/lib
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/RCERemote/lib
#### NLOPT
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/NLopt/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/NLopt/lib
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/NLopt/lib
# OpenCascade
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/OpenCascade/include/oce
win32 {
    CONFIG(debug, debug|release){
        LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/OpenCascade/Win64/libDebug
    } else {
        LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/OpenCascade/Win64/lib
    }
}
unix {
    COVERAGE {
       LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/OpenCascade-cov/lib
    }
    else {
       LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/OpenCascade/lib
    }
}
# CERES
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/include
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/include/config
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/include/eigen
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/include/miniglog
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/ceres/lib
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/lib
# Blade Gen Interface
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/include
win32 {
    LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/lib
    DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/lib
}
unix {
    COVERAGE {
       LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface-cov/lib
       DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface-cov/lib
    }
    else {
       LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/lib
       DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/lib
    }
}

# Google Test
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/include
win32 {
    CONFIG(debug, debug|release){
        LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/libDebug
        DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/libDebug
    } else {
        LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/lib
        DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/lib
    }
}
unix {
    LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/lib
    DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/lib
}

#### OPTYMAL
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/Optymal/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/Optymal/lib
DEPENDPATH  += -L$${DEV_TOOLS}/ThirdPartyLibraries/Optymal/lib

#### PATH SETTINGS
BUILD_DEST = ../../lib/core

COVERAGE {
    # code coverage settings
    unix: QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    unix: LIBS += -lgcov
}


# 17.04.2019: path to perf standalone
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/DLRperformance/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/DLRperformance/lib
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/DLRperformance/lib

INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/LibXML/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/LibXML/lib
DEPENDPATH  += -L$${DEV_TOOLS}/ThirdPartyLibraries/LibXML/lib

INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/SplineLib/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/SplineLib/lib
DEPENDPATH  += -L$${DEV_TOOLS}/ThirdPartyLibraries/SplineLib/lib


# Numerics, Logging and Physics from DEV-Tools
INCLUDEPATH += $${DEV_TOOLS}/include/logging
LIBS        += -L$${DEV_TOOLS}/lib/logging
DEPENDPATH  += -L$${DEV_TOOLS}/lib/logging

INCLUDEPATH += $${DEV_TOOLS}/include/numerics
LIBS        += -L$${DEV_TOOLS}/lib/numerics
DEPENDPATH  += -L$${DEV_TOOLS}/lib/numerics

INCLUDEPATH += $${DEV_TOOLS}/include/physics
LIBS        += -L$${DEV_TOOLS}/lib/physics
DEPENDPATH  += -L$${DEV_TOOLS}/lib/physics

######################################################################

## FUNCTION DEFINITION FOR COPY FUNCTION
mkpath($${PWD}/include/$${TARGET_DIR_NAME})

defineTest(copyHeaders) {

    files = $$1

    dir = $${PWD}/../../include/$${TARGET_DIR_NAME}
    win32:dir ~= s,/,\\,g

    win32 {

        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote(*.h) $$shell_quote($$dir) $$escape_expand(\\n\\t)

        dirNames =

        for(file, files) {

            exists($$file) {

                dirName = $$dirname(file)

                !isEmpty(dirName) {

                    !contains(dirNames, $$dirName) {

                        dirNames += $$dirName
                        sourceDir = $${PWD}/$${dirName}/*.h

                        win32:sourceDir ~= s,/,\\,g

                        exists($${sourceDir}) {

                            QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($${sourceDir}) $$shell_quote($$dir) $$escape_expand(\\n\\t)
                        }
                    }
                }
            }
        }

    }
    unix {
        QMAKE_POST_LINK += find . -name $$shell_quote(*.h) -exec cp $$shell_quote({}) $$shell_quote($$dir) \; $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)

    return(true)
}

defineTest(copyHeaders2) {

    files = $$1

    dir = $${PWD}/../../../include/$${TARGET_DIR_NAME}
    win32:dir ~= s,/,\\,g

    win32 {

        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote(*.h) $$shell_quote($$dir) $$escape_expand(\\n\\t)

        dirNames =

        for(file, files) {

            exists($$file) {

                dirName = $$dirname(file)

                !isEmpty(dirName) {

                    !contains(dirNames, $$dirName) {

                        dirNames += $$dirName
                        sourceDir = $${PWD}/$${dirName}/*.h

                        win32:sourceDir ~= s,/,\\,g

                        exists($${sourceDir}) {

                            QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($${sourceDir}) $$shell_quote($$dir) $$escape_expand(\\n\\t)
                        }
                    }
                }
            }
        }

    }
    unix {
        QMAKE_POST_LINK += find . -name $$shell_quote(*.h) -exec cp $$shell_quote({}) $$shell_quote($$dir) \; $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)

    return(true)
}

defineTest(copyToEnvironmentPath) {

    !isEmpty(GTLAB_ENVIRONMENT_PATH) {

        dllPath = $$1
        win32:dllPath ~= s,/,\\,g

        args = $$ARGS

        count(args, 2) {

            environmentPath = $$2
        } else {

            environmentPath = $${GTLAB_ENVIRONMENT_PATH}
        }

        win32:environmentPath ~= s,/,\\,g

        exists($$environmentPath) {

            win32: QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$dllPath) $$shell_quote($$environmentPath) $$escape_expand(\\n\\t)
            unix:  QMAKE_POST_LINK += find $$LIB_BUILD_DEST -name $$shell_quote(*.so*) -exec cp $$shell_quote({}) $$shell_quote($$environmentPath) \; $$escape_expand(\\n\\t)

            export(QMAKE_POST_LINK)

            return(true)
        } else {

            warning(GTLAB_ENVIRONMENT_PATH ($${environmentPath}) does not exist!)
        }
    }

    return(false)
}

defineTest(copyExecutableToEnvironmentPath) {
    !isEmpty(GTLAB_ENVIRONMENT_PATH) {
        exePath = $$1
        win32:exePath ~= s,/,\\,g

        exeName = $$2
        args = $$ARGS

        environmentPath = $${GTLAB_ENVIRONMENT_PATH}

        win32:environmentPath ~= s,/,\\,g

        exists($$environmentPath) {

            win32: QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$exePath) $$shell_quote($$environmentPath) $$escape_expand(\\n\\t)
            unix:  QMAKE_POST_LINK += find $$exePath -name $$shell_quote($$exeName) -exec cp $$shell_quote({}) $$shell_quote($$environmentPath) \; $$escape_expand(\\n\\t)

            export(QMAKE_POST_LINK)

            return(true)
        } else {

            warning(GTLAB_ENVIRONMENT_PATH ($${environmentPath}) does not exist!)
        }
    }
    return(false)
}
######################################################################
