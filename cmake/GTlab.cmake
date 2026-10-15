# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

# Apply build defaults that are common to the GTlab ecosystem.
# Install directory variables are provided by GNUInstallDirs and the GTlab
# package layout; this helper deliberately leaves CMAKE_INSTALL_* untouched.
#
# usage:
#   gtlab_standard_setup()
macro(gtlab_standard_setup)
    message("Setting up GTlab standards configuration")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    # store all dlls inside build directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})


    if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set(CMAKE_DEBUG_POSTFIX "-d")
    endif()

    # Instruct CMake to run moc automatically when needed.
    set(CMAKE_AUTOMOC ON)
endmacro()

function(_gtlab_get_layout_value name fallback output)
  if (DEFINED ${name})
    set(_value "${${name}}")
  else()
    get_property(_value GLOBAL PROPERTY "${name}")
  endif()

  if (NOT _value)
    set(_value "${fallback}")
  endif()
  set(${output} "${_value}" PARENT_SCOPE)
endfunction()

function(_gtlab_get_relative_install_rpath output install_from install_to)
  if (APPLE)
    set(_origin "@loader_path")
  else()
    set(_origin "$ORIGIN")
  endif()

  if (IS_ABSOLUTE "${install_from}")
    set(_from_path "${install_from}")
  else()
    set(_from_path "${CMAKE_INSTALL_PREFIX}/${install_from}")
  endif()
  if (IS_ABSOLUTE "${install_to}")
    set(_to_path "${install_to}")
  else()
    set(_to_path "${CMAKE_INSTALL_PREFIX}/${install_to}")
  endif()

  file(RELATIVE_PATH _relative_path "${_from_path}" "${_to_path}")
  if (_relative_path STREQUAL ".")
    set(_rpath "${_origin}")
  else()
    set(_rpath "${_origin}/${_relative_path}")
  endif()
  set(${output} "${_rpath}" PARENT_SCOPE)
endfunction()

