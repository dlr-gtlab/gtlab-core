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

# Python
#INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/Python/Python_373/include
#LIBS += $${DEV_TOOLS}/ThirdPartyLibraries/Python/Python_373/libs/python37.lib
#LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/Python/Python_373
#DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/Python/Python_373
# PythonQt
#INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/PythonQt/PythonQt_373/include
#LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/PythonQt/PythonQt_373/lib
#DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/PythonQt/PythonQt_373/lib
# Exprtk
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/Exprtk/include
# minpack
INCLUDEPATH += C:/devel/cminpack-1.3.6
LIBS        += -LC:/devel/cminpack-1.3.6/build
## Qwt
INCLUDEPATH += C:/devel/qwt-6.1.4/src
LIBS        += -LC:/devel/qwt-6.1.4/lib
DEPENDPATH  += C:/devel/qwt-6.1.4/lib

## RCERemote
#INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/RCERemote/include
#LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/RCERemote/lib
#DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/RCERemote/lib
#### NLOPT
INCLUDEPATH += C:/devel/nlopt-2.6.1/build/src/api
LIBS        += -LC:/devel/nlopt-2.6.1/build
DEPENDPATH  += C:/devel/nlopt-2.6.1/build
# OpenCascade
INCLUDEPATH += C:/devel/oce.0.17.2.win64_msvc2015_static/include/oce
win32 {
    CONFIG(debug, debug|release){
        LIBS    += -LC:/devel/oce.0.17.2.win64_msvc2015_static/lib
    } else {
        LIBS    += -LC:/devel/oce.0.17.2.win64_msvc2015_static/lib
    }
}
#unix {
#    COVERAGE {
#       LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/OpenCascade-cov/lib
#    }
#    else {
#       LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/OpenCascade/lib
#    }
#}
# CERES
#INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/include
#INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/include/config
#INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/include/eigen
#INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/include/miniglog
#LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/ceres/lib
#DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/ceres/lib
# Blade Gen Interface
#INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/include
#win32 {
#    LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/lib
#    DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/lib
#}
#unix {
#    COVERAGE {
#       LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface-cov/lib
#       DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface-cov/lib
#    }
#    else {
#       LIBS    += -L$${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/lib
#       DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/BladeGenInterface/lib
#    }
#}

# Google Test
INCLUDEPATH += C:/devel/googletest-master/googletest/include
win32 {
    CONFIG(debug, debug|release){
        LIBS        += -LC:/devel/googletest-master/googletest/build/lib
        DEPENDPATH  += C:/devel/googletest-master/googletest/build/lib
    } else {
        LIBS        += -LC:/devel/googletest-master/googletest/build/lib
        DEPENDPATH  += C:/devel/googletest-master/googletest/build/lib
    }
}
unix {
    LIBS        += -L$${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/lib
    DEPENDPATH  += $${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest/lib
}

#### OPTYMAL
INCLUDEPATH += C:/devel/GTLab_OptimizerInterface-master
LIBS        += -LC:/devel/GTLab_OptimizerInterface-master/build/optymal
DEPENDPATH  += C:/devel/GTLab_OptimizerInterface-master/build/optymal

#### PATH SETTINGS
BUILD_DEST = ../../build

COVERAGE {
    # code coverage settings
    unix: QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    unix: LIBS += -lgcov
}


# 17.04.2019: path to perf standalone
INCLUDEPATH += $${DEV_TOOLS}/ThirdPartyLibraries/DLRperformance/include
LIBS        += -LC:/devel/svn/DLRpp/build
DEPENDPATH  += C:/devel/svn/DLRpp/build

# 17.04.2019: path to perf standalone
# libxml
INCLUDEPATH += C:/devel/libxml2-master/include
INCLUDEPATH += C:/devel/libxml2-master
LIBS        += -LC:/devel/libxml2-master/win32/bin.msvc

######################################################################
