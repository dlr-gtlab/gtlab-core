# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

include(GNUInstallDirs)

# Keep the standard CMake install variables intact. These GTlab variables
# provide the installed layout to Core itself and to external module projects.
if(NOT DEFINED GTLAB_INSTALL_BINDIR)
    set(GTLAB_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}")
endif()
if(NOT DEFINED GTLAB_INSTALL_DEBUG_BINDIR)
    set(GTLAB_INSTALL_DEBUG_BINDIR "binDebug")
endif()
if(NOT DEFINED GTLAB_INSTALL_LIBDIR)
    set(GTLAB_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
endif()

set(_gtlab_default_runtime_dir
    "$<IF:$<CONFIG:Debug>,${GTLAB_INSTALL_DEBUG_BINDIR},${GTLAB_INSTALL_BINDIR}>")
if(NOT DEFINED GTLAB_INSTALL_RUNTIME_DIR)
    set(GTLAB_INSTALL_RUNTIME_DIR "${_gtlab_default_runtime_dir}")
endif()

if(DEFINED CMAKE_INSTALL_MODULEDIR)
    set(_gtlab_default_module_dir "${CMAKE_INSTALL_MODULEDIR}")
else()
    set(_gtlab_default_module_dir "${GTLAB_INSTALL_RUNTIME_DIR}/modules")
endif()
if(NOT DEFINED GTLAB_INSTALL_MODULEDIR)
    set(GTLAB_INSTALL_MODULEDIR "${_gtlab_default_module_dir}")
endif()
if(NOT DEFINED GTLAB_BUILD_MODULEDIR)
    set(GTLAB_BUILD_MODULEDIR "${CMAKE_BINARY_DIR}/modules")
endif()

if("${GTLAB_INSTALL_RUNTIME_DIR}" STREQUAL "${_gtlab_default_runtime_dir}")
    set(GTLAB_INSTALL_RUNTIME_DIR_IS_DEFAULT TRUE)
    set(GTLAB_INSTALL_RUNTIME_DIR_RELEASE "${GTLAB_INSTALL_BINDIR}")
    set(GTLAB_INSTALL_RUNTIME_DIR_DEBUG "${GTLAB_INSTALL_DEBUG_BINDIR}")
else()
    set(GTLAB_INSTALL_RUNTIME_DIR_IS_DEFAULT FALSE)
    if("${GTLAB_INSTALL_RUNTIME_DIR}" MATCHES "\\$<")
        message(FATAL_ERROR
            "A custom GTLAB_INSTALL_RUNTIME_DIR must be a relative path, not a generator expression. "
            "Override GTLAB_INSTALL_BINDIR and GTLAB_INSTALL_DEBUG_BINDIR instead.")
    endif()
    set(GTLAB_INSTALL_RUNTIME_DIR_RELEASE "${GTLAB_INSTALL_RUNTIME_DIR}")
    set(GTLAB_INSTALL_RUNTIME_DIR_DEBUG "${GTLAB_INSTALL_RUNTIME_DIR}")
endif()

if("${GTLAB_INSTALL_MODULEDIR}" STREQUAL "${_gtlab_default_module_dir}"
        AND NOT DEFINED CMAKE_INSTALL_MODULEDIR)
    set(GTLAB_INSTALL_MODULEDIR_IS_DEFAULT TRUE)
    set(GTLAB_INSTALL_MODULEDIR_RELEASE
        "${GTLAB_INSTALL_RUNTIME_DIR_RELEASE}/modules")
    set(GTLAB_INSTALL_MODULEDIR_DEBUG
        "${GTLAB_INSTALL_RUNTIME_DIR_DEBUG}/modules")
else()
    set(GTLAB_INSTALL_MODULEDIR_IS_DEFAULT FALSE)
    if("${GTLAB_INSTALL_MODULEDIR}" MATCHES "\\$<")
        message(FATAL_ERROR
            "A custom GTLAB_INSTALL_MODULEDIR must be a relative path, not a generator expression.")
    endif()
    set(GTLAB_INSTALL_MODULEDIR_RELEASE "${GTLAB_INSTALL_MODULEDIR}")
    set(GTLAB_INSTALL_MODULEDIR_DEBUG "${GTLAB_INSTALL_MODULEDIR}")
endif()

foreach(_gtlab_layout_variable
        GTLAB_INSTALL_BINDIR
        GTLAB_INSTALL_DEBUG_BINDIR
        GTLAB_INSTALL_LIBDIR
        GTLAB_INSTALL_RUNTIME_DIR
        GTLAB_INSTALL_RUNTIME_DIR_RELEASE
        GTLAB_INSTALL_RUNTIME_DIR_DEBUG
        GTLAB_INSTALL_MODULEDIR
        GTLAB_INSTALL_MODULEDIR_RELEASE
        GTLAB_INSTALL_MODULEDIR_DEBUG
        GTLAB_BUILD_MODULEDIR)
    set_property(GLOBAL PROPERTY "${_gtlab_layout_variable}"
        "${${_gtlab_layout_variable}}")
endforeach()
