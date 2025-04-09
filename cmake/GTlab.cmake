# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

# This function set some variables that are common
# to the gtlab build system.
# It makes sure, that libraries are put into the root build folder,
# sets the debug postfix "-d" and sets the "binDebug" folder in
# Debug mode
#
# usage:
#   gtlab_standard_setup()
macro(gtlab_standard_setup)
    message("Setting up GTlab standards configuration")
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    # store all dlls inside build directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})


    if(NOT DEFINED CMAKE_INSTALL_BINDIR)
        set(CMAKE_INSTALL_BINDIR "$<$<CONFIG:DEBUG>:binDebug>$<$<NOT:$<CONFIG:DEBUG>>:bin>")
    endif(NOT DEFINED CMAKE_INSTALL_BINDIR)

    if(NOT DEFINED CMAKE_INSTALL_MODULEDIR)
        if (IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/build)
          message(STATUS "Deploying into GTlab build dir")
          set(CMAKE_INSTALL_MODULEDIR build/modules)

        else ()
          set(CMAKE_INSTALL_MODULEDIR ${CMAKE_INSTALL_BINDIR}/modules)
        endif()
    endif(NOT DEFINED CMAKE_INSTALL_MODULEDIR)

    if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
        set(CMAKE_INSTALL_LIBDIR "lib")
    endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)

    if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set(CMAKE_DEBUG_POSTFIX "-d")
    endif()

    if(NOT DEFINED CMAKE_INSTALL_DOCDIR)
        set(CMAKE_INSTALL_DOCDIR "share/doc/gtlab/")
    endif(NOT DEFINED CMAKE_INSTALL_DOCDIR)

    # Instruct CMake to run moc automatically when needed.
    set(CMAKE_AUTOMOC ON)
endmacro()

# This function adds the packages of GTlab's devtools
# to the cmake prefix path. It adds the GTLAB_DEVTOOLS_DIR
# cache variable, to set the path to GTlab's devtools
#
# Usage:
#   enable_gtlab_devtools()
macro(enable_gtlab_devtools)
    # prefix 3rdparty stuff from devtools
    set(GTLAB_DEVTOOLS_DIR "" CACHE PATH "Path of gtlab devtools")

    if (EXISTS ${GTLAB_DEVTOOLS_DIR})
        message("GTlab DevTools enabled at " ${GTLAB_DEVTOOLS_DIR})

        set (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${GTLAB_DEVTOOLS_DIR})

        set(bladegen_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/BladeGenInterface)
        set(ceres_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/ceres)
        set(GTest_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/GoogleTest)
        set(hdf5_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/hdf5)
        set(LibXml2_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/LibXML)
        set(CMinpack_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/minpack)
        set(nlopt_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/NLopt)
        set(optymal_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/Optymal)

        set(Qwt_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/Qwt)
        set(SplineLib_ROOT ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/SplineLib)

        set(GTlabCompat_ROOT ${GTLAB_DEVTOOLS_DIR}/../../tools/CompatibilityLib)

    endif()
	
endmacro()

