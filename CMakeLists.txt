# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

cmake_minimum_required(VERSION 3.12)

# make sure conan can be used
if (EXISTS ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    cmake_policy(SET CMP0091 NEW)
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

# Set a default build type if none was specified
if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

project(GTlab-Core VERSION 2.0.6)

option(BUILD_WITH_HDF5 "Build with hdf5 support" ON)
option(BUILD_UNITTESTS "Build the unit tests" OFF)
option(BUILD_WITH_COVERAGE "Build with code coverage (linux only)" OFF)
option(BUILD_TESTMODULES "Build the test modules" OFF)

# generic GTlab-Ecosystem defaults
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(GTlab)

gtlab_standard_setup()
enable_gtlab_devtools()

if (BUILD_WITH_COVERAGE)
    set(CODE_COVERAGE_VERBOSE ON)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif(BUILD_WITH_COVERAGE)

if (NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()

if (NOT BUILD_SHARED_LIBS)
    message(WARNING "Static library builds of GTlab are not yet fully supported.") 
endif()

find_package(Qt5 COMPONENTS Core Xml Network Widgets PrintSupport Svg Concurrent Qml QuickWidgets Test REQUIRED)

if (NOT TARGET GTlab::Logging)
  find_package(GTlabLogging REQUIRED)
endif()

if (BUILD_WITH_HDF5 AND NOT TARGET GenH5)
    find_package(GenH5 CONFIG REQUIRED)
endif()

# build libraries and modules
add_subdirectory(src/dataprocessor)
add_subdirectory(src/core)
add_subdirectory(src/batch)
add_subdirectory(src/gui)
add_subdirectory(src/app)

install(FILES CHANGELOG.md DESTINATION .)

# write exports
install(EXPORT GTlabTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GTlab)

# create package version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/GTlabConfigVersion.cmake"
  VERSION ${VERSION}
  COMPATIBILITY AnyNewerVersion
)

# install config files
install (FILES
  "cmake/GTlabConfig.cmake"
  "cmake/GTlab.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/GTlabConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GTlab)

if (BUILD_UNITTESTS)
    add_subdirectory(tests/unittests)
endif(BUILD_UNITTESTS)

if (BUILD_UNITTESTS AND BUILD_WITH_COVERAGE)

    if (EXISTS ${GTLAB_DEVTOOLS_DIR})
      set(DEVTOOLS_DIR_EXCLUDE "${GTLAB_DEVTOOLS_DIR}/*")
    endif()

    setup_target_for_coverage_lcov(
            NAME test-coverage
            EXECUTABLE GTlabUnitTest
            EXCLUDE "tests/*" "build*" "/usr/*" "/opt/*" ${DEVTOOLS_DIR_EXCLUDE}
            BASE_DIRECTORY "${PROJECT_SOURCE_DIR}")
endif()

if (BUILD_TESTMODULES)
    add_subdirectory(tests/modules)
endif(BUILD_TESTMODULES)

include(CPack)
set(CPACK_PACKAGE_VENDOR "DLR AT")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
