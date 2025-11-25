# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

# - Config file for the GTlab package
include(CMakeFindDependencyMacro)

# Qt major version used when GTlab was built
set(QT_VERSION_MAJOR @QT_VERSION_MAJOR@)
set(GTLAB_QT_VERSION_MAJOR @QT_VERSION_MAJOR@)

# Require the same Qt major version for consumers
find_dependency(Qt@QT_VERSION_MAJOR@ Core Gui Xml Widgets Network PrintSupport)
find_dependency(GTlabLogging)
find_dependency(GenH5)

include("${CMAKE_CURRENT_LIST_DIR}/GTlabTargets.cmake")

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
