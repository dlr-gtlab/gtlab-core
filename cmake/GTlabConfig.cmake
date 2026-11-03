# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

if (CMAKE_VERSION VERSION_LESS "3.18")
  message(FATAL_ERROR "GTlab requires CMake 3.18 or newer")
endif()

# Export GTlab's install layout without changing the consumer's standard
# CMAKE_INSTALL_* variables. Values supplied by the consumer take precedence.
if (NOT DEFINED GTLAB_INSTALL_BINDIR)
  set(GTLAB_INSTALL_BINDIR "@GTLAB_INSTALL_BINDIR@")
endif()
if (NOT DEFINED GTLAB_INSTALL_DEBUG_BINDIR)
  set(GTLAB_INSTALL_DEBUG_BINDIR "@GTLAB_INSTALL_DEBUG_BINDIR@")
endif()
if (NOT DEFINED GTLAB_INSTALL_LIBDIR)
  set(GTLAB_INSTALL_LIBDIR "@GTLAB_INSTALL_LIBDIR@")
endif()

set(_gtlab_default_runtime_dir
    "$<IF:$<CONFIG:Debug>,${GTLAB_INSTALL_DEBUG_BINDIR},${GTLAB_INSTALL_BINDIR}>")
if (DEFINED GTLAB_INSTALL_RUNTIME_DIR
    AND NOT ( @GTLAB_INSTALL_RUNTIME_DIR_IS_DEFAULT@
        AND "${GTLAB_INSTALL_RUNTIME_DIR}" STREQUAL "${_gtlab_default_runtime_dir}"))
  set(_gtlab_runtime_dir_overridden TRUE)
elseif (@GTLAB_INSTALL_RUNTIME_DIR_IS_DEFAULT@)
  set(GTLAB_INSTALL_RUNTIME_DIR
      "${_gtlab_default_runtime_dir}")
  set(_gtlab_runtime_dir_overridden FALSE)
else()
  set(GTLAB_INSTALL_RUNTIME_DIR "@GTLAB_INSTALL_RUNTIME_DIR@")
  set(_gtlab_runtime_dir_overridden TRUE)
endif()
if (_gtlab_runtime_dir_overridden AND "${GTLAB_INSTALL_RUNTIME_DIR}" MATCHES "\\$<")
  message(FATAL_ERROR
      "A custom GTLAB_INSTALL_RUNTIME_DIR must be a path; override the release and Debug bindirs separately.")
endif()

if (_gtlab_runtime_dir_overridden)
  set(GTLAB_INSTALL_RUNTIME_DIR_RELEASE "${GTLAB_INSTALL_RUNTIME_DIR}")
  set(GTLAB_INSTALL_RUNTIME_DIR_DEBUG "${GTLAB_INSTALL_RUNTIME_DIR}")
else()
  set(GTLAB_INSTALL_RUNTIME_DIR_RELEASE "${GTLAB_INSTALL_BINDIR}")
  set(GTLAB_INSTALL_RUNTIME_DIR_DEBUG "${GTLAB_INSTALL_DEBUG_BINDIR}")
endif()

set(_gtlab_default_module_dir "${GTLAB_INSTALL_RUNTIME_DIR}/modules")
if (DEFINED GTLAB_INSTALL_MODULEDIR
    AND NOT (@GTLAB_INSTALL_MODULEDIR_IS_DEFAULT@
        AND NOT DEFINED CMAKE_INSTALL_MODULEDIR
        AND "${GTLAB_INSTALL_MODULEDIR}" STREQUAL "${_gtlab_default_module_dir}"))
  set(_gtlab_moduledir_overridden TRUE)
elseif (DEFINED CMAKE_INSTALL_MODULEDIR)
  set(GTLAB_INSTALL_MODULEDIR "${CMAKE_INSTALL_MODULEDIR}")
  set(_gtlab_moduledir_overridden TRUE)
elseif (@GTLAB_INSTALL_MODULEDIR_IS_DEFAULT@)
  set(GTLAB_INSTALL_MODULEDIR "${_gtlab_default_module_dir}")
  set(_gtlab_moduledir_overridden FALSE)
else()
  set(GTLAB_INSTALL_MODULEDIR "@GTLAB_INSTALL_MODULEDIR@")
  set(_gtlab_moduledir_overridden TRUE)
endif()
if (_gtlab_moduledir_overridden AND "${GTLAB_INSTALL_MODULEDIR}" MATCHES "\\$<")
  message(FATAL_ERROR "A custom GTLAB_INSTALL_MODULEDIR must be a path, not a generator expression.")
endif()

if (_gtlab_moduledir_overridden)
  set(GTLAB_INSTALL_MODULEDIR_RELEASE "${GTLAB_INSTALL_MODULEDIR}")
  set(GTLAB_INSTALL_MODULEDIR_DEBUG "${GTLAB_INSTALL_MODULEDIR}")
elseif (_gtlab_runtime_dir_overridden)
  set(GTLAB_INSTALL_MODULEDIR_RELEASE "${GTLAB_INSTALL_RUNTIME_DIR}/modules")
  set(GTLAB_INSTALL_MODULEDIR_DEBUG "${GTLAB_INSTALL_RUNTIME_DIR}/modules")
else()
  set(GTLAB_INSTALL_MODULEDIR_RELEASE "${GTLAB_INSTALL_BINDIR}/modules")
  set(GTLAB_INSTALL_MODULEDIR_DEBUG "${GTLAB_INSTALL_DEBUG_BINDIR}/modules")
endif()

# - Config file for the GTlab package
include(CMakeFindDependencyMacro)

# Qt major version used when GTlab was built
set(QT_VERSION_MAJOR @QT_VERSION_MAJOR@)
set(GTLAB_QT_VERSION_MAJOR @QT_VERSION_MAJOR@)

# Version information embedded into modules built with add_gtlab_module().
set(GTLAB_CORE_VERSION "@PROJECT_VERSION@")
set(GTLAB_MODULE_ABI "@GTLAB_MODULE_ABI@")

# Require the same Qt major version for consumers
find_dependency(Qt@QT_VERSION_MAJOR@ COMPONENTS Core Gui Xml Widgets Network PrintSupport)
if (QT_VERSION_MAJOR EQUAL 6)
  find_dependency(Qt6Core5Compat REQUIRED)
endif()
find_dependency(GTlabLogging)
find_dependency(GenH5)

include("${CMAKE_CURRENT_LIST_DIR}/GTlabTargets.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/GTlab.cmake")

# create aliases.
if (NOT TARGET GTlab::DataProcessor)
  add_library(GTlab::DataProcessor INTERFACE IMPORTED)
  set_target_properties(GTlab::DataProcessor PROPERTIES
    INTERFACE_LINK_LIBRARIES "GTlabDataProcessor"
  )
endif()

if (NOT TARGET GTlab::Core)
  add_library(GTlab::Core INTERFACE IMPORTED)
  set_target_properties(GTlab::Core PROPERTIES
    INTERFACE_LINK_LIBRARIES "GTlabCore"
  )
endif()

if (NOT TARGET GTlab::Gui)
  add_library(GTlab::Gui INTERFACE IMPORTED)

  set_target_properties(GTlab::Gui PROPERTIES
    INTERFACE_LINK_LIBRARIES "GTlabGui"
  )
endif()
