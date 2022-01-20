#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

if (NOT GTLAB_BUILD_UTILITIES)
    message(WARNING "GTlabDatamodel library requires utilities library")
endif()

add_definitions(-DGT_DATAMODEL_DLL)

include_directories(
    .
    ${CMAKE_CURRENT_SOURCE_DIR}/property
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/numerics
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/physics
)

file(GLOB_RECURSE dm_sources
    "*.cpp"
)

file(GLOB_RECURSE dm_headers
    "*.h"
)

add_library(GTlabDatamodel SHARED ${dm_headers} ${dm_sources})
target_link_libraries(GTlabDatamodel
    Qt5::Xml
    GTlabNumerics
    GTlabPhysics
    GTlabLogging
)

install(TARGETS GTlabDatamodel
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

######################################################################
