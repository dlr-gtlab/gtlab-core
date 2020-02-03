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

#### CODE COVERAGE ####
CONFIG(debug, debug|release) {
    contains(ENABLE_COVERAGE, true) {
        CONFIG += COVERAGE
    }
}

TARGET_DIR_NAME = core
LIB_BUILD_DEST = ../../lib/$${TARGET_DIR_NAME}

#### LIBRARIES
# Logging
INCLUDEPATH += $${GTLAB_LOGGING_PATH}/include/logging
LIBS        += -L$${GTLAB_LOGGING_PATH}/lib/logging
DEPENDPATH  += $${GTLAB_LOGGING_PATH}/lib/logging

# Numerics
INCLUDEPATH += $${GTLAB_NUMERICS_PATH}/include/numerics
LIBS        += -L$${GTLAB_NUMERICS_PATH}/lib/numerics
DEPENDPATH  += $${GTLAB_NUMERICS_PATH}/lib/numerics

#### THIRD PARTY LIBRARIES
# SplineLib
INCLUDEPATH += $${SPLINE_LIB_PATH}/include
LIBS        += -L$${SPLINE_LIB_PATH}/lib
DEPENDPATH  += $${SPLINE_LIB_PATH}/lib

# minpack
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/minpack/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/minpack/lib
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/minpack/lib

## Qwt
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/Qwt/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/Qwt/lib
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/Qwt/lib

# Google Test
#INCLUDEPATH += C:/devel/googletest-master/googletest/include
#win32 {
#    CONFIG(debug, debug|release){
#        LIBS        += -LC:/devel/googletest-master/googletest/build/lib
#        DEPENDPATH  += C:/devel/googletest-master/googletest/build/lib
#    } else {
#        LIBS        += -LC:/devel/googletest-master/googletest/build/lib
#        DEPENDPATH  += C:/devel/googletest-master/googletest/build/lib
#    }
#}
#unix {
#    LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/lib
#    DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/lib
#}

#### PATH SETTINGS
BUILD_DEST = ../../lib/core

#COVERAGE {
    # code coverage settings
#    unix: QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
#    unix: LIBS += -lgcov
#}

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
