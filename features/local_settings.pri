#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/              

# path to dev tools
DEV_TOOLS = C:\GTlab-DevTools

# core build settings
BUILD_DATAMODEL =               true
BUILD_CORE =                    true
BUILD_GUI =                     true
BUILD_BATCH =                   true
BUILD_APP =                     true

USE_HDF5 =                      false

# tests
BUILD_UNITTESTS =               false
BUILD_TESTMODULES =             false

# log line numbers 
#DEFINES += GT_LOG_LINE_NUMBERS

# deployment settings
BUILD_DEPLOY =                  true

# First Party
GTLAB_LOGGING_PATH =    $${DEV_TOOLS}
GENH5_PATH =            $${DEV_TOOLS} # HDF5 Wrapper

# THIRD PARTY
GOOGLE_TEST_PATH =      $${DEV_TOOLS}/ThirdPartyLibraries/GoogleTest
HDF5_PATH =             $${DEV_TOOLS}/ThirdPartyLibraries/hdf5
