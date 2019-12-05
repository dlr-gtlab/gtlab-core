#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __
#          / /_/ / / / / / /_/ / /_/ /
#         ____/ /_/ /_/\__,_/_.___/

set(CMAKE_AUTORCC ON)

add_definitions(-DGT_MDI_DLL)

add_definitions(-DQWT_DLL)

include_directories(
    .
    mdi_items
    dock_widgets
    tools
    post
    tools/graphic_items
    mdi_items/map_editor
    mdi_items/memento_viewer
    mdi_items/process_env
    mdi_items/process_env/entities
    mdi_items/label_usages
    mdi_items/session_viewer
    mdi_items/state_viewer
    mdi_items/result_viewer
    mdi_items/collection_env
    dock_widgets/output
    dock_widgets/explorer
    dock_widgets/process
    dock_widgets/process/pages
    dock_widgets/properties
    dock_widgets/properties/delegates
    dock_widgets/properties/editors
    dock_widgets/properties/items
    dock_widgets/labels
    dock_widgets/post
    dock_widgets/calculators
    object_ui
    undoredo
    importer
    exporter
    collection
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel
    ${CMAKE_CURRENT_SOURCE_DIR}/../network
    ${CMAKE_CURRENT_SOURCE_DIR}/../calculators
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/provider
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/settings
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/collection
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel/property
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/numerics
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/numerics/bspline
    ${CMAKE_CURRENT_SOURCE_DIR}/../versioncontrol
)

include_directories(${QWT_INCLUDE_DIR})


file(GLOB_RECURSE sources
    "*.h"
    "*.cpp"
)

set(resources
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/icons/icons.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/pixmaps/pixmaps.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/qml/qml.qrc
)

add_library(GTlabMdi SHARED  ${sources} ${resources})
target_link_libraries(GTlabMdi
    Qt5::Xml
    Qt5::Widgets
    Qt5::PrintSupport
    Qt5::Network
    GTlabNumerics
    GTlabPhysics
    GTlabLogging
    GTlabDatamodel
    GTlabNetwork
    GTlabCalculators
    GTlabVersionControl
    GTlabCore
    ${QWT_LIBRARY}
)

install(TARGETS GTlabMdi
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

######################################################################
