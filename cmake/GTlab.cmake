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
# Note: this function should not be used anymore
#
# Usage:
#   enable_gtlab_devtools()
macro(enable_gtlab_devtools)
    message(WARNING "enable_gtlab_devtools() is deprecated.
Use find_package(GTlabDevtools QUIET) instead")

    # prefix 3rdparty stuff from devtools
    set(GTLAB_DEVTOOLS_DIR "" CACHE PATH "Path of gtlab devtools")

    if (EXISTS ${GTLAB_DEVTOOLS_DIR})
      if (NOT DEFINED GTlabDevtools_ROOT)
        set(GTlabDevtools_ROOT ${GTLAB_DEVTOOLS_DIR})
      endif()
      find_package(GTlabDevtools QUIET)
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
# require_qt(
#   COMPONENTS <comp>...
# )
#
# Summary:
#   Locate and configure single Qt 5 for the entire
#   build and ensure the requested components are available.
#
# Behavior:
#   - Basically just uses
#       find_package(Qt5 REQUIRED COMPONENTS <components>)
#
#   - On the first call, if QT_VERSION_MAJOR is NOT defined:
#       * If Qt has already been found by the parent project
#         (e.g. via find_package(QT ...) or find_package(Qt6/Qt5 ...)),
#         QT_VERSION_MAJOR is taken from the existing Qt configuration or
#         inferred from existing Qt5::* targets.
#       * Otherwise, require_qt() calls:
#             find_package(QT NAMES Qt5 REQUIRED COMPONENTS <components>)
#         which sets QT_VERSION_MAJOR to 5 according to what CMake
#         finds on the search path.
#       * QT_VERSION_MAJOR is then cached so all subsequent calls reuse
#         the same major version.
#
#
# Guarantees:
#   - If the requested Qt components for that major cannot be found,
#     configuration fails with an error.
#
# User control of the chosen Qt version:
#   - Standard CMake mechanisms apply. The user can steer which Qt is found by:
#       * Adjusting CMAKE_PREFIX_PATH / Qt installation order (preferred), or
#       * Setting Qt5_DIR to point at a specific Qt installation
# ==============================================================================
function(require_qt)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs COMPONENTS)
    cmake_parse_arguments(RQT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT RQT_COMPONENTS)
        message(FATAL_ERROR "require_qt() called without COMPONENTS")
    endif()

    # --------------------------------------------------------
    # 1. Decide QT_VERSION_MAJOR once
    # --------------------------------------------------------
    if (NOT DEFINED QT_VERSION_MAJOR)

        # 1a) If user hinted a specific major via Qt5_DIR, respect that
        if (DEFINED Qt6_DIR)
            message(ERROR "GTlab 2.0 does not support Qt6")
        else()
            set(QT_VERSION_MAJOR 5)
        endif()

        # One-time status message
        if (NOT DEFINED GTLAB_QT_VERSION_REPORTED AND DEFINED QT_VERSION_MAJOR)
            message(STATUS "GTlab: using Qt${QT_VERSION_MAJOR} for this build")
            set(GTLAB_QT_VERSION_REPORTED TRUE CACHE INTERNAL
                "Whether the selected Qt version has been reported")
        endif()
    endif()

    # actually find qt
    find_package(Qt5 REQUIRED COMPONENTS ${RQT_COMPONENTS})
    set (QT_VERSION_MAJOR 5 PARENT_SCOPE)
endfunction()