# Function to add a gtlab module
#
# This makes sure to add a module id and deploy to the correct
# folders.
#
# Arguments:
#   MODULE_ID:   String containing the module ID of the module
#   SOURCES:     Sources (and headers) of the module to be built
#
# Optional arguments:
#   README_FILE: Location to the readme. This is used to copy the readme to the module's meta directory.
#   CHANGELOG_FILE: Location to the changelog, which is also copied to the meta directory in the install step.
#   EXAMPLES_DIR: Directory containing the example folders of the project for the automatic example presentation of GTlab
#                 Each example should be in a subdirectory of the following structure:
#                 EXAMPLE_DIR/
#                   -> project/
#                       -> CONTENT OF THE GTLAB PROJECT (e.g. performance.gtmod)
#                   -> index.json
#                   -> picture.png
#
#                 For more details see the documentation of gtlab about offering examples
#
# Usage:
#   add_gtlab_module(mymodule
#     SOURCES mod.cpp mod2.cpp mod.h
#     MODULE_ID "my mod"
#     README_FILE "${PROJECT_SOURCE_DIR}/README.md"
#     CHANGELOG_FILE "${PROJECT_SOURCE_DIR}/CHANGELOG.md"
#     EXAMPLES_DIR "${PROJECT_SOURCE_DIR}/examples/"
#   )
function(add_gtlab_module GTLAB_ADD_MODULE_TARGET)
  cmake_parse_arguments(
    PARSE_ARGV 1 GTLAB_ADD_MODULE "" "MODULE_ID;README_FILE;CHANGELOG_FILE;EXAMPLES_DIR" "SOURCES"
  )

  if (NOT DEFINED GTLAB_ADD_MODULE_MODULE_ID)
      message(FATAL_ERROR "In add_gtlab_module: Missing argument MODULE_ID for target ${GTLAB_ADD_MODULE_TARGET}")
  endif()

  add_library(${GTLAB_ADD_MODULE_TARGET} SHARED ${GTLAB_ADD_MODULE_SOURCES})

  # add module id
  target_compile_definitions(${GTLAB_ADD_MODULE_TARGET}
    PRIVATE -DGT_MODULE_ID="${GTLAB_ADD_MODULE_MODULE_ID}")

  # set rpath for linux
  if (UNIX)
      set_target_properties(${GTLAB_ADD_MODULE_TARGET}
          PROPERTIES INSTALL_RPATH "$ORIGIN:$ORIGIN/.."
                     INSTALL_RPATH_USE_LINK_PATH FALSE)
  endif (UNIX)


  if (UNIX)
    install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
        LIBRARY DESTINATION
        ${CMAKE_INSTALL_MODULEDIR}
    )
  else(UNIX)
    install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
        RUNTIME DESTINATION
        ${CMAKE_INSTALL_MODULEDIR}
    )
  endif(UNIX)

  # Copy of README and CHANGELOG to meta directory
  if (DEFINED GTLAB_ADD_MODULE_README_FILE)
    install(FILES ${GTLAB_ADD_MODULE_README_FILE}
            DESTINATION ${CMAKE_INSTALL_MODULEDIR}/meta/${GTLAB_ADD_MODULE_MODULE_ID})
  endif()

  if (DEFINED GTLAB_ADD_MODULE_CHANGELOG_FILE)
    install(FILES ${GTLAB_ADD_MODULE_CHANGELOG_FILE}
            DESTINATION ${CMAKE_INSTALL_MODULEDIR}/meta/${GTLAB_ADD_MODULE_MODULE_ID})
  endif()

  if (DEFINED GTLAB_ADD_MODULE_EXAMPLES_DIR)
    install(DIRECTORY  ${GTLAB_ADD_MODULE_EXAMPLES_DIR}
            DESTINATION examples/)
  endif()

endfunction()


# ==============================================================================
# require_qt()
#
# Ensures the specified Qt components are available and mapped to versionless
# Qt:: targets, compatible with both Qt5 and Qt6.
#
# This function abstracts away differences between Qt5 and Qt6:
# - For Qt5, it calls `find_package(Qt5)` and creates alias targets like `Qt::Core`.
# - For Qt6, it sets QT_DEFAULT_MAJOR_VERSION and calls `find_package(Qt)` which provides `Qt::` targets directly.
#
# Parameters:
#   VERSION     Optional Qt major version (5 or 6). If omitted, uses QT_DEFAULT_MAJOR_VERSION or defaults to 5.
#   COMPONENTS  List of required Qt modules (e.g. Core, Widgets, Gui, etc.)
#
# Usage:
#   require_qt(
#       VERSION 5
#       COMPONENTS Core Widgets
#   )
#
# Notes:
#   - Safe to call from multiple CMake subprojects and CPM packages.
#   - Does not globally set or affect other Qt-related variables.
# ==============================================================================

function(require_qt)
    set(options)
    set(oneValueArgs VERSION)
    set(multiValueArgs COMPONENTS)
    cmake_parse_arguments(QT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Determine desired Qt version
    if(QT_VERSION)
        set(_qt_major_version ${QT_VERSION})
    elseif(DEFINED QT_DEFAULT_MAJOR_VERSION)
        set(_qt_major_version ${QT_DEFAULT_MAJOR_VERSION})
    else()
        set(_qt_major_version 5)
    endif()

    # Ensure COMPONENTS are specified
    if(NOT QT_COMPONENTS)
        message(FATAL_ERROR "require_qt() called without COMPONENTS")
    endif()

    # Find appropriate Qt version and set up targets
    if(_qt_major_version STREQUAL "5")
        find_package(Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
        foreach(comp IN LISTS QT_COMPONENTS)
            if(TARGET Qt5::${comp} AND NOT TARGET Qt::${comp})
                add_library(Qt::${comp} ALIAS Qt5::${comp})
            endif()
        endforeach()

    elseif(_qt_major_version STREQUAL "6")
        set(QT_DEFAULT_MAJOR_VERSION 6)  # Required before find_package(Qt)
        find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})
        if(TARGET Qt6::${comp} AND NOT TARGET Qt::${comp})
            add_library(Qt::${comp} ALIAS Qt6::${comp})
        endif()
    else()
        message(FATAL_ERROR "require_qt(): Unsupported Qt version '${_qt_major_version}'")
    endif()
endfunction()

