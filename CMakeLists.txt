#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

cmake_minimum_required(VERSION 3.0)

# Set a default build type if none was specified
if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

project(GTlab-hydrogen)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# store all dlls inside build directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

if(NOT DEFINED CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR "bin")
endif(NOT DEFINED CMAKE_INSTALL_BINDIR)

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)

if(NOT DEFINED INSTALL_MODULE_DIR)
    set(INSTALL_MODULE_DIR ${CMAKE_INSTALL_BINDIR}/modules)
endif()

if(NOT DEFINED INSTALL_MODULE_LIB_DIR)
    set(INSTALL_MODULE_LIB_DIR "lib")
endif()

if(NOT DEFINED INSTALL_HEADER_DIR)
    set(INSTALL_HEADER_DIR "include/gtlab")
endif()

# prefix 3rdparty stuff from devtools
set(GTLAB_DEVTOOLS_THIRDPARTY_PATH "c:/devtools" CACHE PATH "Path of the thirdparty dependencies for gtlab")
LIST(APPEND CMAKE_PREFIX_PATH
    ${GTLAB_DEVTOOLS_THIRDPARTY_PATH}/Qwt
    ${GTLAB_DEVTOOLS_THIRDPARTY_PATH}/LibXML
    ${GTLAB_DEVTOOLS_THIRDPARTY_PATH}/minpack
    ${GTLAB_DEVTOOLS_THIRDPARTY_PATH}/GoogleTest
    ${GTLAB_DEVTOOLS_THIRDPARTY_PATH}/NLopt
    ${GTLAB_DEVTOOLS_THIRDPARTY_PATH}/RCERemote
    ${GTLAB_DEVTOOLS_THIRDPARTY_PATH}/OpenCascade
    ${GTLAB_DEVTOOLS_THIRDPARTY_PATH}/BladeGenInterface
    ${GTLAB_DEVTOOLS_THIRDPARTY_PATH}/fsu_stochastics
)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(QT5_PATH "c:/devel/Qt5" CACHE PATH "Path of the Qt 5 installation")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${QT5_PATH}")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Script REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Help REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(QWT REQUIRED)
find_package(CMINPACK REQUIRED)
find_package(FSUSTOCHASTICS REQUIRED)
find_package(BLADEGEN)

# enable automatic linking of qtmain
if (POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif (POLICY CMP0020)

if (NOT BLADEGEN_FOUND)
    message(WARNING "BladeGen Interface not found. Support for 3D blades disabled!")
endif()

# libraries
option(GTLAB_BUILD_CADKERNEL "Build the CAD kernel" ON)
option(GTLAB_BUILD_UTILITIES "Build the utilities library" ON)
option(GTLAB_BUILD_DATAMODEL "Build the datamodel library" ON)
option(GTLAB_BUILD_VERSIONCONTROL "Build the version control library" ON)
option(GTLAB_BUILD_CONSTRAINTSOLVER "Build the constraint solver library" ON)
option(GTLAB_BUILD_NETWORK "Build the network library" ON)
option(GTLAB_BUILD_CALCULATORS_INTERFACE "Build the calculators interface" ON)
option(GTLAB_BUILD_CORE "Build the core library" ON)
option(GTLAB_BUILD_MDI "Build the mdi library" ON)
option(GTLAB_BUILD_GUI "Build the gui library" ON)
option(GTLAB_BUILD_BATCH "Build the batch application" ON)
option(GTLAB_BUILD_APP "Build the gui application" ON)
option(GTLAB_BUILD_UNITTESTS "Build the unit tests" OFF)

# modules
option(GTLAB_BUILD_MODULE_SKETCHPAD "Build the sketchpad module" ON)
option(GTLAB_BUILD_MODULE_PERFORMANCE "Build the performance module" ON)
option(GTLAB_BUILD_MODULE_PREDESIGN "Build the pre-design module" ON)
option(GTLAB_BUILD_MODULE_ZOOMING "Build the zooming module" ON)
option(GTLAB_BUILD_MODULE_COLLECTIONS "Build the collections module" ON)
option(GTLAB_BUILD_MODULE_POST "Build the post processing module" ON)
option(GTLAB_BUILD_MODULE_STOCHASTIC "Build the stochastic module" OFF)
option(GTLAB_BUILD_MODULE_ZOOMING "Build the stochastic module" OFF)
option(GTLAB_BUILD_MODULE_TESTMODULE "Build the test module" OFF)
option(GTLAB_BUILD_MODULE_RCE_REMOTE_PLUGIN "Build the RCE remote plugin" OFF)

if (GTLAB_BUILD_MODULE_RCE_REMOTE_PLUGIN)
    find_package(RCEREMOTE REQUIRED)
endif()

# build libraries and modules
add_subdirectory(src)

if (GTLAB_BUILD_UNITTESTS)
    add_subdirectory(tests/unittests)
endif(GTLAB_BUILD_UNITTESTS)

include(CPack)
