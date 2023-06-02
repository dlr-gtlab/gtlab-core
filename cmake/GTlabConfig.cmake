# - Config file for the GTlab package

include(CMakeFindDependencyMacro)

find_dependency(Qt5 COMPONENTS Core Gui Xml Widgets Network PrintSupport)
find_dependency(GTlabLogging)

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
