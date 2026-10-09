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

project(GTlab-Core VERSION 2.1.0)

option(BUILD_WITH_HDF5 "Build with hdf5 support" ON)
option(BUILD_UNITTESTS "Build the unit tests" OFF)
option(BUILD_WITH_COVERAGE "Build with code coverage (linux only)" OFF)
option(BUILD_TESTMODULES "Build the test modules" OFF)

if (BUILD_UNITTESTS OR BUILD_TESTMODULES)
    enable_testing()
endif()
set(GTLAB_COVERAGE_LCOV_ARGS ""
    CACHE STRING "Additional arguments passed to lcov for the test-coverage target")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# generic GTlab-Ecosystem defaults
include(GTlab)
gtlab_standard_setup()

find_package(GTlabDevtools QUIET)

require_qt(COMPONENTS Core Xml Network Widgets PrintSupport Svg Concurrent Qml QuickWidgets QuickControls2 Test)

# if we are using qt6, we need to load the compat library
if (QT_VERSION_MAJOR EQUAL 6)
    find_package(Qt6Core5Compat REQUIRED)
endif()

if (NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()

if (BUILD_WITH_COVERAGE)
    set(CODE_COVERAGE_VERBOSE ON)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif(BUILD_WITH_COVERAGE)

if (NOT DEFINED VCPKG_TARGET_TRIPLET)
    include(GetDependencies)
endif()

if (NOT BUILD_SHARED_LIBS)
    message(WARNING "Static library builds of GTlab are not yet fully supported.") 
endif()

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

add_subdirectory(docs)

# ChangeLog installation
install(FILES CHANGELOG.md DESTINATION .)
# License installation
install(DIRECTORY LICENSES/ DESTINATION share/gtlab/LICENSES)

# write exports
install(EXPORT GTlabTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GTlab)

# create package version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/GTlabConfigVersion.cmake"
  VERSION ${VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GTlabConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/GTlabConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/GTlab"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# install config files
install (FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/GTlabConfig.cmake"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GTlab.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/GTlabConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GTlab)

# ---------------------------------------------------------------------------
# Deploy the Qt runtime next to the *installed* GTlab.exe (Windows only).
#
# run windeployqt on the installed executable as the last step of the install
# so the install prefix becomes self-contained (GTlab.exe + Qt*.dll +
# qtplugins\ + qml\ + qt.conf), same layout as the official devtools
# install.
# ---------------------------------------------------------------------------
if (TARGET GTlab AND WIN32)
    if (Qt6_DIR)
        get_filename_component(_GT_QT_BIN_DIR "${Qt6_DIR}/../../../bin" ABSOLUTE)
    elseif (Qt5_DIR)
        get_filename_component(_GT_QT_BIN_DIR "${Qt5_DIR}/../../../bin" ABSOLUTE)
    endif()

    if (_GT_QT_BIN_DIR)
        find_program(WINDEPLOYQT windeployqt HINTS "${_GT_QT_BIN_DIR}")
    endif()

    if (WINDEPLOYQT)
        # CMAKE_INSTALL_BINDIR is a generator expression (set by
        # gtlab_standard_setup() or by the consuming build) that cannot be
        # evaluated inside an install(CODE) script, so mirror its logic here
        # for the current configuration
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(_GT_QT_INSTALL_BINDIR "binDebug")
        else()
            set(_GT_QT_INSTALL_BINDIR "bin")
        endif()

        message(STATUS "Qt runtime will be deployed by: ${WINDEPLOYQT}")

        install(CODE "
            execute_process(
                COMMAND \"${WINDEPLOYQT}\"
                    --qmldir \"${CMAKE_CURRENT_SOURCE_DIR}/src/resources/qml\"
                    --plugindir \"\${CMAKE_INSTALL_PREFIX}/${_GT_QT_INSTALL_BINDIR}/qtplugins\"
                    --no-translations
                    --qml-deploy-dir \"\${CMAKE_INSTALL_PREFIX}/${_GT_QT_INSTALL_BINDIR}/qml\"
                    --printsupport --core5compat
                    \"\${CMAKE_INSTALL_PREFIX}/${_GT_QT_INSTALL_BINDIR}/GTlab.exe\"
                RESULT_VARIABLE _gt_windeployqt_rc
                COMMAND_ECHO STDOUT)
            if(NOT _gt_windeployqt_rc EQUAL 0)
                message(FATAL_ERROR \"windeployqt failed with exit code \${_gt_windeployqt_rc}\")
            endif()
        ")

        # Ship the qt.conf next to the Qt DLLs so the custom "qtplugins" /
        # "qml" directories are found (relative paths in a qt.conf are
        # resolved against the directory containing the qt.conf)
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/qt.conf"
            DESTINATION ${CMAKE_INSTALL_BINDIR})
    else()
        message(WARNING "windeployqt not found - the Qt runtime will NOT be deployed to the install prefix automatically")
    endif()
endif()

if (BUILD_UNITTESTS)
    add_subdirectory(tests/unittests)
endif(BUILD_UNITTESTS)

if (BUILD_TESTMODULES)
    add_subdirectory(tests/modules)
    add_subdirectory(tests/systemtests)
endif(BUILD_TESTMODULES)

if (BUILD_UNITTESTS AND BUILD_WITH_COVERAGE)

    # Bundled dependencies can overwrite the coverage functions with their own
    # versions. Restore GTlab's module before creating the coverage targets.
    include("${PROJECT_SOURCE_DIR}/cmake/CodeCoverage.cmake")

    if (EXISTS ${GTLAB_DEVTOOLS_DIR})
      set(DEVTOOLS_DIR_EXCLUDE "${GTLAB_DEVTOOLS_DIR}/*")
    endif()

    separate_arguments(GTLAB_COVERAGE_LCOV_ARGS_LIST NATIVE_COMMAND
                       "${GTLAB_COVERAGE_LCOV_ARGS}")

    set(GTLAB_COVERAGE_DEPENDENCIES GTlabUnitTest)
    set(GTLAB_COVERAGE_CTEST_ARGS)
    if (BUILD_TESTMODULES AND GTLAB_SYSTEMTESTS_ENABLED)
        list(APPEND GTLAB_COVERAGE_DEPENDENCIES systemtests)
        list(APPEND GTLAB_COVERAGE_CTEST_ARGS -L SystemTest --output-on-failure)
    endif()

    setup_target_for_coverage_lcov(
            NAME test-coverage
            EXECUTABLE GTlabUnitTest
            DEPENDENCIES ${GTLAB_COVERAGE_DEPENDENCIES}
            CTEST_ARGS ${GTLAB_COVERAGE_CTEST_ARGS}
            LCOV_ARGS ${GTLAB_COVERAGE_LCOV_ARGS_LIST}
            EXCLUDE "tests/*" "build*" "src/gui/*" "/usr/*" "/opt/*" ${DEVTOOLS_DIR_EXCLUDE}
            BASE_DIRECTORY "${PROJECT_SOURCE_DIR}")

    setup_target_for_diff_coverage(
            NAME diff-coverage
            DEPENDENCIES GTlabUnitTest
            BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
            EXCLUDE "tests/*" "build*" "src/gui/*" "/usr/*" "/opt/*" ${DEVTOOLS_DIR_EXCLUDE}
            FILTER ".*/src/core/" ".*/src/dataprocessor/")
endif()

include(CPack)
set(CPACK_PACKAGE_VENDOR "DLR AT")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