# Set a relocatable install RPATH from one or two configuration-specific
# install directories to GTlab's library directory. Existing CMake RPATH
# controls and target-level overrides take precedence.
function(gtlab_set_install_rpath target install_dir_release)
  if ((NOT APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
      OR CMAKE_SKIP_RPATH OR CMAKE_SKIP_INSTALL_RPATH)
    return()
  endif()
  if (DEFINED CMAKE_INSTALL_RPATH AND NOT "${CMAKE_INSTALL_RPATH}" STREQUAL "")
    return()
  endif()

  get_target_property(_target_install_rpath "${target}" INSTALL_RPATH)
  if (_target_install_rpath AND NOT _target_install_rpath STREQUAL "_target_install_rpath-NOTFOUND")
    return()
  endif()

  _gtlab_get_layout_value(GTLAB_INSTALL_LIBDIR "lib" _libdir)
  _gtlab_get_relative_install_rpath(_release_rpath "${install_dir_release}" "${_libdir}")

  if (ARGC GREATER 2)
    set(_install_dir_debug "${ARGV2}")
    _gtlab_get_relative_install_rpath(_debug_rpath "${_install_dir_debug}" "${_libdir}")
    if (NOT "${_debug_rpath}" STREQUAL "${_release_rpath}")
      set(_install_rpath "$<IF:$<CONFIG:Debug>,${_debug_rpath},${_release_rpath}>")
    else()
      set(_install_rpath "${_release_rpath}")
    endif()
  else()
    set(_install_rpath "${_release_rpath}")
  endif()

  set_target_properties("${target}" PROPERTIES INSTALL_RPATH "${_install_rpath}")
endfunction()

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

  # A master build can include this helper from a sibling module directory.
  # In that case the normal variables set by GTlab Core are out of scope.
  if (NOT GTLAB_CORE_VERSION)
      get_property(GTLAB_CORE_VERSION GLOBAL PROPERTY GTLAB_CORE_VERSION)
  endif()
  if (NOT GTLAB_MODULE_ABI)
      get_property(GTLAB_MODULE_ABI GLOBAL PROPERTY GTLAB_MODULE_ABI)
  endif()

  if (NOT DEFINED GTLAB_ADD_MODULE_MODULE_ID)
      message(FATAL_ERROR "In add_gtlab_module: Missing argument MODULE_ID for target ${GTLAB_ADD_MODULE_TARGET}")
  endif()

  if (NOT GTLAB_CORE_VERSION OR NOT GTLAB_MODULE_ABI)
      message(FATAL_ERROR
          "In add_gtlab_module: GTLAB_CORE_VERSION and GTLAB_MODULE_ABI must be defined")
  endif()

  _gtlab_get_layout_value(GTLAB_INSTALL_BINDIR "bin" _install_bindir)
  _gtlab_get_layout_value(GTLAB_INSTALL_DEBUG_BINDIR "binDebug" _install_debug_bindir)
  _gtlab_get_layout_value(GTLAB_INSTALL_LIBDIR "lib" _install_libdir)
  set(GTLAB_INSTALL_LIBDIR "${_install_libdir}")
  _gtlab_get_layout_value(GTLAB_INSTALL_MODULEDIR
      "${_install_bindir}/modules" _install_moduledir)
  _gtlab_get_layout_value(GTLAB_INSTALL_MODULEDIR_RELEASE
      "${_install_moduledir}" _install_moduledir_release)
  _gtlab_get_layout_value(GTLAB_INSTALL_MODULEDIR_DEBUG
      "${_install_moduledir}" _install_moduledir_debug)
  if (NOT "${_install_moduledir}" MATCHES "\\$<")
    set(_install_moduledir_release "${_install_moduledir}")
    set(_install_moduledir_debug "${_install_moduledir}")
  endif()
  _gtlab_get_layout_value(GTLAB_BUILD_MODULEDIR
      "${CMAKE_BINARY_DIR}/modules" _build_moduledir)
  if (NOT IS_ABSOLUTE "${_build_moduledir}")
    get_filename_component(_build_moduledir "${_build_moduledir}" ABSOLUTE
        BASE_DIR "${CMAKE_BINARY_DIR}")
  endif()

  add_library(${GTLAB_ADD_MODULE_TARGET} SHARED ${GTLAB_ADD_MODULE_SOURCES})

  set_target_properties(${GTLAB_ADD_MODULE_TARGET} PROPERTIES
      PREFIX ""
      DEBUG_POSTFIX ""
      OUTPUT_NAME "${GTLAB_ADD_MODULE_TARGET}.gtm.${GTLAB_MODULE_ABI}"
      LIBRARY_OUTPUT_DIRECTORY "$<1:${_build_moduledir}>"
      RUNTIME_OUTPUT_DIRECTORY "$<1:${_build_moduledir}>")
  set_property(TARGET ${GTLAB_ADD_MODULE_TARGET} PROPERTY
      GTLAB_MODULE_OUTPUT_DIRECTORY_MANAGED TRUE)
  set_property(TARGET ${GTLAB_ADD_MODULE_TARGET} APPEND PROPERTY
      AUTOMOC_MOC_OPTIONS
      "-Mgtlab_core_version=${GTLAB_CORE_VERSION}"
      "-Mgtlab_module_abi=${GTLAB_MODULE_ABI}")

  # add module id
  target_compile_definitions(${GTLAB_ADD_MODULE_TARGET}
    PRIVATE -DGT_MODULE_ID="${GTLAB_ADD_MODULE_MODULE_ID}")

  gtlab_set_install_rpath(${GTLAB_ADD_MODULE_TARGET}
      "${_install_moduledir_release}" "${_install_moduledir_debug}")


  if (UNIX)
    install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
        LIBRARY DESTINATION
        ${_install_moduledir}
    )
  else(UNIX)
    install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
        RUNTIME DESTINATION
        ${_install_moduledir}
    )
  endif(UNIX)

  # Copy of README and CHANGELOG to meta directory
  if (DEFINED GTLAB_ADD_MODULE_README_FILE)
    install(FILES ${GTLAB_ADD_MODULE_README_FILE}
            DESTINATION ${_install_moduledir}/meta/${GTLAB_ADD_MODULE_MODULE_ID})
  endif()

  if (DEFINED GTLAB_ADD_MODULE_CHANGELOG_FILE)
    install(FILES ${GTLAB_ADD_MODULE_CHANGELOG_FILE}
            DESTINATION ${_install_moduledir}/meta/${GTLAB_ADD_MODULE_MODULE_ID})
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
#   Locate and configure a single Qt major version (5 or 6) for the entire
#   build and ensure the requested components are available.
#
# Behavior:
#   - Uses Qt’s “dual version” pattern:
#       find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS <components>)
#       find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS <components>)
#
#   - On the first call, if QT_VERSION_MAJOR is NOT defined:
#       * If Qt has already been found by the parent project
#         (e.g. via find_package(QT ...) or find_package(Qt6/Qt5 ...)),
#         QT_VERSION_MAJOR is taken from the existing Qt configuration or
#         inferred from existing Qt6::*/Qt5::* targets.
#       * Otherwise, require_qt() calls:
#             find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS <components>)
#         which sets QT_VERSION_MAJOR to 5 or 6 according to what CMake
#         finds on the search path.
#       * QT_VERSION_MAJOR is then cached so all subsequent calls reuse
#         the same major version.
#
#   - On subsequent calls (QT_VERSION_MAJOR already defined):
#       * require_qt() simply calls:
#             find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS <components>)
#         to make sure the requested modules for the chosen major are available.
#
# Guarantees:
#   - All callers (core and modules) in a single build tree use the same Qt
#     major version, as determined by the first successful Qt detection.
#   - If the requested Qt components for that major cannot be found,
#     configuration fails with an error.
#
# User control of the chosen Qt version:
#   - Standard CMake mechanisms apply. The user can steer which Qt is found by:
#       * Adjusting CMAKE_PREFIX_PATH / Qt installation order (preferred), or
#       * Setting Qt5_DIR / Qt6_DIR to point at a specific Qt installation
#
# Notes:
#   - This function does NOT create versionless Qt:: targets. Callers are
#     expected to link against versioned targets, e.g.:
#         Qt${QT_VERSION_MAJOR}::Core
#         Qt${QT_VERSION_MAJOR}::Widgets
#         Qt${QT_VERSION_MAJOR}::<OtherComponent>
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

        # 1a) If user hinted a specific major via Qt6_DIR / Qt5_DIR, respect that
        if (DEFINED Qt6_DIR AND NOT DEFINED Qt5_DIR)
            set(QT_VERSION_MAJOR 6)
        elseif (DEFINED Qt5_DIR AND NOT DEFINED Qt6_DIR)
            set(QT_VERSION_MAJOR 5)

        elseif (DEFINED Qt5_DIR AND DEFINED Qt6_DIR)
            set(QT_VERSION_MAJOR 6)
        else()
            # 1b) No specific *_DIR hints: use the standard Qt dual-version pattern
            #     This will honor QT_DIR, CMAKE_PREFIX_PATH, etc.
            find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${RQT_COMPONENTS})
        endif()

        # One-time status message
        if (NOT DEFINED GTLAB_QT_VERSION_REPORTED AND DEFINED QT_VERSION_MAJOR)
            message(STATUS "GTlab: using Qt${QT_VERSION_MAJOR} for this build")
            set(GTLAB_QT_VERSION_REPORTED TRUE CACHE INTERNAL
                "Whether the selected Qt version has been reported")
        endif()
    endif()

    if (NOT DEFINED QT_VERSION_MAJOR)
        message(FATAL_ERROR
            "require_qt(): QT_VERSION_MAJOR is still undefined after detection. "
            "Check your Qt hints: QT_DIR, Qt5_DIR, Qt6_DIR, CMAKE_PREFIX_PATH.")
    endif()

    # actually find qt
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${RQT_COMPONENTS})
    set (QT_VERSION_MAJOR ${QT_VERSION_MAJOR} PARENT_SCOPE)
endfunction()
