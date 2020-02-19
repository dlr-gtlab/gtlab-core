#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/              

# path to dev tools
DEV_TOOLS = C:\GTlab-DevTools

# core build settings
BUILD_DATAMODEL =               true
BUILD_CALCULATOR_INTERFACE =    true
BUILD_NETWORK =                 true
BUILD_CORE =                    true
BUILD_MDI =                     true
BUILD_GUI =                     true
BUILD_BATCH =                   true
BUILD_APP =                     true

# tests
BUILD_UNITTESTS =               false
BUILD_REGRESSIONTESTS =         false

# deployment settings
BUILD_DEPLOY =                  true

# First Party
GTLAB_LOGGING_PATH = $${DEV_TOOLS}
GTLAB_NUMERICS_PATH = $${DEV_TOOLS}

# THIRD PARTY
GOOGLE_TEST_PATH = $${DEV_TOOLS}\ThirdPartyLibraries\GoogleTest
MINPACK_PATH = $${DEV_TOOLS}\ThirdPartyLibraries\minpack
SPLINE_LIB_PATH = $${DEV_TOOLS}\ThirdPartyLibraries\SplineLib
QWT_PATH = $${DEV_TOOLS}\ThirdPartyLibraries\Qwt
NLOPT_PATH = $${DEV_TOOLS}\ThirdPartyLibraries\NLopt
