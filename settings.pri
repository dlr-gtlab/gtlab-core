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

#### LIBRARIES
# Logging
INCLUDEPATH += $${DEV_TOOLS}/include/logging
LIBS        += -L$${DEV_TOOLS}/lib/logging
DEPENDPATH  += $${DEV_TOOLS}/lib/logging

# Numerics
INCLUDEPATH += $${DEV_TOOLS}/include/numerics
LIBS        += -L$${DEV_TOOLS}/lib/numerics
DEPENDPATH  += $${DEV_TOOLS}/lib/numerics

#### THIRD PARTY LIBRARIES
# SplineLib
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/SplineLib/include
LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/SplineLib/lib
DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/SplineLib/lib

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
BUILD_DEST = ../../build

#COVERAGE {
    # code coverage settings
#    unix: QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
#    unix: LIBS += -lgcov
#}

######################################################################
