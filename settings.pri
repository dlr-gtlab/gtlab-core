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

#### DEPLOYMENT ####
contains(BUILD_DEPLOY, true) {

    TARGET_DIR_NAME = core
    GTLAB_ENVIRONMENT_PATH = $${PWD}/lib/$${TARGET_DIR_NAME}
    mkpath($${GTLAB_ENVIRONMENT_PATH})

    dir = $${GTLAB_ENVIRONMENT_PATH}

    win32 {
        dir ~= s,/,\\,g

        QMAKE_POST_LINK += if not exist $$shell_quote($$dir) $$QMAKE_MKDIR $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    unix {
        QMAKE_POST_LINK += $$QMAKE_CHK_DIR_EXISTS $$shell_quote($$dir) || $$QMAKE_MKDIR $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    include( deployment.pri )
}

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
INCLUDEPATH += $${MINPACK_PATH}/include
LIBS        += -L$${MINPACK_PATH}/lib
DEPENDPATH  += $${MINPACK_PATH}/lib

## Qwt
INCLUDEPATH += $${QWT_PATH}/include
LIBS        += -L$${QWT_PATH}/lib
DEPENDPATH  += $${QWT_PATH}/lib

## NLOPT
INCLUDEPATH += $${NLOPT_PATH}/include
LIBS        += -L$${NLOPT_PATH}/lib
DEPENDPATH  += $${NLOPT_PATH}/lib

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
BUILD_DEST = ../../build

#COVERAGE {
    # code coverage settings
#    unix: QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
#    unix: LIBS += -lgcov
#}

######################################################################
