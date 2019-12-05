#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

if (NOT GTLAB_BUILD_UTILITIES)
    message(WARNING "GTlabNetwork library requires utilities library")
endif()

if (NOT GTLAB_BUILD_DATAMODEL)
    message(WARNING "GTlabNetwork library requires datamodel library")
endif()

add_definitions(-DGT_NETWORK_DLL)

# qstring restrictions
add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel/property
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/logging
)

set(headers
    gt_network_exports.h
    gt_accessdata.h
    gt_accessmanager.h
    gt_accessgroup.h
    gt_networkinterface.h
    gt_accessdataconnection.h
    gt_abstractaccessdataconnection.h
    gt_accessselectionproperty.h
    gt_downloaditem.h
    gt_downloader.h
)

set(sources
    gt_accessdata.cpp
    gt_accessmanager.cpp
    gt_accessgroup.cpp
    gt_accessdataconnection.cpp
    gt_abstractaccessdataconnection.cpp
    gt_accessselectionproperty.cpp
    gt_downloaditem.cpp
    gt_downloader.cpp
)


add_library(GTlabNetwork SHARED ${headers} ${sources})
target_link_libraries(GTlabNetwork
    Qt5::Core
    Qt5::Xml
    Qt5::Network
    GTlabNumerics
    GTlabPhysics
    GTlabLogging
    GTlabDatamodel
)

install(TARGETS GTlabNetwork
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

######################################################################
