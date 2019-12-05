#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

set(CMAKE_AUTORCC ON)

add_definitions(-DGT_CORE_DLL)

# qstring restrictions
add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
)

include_directories(
    .
    settings
    provider
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel
    ${CMAKE_CURRENT_SOURCE_DIR}/../calculators
    ${CMAKE_CURRENT_SOURCE_DIR}/../network
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel/property
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/../versioncontrol
)

file(GLOB_RECURSE sources
    "*.h"
    "*.cpp"
)

set(resources
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/templates/templates.qrc
)


add_library(GTlabCore SHARED ${sources} ${resources})
target_link_libraries(GTlabCore
    Qt5::Xml
    Qt5::Network
    GTlabNumerics
    GTlabPhysics
    GTlabLogging
    GTlabDatamodel
    GTlabCalculators
    GTlabVersionControl
    GTlabNetwork
)

install(TARGETS GTlabCore
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


######################################################################
